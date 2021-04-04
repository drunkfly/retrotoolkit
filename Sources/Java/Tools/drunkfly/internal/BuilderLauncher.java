package drunkfly.internal;

import java.io.File;
import java.util.ArrayList;
import java.util.HashSet;

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
            SourceFiles sourceFiles = new SourceFiles();

            long jarLastModified = classLoader.getJarFilesLastModificationTime();

            int n = args.length;
            for (int i = 0; i < n; i += 2) {
                String name = args[i + 0];
                String path = args[i + 1];

                name = name.substring(0, name.length() - 5);  // remove ".java"
                name = name.replace('/', '.');
                name = name.replace('\\', '.');

                sourceFiles.add(name, new File(path));

                Class<?> classInstance = classLoader.loadClass(name);
                if (Builder.class.isAssignableFrom(classInstance))
                    builders.add(classInstance);
            }

            for (Class<?> builderClass : builders) {
                long lastModified = -1;
                HashSet<File> sources = sourceFiles.forClass(builderClass);
                for (File file : sources) {
                    long fileLastModified = file.lastModified();
                    if (lastModified < fileLastModified)
                        lastModified = fileLastModified;
                }

                if (lastModified < jarLastModified)
                    lastModified = jarLastModified;

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
