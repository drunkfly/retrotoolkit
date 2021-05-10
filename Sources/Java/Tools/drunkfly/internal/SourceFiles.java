package drunkfly.internal;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.util.HashMap;
import java.util.HashSet;

import org.objectweb.asm.Attribute;
import org.objectweb.asm.ClassReader;
import org.objectweb.asm.ClassVisitor;
import org.objectweb.asm.Handle;
import org.objectweb.asm.Label;
import org.objectweb.asm.Opcodes;
import org.objectweb.asm.Type;
import org.objectweb.asm.TypePath;
import org.objectweb.asm.signature.SignatureReader;
import org.objectweb.asm.signature.SignatureVisitor;
import org.objectweb.asm.util.Printer;
import org.objectweb.asm.util.Textifier;
import org.objectweb.asm.util.TraceClassVisitor;

public final class SourceFiles
{
    private final class SignatureParser extends SignatureVisitor
    {
        private final HashSet<File> result;

        public SignatureParser(HashSet<File> result) { super(Opcodes.ASM9); this.result = result; }

        @Override public void visitClassType(String name) { addClass(result, name.replace('/', '.'), true); }
        @Override public void visitInnerClassType(String name) { addClass(result, name.replace('/', '.'), true); }

        @Override public void visitBaseType(char descriptor) {}
        @Override public void visitFormalTypeParameter(String name) {}
        @Override public void visitTypeArgument() {}
        @Override public void visitTypeVariable(String name) {}
        @Override public void visitEnd() {}

        @Override public SignatureParser visitClassBound() { return this; }
        @Override public SignatureParser visitInterfaceBound() { return this; }
        @Override public SignatureParser visitSuperclass() { return this; }
        @Override public SignatureParser visitInterface() { return this; }
        @Override public SignatureParser visitParameterType() { return this; }
        @Override public SignatureParser visitReturnType() { return this; }
        @Override public SignatureParser visitExceptionType() { return this; }
        @Override public SignatureParser visitArrayType() { return this; }
        @Override public SignatureParser visitTypeArgument(char tag) { return this; }
    }

    private final class Visitor extends Printer
    {
        private final HashSet<File> result;

        public Visitor(HashSet<File> result) { super(Opcodes.ASM9); this.result = result; }

        @Override public void visitSource(String file, String debug) {}
        @Override public void visitClassAttribute(Attribute attribute) {}
        @Override public void visitFieldAttribute(Attribute attribute) {}
        @Override public void visitMethodAttribute(Attribute attribute) {}
        @Override public void visitRecordComponentAttribute(Attribute attribute) {}
        @Override public void visitPermittedSubclass(String c) { addClass(result, c.replace('/', '.'), true); }
        @Override public void visitEnum(String name, String descriptor, String value) { parseDescriptor(descriptor); }
        @Override public void visitMainClass(String name) { addClass(result, name.replace('/', '.'), true); }
        @Override public void visitNestHost(String name) { addClass(result, name.replace('/', '.'), true); }
        @Override public void visitNestMember(String name) { addClass(result, name.replace('/', '.'), true); }
        @Override public void visitUse(String name) { addClass(result, name.replace('/', '.'), true); }
        @Override public void visitRequire(String require, int access, String version) {}
        @Override public void visitExport(String packaze, int access, String... modules) {}
        @Override public void visitOpen(String packaze, int access, String... modules) {}
        @Override public void visitPackage(String packaze) {}
        @Override public void visitParameter(String name, int access) {}
        @Override public void visitAnnotationEnd() {}
        @Override public void visitRecordComponentEnd() {}
        @Override public void visitFieldEnd() {}
        @Override public void visitClassEnd() {}
        @Override public void visitModuleEnd() {}
        @Override public void visitCode() {}

        @Override public void visitInsn(int opcode) {}
        @Override public void visitIntInsn(int opcode, int operand) {}
        @Override public void visitVarInsn(int opcode, int var) {}
        @Override public void visitJumpInsn(int opcode, Label label) {}
        @Override public void visitLabel(Label label) {}
        @Override public void visitIincInsn(int var, int increment) {}
        @Override public void visitTableSwitchInsn(int min, int max, Label dflt, Label... labels) {}
        @Override public void visitLookupSwitchInsn(Label dflt, int[] keys, Label[] labels) {}
        @Override public void visitMultiANewArrayInsn(String descriptor, int numDimensions) {}
        @Override public void visitLineNumber(int line, Label start) {}
        @Override public void visitMaxs(int maxStack, int maxLocals) {}
        @Override public void visitMethodEnd() {}

