package drunkfly;

public final class Bytes
{
    private static final String HEX = "0123456789abcdef";

    private Bytes() {}

    public static boolean isZero(byte[] buffer)
    {
        int n = buffer.length;
        for (int i = 0; i < n; i++) {
            if (buffer[i] != 0)
                return false;
        }
        return true;
    }

    public static boolean equals(byte[] buffer, int a, int b, int c, int d, int e, int f, int g, int h)
    {
        return (buffer.length == 8
             && ((int)buffer[0] & 0xff) == a
             && ((int)buffer[1] & 0xff) == b
             && ((int)buffer[2] & 0xff) == c
             && ((int)buffer[3] & 0xff) == d
             && ((int)buffer[4] & 0xff) == e
             && ((int)buffer[5] & 0xff) == f
             && ((int)buffer[6] & 0xff) == g
             && ((int)buffer[7] & 0xff) == h
             );
    }

    public static String toHex(byte[] data)
    {
        char[] result = new char[data.length * 2];
        for (int i = 0; i < data.length; i++) {
            result[i * 2 + 0] = HEX.charAt((data[i] >>> 4) & 0xf);
            result[i * 2 + 1] = HEX.charAt(data[i] & 0xf);
        }
        return new String(result);
    }

    public static void appendHex(StringBuilder builder, int value)
    {
        builder.append(HEX.charAt((value >>> 4) & 0xf));
        builder.append(HEX.charAt(value & 0xf));
    }

    public static void appendHexWithPrefix(StringBuilder builder, int value)
    {
        builder.append("0x");
        builder.append(HEX.charAt((value >>> 4) & 0xf));
        builder.append(HEX.charAt(value & 0xf));
    }

    public static void toAssembler(StringBuilder builder, byte[] data)
    {
        toAssembler(builder, data, 120);
    }

    public static void toAssembler(StringBuilder builder, byte[] data, int maxLen)
    {
        int n = data.length;
        if (n == 0)
            return;

        int l = 3;
        boolean first = true;
        builder.append("db ");

        for (int i = 0; i < n; i++) {
            int value = (int)data[i] & 0xff;

            int len;
            if (value > 99) len = 3;
            else if (value > 9) len = 2;
            else len = 1;

            if (!first)
                ++len; // comma

            if (l + len > maxLen) {
                first = true;
                l = 3;
                --len; // no comma
                builder.append("\ndb ");
            }

            if (!first)
                builder.append(',');
            builder.append(value);

            l += len;
            first = false;
        }

        builder.append('\n');
    }
}
