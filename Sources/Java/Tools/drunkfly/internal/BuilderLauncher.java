package drunkfly.internal;

import java.io.File;
import java.util.ArrayList;
import java.util.HashMap;

import drunkfly.Builder;
import drunkfly.Messages;

public final class BuilderLauncher
{
    private BuilderLauncher() {}

    public static void main(String[] args)
        throws ClassNotFoundException, InstantiationException, IllegalAccessException
    {
        Thread currentThread = Thread.currentThread();
        ClassLoader oldClassLoader = currentThread.getContextClassLoader();

        try {
            BuilderClassLoader classLoader = BuilderClassLoader.getInstance();
            currentThread.setContextClassLoader(classLoader);

            ArrayList<Class<?>> builders = new ArrayList<Class<?>>();
            HashMap<String, File> classFiles = new HashMap<String, File>();

            int n = args.length;
            for (int i = 0; i < n; i += 2) {
                String name = args[i * 2 + 0];
                String path = args[i * 2 + 1];

                name = name.substring(0, name.length() - 5);  // remove ".java"
                name = name.replace('/', '.');
                name = name.replace('\\', '.');

                classFiles.put(name, new File(path));

                Class<?> classInstance = classLoader.loadClass(name);
                if (Builder.class.isAssignableFrom(classInstance))
                    builders.add(classInstance);
            }

            for (Class<?> builderClass : builders) {
                // FIXME: only check files the build script actually depends on
                long lastModified = 0;
                for (File file : classFiles.values()) {
                    long fileLastModified = file.lastModified();
                    if (lastModified < fileLastModified)
                        lastModified = fileLastModified;
                }

                Builder builder = (Builder)builderClass.newInstance();
                builder.build(lastModified, false);
            }
        } catch (Throwable e) {
            e.printStackTrace(Messages.getPrintWriter());
            throw new RuntimeException(e);
        } finally {
            currentThread.setContextClassLoader(oldClassLoader);
        }
    }
}
