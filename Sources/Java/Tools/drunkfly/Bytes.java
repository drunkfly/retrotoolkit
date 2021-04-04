package drunkfly;

public final class Bytes
{
    private static final String hex = "0123456789abcdef";

    private Bytes() {}

    public static String toHex(byte[] data)
    {
        char[] result = new char[data.length * 2];
        for (int i = 0; i < data.length; i++) {
            result[i * 2 + 0] = hex.charAt((data[i] >> 4) & 0xf);
            result[i * 2 + 1] = hex.charAt(data[i] & 0xf);
        }
        return new String(result);
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
