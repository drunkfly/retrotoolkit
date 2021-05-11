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

        public int getX()
        {
            return x;
        }

        public int getY()
        {
            return y;
        }
    }

    public final class Layer
    {
        private final int id;
        private final String name;
        private final Tile[] tiles;

        Layer(int id, String name)
        {
            this.id = id;
            this.name = name;
            tiles = new Tile[width * height];
        }

        public int getId()
        {
            return id;
        }

        public String getName()
        {
            return name;
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
            if (x < 0 || y < 0 || x >= width || y >= height)
                throw new RuntimeException("Tile coordinate is out of range.");
            tiles[y * width + x] = tile;
        }
    }

    @AccessibleWithJNI private int width;
    @AccessibleWithJNI private int height;
    private final ArrayList<Layer> layers = new ArrayList<Layer>();
    private final ArrayList<Obj> objects = new ArrayList<Obj>();

    public Tilemap(File file)
    {
        loadXml(file.getAbsolutePath());
    }

    public Tilemap(int width, int height)
    {
        this.width = width;
        this.height = height;
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

    @CallableWithJNI
    public Obj addObject(int id, String name, int x, int y)
    {
        Obj obj = new Obj(id, name, x, y);
        objects.add(obj);
        return obj;
    }

    public List<Obj> getObjects()
    {
        return Collections.unmodifiableList(objects);
    }
}
