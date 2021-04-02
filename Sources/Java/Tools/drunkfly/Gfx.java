package drunkfly;

import java.awt.image.BufferedImage;

public final class Gfx
{
    final BufferedImage image;

    public Gfx(int width, int height)
    {
        image = new BufferedImage(width, height, BufferedImage.TYPE_INT_ARGB);
    }

    public Gfx(BufferedImage image)
    {
        this.image = image;
    }
}
