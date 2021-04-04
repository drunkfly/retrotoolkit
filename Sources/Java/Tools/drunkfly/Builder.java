package drunkfly;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import javax.imageio.ImageIO;

import drunkfly.internal.BuilderClassLoader;
import drunkfly.internal.InputFile;
import drunkfly.internal.OutputFile;

public abstract class Builder
{
    ArrayList<InputFile> inputFiles;
    ArrayList<OutputFile> outputFiles;
    long classLastModified;

    protected abstract void run();

    public final void build(long lastModified, boolean force)
    {
        loadDependencyInfo();

        if (!force) {
            boolean changed = false;

            if (lastModified > classLastModified)
                changed = true;

            if (!changed) {
                for (InputFile it : inputFiles) {
                    File file = it.file;
                    if (!file.isFile() || file.lastModified() > it.lastModified) {
                        changed = true;
                        break;
                    }
                }
            }

            if (!changed) {
                for (OutputFile it : outputFiles) {
                    File file = it.file;
                    if (!file.isFile()) {
                        changed = true;
                        break;
                    }
                }
            }

            if (!changed && (!inputFiles.isEmpty() || !outputFiles.isEmpty()))
                return;
        }

        for (OutputFile it : outputFiles)
            it.file.delete();

        classLastModified = lastModified;

        inputFiles.clear();
        outputFiles.clear();

        run();

        saveDependencyInfo();
    }

    // I/O operations for builders

    protected byte[] loadBytes(String path)
    {
        File file = registerInputFile(path);
        try {
            return IO.loadFile(file);
        } catch (IOException e) {
            throw new RuntimeException("Unable to load \"" + path + "\".", e);
        }
    }

    protected Gfx loadGfx(String path)
    {
        try {
            File file = registerInputFile(path);
            return new Gfx(ImageIO.read(file));
        } catch (IOException e) {
            throw new RuntimeException("Unable to load \"" + path + "\".");
        }
    }

    protected void writeFile(String path, byte[] data)
    {
        File file = registerOutputFile(path);
        try {
            IO.writeFile(file, data);
        } catch (IOException e) {
            throw new RuntimeException("Unable to write \"" + path + "\".", e);
        }
    }

    // Dependency management

    private File resolveInputFile(String path)
    {
        return BuilderClassLoader.getInstance().findResourceFile(path);
    }

    private File resolveOutputFile(String path)
    {
        return new File(BuilderClassLoader.getInstance().getOutputDirectory(), path);
    }

    private File registerInputFile(String path)
    {
        File file = resolveInputFile(path);
        inputFiles.add(new InputFile(path, file));
        return file;
    }

    private File registerOutputFile(String path)
    {
        File file = resolveOutputFile(path);
        outputFiles.add(new OutputFile(path, file));
        return file;
    }

    private File dependencyFile()
    {
        return new File(BuilderClassLoader.getInstance().getOutputDirectory(), ".deps/" + getClass().getName());
    }

    private final String DEPENDENCY_FILE_ID = "DEPINFO\u001a";

    private void loadDependencyInfo()
    {
        inputFiles = new ArrayList<InputFile>();
        outputFiles = new ArrayList<OutputFile>();

        try {
            ByteArrayInputStream byteStream = new ByteArrayInputStream(IO.loadFile(dependencyFile()));
            DataInputStream stream = new DataInputStream(byteStream);

            if (!DEPENDENCY_FILE_ID.equals(stream.readUTF()))
                return;

            classLastModified = stream.readLong();

            int numInputFiles = stream.readInt();
            for (int i = 0; i < numInputFiles; i++) {
                String path = stream.readUTF();
                File file = resolveInputFile(path);
                long lastModified = stream.readLong();
                inputFiles.add(new InputFile(path, file, lastModified));
            }

            int numOutputFiles = stream.readInt();
            for (int i = 0; i < numOutputFiles; i++) {
                String path = stream.readUTF();
                File file = resolveOutputFile(path);
                long lastModified = stream.readLong();
                outputFiles.add(new OutputFile(path, file, lastModified));
            }
        } catch (IOException e) {
            inputFiles.clear();
            outputFiles.clear();
        }
    }

    private void saveDependencyInfo()
    {
        File depFile = dependencyFile();
        byte[] data;

        try {
            ByteArrayOutputStream byteStream = new ByteArrayOutputStream(16384);
            DataOutputStream stream = new DataOutputStream(byteStream);

            stream.writeUTF(DEPENDENCY_FILE_ID);
            stream.writeLong(classLastModified);

            int n = inputFiles.size();
            stream.writeInt(n);
            for (int i = 0; i < n; i++) {
                stream.writeUTF(inputFiles.get(i).path);
                stream.writeLong(inputFiles.get(i).lastModified);
            }

            n = outputFiles.size();
            stream.writeInt(n);
            for (int i = 0; i < n; i++) {
                stream.writeUTF(outputFiles.get(i).path);
                stream.writeLong(outputFiles.get(i).lastModified);
            }

            data = byteStream.toByteArray();

            IO.writeFile(depFile, data);
        } catch (IOException e) {
            throw new RuntimeException("Unable to write file \"" + depFile + "\".", e);
        }
    }
}
