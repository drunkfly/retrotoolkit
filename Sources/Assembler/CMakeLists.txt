
add(Assembler
    STATIC_LIBRARY
    LIBS
        Common
    SOURCES
        Instruction.cpp
        Instruction.h
        Instructions.Z80.cpp
        Instructions.Z80.h
        Instructions.Z80.hh
        OpcodeParseContext.cpp
        OpcodeParseContext.h
    )
