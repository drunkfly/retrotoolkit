package drunkfly;

import java.io.File;
import java.io.UnsupportedEncodingException;

public final class Util
{
    public static boolean isNullOrEmpty(String string)
    {
        return string == null || string.length() == 0;
    }

    public static String fromUtf8(byte[] bytes)
    {
        try {
            return new String(bytes, "UTF-8");
        } catch (UnsupportedEncodingException e) {
            throw new RuntimeException("UTF-8 encoding is not supported.", e);
        }
    }

    public static byte[] toUtf8(String string)
    {
        try {
            return string.getBytes("UTF-8");
        } catch (UnsupportedEncodingException e) {
            throw new RuntimeException("UTF-8 encoding is not supported.", e);
        }
    }

    public static String identifierFromString(String string)
    {
        int n = string.length();
        if (n == 0)
            return "_";

        StringBuilder builder = new StringBuilder();
        if (string.charAt(0) >= '0' && string.charAt(0) <= '9')
            builder.append('_');

        for (int i = 0; i < n; i++) {
            char ch = string.charAt(i);
            if (ch >= 'a' && ch <= 'z')
                builder.append(ch);
            else if (ch >= 'A' && ch <= 'Z')
                builder.append(ch);
            else if (ch >= '0' && ch <= '9')
                builder.append(ch);
            else
                builder.append('_');
        }

        return builder.toString();
    }
}
