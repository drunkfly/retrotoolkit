package drunkfly;

import java.io.File;

public final class Util
{
    public static String identifierFromFileName(String fileName)
    {
        return identifierFromFileName(new File(fileName));
    }

    public static String identifierFromFileName(File file)
    {
        String fileName = IO.getBaseName(file);

        int n = fileName.length();
        if (n == 0)
            return "_";

        StringBuilder builder = new StringBuilder();
        if (fileName.charAt(0) >= '0' && fileName.charAt(0) <= '9')
            builder.append('_');

        for (int i = 0; i < n; i++) {
            char ch = fileName.charAt(i);
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
