package drunkfly;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;

public final class IO
{
    private IO() {}

    public static byte[] loadAsset(String path)
    {
        return loadFile(BuilderClassLoader.getInstance().findResourceFile(path));
    }

    public static byte[] loadFile(File file)
    {
        try {
            int fileSize = (int)file.length();
            byte[] buffer = new byte[fileSize];

            int offset = 0;
            int bytesLeft = fileSize;

            FileInputStream stream = new FileInputStream(file);
            try {
                while (bytesLeft > 0) {
                    int bytesRead = stream.read(buffer, offset, bytesLeft);
                    if (bytesRead <= 0)
                        throw new RuntimeException("Unexpected end of file \"" + file + "\".");

                    offset += bytesRead;
                    bytesLeft -= bytesRead;
                }
            } finally {
                if (stream != null)
                    stream.close();
            }

            return buffer;
        } catch (Throwable t) {
            throw new RuntimeException("Unable to load file \"" + file + "\".", t);
        }
    }

    public static void generateFile(String name, byte[] data)
    {
        File file = new File(BuilderClassLoader.getInstance().getOutputDirectory(), name);

        File parentDir = file.getParentFile();
        if (parentDir != null)
            parentDir.mkdirs();

        try {
            FileOutputStream stream = new FileOutputStream(file);
            boolean deleteFile = true;

            try {
                stream.write(data);
                stream.flush();
                deleteFile = false;
            } finally {
                if (stream != null)
                    stream.close();
                if (deleteFile)
                    file.delete();
            }
        } catch (Throwable t) {
            throw new RuntimeException("Unable to write file \"" + file + "\".", t);
        }
    }
}
