package drunkfly;

import drunkfly.jni.AccessibleWithJNI;
import drunkfly.jni.CallableWithJNI;
import java.io.File;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

public final class Tileset
{
    private final File file;
    @AccessibleWithJNI private String imagePath;
    @AccessibleWithJNI private int tileWidth;
    @AccessibleWithJNI private int tileHeight;
    @AccessibleWithJNI private int tileCount;
    @AccessibleWithJNI private int columnCount;
    private final ArrayList<Tile> tiles = new ArrayList<Tile>();

    public Tileset(File file)
    {
        this.file = file;
        loadXml(file.getAbsolutePath());
    }

    @CallableWithJNI
    public Tileset(String filePath)
    {
        this(new File(filePath));
    }

    public Tileset(String imagePath, int tileWidth, int tileHeight, int tileCount, int columnCount)
    {
        this.file = new File(imagePath);
        this.imagePath = imagePath;
        this.tileWidth = tileWidth;
        this.tileHeight = tileHeight;
        this.tileCount = tileCount;
        this.columnCount = columnCount;
    }

    private native void loadXml(String fileName);

    public int getTileWidth()
    {
        return tileWidth;
    }

    public int getTileHeight()
    {
        return tileHeight;
    }

    public int getTileCount()
    {
        return tileCount;
    }

    public int getColumnCount()
    {
        return columnCount;
    }

    public List<Tile> getTiles()
    {
        return Collections.unmodifiableList(tiles);
    }

    @CallableWithJNI
    public Tile getTile(int index)
    {
        if (index < 0 || index >= tiles.size())
            return null;
        return tiles.get(index);
    }

    @CallableWithJNI
    public Tile getTile(int x, int y)
    {
        int index = y * columnCount + x;
        if (index < 0 || index >= tiles.size())
            return null;
        return tiles.get(index);
    }

    public Tile getTile(String id)
    {
        for (Tile tile : tiles) {
            if (tile == null)
                continue;

            String tileId = tile.getId();
            if (tileId != null && tileId.equals(id))
                return tile;
        }

        return null;
    }

    public Tile getRequiredTile(String id)
    {
        Tile tile = getTile(id);
        if (tile == null)
            throw new RuntimeException("Missing tile \"" + id + "\" from tileset \"" + file + "\".");
        return tile;
    }

    @CallableWithJNI
    public Tile setTile(int index, String id)
    {
        if (index < 0 || index >= tileCount)
            throw new RuntimeException("Invalid tile index (" + index + ") in tileset \"" + file + "\".");

        int x = index % columnCount;
        int y = index / columnCount;
        return setTile(x, y, id);
    }

    @CallableWithJNI
    public Tile setTile(int x, int y, String id)
    {
        Tile tile = new Tile(this, x, y, id);
        int index = tile.getIndex();

        if (x < 0 || y < 0 || x >= columnCount || index >= tileCount)
            throw new RuntimeException("Invalid tile coordinate (" + x + ", " + y + ") in tileset \"" + file + "\".");

        while (index >= tiles.size())
            tiles.add(null);

        tiles.set(index, tile);
        return tile;
    }
}
