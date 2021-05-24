package drunkfly;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;

public final class IO
{
    private IO() {}

    public static String getBaseName(File file)
    {
        String fileName = file.getName();
        int dotIndex = fileName.lastIndexOf('.');
        return (dotIndex < 0 ? fileName : fileName.substring(0, dotIndex));
    }

    public static byte[] loadFile(File file) throws IOException
    {
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
    }

    public static void writeFile(File file, byte[] data) throws IOException
    {
        File parentDir = file.getParentFile();
        if (parentDir != null)
            parentDir.mkdirs();

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
    }

    public static void writeFile(File file, String data) throws IOException
    {
        writeFile(file, Util.toUtf8(data));
    }

    public static void writeFile(File file, StringBuilder data) throws IOException
    {
        writeFile(file, data.toString());
    }
}
