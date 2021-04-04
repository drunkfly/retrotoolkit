package drunkfly.internal;

import java.io.File;

public final class OutputFile
{
    public final String path;
    public final File file;
    public final long lastModified;

    public OutputFile(String path, File file)
    {
        this.path = path;
        this.file = file;
        this.lastModified = file.lastModified();
    }

    public OutputFile(String path, File file, long lastModified)
    {
        this.path = path;
        this.file = file;
        this.lastModified = lastModified;
    }
}
