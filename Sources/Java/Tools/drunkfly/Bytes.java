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
}
