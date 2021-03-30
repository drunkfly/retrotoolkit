package drunkfly;

import java.io.IOException;
import java.io.PrintWriter;
import java.io.Writer;

public final class Messages extends Writer
{
    public static native void print(String message);
    public static void println(String message) { print(message + "\n"); }

    public static native Messages getInstance();
    public static native PrintWriter getPrintWriter();

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
