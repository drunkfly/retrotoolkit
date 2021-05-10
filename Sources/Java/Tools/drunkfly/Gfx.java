package drunkfly;

import java.awt.image.BufferedImage;

public final class Gfx
{
    private final BufferedImage image;
    private final GfxFormat format;
    private String name;

    public Gfx(int width, int height, GfxFormat format)
    {
        this("unnamed image", width, height, format);
    }

    public Gfx(String name, int width, int height, GfxFormat format)
    {
        image = new BufferedImage(width, height, BufferedImage.TYPE_INT_ARGB);
        this.name = name;
        this.format = format;
    }

    public Gfx(BufferedImage image, GfxFormat format)
    {
        this("unnamed image", image, format);
    }

    public Gfx(String name, BufferedImage image, GfxFormat format)
    {
        this.image = image;
        this.format = format;
        this.name = name;
    }

    public String getName()
    {
        return name;
    }

    public void setName(String name)
    {
        this.name = name;
    }

    public int getWidth()
    {
        return image.getWidth();
    }

    public int getHeight()
    {
        return image.getHeight();
    }

    public GfxFormat getFormat()
    {
        return format;
    }

    public int getPixel(int x, int y)
    {
        if (x < 0 || y < 0 || x >= image.getWidth() || y >= image.getHeight()) {
            switch (format) {
                case RGB: return RGB.Black;
                case RGBA: return RGB.Transparent;
            }
            throw new RuntimeException("Invalid GFX format.");
        }

        int value = image.getRGB(x, y);
        switch (format) {
            case RGB: return value | RGB.AlphaMask;
            case RGBA: return value;
        }
        throw new RuntimeException("Invalid GFX format.");
    }

    public void setPixel(int x, int y, int value)
    {
        if (x < 0 || y < 0 || x >= image.getWidth() || y >= image.getHeight())
            return;

        switch (format) {
            case RGB: image.setRGB(x, y, value | RGB.AlphaMask); return;
            case RGBA: image.setRGB(x, y, value); return;
        }
        throw new RuntimeException("Invalid GFX format.");
    }

    public Gfx getImage(int x, int y, int w, int h)
    {
        return getImage(x, y, w, h, format);
    }

    public Gfx getImage(int x, int y, int w, int h, GfxFormat format)
    {
        String resultName;
        if (x == 0 && y == 0 && w == image.getWidth() && h == image.getHeight())
            resultName = name;
        else if (name.startsWith("subimage of "))
            resultName = name;
        else
            resultName = "subimage of " + name;

        Gfx result = new Gfx(resultName, w, h, format);
        result.putImage(0, 0, this, x, y, w, h);

        return result;
    }

    public void putImage(int dstX, int dstY, Gfx image, int srcX, int srcY)
    {
        putImage(dstX, dstY, image, srcX, srcY, image.getWidth(), image.getHeight());
    }

    public void putImage(int dstX, int dstY, Gfx image, int srcX, int srcY, int srcW, int srcH)
    {
        for (int y = 0; y < srcH; y++) {
            for (int x = 0; x < srcW; x++)
                setPixel(dstX + x, dstY + y, image.getPixel(srcX + x, srcY + y));
        }
    }

    public ZXScreen toZXScreen()
    {
        ZXScreen screen = new ZXScreen();
        screen.putImage(0, 0, this);
        return screen;
    }
}
