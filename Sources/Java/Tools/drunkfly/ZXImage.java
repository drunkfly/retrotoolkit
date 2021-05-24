package drunkfly;

import java.util.HashMap;

public class ZXImage
{
    public static final int CHAR_WIDTH = 8;
    public static final int CHAR_HEIGHT = 8;

    private final int widthInChars;
    private final int heightInChars;
    private final byte[] pixels;
    private final byte[] attribs;

    public ZXImage(int width, int height)
    {
        widthInChars = (width + CHAR_WIDTH - 1) / CHAR_WIDTH;
        heightInChars = (height + CHAR_HEIGHT - 1) / CHAR_HEIGHT;

        pixels = new byte[widthInChars * (heightInChars * CHAR_HEIGHT)];
        attribs = new byte[widthInChars * heightInChars];
    }

    public int getWidthInChars()
    {
        return widthInChars;
    }

    public int getWidthInPixels()
    {
        return widthInChars * CHAR_WIDTH;
    }

    public int getHeightInChars()
    {
        return heightInChars;
    }

    public int getHeightInPixels()
    {
        return heightInChars * CHAR_HEIGHT;
    }

    public byte[] getPixelBytes()
    {
        return pixels;
    }

    public byte[] getAttribBytes()
    {
        return attribs;
    }

    public byte[] getBytes()
    {
        byte[] result = new byte[pixels.length + attribs.length];
        System.arraycopy(pixels, 0, result, 0, pixels.length);
        System.arraycopy(attribs, 0, result, pixels.length, attribs.length);
        return result;
    }

    public byte[] getZigzagPixelBytes()
    {
        int h = getHeightInPixels();

        byte[] result = new byte[pixels.length];
        int off = 0;

        for (int y = 0; y < h; y++) {
            if ((y & 1) == 0) {
                for (int x = 0; x < widthInChars; x++)
                    result[off++] = pixels[y * widthInChars + x];
            } else {
                for (int x = widthInChars - 1; x >= 0; x--)
                    result[off++] = pixels[y * widthInChars + x];
            }
        }

        return result;
    }

    public byte[] getZigzagAttribBytes()
    {
        byte[] result = new byte[attribs.length];
        int off = 0;

        for (int y = heightInChars - 1; y >= 0; y--) {
            if (y % (CHAR_HEIGHT * 2) == 0) {
                for (int x = 0; x < widthInChars; x++)
                    result[off++] = attribs[y * widthInChars + x];
            } else {
                for (int x = widthInChars - 1; x >= 0; x--)
                    result[off++] = attribs[y * widthInChars + x];
            }
        }

        return result;
    }

    public byte[] getZigzagBytes(int tileHeight)
    {
        byte[] result = new byte[pixels.length + attribs.length];
        System.arraycopy(getZigzagPixelBytes(), 0, result, 0, pixels.length);
        System.arraycopy(getZigzagAttribBytes(), 0, result, pixels.length, attribs.length);
        return result;
    }

    public byte[] getExtendedZigzagPixelBytes()
    {
        int h = getHeightInPixels();

        byte[] result = new byte[pixels.length];
        int off = 0;

        for (int yy = 0; yy < h; yy += CHAR_HEIGHT) {
            int yend = Math.min(yy + CHAR_HEIGHT, h);
            if (yy % (CHAR_HEIGHT *2) == 0) {
                for (int y = yy; y < yend; y++) {
                    if ((y & 1) == 0) {
                        for (int x = 0; x < widthInChars; x++)
                            result[off++] = pixels[y * widthInChars + x];
                    } else {
                        for (int x = widthInChars - 1; x >= 0; x--)
                            result[off++] = pixels[y * widthInChars + x];
                    }
                }
            } else {
                for (int y = yend - 1; y >= yy; y--) {
                    if ((y & 1) != 0) {
                        for (int x = 0; x < widthInChars; x++)
                            result[off++] = pixels[y * widthInChars + x];
                    } else {
                        for (int x = widthInChars - 1; x >= 0; x--)
                            result[off++] = pixels[y * widthInChars + x];
                    }
                }
            }
        }

        return result;
    }

    public byte[] getExtendedZigzagBytes()
    {
        byte[] result = new byte[pixels.length + attribs.length];
        System.arraycopy(getExtendedZigzagPixelBytes(), 0, result, 0, pixels.length);
        System.arraycopy(getZigzagAttribBytes(), 0, result, pixels.length, attribs.length);
        return result;
    }

    public boolean getPixel(int x, int y)
    {
        int off = pixelOffset(x, y);
        return (pixels[off] & pixelMask(x)) != 0;
    }

    public void setPixel(int x, int y, boolean value)
    {
        int off = pixelOffset(x, y);
        if (value)
            pixels[off] |= pixelMask(x);
        else
            pixels[off] &= ~pixelMask(x);
    }

    public int getAttrib(int x, int y)
    {
        return (int)attribs[attribOffset(x, y)] & 0xff;
    }

    public void setAttrib(int x, int y, int value)
    {
        attribs[attribOffset(x, y)] = (byte)value;
    }

    public void putImage(int dstX, int dstY, Gfx image)
    {
        putImage(dstX, dstY, image, image.getWidth(), image.getHeight());
    }

