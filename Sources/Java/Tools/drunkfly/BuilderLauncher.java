package drunkfly;

final class BuilderLauncher
{
    private BuilderLauncher() {}

    public static void main(String[] args)
        throws ClassNotFoundException, InstantiationException, IllegalAccessException
    {
        BuilderClassLoader classLoader = BuilderClassLoader.getInstance();

        Thread currentThread = Thread.currentThread();
        ClassLoader oldClassLoader = currentThread.getContextClassLoader();

        try {
            currentThread.setContextClassLoader(classLoader);

            for (String name : args) {
                name = name.substring(0, name.length() - 5);  // remove ".java"
                name = name.replace('/', '.');
                name = name.replace('\\', '.');

                Class<?> classInstance = classLoader.loadClass(name);
                if (!Builder.class.isAssignableFrom(classInstance))
                    continue;

                Builder builder = (Builder)classInstance.newInstance();
                builder.build(false);
            }
        } finally {
            currentThread.setContextClassLoader(oldClassLoader);
        }
    }
}
