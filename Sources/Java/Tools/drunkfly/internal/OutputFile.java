package drunkfly.internal;

import java.io.File;

public final class OutputFile
{
    public final String path;
    public final File file;

    public OutputFile(String path, File file)
    {
        this.path = path;
        this.file = file;
    }
}
