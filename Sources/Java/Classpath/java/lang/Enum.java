package java.lang;

public abstract class Enum<E extends Enum<E>>
{
    private final String mName;
    private final int mOrdinal;

    protected Enum(String name, int ordinal)
    {
        mName = name;
        mOrdinal = ordinal;
    }

    public final String name()
    {
        return mName;
    }

    public final int ordinal()
    {
        return mOrdinal;
    }

    @Override public String toString()
    {
        return mName;
    }

    @Override public final boolean equals(Object other)
    {
        return (this == other);
    }

    public static <T extends Enum<T>> T valueOf(Class<T> enumType, String name)
    {
        // FIXME
        return null;
    }
}
