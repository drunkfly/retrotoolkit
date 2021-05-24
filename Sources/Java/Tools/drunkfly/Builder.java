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
    private ArrayList<InputFile> inputFiles;
    private ArrayList<OutputFile> outputFiles;
    private long classLastModified;

    protected abstract void run();

    public final void build(long lastModified, boolean force)
    {
        loadDependencyInfo();

        if (!force) {
            boolean changed = false;

            if (lastModified != -1 && lastModified > classLastModified)
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

            if (!changed && (!inputFiles.isEmpty() || !outputFiles.isEmpty())) {
                for (OutputFile it : outputFiles)
                    compilerAddSource(it.path, it.file.getAbsolutePath());
                return;
            }
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

    public boolean inputFileExists(String path)
    {
        File file = resolveInputFile(path);
        return file.exists();
    }

    public byte[] loadBytes(String path)
    {
        File file = registerInputFile(path);
        try {
            return IO.loadFile(file);
        } catch (IOException e) {
            throw new RuntimeException("Unable to load \"" + path + "\".", e);
        }
    }

    public String loadText(String path)
    {
        return Util.fromUtf8(loadBytes(path));
    }

    public Gfx loadGfx(String path)
    {
        try {
            File file = registerInputFile(path);
            return new Gfx(path, ImageIO.read(file), GfxFormat.RGBA);
        } catch (IOException e) {
            throw new RuntimeException("Unable to load \"" + path + "\".");
        }
    }

    public Tileset loadTileset(String path)
    {
        File file = registerInputFile(path);
        return new Tileset(file);
    }

    public Tilemap loadTilemap(String path)
    {
        File file = registerInputFile(path);
        return new Tilemap(file);
    }

    public void writeFile(String path, byte[] data)
    {
        writeFile(path, data, true);
    }

    public void writeFile(String path, byte[] data, boolean addSource)
    {
        File file = registerOutputFile(path, addSource);
        try {
            IO.writeFile(file, data);
        } catch (IOException e) {
            throw new RuntimeException("Unable to write \"" + path + "\".", e);
        }
    }

    public void writeFile(String path, String data)
    {
        writeFile(path, Util.toUtf8(data), true);
    }

    public void writeFile(String path, String data, boolean addSource)
    {
        writeFile(path, Util.toUtf8(data), addSource);
    }

    public void writeFile(String path, StringBuilder data)
    {
        writeFile(path, data.toString(), true);
    }

    public void writeFile(String path, StringBuilder data, boolean addSource)
    {
        writeFile(path, data.toString(), addSource);
    }

    public void writePNG(String path, Gfx gfx)
    {
        writePNG(path, gfx, true);
    }

    public void writePNG(String path, Gfx gfx, boolean addSource)
    {
        File file = registerOutputFile(path, addSource);
        gfx.write(file, "png");
    }

    public void writeJPG(String path, Gfx gfx)
    {
        writeJPG(path, gfx, true);
    }

    public void writeJPG(String path, Gfx gfx, boolean addSource)
    {
        File file = registerOutputFile(path, addSource);
        gfx.write(file, "jpg");
    }

    // Dependency management

    private File resolveInputFile(String path)
    {
        File file = BuilderClassLoader.getInstance().findResourceFile(path);
        if (file == null) {
            file = new File(path);
            if (!file.isAbsolute() || !file.exists())
                throw new RuntimeException("File \"" + path + "\" does not exist.");
        }
        return file;
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

    private File registerOutputFile(String path, boolean addSource)
    {
        File file = resolveOutputFile(path);
        if (addSource)
            compilerAddSource(path, file.getAbsolutePath());
        outputFiles.add(new OutputFile(path, file));
        return file;
    }

    private native void compilerAddSource(String name, String path);

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
