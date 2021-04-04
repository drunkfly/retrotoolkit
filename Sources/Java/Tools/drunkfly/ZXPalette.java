package drunkfly;

public final class ZXPalette
{
    public static final int Black = 0;
    public static final int Blue = 1;
    public static final int Red = 2;
    public static final int Magenta = 3;
    public static final int Green = 4;
    public static final int Cyan = 5;
    public static final int Yellow = 6;
    public static final int White = 7;

    public static final int Bright = 0x40;
    public static final int Flash = 0x80;

    public static final int InkShift = 0;
    public static final int InkMask = 7;
    public static final int PaperShift = 3;
    public static final int PaperMask = 0x38;

    public static final int[] Colors = {
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
        int result = (ink << InkShift) & InkMask;
        if (bright)
            result |= Bright;
        return result;
    }

    public static int paper(int paper, boolean bright)
    {
        int result = (paper << PaperShift) & PaperMask;
        if (bright)
            result |= Bright;
        return result;
    }

    public static int attrib(int ink, int paper, boolean bright)
    {
        return attrib(ink, paper, bright, false);
    }

    public static int attrib(int ink, int paper, boolean bright, boolean flash)
    {
        ink = (ink << InkShift) & InkMask;
        paper = (paper << PaperShift) & PaperMask;
        if (bright)
            ink |= Bright;
        if (flash)
            ink |= Flash;
        return ink | paper;
    }
}
