package drunkfly;

import java.io.IOException;
import java.io.Writer;

public class Output extends Writer
{
    public static native void print(String message);

    @Override public void write(char chars[], int offset, int length) throws IOException
    {
        print(new String(chars, offset, length));
    }

    @Override public void flush() throws IOException
    {
    }

    @Override public void close() throws IOException
    {
    }
}
