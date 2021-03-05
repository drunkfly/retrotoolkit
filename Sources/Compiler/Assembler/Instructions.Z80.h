#ifndef COMPILER_ASSEMBLER_INSTRUCTIONS_Z80_H
#define COMPILER_ASSEMBLER_INSTRUCTIONS_Z80_H

#include "Compiler/ParsingContext.h"
#include "Compiler/Tree/Expr.h"
#include "Compiler/Assembler/Instruction.h"
#include "Common/StringSet.h"
#include "Common/TemplateMagic.h"
#include <sstream>
#include <stdint.h>

namespace Z80
{
    extern StringSet RegisterNames;
    extern StringSet ConditionNames;

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    namespace Mnemonic
    {
        #define Z80_MNEMONIC(NAME) \
            struct NAME { \
                static void toString(std::stringstream& ss) { ss << #NAME; } \
                static bool tryParse(ParsingContext* context) { return context->consumeIdentifier(#NAME); } \
                static bool canEvaluate(const int64_t*, std::unique_ptr<CompilerError>&) { return true; } \
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
        void toString(std::stringstream& ss) const;
        bool tryParse(ParsingContext* c);
        bool canEvaluate(const int64_t* nextAddress, std::unique_ptr<CompilerError>& resolveError) const;
        int value(int64_t currentAddress, uint8_t baseByte) const;
    private:
        Expr* mValue;
    };

    class byte
    {
    public:
        void toString(std::stringstream& ss) const;
        bool tryParse(ParsingContext* c);
        bool canEvaluate(const int64_t* nextAddress, std::unique_ptr<CompilerError>& resolveError) const;
        int value(int64_t currentAddress) const;
    private:
        Expr* mValue;
    };

    class word
    {
    public:
        void toString(std::stringstream& ss) const;
        bool tryParse(ParsingContext* c);
        bool canEvaluate(const int64_t* nextAddress, std::unique_ptr<CompilerError>& resolveError) const;
        int low(int64_t currentAddress, int& high) const;
    private:
        Expr* mValue;
    };

    struct memBC
    {
        void toString(std::stringstream& ss) const;
        bool tryParse(ParsingContext* c);
        bool canEvaluate(const int64_t*, std::unique_ptr<CompilerError>&) const { return true; }
    };

    struct memDE
    {
        void toString(std::stringstream& ss) const;
        bool tryParse(ParsingContext* c);
        bool canEvaluate(const int64_t*, std::unique_ptr<CompilerError>&) const { return true; }
    };

    struct memHL
    {
        void toString(std::stringstream& ss) const;
        bool tryParse(ParsingContext* c);
        bool canEvaluate(const int64_t*, std::unique_ptr<CompilerError>&) const { return true; }
    };

    struct memIX
    {
        void toString(std::stringstream& ss) const;
        bool tryParse(ParsingContext* c);
        bool canEvaluate(const int64_t*, std::unique_ptr<CompilerError>&) const { return true; }
    };

    struct memIY
    {
        void toString(std::stringstream& ss) const;
        bool tryParse(ParsingContext* c);
        bool canEvaluate(const int64_t*, std::unique_ptr<CompilerError>&) const { return true; }
    };

    struct memSP
    {
        void toString(std::stringstream& ss) const;
        bool tryParse(ParsingContext* c);
        bool canEvaluate(const int64_t*, std::unique_ptr<CompilerError>&) const { return true; }
    };

    class memAddr
    {
    public:
        void toString(std::stringstream& ss) const;
        bool tryParse(ParsingContext* c);
        bool canEvaluate(const int64_t* nextAddress, std::unique_ptr<CompilerError>& resolveError) const;
        int low(int64_t currentAddress, int& high) const;
    private:
        Expr* mValue;
    };

    class IX_byte
    {
    public:
        void toString(std::stringstream& ss) const;
        bool tryParse(ParsingContext* c);
        bool canEvaluate(const int64_t* nextAddress, std::unique_ptr<CompilerError>& resolveError) const;
        int value(int64_t currentAddress) const;
    private:
        Expr* mValue;
    };

    class IY_byte
    {
    public:
        void toString(std::stringstream& ss) const;
        bool tryParse(ParsingContext* c);
        bool canEvaluate(const int64_t* nextAddress, std::unique_ptr<CompilerError>& resolveError) const;
        int value(int64_t currentAddress) const;
    private:
        Expr* mValue;
    };

    class relOffset
    {
    public:
        void toString(std::stringstream& ss) const;
        bool tryParse(ParsingContext* c);
        bool canEvaluate(const int64_t* nextAddress, std::unique_ptr<CompilerError>& resolveError) const;
        int value(int64_t currentAddress, int64_t nextAddress) const;
    private:
        Expr* mValue;
    };

    class portC
    {
    public:
        void toString(std::stringstream& ss) const;
        bool tryParse(ParsingContext* c);
        bool canEvaluate(const int64_t*, std::unique_ptr<CompilerError>&) const { return true; }
    };

    class portAddr
    {
    public:
        void toString(std::stringstream& ss) const;
        bool tryParse(ParsingContext* c);
        bool canEvaluate(const int64_t* nextAddress, std::unique_ptr<CompilerError>& resolveError) const;
        int value(int64_t currentAddress) const;
    private:
        Expr* mValue;
    };

    class intMode
    {
    public:
        void toString(std::stringstream& ss) const;
        bool tryParse(ParsingContext* c);
        bool canEvaluate(const int64_t* nextAddress, std::unique_ptr<CompilerError>& resolveError) const;
        int value(int64_t currentAddress) const;
    private:
        Expr* mValue;
    };

    class rstIndex
    {
    public:
        void toString(std::stringstream& ss) const;
        bool tryParse(ParsingContext* c);
        bool canEvaluate(const int64_t* nextAddress, std::unique_ptr<CompilerError>& resolveError) const;
        int value(int64_t currentAddress, uint8_t baseByte) const;
    private:
        Expr* mValue;
    };

    #define Z80_REGOP(NAME) \
        struct NAME { \
            static void toString(std::stringstream& ss) { ss << #NAME; } \
            static bool tryParse(ParsingContext* context) { return context->consumeIdentifier(#NAME); } \
            static bool canEvaluate(const int64_t*, std::unique_ptr<CompilerError>&) { return true; } \
        }

    struct AF_
    {
        static void toString(std::stringstream& ss);
        static bool tryParse(ParsingContext* context);
        static bool canEvaluate(const int64_t*, std::unique_ptr<CompilerError>&) { return true; }
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
            static bool tryParse(ParsingContext* context) { return context->consumeIdentifier(#NAME); } \
            static bool canEvaluate(const int64_t*, std::unique_ptr<CompilerError>&) { return true; } \
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
        explicit Opcode(SourceLocation* location)
            : Instruction(location)
        {
        }

        bool isZ80Opcode() const final override;

        virtual void toString(std::stringstream& ss) const = 0;

        DISABLE_COPY(Opcode);
    };

    template <typename OP> class Opcode0 : public Opcode
    {
    public:
        explicit Opcode0(SourceLocation* location)
            : Opcode(location)
        {
        }

        void toString(std::stringstream& ss) const override { OP::toString(ss); }
        static bool tryParse(ParsingContext* context) { return context->checkEnd(); }

        DISABLE_COPY(Opcode0);
    };

    template <typename OP, typename OP1> class Opcode1 : public Opcode
    {
    public:
        Opcode1(SourceLocation* location, OP1 op1)
            : Opcode(location)
            , mOp1(op1)
        {
        }

        void toString(std::stringstream& ss) const override
        {
            OP::toString(ss);
            ss << ' ';
            mOp1.toString(ss);
        }

        static bool tryParse(ParsingContext* context, OP1& op1)
        {
            if (!op1.tryParse(context)) return false;
            return context->checkEnd();
        }

    protected:
        OP1 mOp1;

        DISABLE_COPY(Opcode1);
    };

    template <typename OP, typename OP1, typename OP2> class Opcode2 : public Opcode
    {
    public:
        Opcode2(SourceLocation* location, OP1 op1, OP2 op2)
            : Opcode(location)
            , mOp1(op1)
            , mOp2(op2)
        {
        }

        void toString(std::stringstream& ss) const override
        {
            OP::toString(ss);
            ss << ' ';
            mOp1.toString(ss);
            ss << ", ";
            mOp2.toString(ss);
        }

        static bool tryParse(ParsingContext* context, OP1& op1, OP2& op2)
        {
            if (!op1.tryParse(context)) return false;
            if (!context->consumeComma()) return false;
            if (!op2.tryParse(context)) return false;
            return context->checkEnd();
        }

    protected:
        OP1 mOp1;
        OP2 mOp2;

        DISABLE_COPY(Opcode2);
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    #define Z80_OPCODE_0(OP, BYTES, TSTATES) \
        class OP final : public Opcode0<Mnemonic::OP> \
        { \
        public: \
            explicit OP(SourceLocation* location) : Opcode0(location) {} \
            bool calculateSizeInBytes(size_t& outSize, \
                std::unique_ptr<CompilerError>& resolveError) const final override; \
            bool emitCode(CodeEmitter* emitter, int64_t& nextAddress, \
                std::unique_ptr<CompilerError>& resolveError) const final override; \
        private: \
            static constexpr size_t arraySizeInBytes(); \
        }

    #define Z80_OPCODE_1(OP, OP1, BYTES, TSTATES) \
        class OP##_##OP1 final : public Opcode1<Mnemonic::OP, OP1> \
        { \
        public: \
            OP##_##OP1(SourceLocation* location, OP1 op1) : Opcode1(location, op1) {} \
            bool calculateSizeInBytes(size_t& outSize, \
                std::unique_ptr<CompilerError>& resolveError) const final override; \
            bool emitCode(CodeEmitter* emitter, int64_t& nextAddress, \
                std::unique_ptr<CompilerError>& resolveError) const final override; \
        private: \
            static constexpr size_t arraySizeInBytes(); \
        }

    #define Z80_OPCODE_2(OP, OP1, OP2, BYTES, TSTATES) \
        class OP##_##OP1##_##OP2 final : public Opcode2<Mnemonic::OP, OP1, OP2> \
        { \
        public: \
            OP##_##OP1##_##OP2(SourceLocation* location, OP1 op1, OP2 op2) : Opcode2(location, op1, op2) {} \
            bool calculateSizeInBytes(size_t& outSize, \
                std::unique_ptr<CompilerError>& resolveError) const final override; \
            bool emitCode(CodeEmitter* emitter, int64_t& nextAddress, \
                std::unique_ptr<CompilerError>& resolveError) const final override; \
        private: \
            static constexpr size_t arraySizeInBytes(); \
        }

    #define OP1
    #define OP2
    #define OP1V(X)
    #define OP2V(X)
    #define OP1W
    #define OP2W
    #define NEXT

    #include "Instructions.Z80.hh"

    #undef NEXT
    #undef OP2W
    #undef OP1W
    #undef OP2V
    #undef OP1V
    #undef OP2
    #undef OP1

    #undef Z80_OPCODE_2
    #undef Z80_OPCODE_1
    #undef Z80_OPCODE_0
}

#endif
