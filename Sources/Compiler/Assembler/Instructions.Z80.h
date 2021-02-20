#ifndef COMPILER_ASSEMBLER_INSTRUCTIONS_Z80_H
#define COMPILER_ASSEMBLER_INSTRUCTIONS_Z80_H

#include "Compiler/Assembler/Instruction.h"
#include "Compiler/Assembler/OpcodeParseContext.h"
#include <sstream>
#include <stdint.h>

namespace Z80
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    namespace Mnemonic
    {
        #define Z80_MNEMONIC(NAME) \
            struct NAME { \
                static void toString(std::stringstream& ss) { ss << #NAME; } \
                static bool tryParse(OpcodeParseContext* context) { return context->consumeIdentifier(#NAME); } \
            }

        Z80_MNEMONIC(ADC);
        Z80_MNEMONIC(ADD);
        Z80_MNEMONIC(AND);
        Z80_MNEMONIC(BIT);
        Z80_MNEMONIC(CALL);
        Z80_MNEMONIC(CCF);
        Z80_MNEMONIC(CP);
        Z80_MNEMONIC(CPD);
        Z80_MNEMONIC(CPDR);
        Z80_MNEMONIC(CPI);
        Z80_MNEMONIC(CPIR);
        Z80_MNEMONIC(CPL);
        Z80_MNEMONIC(DAA);
        Z80_MNEMONIC(DEC);
        Z80_MNEMONIC(DI);
        Z80_MNEMONIC(DJNZ);
        Z80_MNEMONIC(EI);
        Z80_MNEMONIC(EX);
        Z80_MNEMONIC(EXX);
        Z80_MNEMONIC(HALT);
        Z80_MNEMONIC(IM);
        Z80_MNEMONIC(IN);
        Z80_MNEMONIC(INC);
        Z80_MNEMONIC(IND);
        Z80_MNEMONIC(INDR);
        Z80_MNEMONIC(INI);
        Z80_MNEMONIC(INIR);
        Z80_MNEMONIC(JP);
        Z80_MNEMONIC(JR);
        Z80_MNEMONIC(LD);
        Z80_MNEMONIC(LDD);
        Z80_MNEMONIC(LDDR);
        Z80_MNEMONIC(LDI);
        Z80_MNEMONIC(LDIR);
        Z80_MNEMONIC(NEG);
        Z80_MNEMONIC(NOP);
        Z80_MNEMONIC(OR);
        Z80_MNEMONIC(OTDR);
        Z80_MNEMONIC(OTIR);
        Z80_MNEMONIC(OUT);
        Z80_MNEMONIC(OUTD);
        Z80_MNEMONIC(OUTI);
        Z80_MNEMONIC(POP);
        Z80_MNEMONIC(PUSH);
        Z80_MNEMONIC(RES);
        Z80_MNEMONIC(RET);
        Z80_MNEMONIC(RETI);
        Z80_MNEMONIC(RETN);
        Z80_MNEMONIC(RL);
        Z80_MNEMONIC(RLA);
        Z80_MNEMONIC(RLC);
        Z80_MNEMONIC(RLCA);
        Z80_MNEMONIC(RLD);
        Z80_MNEMONIC(RR);
        Z80_MNEMONIC(RRA);
        Z80_MNEMONIC(RRC);
        Z80_MNEMONIC(RRCA);
        Z80_MNEMONIC(RRD);
        Z80_MNEMONIC(RST);
        Z80_MNEMONIC(SBC);
        Z80_MNEMONIC(SCF);
        Z80_MNEMONIC(SET);
        Z80_MNEMONIC(SLA);
        Z80_MNEMONIC(SLL);
        Z80_MNEMONIC(SRA);
        Z80_MNEMONIC(SRL);
        Z80_MNEMONIC(SUB);
        Z80_MNEMONIC(XOR);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class bit
    {
    public:
        void toString(std::stringstream& ss) const { /*FIXME */ }
        bool tryParse(OpcodeParseContext* context) { return false; /* FIXME */ }
    };

    class byte
    {
    public:
        void toString(std::stringstream& ss) const { /*FIXME */ }
        bool tryParse(OpcodeParseContext* context) { return false; /* FIXME */ }
    };

    class word
    {
    public:
        void toString(std::stringstream& ss) const { /*FIXME */ }
        bool tryParse(OpcodeParseContext* context) { return false; /* FIXME */ }
    };

    struct memBC
    {
        void toString(std::stringstream& ss) const { ss << "(bc)"; }
        bool tryParse(OpcodeParseContext* context) { return false; /* FIXME */ }
    };

    struct memDE
    {
        void toString(std::stringstream& ss) const { ss << "(de)"; }
        bool tryParse(OpcodeParseContext* context) { return false; /* FIXME */ }
    };

    struct memHL
    {
        void toString(std::stringstream& ss) const { ss << "(hl)"; }
        bool tryParse(OpcodeParseContext* context) { return false; /* FIXME */ }
    };

    struct memIX
    {
        void toString(std::stringstream& ss) const { ss << "(ix)"; }
        bool tryParse(OpcodeParseContext* context) { return false; /* FIXME */ }
    };

    struct memIY
    {
        void toString(std::stringstream& ss) const { ss << "(iy)"; }
        bool tryParse(OpcodeParseContext* context) { return false; /* FIXME */ }
    };

    struct memSP
    {
        void toString(std::stringstream& ss) const { ss << "(sp)"; }
        bool tryParse(OpcodeParseContext* context) { return false; /* FIXME */ }
    };

    class memAddr
    {
    public:
        void toString(std::stringstream& ss) const { /* FIXME */ }
        bool tryParse(OpcodeParseContext* context) { return false; /* FIXME */ }
    };

    class IX_byte
    {
    public:
        void toString(std::stringstream& ss) const { /* FIXME */ }
        bool tryParse(OpcodeParseContext* context) { return false; /* FIXME */ }
    };

    class IY_byte
    {
    public:
        void toString(std::stringstream& ss) const { /* FIXME */ }
        bool tryParse(OpcodeParseContext* context) { return false; /* FIXME */ }
    };

    class relOffset
    {
    public:
        void toString(std::stringstream& ss) const { /* FIXME */ }
        bool tryParse(OpcodeParseContext* context) { return false; /* FIXME */ }
    };

    class portC
    {
    public:
        void toString(std::stringstream& ss) const { ss << "(c)"; }
        bool tryParse(OpcodeParseContext* context) { return false; /* FIXME */ }
    };

    class portAddr
    {
    public:
        void toString(std::stringstream& ss) const { /* FIXME */ }
        bool tryParse(OpcodeParseContext* context) { return false; /* FIXME */ }
    };

    class intMode
    {
    public:
        void toString(std::stringstream& ss) const { /* FIXME */ }
        bool tryParse(OpcodeParseContext* context) { return false; /* FIXME */ }
    };

    class rstIndex
    {
    public:
        void toString(std::stringstream& ss) const { /* FIXME */ }
        bool tryParse(OpcodeParseContext* context) { return false; /* FIXME */ }
    };

    #define Z80_REGOP(NAME) \
        struct NAME { \
            static void toString(std::stringstream& ss) { ss << #NAME; } \
            static bool tryParse(OpcodeParseContext* context) { return context->consumeIdentifier(#NAME); } \
        }

    struct AF_
    {
        static void toString(std::stringstream& ss) { ss << "AF'"; }
        static bool tryParse(OpcodeParseContext* context) { return context->consumeIdentifier("AF'"); }
    };

    Z80_REGOP(A);
    Z80_REGOP(B);
    Z80_REGOP(C);
    Z80_REGOP(D);
    Z80_REGOP(E);
    Z80_REGOP(H);
    Z80_REGOP(L);
    Z80_REGOP(IXL);
    Z80_REGOP(IXH);
    Z80_REGOP(IYL);
    Z80_REGOP(IYH);

    Z80_REGOP(AF);
    Z80_REGOP(BC);
    Z80_REGOP(DE);
    Z80_REGOP(HL);
    Z80_REGOP(IX);
    Z80_REGOP(IY);
    Z80_REGOP(SP);

    Z80_REGOP(I);
    Z80_REGOP(R);

    #define Z80_FLAGOP(NAME) \
        struct flag##NAME { \
            static void toString(std::stringstream& ss) { ss << #NAME; } \
            static bool tryParse(OpcodeParseContext* context) { return context->consumeIdentifier(#NAME); } \
        }

    Z80_FLAGOP(C);
    Z80_FLAGOP(M);
    Z80_FLAGOP(NC);
    Z80_FLAGOP(NZ);
    Z80_FLAGOP(P);
    Z80_FLAGOP(PE);
    Z80_FLAGOP(PO);
    Z80_FLAGOP(Z);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    struct TStates
    {
        const int t1;
        const int t2;

        TStates(int v) : t1(v), t2(v) {}
        TStates(int v1, int v2) : t1(v1), t2(v2) {}
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class Opcode : public Instruction
    {
    public:
        Opcode() = default;

        bool isZ80Opcode() const override;
        virtual void toString(std::stringstream& ss) const = 0;

        DISABLE_COPY(Opcode);
    };

    template <typename OP> class Opcode0 : public Opcode
    {
    public:
        Opcode0() = default;

        void toString(std::stringstream& ss) const override { OP::toString(ss); }
        static bool tryParse(OpcodeParseContext* context) { return OP::tryParse(context) && context->checkEnd(); }

        DISABLE_COPY(Opcode0);
    };

    template <typename OP, typename OP1> class Opcode1 : public Opcode
    {
    public:
        explicit Opcode1(OP1 op1) : mOp1(op1) {}

        void toString(std::stringstream& ss) const override
        {
            OP::toString(ss);
            ss << ' ';
            mOp1.toString(ss);
        }

        static bool tryParse(OpcodeParseContext* context, OP1& op1)
        {
            if (!OP::tryParse(context)) return false;
            if (!op1.tryParse(context)) return false;
            return context->checkEnd();
        }

    private:
        OP1 mOp1;

        DISABLE_COPY(Opcode1);
    };

    template <typename OP, typename OP1, typename OP2> class Opcode2 : public Opcode
    {
    public:
        Opcode2(OP1 op1, OP2 op2) : mOp1(op1), mOp2(op2) {}

        void toString(std::stringstream& ss) const override
        {
            OP::toString(ss);
            ss << ' ';
            mOp1.toString(ss);
            ss << ", ";
            mOp2.toString(ss);
        }

        static bool tryParse(OpcodeParseContext* context, OP1& op1, OP2& op2)
        {
            if (!OP::tryParse(context)) return false;
            if (!op1.tryParse(context)) return false;
            if (!context->consumeComma()) return false;
            if (!op2.tryParse(context)) return false;
            return context->checkEnd();
        }

    private:
        OP1 mOp1;
        OP2 mOp2;

        DISABLE_COPY(Opcode2);
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    #define Z80_OPCODE_0(OP, BYTES, TSTATES) \
        class OP : public Opcode0<Mnemonic::OP> \
        { \
        public: \
            OP() = default; \
        }

    #define Z80_OPCODE_1(OP, OP1, BYTES, TSTATES) \
        class OP##_##OP1 : public Opcode1<Mnemonic::OP, OP1> \
        { \
        public: \
            explicit OP##_##OP1(OP1 op1) : Opcode1(op1) {} \
        }

    #define Z80_OPCODE_2(OP, OP1, OP2, BYTES, TSTATES) \
        class OP##_##OP1##_##OP2 : public Opcode2<Mnemonic::OP, OP1, OP2> \
        { \
        public: \
            OP##_##OP1##_##OP2(OP1 op1, OP2 op2) : Opcode2(op1, op2) {} \
        }

    #include "Instructions.Z80.hh"

    #undef Z80_OPCODE_2
    #undef Z80_OPCODE_1
    #undef Z80_OPCODE_0
}

#endif
