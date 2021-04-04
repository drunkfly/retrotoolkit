package java.lang;

public abstract class Enum<E extends Enum<E>>
{
    protected Enum(String name, int ordinal) {}
    public final native String name();
    public final native int ordinal();
    @Override public native String toString();
    @Override public native final boolean equals(Object other);
    public static native <T extends Enum<T>> T valueOf(Class<T> enumType, String name);
}
