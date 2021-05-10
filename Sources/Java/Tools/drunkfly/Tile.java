package drunkfly;

public final class Tile
{
    private int x;
    private int y;
    private Tileset tileset;
    private String id;

    public Tile(Tileset tileset, int x, int y, String id)
    {
        this.x = x;
        this.y = y;
        this.tileset = tileset;
        this.id = id;
    }

    public Tileset getTileset()
    {
        return tileset;
    }

    public int getX()
    {
        return x;
    }

    public int getY()
    {
        return y;
    }

    public int getIndex()
    {
        return y * tileset.getColumnCount() + x;
    }

    public String getId()
    {
        return id;
    }
}
