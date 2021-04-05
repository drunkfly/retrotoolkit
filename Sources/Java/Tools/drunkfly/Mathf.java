package drunkfly;

public final class Mathf
{
    private Mathf() {}

    public static float clamp(float x, float min, float max)
    {
        if (x < min)
            x = min;
        if (x > max)
            x = max;
        return x;
    }

    public static float clamp01(float x)
    {
        if (x < 0.0f)
            x = 0.0f;
        if (x > 1.0f)
            x = 1.0f;
        return x;
    }
}
