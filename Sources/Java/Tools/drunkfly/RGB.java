package drunkfly;

public final class RGB
{
    public static final int RED_MASK = 0x00ff0000;
    public static final int GREEN_MASK = 0x0000ff00;
    public static final int BLUE_MASK = 0x000000ff;
    public static final int ALPHA_MASK = 0xff000000;

    public static final int RED_SHIFT = 16;
    public static final int GREEN_SHIFT = 8;
    public static final int BLUE_SHIFT = 0;
    public static final int ALPHA_SHIFT = 24;

    public static final int TRANSPARENT = RGB.make(0, 0, 0, 0);
    public static final int BLACK = RGB.make(0, 0, 0);

    private RGB() {}

    public static int make(int r, int g, int b)
    {
        return ((r & 0xff) << RED_SHIFT)
             | ((g & 0xff) << GREEN_SHIFT)
             | ((b & 0xff) << BLUE_SHIFT)
             | ALPHA_MASK;
    }

    public static int make(int r, int g, int b, int a)
    {
        return ((r & 0xff) << RED_SHIFT)
             | ((g & 0xff) << GREEN_SHIFT)
             | ((b & 0xff) << BLUE_SHIFT)
             | ((a & 0xff) << ALPHA_SHIFT);
    }

    public static int getR(int color)
    {
        return (color & RED_MASK) >>> RED_SHIFT;
    }

    public static int getG(int color)
    {
        return (color & GREEN_MASK) >>> GREEN_SHIFT;
    }

    public static int getB(int color)
    {
        return (color & BLUE_MASK) >>> BLUE_SHIFT;
    }

    public static int getA(int color)
    {
        return (color & ALPHA_MASK) >>> ALPHA_SHIFT;
    }

    public static float getFloatR(int color)
    {
        return (float)((color & RED_MASK) >>> RED_SHIFT) / 255.0f;
    }

    public static float getFloatG(int color)
    {
        return (float)((color & GREEN_MASK) >>> GREEN_SHIFT) / 255.0f;
    }

    public static float getFloatB(int color)
    {
        return (float)((color & BLUE_MASK) >>> BLUE_SHIFT) / 255.0f;
    }

    public static float getFloatA(int color)
    {
        return (float)((color & ALPHA_MASK) >>> ALPHA_SHIFT) / 255.0f;
    }

    public static int setR(int color, int r)
    {
        color &= ~RED_MASK;
        color |= (r & 0xff) << RED_SHIFT;
        return color;
    }

    public static int setG(int color, int g)
    {
        color &= ~GREEN_MASK;
        color |= (g & 0xff) << GREEN_SHIFT;
        return color;
    }

    public static int setB(int color, int b)
    {
        color &= ~BLUE_MASK;
        color |= (b & 0xff) << BLUE_SHIFT;
        return color;
    }

    public static int setA(int color, int a)
    {
        color &= ~ALPHA_MASK;
        color |= (a & 0xff) << ALPHA_SHIFT;
        return color;
    }

    public static int setFloatR(int color, float r)
    {
        int value = (int)(r * 255.0f);
        return setR(color, value);
    }

    public static int setFloatG(int color, float g)
    {
        int value = (int)(g * 255.0f);
        return setG(color, value);
    }

    public static int setFloatB(int color, float b)
    {
        int value = (int)(b * 255.0f);
        return setB(color, value);
    }

    public static int setFloatA(int color, float a)
    {
        int value = (int)(a * 255.0f);
        return setA(color, value);
    }

    public static float getFloatIntensity(int color)
    {
        float r = getFloatR(color);
        float g = getFloatG(color);
        float b = getFloatB(color);
        return Mathf.clamp01(r * 0.2989f + g * 0.5870f + b * 0.1140f);
    }

    public static float getFloatPremultipliedIntensity(int color)
    {
        return getFloatIntensity(color) * getFloatA(color);
    }

    public static int getIntensity(int color)
    {
        return (int)(getFloatIntensity(color) * 255.0f);
    }

    public static int getPremultipliedIntensity(int color)
    {
        return (int)(getFloatPremultipliedIntensity(color) * 255.0f);
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
