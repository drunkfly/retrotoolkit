package drunkfly.internal;

import java.io.File;
import java.io.IOException;
import java.net.MalformedURLException;
import java.net.URL;
import java.net.URLClassLoader;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Enumeration;

import drunkfly.IO;

public final class BuilderClassLoader extends URLClassLoader
{
    private final ArrayList<File> classDirectories;
    private final ArrayList<File> resourceDirectories;
    private final File outputDirectory;

    public BuilderClassLoader(String[] classpaths) throws MalformedURLException
    {
        super(makeURLs(classpaths));

        classDirectories = new ArrayList<File>();
        resourceDirectories = new ArrayList<File>();

        File outputDir = null;
        if (classpaths != null) {
            for (String classpath : classpaths) {
                if (classpath.endsWith(".jar"))
                    continue;

                if (classpath.endsWith("/*.class") || classpath.endsWith("\\*.class"))
                    classDirectories.add(new File(classpath.substring(0, classpath.length() - 8)));
                else if (classpath.endsWith("/!*.class") || classpath.endsWith("\\!*.class"))
                    resourceDirectories.add(new File(classpath.substring(0, classpath.length() - 9)));
                else if (classpath.endsWith("/=>") || classpath.endsWith("\\=>")) {
                    if (outputDir != null)
                        throw new RuntimeException("Too many output directories.");
                    outputDir = new File(classpath.substring(0, classpath.length() - 3));
                } else {
                    File dir = new File(classpath);
                    classDirectories.add(dir);
                    resourceDirectories.add(dir);
                }
            }
        }

        outputDirectory = outputDir;
    }

    private static URL[] makeURLs(String[] classpaths) throws MalformedURLException
    {
        ArrayList<URL> result = new ArrayList<URL>();

        if (classpaths != null) {
            for (String classpath : classpaths) {
                if (classpath.endsWith(".jar"))
                    result.add(new File(classpath).toURI().toURL());
            }
        }

        return result.toArray(new URL[result.size()]);
    }

    public File getOutputDirectory()
    {
        if (outputDirectory == null)
            throw new RuntimeException("No output directory specified.");

        return outputDirectory;
    }

    public static native BuilderClassLoader getInstance();

    // Classes

    @Override public Class<?> loadClass(String name, boolean resolve) throws ClassNotFoundException
    {
        Class<?> classInstance = findLoadedClass(name);
        if (classInstance == null) {
            classInstance = findBuilderClass(name);
            if (classInstance == null)
                return super.loadClass(name, resolve);
        }

        if (resolve)
            resolveClass(classInstance);

        return classInstance;
    }

    @Override protected Class<?> findClass(String name) throws ClassNotFoundException
    {
        Class<?> builderClass = findBuilderClass(name);
        if (builderClass != null)
            return builderClass;
        else
            return super.findClass(name);
    }

    private Class<?> findBuilderClass(String name)
    {
        String classFileName = name.replace('.', '/') + ".class";
        for (File dir : classDirectories) {
            File classFile = new File(dir, classFileName);
            if (classFile.isFile()) {
                try {
                    byte[] classData = IO.loadFile(classFile);
                    return defineClass(name, classData, 0, classData.length);
                } catch (IOException e) {
                    throw new RuntimeException("Unable to load class file \"" + classFile + "\".", e);
                }
            }
        }

        return null;
    }

    // Resources

    @Override public URL getResource(String name)
    {
        return findResource(name);
    }

    @Override public Enumeration<URL> getResources(String name) throws IOException
    {
        return findResources(name);
    }

    @Override public URL findResource(String name)
    {
        try {
            File resourceFile = findResourceFile(name);
            return (resourceFile != null ? resourceFile.toURI().toURL() : null);
        } catch (MalformedURLException e) {
            throw new RuntimeException(e);
        }
    }

    @Override public Enumeration<URL> findResources(String name) throws IOException
    {
        ArrayList<URL> resources = new ArrayList<URL>();
        for (File dir : resourceDirectories) {
            File resourceFile = new File(dir, name);
            if (resourceFile.isFile())
                resources.add(resourceFile.toURI().toURL());
        }

        return Collections.enumeration(resources);
    }

    public File findResourceFile(String name)
    {
        for (File dir : resourceDirectories) {
            File resourceFile = new File(dir, name);
            if (resourceFile.isFile())
                return resourceFile;
        }
        return null;
    }

    // Libraries

    @Override protected String findLibrary(String name)
    {
        throw new RuntimeException("BuilderClassLoader.findLibrary is not implemented.");
    }
}