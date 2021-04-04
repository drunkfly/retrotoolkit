package drunkfly;

public final class RGB
{
    public static final int RedMask = 0x00ff0000;
    public static final int GreenMask = 0x0000ff00;
    public static final int BlueMask = 0x000000ff;
    public static final int AlphaMask = 0xff000000;

    public static final int RedShift = 16;
    public static final int GreenShift = 8;
    public static final int BlueShift = 0;
    public static final int AlphaShift = 24;

    public static final int Transparent = RGB.make(0, 0, 0, 0);
    public static final int Black = RGB.make(0, 0, 0);

    private RGB() {}

    public static int make(int r, int g, int b)
    {
        return ((r & 0xff) << RedShift)
             | ((g & 0xff) << GreenShift)
             | ((b & 0xff) << BlueShift)
             | AlphaMask;
    }

    public static int make(int r, int g, int b, int a)
    {
        return ((r & 0xff) << RedShift)
             | ((g & 0xff) << GreenShift)
             | ((b & 0xff) << BlueShift)
             | ((a & 0xff) << AlphaShift);
    }

    public static int getR(int color)
    {
        return (color & RedMask) >>> RedShift;
    }

    public static int getG(int color)
    {
        return (color & GreenMask) >>> GreenShift;
    }

    public static int getB(int color)
    {
        return (color & BlueMask) >>> BlueShift;
    }

    public static int getA(int color)
    {
        return (color & AlphaMask) >>> AlphaShift;
    }

    public static int setR(int color, int r)
    {
        color &= ~RedMask;
        color |= (r & 0xff) << RedShift;
        return color;
    }

    public static int setG(int color, int g)
    {
        color &= ~GreenMask;
        color |= (g & 0xff) << GreenShift;
        return color;
    }

    public static int setB(int color, int b)
    {
        color &= ~BlueMask;
        color |= (b & 0xff) << BlueShift;
        return color;
    }

    public static int setA(int color, int a)
    {
        color &= ~AlphaMask;
        color |= (a & 0xff) << AlphaShift;
        return color;
    }

    public static int distance(int color1, int color2)
    {
        int r = RGB.getR(color2) - RGB.getR(color1);
        int g = RGB.getG(color2) - RGB.getG(color1);
        int b = RGB.getB(color2) - RGB.getB(color1);
        return r * r + g * g + b * b;
    }

    public static int nearest(int[] colors, int color)
    {
        int nearestIndex = 0;
        int nearestDistance = distance(colors[0], color);
        int nearestAlpha = RGB.getA(colors[0]) - RGB.getA(color);

        int n = colors.length;
        for (int i = 1; i < n; i++) {
            int dist = distance(colors[i], color);
            int alpha = RGB.getA(colors[i]);
            if (dist < nearestDistance || (dist == nearestDistance && alpha < nearestAlpha)) {
                nearestIndex = i;
                nearestDistance = dist;
                nearestAlpha = alpha;
            }
        }

        return nearestIndex;
    }
}
