package drunkfly;

import java.io.File;

public final class Util
{
    public static boolean isNullOrEmpty(String string)
    {
        return string == null || string.length() == 0;
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
