#pragma once
#include "LexicalAnalysis.h"
#include "Token.h"
#include "IR.h"

class SyntaxAnalysis
{
public:

	/**
	* Constructor
	*/
	SyntaxAnalysis(LexicalAnalysis& lex);

	/**
	* Method which performs syntax analysis
	*/
	bool Do();

	/**
	* Method which performs the second pass
	*/
	void DoSecondPass();

private:
	/**
	* Current line of code
	*/
	int position;

	/**
	* List of memory variables
	*/
	Variables memory_variables;

	/**
	* List of reg variables
	*/
	Variables reg_variables;

	/**
	* List of labels
	*/
	Labels labels;

	/**
	* List of function
	*/
	Functions functions;

	/**
	* List of instructions
	*/
	Instructions instructions;

	/**
	* Indicates if analysis is performed in second pass
	*/
	bool secondPass;

	/**
	* Saves a pointer from a previous instruction
	*/
	Instruction* previousInstruction;

    /**
	* Fills the index attribute of every member by the order of attributes in the list
	*/
    void fillVariableIndexes(Variables& v);

	/**
	* Prints the error message, and token that caused the syntax error
	*/
	void printSyntaxError(Token token);

	/**
	* Prints the error message, and token that caused the undeclared error
	*/
	void printUndeclaredError(Token token);

	/**
	* Prints the error message, and token that caused the declaredTwice error
	*/
	void printAlreadyDeclaredError(Token token, int position_);

	/**
	* Checks if token used is previously declared
	*/
	void declared(Token token);

	/**
	* Checks if token declared already exists
	*/
	void exists(Token token);

	/**
	* Returns a pointer to a variable found by token value
	*/
	Variable* findVariableByName(Token t);

	/**
	* Returns a pointer to a variable found by token value
	*/
	Label* findLabelByName(Token t);

	/**
	* Links instructions after a jump
	*/
	void linkJumpInstructions(Instruction* i, Label* l);

	/**
	* Links instruction with the previous one
	*/
	void linkWithPreviousInstruction(Instruction* i);

	/**
	* Eats the current token if its type is "t"
	* otherwise reports syntax error
	* Checks if memory and registry variables are declared
	*
	* param[in] - t - the expected token type
	*/
	void eat(TokenType t);

	/**
	* Returns the next token from the token list
	*/
	Token getNextToken();

	/**
	 * Nonterminal Q
	 */
	void Q();

	/**
	 * Nonterminal S
	 */
	void S();

	/**
	 * Nonterminal L
	 */
	void L();

	/**
	 * Nonterminal E
	 */
	void E();

	/**
	* Reference to lexical analysis module
	*/
	LexicalAnalysis& lexicalAnalysis;

	/**
	* Syntax error indicator
	*/
	bool errorFound;

	/**
	* Iterator to the token list which represents the output of the lexical analysis
	*/
	TokenList::iterator tokenIterator;

	/**
	* Current token that is being analyzed
	*/
	Token currentToken;

	friend class InterferenceGraph;
	friend class LivenessAnalysis;
};