    public void putImage(int dstX, int dstY, Gfx image, int srcW, int srcH)
    {
        HashMap<Integer, Integer> color1 = new HashMap<Integer, Integer>();
        HashMap<Integer, Integer> color2 = new HashMap<Integer, Integer>();
        byte[] pixels = new byte[srcW * srcH];

        int w = getWidthInPixels();
        int h = getHeightInPixels();

        for (int y = 0; y < srcH; y++) {
            for (int x = 0; x < srcW; x++) {
                int dx = dstX + x;
                int dy = dstY + y;
                if (dx < 0 || dy < 0 || dx >= w || dy >= h)
                    continue;

                int off = attribOffset(dx, dy);

                int rgb = image.getPixel(x, y);
                int color = RGB.nearest(ZXPalette.COLORS, rgb);
                pixels[y * srcW + x] = (byte)color;

                Integer c1 = color1.get(off);
                if (c1 == null)
                    color1.put(off, color);
                else if (!c1.equals(color)) {
                    Integer c2 = color2.get(off);
                    if (c2 == null)
                        color2.put(off, color);
                    else if (!c2.equals(color)) {
                        Messages.println("Image \"" + image.getName() +
                            "\" has conflicting colors around (" + x + ", " + y + ").");
                    }
                }
            }
        }

        HashMap<Integer, Integer> attrib = new HashMap<Integer, Integer>();

        for (int y = 0; y < srcH; y++) {
            for (int x = 0; x < srcW; x++) {
                int dx = dstX + x;
                int dy = dstY + y;
                if (dx < 0 || dy < 0 || dx >= w || dy >= h)
                    continue;

                int off = attribOffset(dx, dy);
                Integer c1 = color1.get(off);
                Integer c2 = color2.get(off);

                if (attrib.get(off) == null) {
                    int attr;
                    if (c1 == null && c2 == null) {
                        c1 = ZXPalette.WHITE;
                        c2 = ZXPalette.BLACK;
                        color1.put(off, c1);
                        color2.put(off, c2);
                        attr = ZXPalette.attrib(ZXPalette.WHITE, ZXPalette.BLACK, false);
                    } else if (c2 == null) {
                        int cc1 = c1;
                        boolean bright = cc1 > 7;
                        c2 = (bright ? ZXPalette.BLACK + 8 : ZXPalette.BLACK);
                        color2.put(off, c2);
                        attr = ZXPalette.attrib(cc1, ZXPalette.BLACK, bright);
                    } else {
                        int cc1 = c1;
                        int cc2 = c2;

                        if (cc1 < cc2) {
                            int tmp = cc1;
                            cc1 = cc2;
                            cc2 = tmp;
                        }

                        boolean bright1 = cc1 > 7;
                        boolean bright2 = cc2 > 7;
                        if (bright1 != bright2) {
                            if (!bright2 && cc2 == ZXPalette.BLACK)
                                bright2 = true;
                            else if (cc1 == ZXPalette.BLACK + 8)
                                bright2 = false;
                            else {
                                Messages.println("Image \"" + image.getName() +
                                    "\" has conflicting brightness around (" + x + ", " + y + ").");
                            }
                        }

                        cc1 = (cc1 & 7) | (bright2 ? 8 : 0);
                        cc2 = (cc2 & 7) | (bright2 ? 8 : 0);
                        c1 = cc1;
                        c2 = cc2;

                        color1.put(off, c1);
                        color2.put(off, c2);
                        attr = ZXPalette.attrib(cc1, cc2, bright2);
                    }

                    setAttrib(dx, dy, attr);
                    attrib.put(off, attr);
                }

                int color = pixels[y * srcW + x] & 0xff;
                setPixel(dx, dy, c1.equals(color));
            }
        }
    }

    public void generatePixelsAssembly(StringBuilder builder)
    {
        Bytes.toAssembler(builder, pixels);
    }

    public void generateAttribsAssembly(StringBuilder builder)
    {
        Bytes.toAssembler(builder, attribs);
    }

    public void generateAssembly(StringBuilder builder)
    {
        builder.append("; pixels\n");
        generatePixelsAssembly(builder);
        builder.append("; attribs\n");
        generateAttribsAssembly(builder);
    }

    public void generateAssembly(StringBuilder builder, String symbol)
    {
        builder.append(symbol);
        builder.append(":\n");
        generateAssembly(builder);
    }

    public void generateAssembly(StringBuilder builder, String section, String symbol)
    {
        builder.append("#section ");
        builder.append(section);
        builder.append('\n');
        generateAssembly(builder, symbol);
    }

    public void generateAssembly(Builder builder, String path, String section, String symbol)
    {
        StringBuilder str = new StringBuilder();
        generateAssembly(str, section, symbol);
        builder.writeFile(path, str);
    }

    public int pixelOffset(int x, int y)
    {
        if (x < 0 || y < 0 || x >= getWidthInPixels() || y >= getHeightInPixels())
            throw new RuntimeException("Pixel coordinates (\"" + x + ", " + y + "\") are out of range.");

        return y * widthInChars + (x / CHAR_WIDTH);
    }

    public static int pixelMask(int x)
    {
        return 1 << (7 - (x & 7));
    }

    public int attribOffset(int x, int y)
    {
        if (x < 0 || y < 0 || x >= getWidthInPixels() || y >= getHeightInPixels())
            throw new RuntimeException("Attribute coordinates (\"" + x + ", " + y + "\") are out of range.");

        x /= CHAR_WIDTH;
        y /= CHAR_HEIGHT;
        return y * widthInChars + x;
    }
}
