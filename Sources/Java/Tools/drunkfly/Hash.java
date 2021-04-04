package drunkfly;

import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

public final class Hash
{
    private Hash() {}

    public static String MD5(byte[] data)
    {
        try {
            MessageDigest md5 = MessageDigest.getInstance("MD5");
            md5.update(data);
            byte[] digest = md5.digest();
            return Bytes.toHex(digest);
        } catch (NoSuchAlgorithmException e) {
            throw new RuntimeException(e);
        }
    }
}
