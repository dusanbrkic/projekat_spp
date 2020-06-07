#ifndef __IR__
#define __IR__

#include "Types.h"

/**
 * This class represents one variable from program code.
 */
class Variable
{
public:
	enum VariableType
	{
		MEM_VAR,
		REG_VAR,
		NO_TYPE
	};

	Variable() : m_type(NO_TYPE), m_name(""), m_position(-1), m_assignment(no_assign), index(0) {}
	Variable(std::string name, int pos) : m_type(NO_TYPE), m_name(name), m_position(pos), m_assignment(no_assign), index(0) {}
	Variable(std::string name, int pos, VariableType type) : m_type(type), m_name(name), m_position(pos), m_assignment(no_assign), index(0) {}
	std::string getName() { return m_name; }
	int getPosition() { return m_position; }
    int index;
private:
	VariableType m_type;
	std::string m_name;
	int m_position;
	Regs m_assignment;
};


/**
 * This type represents list of variables from program code.
 */
typedef std::list<Variable*> Variables;


/**
 * This class represents one instruction in program code.
 */
class Instruction
{
public:
	Instruction () : m_position(0), m_type(I_NO_TYPE) {}
	Instruction (int pos, InstructionType type, Variables& dst, Variables& src, Variables& def_, Variables& use_) :
		m_position(pos), m_type(type), m_dst(dst), m_src(src), m_def(def_), m_use(use_) {}
	int getPos() { return m_position; }
	void addSucc(Instruction* i) { m_succ.push_back(i); }
	void addPred(Instruction* i) { m_pred.push_back(i); }
	void printInfo() {
        std::cout << "I: " << getPos() << " " << m_type << " dst ";
        for (Variable *d : m_dst) std::cout << d->getName() << " ";
        std::cout << "src ";
        for (Variable *s : m_src) std::cout << s->getName() << " ";
        std::cout << "def: ";
        for (Variable *d : m_def) std::cout << d->getName() << " ";
        std::cout << "use: ";
        for (Variable *s : m_use) std::cout << s->getName() << " ";
        std::cout << "succ ";
        for (Instruction *i : m_succ) std::cout << i->m_position << ":"<< i->m_type << " ";
        std::cout << " pred: ";
        for (Instruction *i : m_pred) std::cout << i->m_position << ":"<< i->m_type << " ";
        std::cout << "in: ";
        for (Variable *i : m_in) std::cout << i->getName() << " ";
        std::cout << "out: ";
        for (Variable *o : m_out) std::cout << o->getName() << " ";
        std::cout << std::endl;
    }
    friend class LivenessAnalysis;
	friend class InterferenceGraph;
private:
	int m_position;
	InstructionType m_type;
	
	Variables m_dst;
	Variables m_src;

	Variables m_use;
	Variables m_def;
	Variables m_in;
	Variables m_out;
	std::list<Instruction*> m_succ;
	std::list<Instruction*> m_pred;
};

/**
 * This type represents list of instructions from program code.
 */
typedef std::list<Instruction*> Instructions;

/**
 * This class represents one label in program code.
 */
class Label {
public:
	Label(std::string name_, int pos_) : name(name_), pos(pos_) {}
	std::string getName() { return name; }
	int getPosition() { return pos; }
	Instruction* nextInstruction(Instructions& i) {
		for (Instruction* in : i) {
			if (in->getPos() == pos) return in;
		}
		return nullptr;
	}
private:
	int pos;
	std::string name;
};

/**
 * This type represents list of labels from program code.
 */
typedef std::list<Label*> Labels;

class Function {
public:
	Function(std::string name_, int pos_) : name(name_), pos(pos_) {}
	std::string getName() { return name; }
	int getPosition() { return pos; }
private:
	int pos;
	std::string name;
};

/**
 * This type represents list of functions from program code.
 */
typedef std::list<Function*> Functions;

#endif
