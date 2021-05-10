package drunkfly;

import java.io.File;
import java.util.ArrayList;

public final class Tilemap
{
    public final class Layer
    {
        private String name;

        Layer(String name)
        {
            this.name = name;
        }

        public String getName()
        {
            return name;
        }

        /*
        public Tile getTile(int x, int y)
        {
        }
        */
    }

    private int width;
    private int height;
    private final ArrayList<Layer> layers = new ArrayList<Layer>();

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
}
