package drunkfly;

import java.util.HashMap;

public final class ZXScreen extends ZXImage
{
    public static final int WIDTH_IN_PIXELS = 256;
    public static final int HEIGHT_IN_PIXELS = 192;
    public static final int WIDTH_IN_CHARS = WIDTH_IN_PIXELS / CHAR_WIDTH;
    public static final int HEIGHT_IN_CHARS = HEIGHT_IN_PIXELS / CHAR_HEIGHT;

    public ZXScreen()
    {
        super(WIDTH_IN_PIXELS, HEIGHT_IN_PIXELS);
    }

    public @Override int pixelOffset(int x, int y)
    {
        return pixelDataOffset(x, y);
    }

    public static int pixelDataOffset(int x, int y)
    {
        if (x < 0 || y < 0 || x >= WIDTH_IN_PIXELS || y >= HEIGHT_IN_PIXELS)
            throw new RuntimeException("Pixel coordinates (\"" + x + ", " + y + "\") are out of range.");

        int addr1 = (y & 7) << 8;
        int addr2 = (y & 0x38) << 2;
        int addr3 = (y & 0xc0) << 5;
        int addr4 = (x / CHAR_WIDTH) & 0x1f;
        return (addr1 | addr2 | addr3 | addr4);
    }
}