        @Override public Printer visitArray(String name) { return this; }
        @Override public Printer visitAnnotation(String name, String desc) { parseDescriptor(desc); return this; }
        @Override public Printer visitModule(String name, int access, String version) { return this; }
        @Override public Printer visitClassAnnotation(String d, boolean v) { parseDescriptor(d); return this; }
        @Override public Printer visitRecordComponentAnnotation(String d, boolean v) { parseDescriptor(d); return this; }
        @Override public Printer visitFieldAnnotation(String d, boolean v) { parseDescriptor(d); return this; }
        @Override public Printer visitMethodAnnotation(String d, boolean v) { parseDescriptor(d); return this; }
        @Override public Printer visitAnnotableParameterCount(int parameterCount, boolean visible) { return this; }
        @Override public Printer visitAnnotationDefault() { return this; }

        @Override public void visitTypeInsn(int opcode, String t) { addClass(result, t.replace('/', '.'), true); }
        @Override public void visitTryCatchBlock(Label start, Label end, Label handler, String type)
            { addClass(result, type.replace('/', '.'), true); }

        @Override public Printer visitClassTypeAnnotation(
                int typeRef, TypePath typePath, String descriptor, boolean visible)
            { parseDescriptor(descriptor); return this; }

        @Override public Printer visitRecordComponentTypeAnnotation(
                int typeRef, TypePath typePath, String descriptor, boolean visible)
            { parseDescriptor(descriptor); return this; }

        @Override public Printer visitFieldTypeAnnotation(
                int typeRef, TypePath typePath, String descriptor, boolean visible)
            { parseDescriptor(descriptor); return this; }

        @Override public Printer visitMethodTypeAnnotation(
                int typeRef, TypePath typePath, String descriptor, boolean visible)
            { parseDescriptor(descriptor); return this; }

        @Override public Printer visitInsnAnnotation(
                int typeRef, TypePath typePath, String descriptor, boolean visible)
            { parseDescriptor(descriptor); return this; }

        @Override public Printer visitTryCatchAnnotation(
                int typeRef, TypePath typePath, String descriptor, boolean visible)
            { parseDescriptor(descriptor); return this; }

        @Override public Printer visitLocalVariableAnnotation(int typeRef, TypePath typePath,
                Label[] start, Label[] end, int[] index, String descriptor, boolean visible)
            { parseDescriptor(descriptor); return this; }

        @Override public Printer visitParameterAnnotation(int parameter, String descriptor, boolean visible)
        {
            parseDescriptor(descriptor);
            return this;
        }

        @Override public void visitOuterClass(String owner, String name, String descriptor)
        {
            addClass(result, owner.replace('/', '.'), true);
            parseDescriptor(descriptor);
        }

        @Override public void visit(String name, Object value)
        {
            if (value instanceof Type)
                parseType((Type)value);
        }

        @Override public void visitFieldInsn(int opcode, String owner, String name, String descriptor)
        {
            addClass(result, owner.replace('/', '.'), true);
            parseDescriptor(descriptor);
        }

        @Override public void visitMethodInsn(int opcode, String owner, String name, String desc, boolean isInterface)
        {
            addClass(result, owner.replace('/', '.'), true);
            parseDescriptor(desc);
        }

        @Override public void visitLdcInsn(Object value)
        {
            if (value instanceof Type)
                parseType((Type)value);
        }

        @Override public void visitLocalVariable(String name, String desc, String sign, Label from, Label to, int idx)
        {
            parseDescriptor(desc);
            parseSignature(sign);
        }

        @Override public void visitInnerClass(String name, String outerName, String innerName, int access)
        {
            addClass(result, name.replace('/', '.'), true);
            addClass(result, outerName.replace('/', '.'), true);
            addClass(result, innerName.replace('/', '.'), true);
        }

        @Override public Printer visitRecordComponent(String name, String descriptor, String signature)
        {
            parseSignature(signature);
            parseDescriptor(descriptor);
            return this;
        }

        @Override public Printer visitField(int access, String name, String descriptor, String signature, Object value)
        {
            parseSignature(signature);
            parseDescriptor(descriptor);
            return this;
        }

