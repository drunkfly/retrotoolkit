package drunkfly;

public final class ZXPalette
{
    public static final int BLACK = 0;
    public static final int BLUE = 1;
    public static final int RED = 2;
    public static final int MAGENTA = 3;
    public static final int GREEN = 4;
    public static final int CYAN = 5;
    public static final int YELLOW = 6;
    public static final int WHITE = 7;

    public static final int BRIGHT = 0x40;
    public static final int FLASH = 0x80;

    public static final int INK_SHIFT = 0;
    public static final int INK_MASK = 7;
    public static final int PAPER_SHIFT = 3;
    public static final int PAPER_MASK = 0x38;

    public static final int[] COLORS = {
            RGB.make(0x00, 0x00, 0x00),
            RGB.make(0x00, 0x00, 0xc0),
            RGB.make(0xc0, 0x00, 0x00),
            RGB.make(0xc0, 0x00, 0xc0),
            RGB.make(0x00, 0xc0, 0x00),
            RGB.make(0x00, 0xc0, 0xc0),
            RGB.make(0xc0, 0xc0, 0x00),
            RGB.make(0xc0, 0xc0, 0xc0),
            RGB.make(0x00, 0x00, 0x00),
            RGB.make(0x00, 0x00, 0xff),
            RGB.make(0xff, 0x00, 0x00),
            RGB.make(0xff, 0x00, 0xff),
            RGB.make(0x00, 0xff, 0x00),
            RGB.make(0x00, 0xff, 0xff),
            RGB.make(0xff, 0xff, 0x00),
            RGB.make(0xff, 0xff, 0xff),
        };

    public static int ink(int ink, boolean bright)
    {
        int result = (ink << INK_SHIFT) & INK_MASK;
        if (bright)
            result |= BRIGHT;
        return result;
    }

    public static int paper(int paper, boolean bright)
    {
        int result = (paper << PAPER_SHIFT) & PAPER_MASK;
        if (bright)
            result |= BRIGHT;
        return result;
    }

    public static int attrib(int ink, int paper, boolean bright)
    {
        return attrib(ink, paper, bright, false);
    }

    public static int attrib(int ink, int paper, boolean bright, boolean flash)
    {
        ink = (ink << INK_SHIFT) & INK_MASK;
        paper = (paper << PAPER_SHIFT) & PAPER_MASK;
        if (bright)
            ink |= BRIGHT;
        if (flash)
            ink |= FLASH;
        return ink | paper;
    }
}
