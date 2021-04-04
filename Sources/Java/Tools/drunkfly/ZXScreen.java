package drunkfly;

import java.util.HashMap;

public final class ZXScreen
{
    public static final int CharWidth = 8;
    public static final int CharHeight = 8;

    public static final int PixelWidth = 256;
    public static final int PixelHeight = 192;
    public static final int ByteWidth = PixelWidth / CharWidth;

    public static final int AttribWidth = PixelWidth / CharWidth;
    public static final int AttribHeight = PixelHeight / CharHeight;

    public static final int PixelAreaOffset = 0;
    public static final int PixelAreaSize = ByteWidth * PixelHeight;
    public static final int AttribAreaOffset = PixelAreaSize;
    public static final int AttribAreaSize = AttribWidth * AttribHeight;
    public static final int SizeInBytes = PixelAreaSize + AttribAreaSize;

    final byte[] pixels;
    final byte[] attribs;

    public ZXScreen()
    {
        pixels = new byte[ByteWidth * PixelHeight];
        attribs = new byte[AttribWidth * AttribHeight];
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

    public byte[] getBytes()
    {
        byte[] result = new byte[SizeInBytes];
        System.arraycopy(pixels, 0, result, PixelAreaOffset, PixelAreaSize);
        System.arraycopy(attribs, 0, result, AttribAreaOffset, AttribAreaSize);
        return result;
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

        for (int y = 0; y < srcH; y++) {
            for (int x = 0; x < srcW; x++) {
                int dx = dstX + x;
                int dy = dstY + y;
                if (dx < 0 || dy < 0 || dx >= PixelWidth || dy >= PixelHeight)
                    continue;

                int off = attribOffset(dx, dy);

                int rgb = image.getPixel(x, y);
                int color = RGB.nearest(ZXPalette.Colors, rgb);
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
                if (dx < 0 || dy < 0 || dx >= PixelWidth || dy >= PixelHeight)
                    continue;

                int off = attribOffset(dx, dy);
                Integer c1 = color1.get(off);
                Integer c2 = color2.get(off);

                if (attrib.get(off) == null) {
                    int attr;
                    if (c1 == null && c2 == null) {
                        c1 = ZXPalette.White;
                        c2 = ZXPalette.Black;
                        color1.put(off, c1);
                        color2.put(off, c2);
                        attr = ZXPalette.attrib(ZXPalette.White, ZXPalette.Black, false);
                    } else if (c2 == null) {
                        int cc1 = c1;
                        boolean bright = cc1 > 7;
                        c2 = (bright ? ZXPalette.Black + 8 : ZXPalette.Black);
                        color2.put(off, c2);
                        attr = ZXPalette.attrib(cc1, ZXPalette.Black, bright);
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
                            if (!bright2 && cc2 == ZXPalette.Black)
                                bright2 = true;
                            else if (cc1 == ZXPalette.Black + 8)
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

    public static int pixelOffset(int x, int y)
    {
        if (x < 0 || y < 0 || x >= PixelWidth || y >= PixelHeight)
            throw new RuntimeException("Pixel coordinates (\"" + x + ", " + y + "\") are out of range.");

        int addr1 = (y & 7) << 8;
        int addr2 = (y & 0x38) << 2;
        int addr3 = (y & 0xc0) << 5;
        int addr4 = (x >> 3) & 0x1f;
        return (addr1 | addr2 | addr3 | addr4);
    }

    public static int pixelMask(int x)
    {
        return 1 << (7 - (x & 7));
    }

    public static int attribOffset(int x, int y)
    {
        if (x < 0 || y < 0 || x >= PixelWidth || y >= PixelHeight)
            throw new RuntimeException("Attribute coordinates (\"" + x + ", " + y + "\") are out of range.");

        x /= CharWidth;
        y /= CharHeight;
        return y * AttribWidth + x;
    }
}
