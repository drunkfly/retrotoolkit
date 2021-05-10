package drunkfly;

import drunkfly.jni.AccessibleWithJNI;
import drunkfly.jni.CallableWithJNI;
import java.io.File;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

public final class Tileset
{
    @AccessibleWithJNI private String imagePath;
    @AccessibleWithJNI private int tileWidth;
    @AccessibleWithJNI private int tileHeight;
    @AccessibleWithJNI private int tileCount;
    @AccessibleWithJNI private int columnCount;
    private final ArrayList<Tile> tiles = new ArrayList<Tile>();

    public Tileset(File file)
    {
        loadXml(file.getAbsolutePath());
    }

    public Tileset(String imagePath, int tileWidth, int tileHeight, int tileCount, int columnCount)
    {
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

    @CallableWithJNI
    public void addTile(int x, int y, String id)
    {
        tiles.add(new Tile(this, x, y, id));
    }

    public List<Tile> getTiles()
    {
        return Collections.unmodifiableList(tiles);
    }
}
