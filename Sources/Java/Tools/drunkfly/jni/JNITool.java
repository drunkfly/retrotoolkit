package drunkfly.jni;

import java.io.IOException;
import java.util.ArrayList;
import org.objectweb.asm.AnnotationVisitor;
import org.objectweb.asm.ClassReader;
import org.objectweb.asm.ClassVisitor;
import org.objectweb.asm.FieldVisitor;
import org.objectweb.asm.MethodVisitor;
import org.objectweb.asm.Opcodes;
import org.objectweb.asm.Type;

public final class JNITool extends ClassVisitor
{
    private static ArrayList<String> result = new ArrayList<String>();
    private static boolean hasNativeMethods = false;

    private final static class JNIFieldVisitor extends FieldVisitor
    {
        private final String name;
        private final String type;

        public JNIFieldVisitor(String name, String type)
        {
            super(Opcodes.ASM9);
            this.name = name;
            this.type = type;
        }

        @Override
        public AnnotationVisitor visitAnnotation(String desc, boolean visible)
        {
            if ("Ldrunkfly/jni/AccessibleWithJNI;".equals(desc)) {
                Type t = Type.getType(type);
                String d = type.replace(';', '@').replace('[', '!');
                result.add("field|" + d + '|' + toCxxType(t) + '|' + name + '|' + toMethodName(t));
            }
            return null;
        }
    }

    private final static class JNIMethodVisitor extends MethodVisitor
    {
        private final String name;
        private final String type;
        private final boolean isNative;

        public JNIMethodVisitor(String name, String type, boolean isNative)
        {
            super(Opcodes.ASM9);

            this.name = name;
            this.type = type;
            this.isNative = isNative;

            if (isNative) {
                Type t = Type.getType(type);
                if (t.getSort() != Type.METHOD)
                    throw new RuntimeException("Invalid method descriptor \"" + type + "\".");

                StringBuilder args = new StringBuilder();
                int i = 0;
                for (Type argType : t.getArgumentTypes()) {
                    args.append(", ");
                    args.append(toCxxType(argType));
                    args.append(" arg");
                    args.append(i);
                    ++i;
                }

                Type r = t.getReturnType();
                String d = type.replace(';', '@').replace('[', '!');
                result.add("native|" + d + '|' + name + '|' + toCxxType(r) + '|' + args);
            }
        }

        @Override
        public AnnotationVisitor visitAnnotation(String desc, boolean visible)
        {
            if ("Ldrunkfly/jni/CallableWithJNI;".equals(desc)) {
                boolean isConstructor = "<init>".equals(name);

                Type t = Type.getType(type);
                if (t.getSort() != Type.METHOD)
                    throw new RuntimeException("Invalid method descriptor \"" + type + "\".");

                StringBuilder args = new StringBuilder();
                StringBuilder call = new StringBuilder();
                int i = 0;
                for (Type argType : t.getArgumentTypes()) {
                    if (i != 0 || !isConstructor)
                        args.append(", ");
                    args.append(toCxxType(argType));
                    args.append(" arg");
                    args.append(i);

                    call.append(", ");
                    call.append("arg");
                    call.append(i);

                    ++i;
                }

                Type r = t.getReturnType();
                String d = type.replace(';', '@').replace('[', '!');
                result.add("method|" + d +
                    '|' + name + '|' + toCxxType(r, true) + '|' + args + '|' + call + '|' + toMethodName(r));
            }
            return null;
        }
    }

    private JNITool()
    {
        super(Opcodes.ASM9);
    }

    @Override
    public void visit(int version, int access, String name, String signature, String superName, String[] interfaces)
    {
    }

    @Override
    public AnnotationVisitor visitAnnotation(String desc, boolean visible)
    {
        return null;
    }

    @Override
    public FieldVisitor visitField(int access, String name, String desc, String signature, Object value)
    {
        return new JNIFieldVisitor(name, desc);
    }

    @Override
    public MethodVisitor visitMethod(int access, String name, String desc, String signature, String[] exceptions)
    {
        boolean isNative = (access & Opcodes.ACC_NATIVE) != 0;
        if (isNative)
            hasNativeMethods = true;

        return new JNIMethodVisitor(name, desc, isNative);
    }

    private static String toMethodName(Type type)
    {
        switch (type.getSort()) {
            case Type.VOID: return "Void";
            case Type.OBJECT: return "Object";
            case Type.ARRAY: return "Object";
            case Type.BOOLEAN: return "Boolean";
            case Type.BYTE: return "Byte";
            case Type.CHAR: return "Char";
            case Type.SHORT: return "Short";
            case Type.INT: return "Int";
            case Type.LONG: return "Long";
            case Type.FLOAT: return "Float";
            case Type.DOUBLE: return "Double";
        }

        throw new RuntimeException("Invalid type " + type);
    }

    private static String toCxxType(Type type)
    {
        return toCxxType(type, false);
    }

    private static String toCxxType(Type type, boolean returnValue)
    {
        switch (type.getSort()) {
            case Type.VOID: return "void";
            case Type.BOOLEAN: return "jboolean";
            case Type.CHAR: return "jchar";
            case Type.BYTE: return "jbyte";
            case Type.SHORT: return "jshort";
            case Type.INT: return "jint";
            case Type.FLOAT: return "jfloat";
            case Type.LONG: return "jlong";
            case Type.DOUBLE: return "jdouble";

            case Type.ARRAY:
                if (type.getDimensions() != 1)
                    return (returnValue ? "JNIRef" : "jobjectArray");
                switch (type.getElementType().getSort()) {
                    case Type.BOOLEAN: return (returnValue ? "JNIRef" : "jbooleanArray");
                    case Type.CHAR: return (returnValue ? "JNIRef" : "jcharArray");
                    case Type.BYTE: return (returnValue ? "JNIRef" : "jbyteArray");
                    case Type.SHORT: return (returnValue ? "JNIRef" : "jshortArray");
                    case Type.INT: return (returnValue ? "JNIRef" : "jintArray");
                    case Type.FLOAT: return (returnValue ? "JNIRef" : "jfloatArray");
                    case Type.LONG: return (returnValue ? "JNIRef" : "jlongArray");
                    case Type.DOUBLE: return (returnValue ? "JNIRef" : "jdoubleArray");
                    case Type.OBJECT: return (returnValue ? "JNIRef" : "jobjectArray");
                }
                break;

            case Type.OBJECT:
                if ("java/lang/String".equals(type.getInternalName()))
                    return (returnValue ? "JNIStringRef" : "jstring");
                else if ("java/lang/Class".equals(type.getInternalName()))
                    return (returnValue ? "JNIClassRef" : "jclass");
                else if ("java/lang/Throwable".equals(type.getInternalName()))
                    return (returnValue ? "JNIThrowableRef" : "jthrowable");
                return (returnValue ? "JNIRef" : "jobject");
        }

        throw new RuntimeException("Invalid type " + type);
    }

    public static void main(String[] args) throws IOException
    {
        ClassReader reader = new ClassReader(args[0]);
        reader.accept(new JNITool(), 0);

        if (hasNativeMethods)
            result.add("hasNative");

        if (result.size() > 0) {
            StringBuilder str = new StringBuilder();
            for (int i = 0; i < result.size(); i++) {
                if (i != 0)
                    str.append(';');
                str.append(result.get(i));
            }
            System.out.print(str.toString());
        }
    }
}
