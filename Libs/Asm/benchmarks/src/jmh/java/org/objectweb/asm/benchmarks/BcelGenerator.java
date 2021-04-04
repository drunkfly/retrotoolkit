// ASM: a very small and fast Java bytecode manipulation framework
// Copyright (c) 2000-2011 INRIA, France Telecom
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. Neither the name of the copyright holders nor the names of its
//    contributors may be used to endorse or promote products derived from
//    this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
// THE POSSIBILITY OF SUCH DAMAGE.
package org.objectweb.asm.benchmarks;

import org.apache.bcel.Const;
import org.apache.bcel.generic.ArrayType;
import org.apache.bcel.generic.ClassGen;
import org.apache.bcel.generic.ConstantPoolGen;
import org.apache.bcel.generic.InstructionFactory;
import org.apache.bcel.generic.InstructionList;
import org.apache.bcel.generic.MethodGen;
import org.apache.bcel.generic.PUSH;
import org.apache.bcel.generic.Type;

/**
 * A "Hello World!" class generator using the BCEL library.
 *
 * @author Eric Bruneton
 */
public class BcelGenerator extends Generator {

  private static final Type PRINT_STREAM_TYPE = Type.getType("Ljava/io/PrintStream;");

  @Override
  public byte[] generateClass() {
    ClassGen classGen =
        new ClassGen("HelloWorld", "java/lang/Object", "HelloWorld.java", Const.ACC_PUBLIC, null);

    classGen.addEmptyConstructor(Const.ACC_PUBLIC);

    ConstantPoolGen constantPoolGen = classGen.getConstantPool();
    InstructionList insnList = new InstructionList();
    InstructionFactory insnFactory = new InstructionFactory(classGen);

    MethodGen methodGen =
        new MethodGen(
            Const.ACC_STATIC | Const.ACC_PUBLIC,
            Type.VOID,
            new Type[] {new ArrayType(Type.STRING, 1)},
            null,
            "main",
            "HelloWorld",
            insnList,
            constantPoolGen);
    insnList.append(insnFactory.createGetStatic("java/lang/System", "out", PRINT_STREAM_TYPE));
    insnList.append(new PUSH(constantPoolGen, "Hello world!"));
    insnList.append(
        insnFactory.createInvoke(
            "java.io.PrintStream",
            "println",
            Type.VOID,
            new Type[] {Type.STRING},
            Const.INVOKESPECIAL));

    methodGen.setMaxStack();
    classGen.addMethod(methodGen.getMethod());

    return classGen.getJavaClass().getBytes();
  }
}
