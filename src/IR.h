#ifndef __IR__
#define __IR__

#include <iomanip>
#include "Types.h"

/**
 * This class represents one variable from program code.
 */
class Variable {
public:
    enum VariableType {
        MEM_VAR,
        REG_VAR,
        NO_TYPE
    };

    Variable() : m_type(NO_TYPE), m_name(""), m_position(-1), m_assignment(no_assign), index(0), value("0") {}

    Variable(std::string name, int pos) : m_type(NO_TYPE), m_name(name), m_position(pos), m_assignment(no_assign),
                                          index(0), value("0") {}

    Variable(std::string name, int pos, VariableType type) : m_type(type), m_name(name), m_position(pos),
                                                             m_assignment(no_assign), index(0), value("0") {}

    std::string getName() { return m_name; }

    int getPosition() { return m_position; }

    friend class InterferenceGraph;

    friend class SyntaxAnalysis;

    friend class MipsCode;

    friend class Instruction;

private:
    std::string value;
    int index;
    VariableType m_type;
    std::string m_name;
    int m_position;
    Regs m_assignment;
};


/**
 * This type represents list of variables from program code.
 */
typedef std::list<Variable *> Variables;


/**
 * This class represents one instruction in program code.
 */
class Instruction {
public:
    Instruction() : m_position(0), m_type(I_NO_TYPE) {}

    Instruction(int pos, InstructionType type, Variables &dst, Variables &src, Variables &def_, Variables &use_,
                std::string lv, std::string labelID_) :
            m_position(pos), m_type(type), m_dst(dst), m_src(src), m_def(def_), m_use(use_), literalValue(lv),
            labelID(labelID_) {}

    int getPos() { return m_position; }

    void addSucc(Instruction *i) { m_succ.push_back(i); }

    void addPred(Instruction *i) { m_pred.push_back(i); }

    std::string writeInfo(std::ostream &outFile) {
            if (m_type == I_ADD)
                    outFile << std::setw(LEFT_ALIGN) << "add" << std::setw(LEFT_ALIGN) << "$t"
                            << m_dst.back()->m_assignment - 1 << ", $t" << m_src.front()->m_assignment - 1
                            << ", $t" << m_src.back()->m_assignment - 1 << std::endl;
            else if (m_type == I_ADDI)
                    outFile << std::setw(LEFT_ALIGN) << "addi" << std::setw(LEFT_ALIGN) << "$t"
                            << m_dst.back()->m_assignment - 1 << ",$t" << m_src.front()->m_assignment - 1
                            << ", " << literalValue << std::endl;
            else if (m_type == I_SUB)
                    outFile << std::setw(LEFT_ALIGN) << "sub" << std::setw(LEFT_ALIGN) << "$t"
                            << m_dst.back()->m_assignment - 1 << ", $t" << m_src.front()->m_assignment - 1
                            << ", $t" << m_src.back()->m_assignment - 1 << std::endl;
            else if (m_type == I_LA)
                    outFile << std::setw(LEFT_ALIGN) << "la" << std::setw(LEFT_ALIGN) << "$t"
                            << m_dst.back()->m_assignment - 1 << ", " << m_src.front()->m_name << std::endl;
            else if (m_type == I_LW)
                    outFile << std::setw(LEFT_ALIGN) << "lw" << std::setw(LEFT_ALIGN) << "$t"
                            << m_dst.back()->m_assignment - 1 << ", " << literalValue << "($t"
                            << m_src.back()->m_assignment - 1 << ")" << std::endl;
            else if (m_type == I_LI)
                    outFile << std::setw(LEFT_ALIGN) << "li" << std::setw(LEFT_ALIGN) << "$t"
                            << m_dst.back()->m_assignment - 1 << ", " << literalValue << std::endl;
            else if (m_type == I_SW)
                    outFile << std::setw(LEFT_ALIGN) << "sw" << std::setw(LEFT_ALIGN) << "$t"
                            << m_src.back()->m_assignment - 1 << ", " << literalValue << "($t"
                            << m_dst.back()->m_assignment - 1 << ")" << std::endl;
            else if (m_type == I_B)
                    outFile << std::setw(LEFT_ALIGN) << "b" << std::setw(LEFT_ALIGN) << labelID << std::endl;
            else if (m_type == I_BLTZ)
                    outFile << std::setw(LEFT_ALIGN) << "bltz" << std::setw(LEFT_ALIGN) << "$t"
                            << m_src.back()->m_assignment - 1 << ", " << labelID << std::endl;
            else if (m_type == I_NOP)
                    outFile << std::setw(LEFT_ALIGN) << "nop" << std::endl;
            else if (m_type == I_BEQ)
                    outFile << std::setw(LEFT_ALIGN) << "beq" << std::setw(LEFT_ALIGN) << "$t" << m_src.front()->m_assignment - 1
                            << ", $t"
                            << m_src.back()->m_assignment - 1 << ", " << labelID << std::endl;
            else if (m_type == I_AND)
                    outFile << std::setw(LEFT_ALIGN) << "and" << std::setw(LEFT_ALIGN) << "$t"
                            << m_dst.back()->m_assignment - 1 << ", $t" << m_src.front()->m_assignment - 1
                            << ", $t" << m_src.back()->m_assignment - 1 << std::endl;
            else if (m_type == I_ABS)
                    outFile << std::setw(LEFT_ALIGN) << "abs" << std::setw(LEFT_ALIGN) << "$t"
                            << m_dst.back()->m_assignment - 1 << ", $t" << m_src.front()->m_assignment - 1
                            << std::endl;
    }

    friend class LivenessAnalysis;
    friend class InterferenceGraph;
    friend class MipsCode;

private:
    std::string literalValue;
    std::string labelID;
    int m_position;
    InstructionType m_type;

    Variables m_dst;
    Variables m_src;

    Variables m_use;
    Variables m_def;
    Variables m_in;
    Variables m_out;
    std::list<Instruction *> m_succ;
    std::list<Instruction *> m_pred;
};

/**
 * This type represents list of instructions from program code.
 */
typedef std::list<Instruction *> Instructions;

/**
 * This class represents one label in program code.
 */
class Label {
public:
    Label(std::string name_, int pos_) : name(name_), pos(pos_) {}

    std::string getName() { return name; }

    int getPosition() { return pos; }

    Instruction *nextInstruction(Instructions &i) {
            for (Instruction *in : i) {
                    if (in->getPos() == pos) return in;
            }
            return nullptr;
    }

    friend class MipsCode;

private:
    int pos;
    std::string name;
};

/**
 * This type represents list of labels from program code.
 */
typedef std::list<Label *> Labels;

class Function {
public:
    Function(std::string name_, int pos_) : name(name_), pos(pos_) {}

    std::string getName() { return name; }

    int getPosition() { return pos; }

    friend class MipsCode;

private:
    int pos;
    std::string name;
};

/**
 * This type represents list of functions from program code.
 */
typedef std::list<Function *> Functions;

#endif