        @Override public void visitProvide(String provide, String... providers)
        {
            addClass(result, provide.replace('/', '.'), true);
            for (String provider : providers)
                addClass(result, provider.replace('/', '.'), true);
        }

        @Override public void visit(int ver, int acc, String name, String signature, String superName, String[] ifaces)
        {
            parseSignature(signature);
            addClass(result, superName.replace('/', '.'), true);
            for (String iface : ifaces)
                addClass(result, iface.replace('/', '.'), true);
        }

        @Override public Printer visitMethod(int acc, String name, String descriptor, String signature, String[] exc)
        {
            parseSignature(signature);
            parseDescriptor(descriptor);
            if (exc != null) {
                for (String exception : exc)
                    addClass(result, exception.replace('/', '.'), true);
            }
            return this;
        }

        @Override public void visitInvokeDynamicInsn(String name, String desc, Handle bootHandle, Object... bootArgs)
        {
            parseDescriptor(desc);
            parseHandle(bootHandle);
            for (Object value : bootArgs) {
                if (value instanceof Handle)
                    parseHandle((Handle)value);
                else if (value instanceof Type)
                    parseType((Type)value);
            }
        }

        @Override public void visitFrame(int type, int numLocal, Object[] local, int numStack, Object[] stack)
        {
            switch (type) {
                case Opcodes.F_NEW:
                case Opcodes.F_FULL:
                    parseFrameTypes(numLocal, local);
                    parseFrameTypes(numStack, stack);
                    break;
                case Opcodes.F_APPEND:
                    parseFrameTypes(numLocal, local);
                    break;
                case Opcodes.F_CHOP:
                case Opcodes.F_SAME:
                    break;
                case Opcodes.F_SAME1:
                    parseFrameTypes(1, stack);
                    break;
                default:
                    throw new IllegalArgumentException();
            }
        }

        private void parseHandle(Handle handle)
        {
            addClass(result, handle.getOwner().replace('/', '.'), true);
            parseDescriptor(handle.getDesc());
        }

        private void parseSignature(String signature)
        {
            if (signature != null)
                new SignatureReader(signature).accept(new SignatureParser(result));
        }

        private void parseDescriptor(String descriptor)
        {
            parseType(Type.getType(descriptor));
        }

        private void parseType(Type type)
        {
            switch (type.getSort()) {
                case Type.ARRAY:
                    parseType(type.getElementType());
                    return;

                case Type.OBJECT:
                    addClass(result, type.getInternalName().replace('/', '.'), true);
                    return;

                case Type.METHOD:
                    parseType(type.getReturnType());
                    for (Type arg : type.getArgumentTypes())
                        parseType(arg);
                    return;
            }
        }

        private void parseFrameTypes(int numTypes, Object[] frameTypes)
        {
            for (int i = 0; i < numTypes; ++i) {
                if (frameTypes[i] instanceof String) {
                    String descriptor = (String) frameTypes[i];
                    if (descriptor.charAt(0) == '[')
                        parseDescriptor(descriptor);
                    else
                        addClass(result, descriptor.replace('/', '.'), true);
                }
            }
        }
    }

    private final BuilderClassLoader classLoader;
    private final HashMap<String, File> classFiles;

    public SourceFiles()
    {
        classLoader = BuilderClassLoader.getInstance();
        classFiles = new HashMap<String, File>();
    }

    public void add(String className, File sourceFile)
    {
        classFiles.put(className, sourceFile);
    }

    public HashSet<File> forClass(Class<?> classInstance)
    {
        HashSet<File> result = new HashSet<File>();
        addClass(result, classInstance.getName(), false);
        return result;
    }

    public void addClass(HashSet<File> result, String className, boolean ignoreNull)
    {
        File sourceFile = classFiles.get(className);
        if (sourceFile == null) {
            if (ignoreNull)
                return;
            throw new RuntimeException("Unable to locate source file for class \"" + className + "\".");
        }

        if (!result.add(sourceFile))
            return;

        File binaryFile = classLoader.getClassFile(className);
        if (binaryFile == null) {
            if (ignoreNull)
                return;
            throw new RuntimeException("Class \"" + className + "\" was loaded with wrong class loader.");
        }

        try {
            FileInputStream inputStream = new FileInputStream(binaryFile);
            try {
                ClassReader reader = new ClassReader(inputStream);
                reader.accept(new TraceClassVisitor(null, new Visitor(result), null), 0);
            } finally {
                inputStream.close();
            }
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }
}
