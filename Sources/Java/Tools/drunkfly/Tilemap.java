package drunkfly;

import drunkfly.jni.AccessibleWithJNI;
import drunkfly.jni.CallableWithJNI;
import java.io.File;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

public final class Tilemap
{
    public final class Obj
    {
        private final int id;
        private final String name;
        private final int x;
        private final int y;

        Obj(int id, String name, int x, int y)
        {
            this.id = id;
            this.name = name;
            this.x = x;
            this.y = y;
        }

        public int getId()
        {
            return id;
        }

        public String getName()
        {
            return name;
        }

        public boolean hasName(String strName)
        {
            if (strName == null)
                return name == null;
            return name != null && name.equals(strName);
        }

        public int getX()
        {
            return x;
        }

        public int getY()
        {
            return y;
        }

        public int getTileX()
        {
            return x / tileWidth;
        }

        public int getTileY()
        {
            return y / tileHeight;
        }
    }

    public final class Layer
    {
        private final int id;
        private final String name;
        private final Tile[] tiles;
        private final ArrayList<Obj>[] objects;

        @SuppressWarnings("unchecked")
        Layer(int id, String name)
        {
            this.id = id;
            this.name = name;
            tiles = new Tile[width * height];
            objects = new ArrayList[width * height];
        }

        public int getId()
        {
            return id;
        }

        public String getName()
        {
            return name;
        }

        public List<Tileset> getTilesets()
        {
            ArrayList<Tileset> result = new ArrayList<Tileset>();
            for (Tile tile : tiles) {
                if (tile != null)
                    result.add(tile.getTileset());
            }
            return result;
        }

        public Tile getTile(int x, int y)
        {
            if (x < 0 || y < 0 || x >= width || y >= height)
                return null;
            return tiles[y * width + x];
        }

        @CallableWithJNI
        public void setTile(int x, int y, Tile tile)
        {
            if (x < 0 || y < 0 || x >= width || y >= height) {
                if (file == null)
                    throw new RuntimeException("Tile coordinates are out of range.");
                else
                    throw new RuntimeException("Tile coordinates are out of range in file \"" + file + "\".");
            }
            tiles[y * width + x] = tile;
        }

        public List<Obj> getObjects(int x, int y)
        {
            if (x < 0 || y < 0 || x >= width || y >= height)
                return new ArrayList<Obj>();

            ArrayList<Obj> list = objects[y * width + x];
            if (list == null) {
                list = new ArrayList<Obj>();
                objects[y * width + x] = list;
            }

            return Collections.unmodifiableList(list);
        }

        @CallableWithJNI
        public void addObject(int x, int y, int id, String name)
        {
            Obj obj = new Obj(id, name, x, y);

            x /= tileWidth;
            y /= tileHeight;

            if (x < 0 || y < 0 || x >= width || y >= height) {
                if (file == null)
                    throw new RuntimeException("Coordinates of object \"" + name + "\" are out of range.");
                else {
                    throw new RuntimeException(
                        "Coordinates of object \"" + name + "\" are out of range in file \"" + file + "\".");
                }
            }

            ArrayList<Obj> list = objects[y * width + x];
            if (list == null) {
                list = new ArrayList<Obj>();
                objects[y * width + x] = list;
            }

            list.add(obj);
            allObjects.add(obj);
        }
    }

    private final File file;
    @AccessibleWithJNI private int width;
    @AccessibleWithJNI private int height;
    @AccessibleWithJNI private int tileWidth;
    @AccessibleWithJNI private int tileHeight;
    private final ArrayList<Layer> layers = new ArrayList<Layer>();
    private final ArrayList<Obj> allObjects = new ArrayList<Obj>();

    public Tilemap(File file)
    {
        this.file = file;
        loadXml(file.getAbsolutePath());
    }

    public Tilemap(int width, int height, int tileWidth, int tileHeight)
    {
        this.file = null;
        this.width = width;
        this.height = height;
        this.tileWidth = tileWidth;
        this.tileHeight = tileHeight;
    }

    private native void loadXml(String fileName);

    public int getWidth()
    {
        return width;
    }

    public int getHeight()
    {
        return height;
    }

    public int getTileWidth()
    {
        return tileWidth;
    }

    public int getTileHeight()
    {
        return tileHeight;
    }

    public int getLayerCount()
    {
        return layers.size();
    }

    @CallableWithJNI
    public Layer addLayer(int id, String name)
    {
        Layer layer = new Layer(id, name);
        layers.add(layer);
        return layer;
    }

    public Layer getLayer(int index)
    {
        if (index < 0 || index >= layers.size())
            return null;
        return layers.get(index);
    }

    public Layer getLayer(String name)
    {
        for (Layer layer : layers) {
            if (layer.name.equals(name))
                return layer;
        }
        return null;
    }

    public List<Obj> getAllObjects()
    {
        return Collections.unmodifiableList(allObjects);
    }
}
