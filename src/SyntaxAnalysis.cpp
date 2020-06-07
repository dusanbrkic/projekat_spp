#include <iostream>
#include <iomanip>

#include "SyntaxAnalysis.h"

using namespace std;


SyntaxAnalysis::SyntaxAnalysis(LexicalAnalysis& lex)
	: lexicalAnalysis(lex), errorFound(false), tokenIterator(lexicalAnalysis.getTokenList().begin()), position(0), secondPass(false), previousInstruction(nullptr)
{
}

void SyntaxAnalysis::fillVariableIndexes(Variables& v) {
    int index = 0;
    for (Variable* var : v){
        var->index = index;
        index++;
    }
}

bool SyntaxAnalysis::Do()
{
	currentToken = getNextToken();
	Q();
    fillVariableIndexes(reg_variables);
	DoSecondPass();

	return !errorFound;
}

void SyntaxAnalysis::DoSecondPass(){
	tokenIterator = lexicalAnalysis.getTokenList().begin();
	currentToken = getNextToken();
	position = 0;
	secondPass = true;
	Q();
}


void SyntaxAnalysis::printSyntaxError(Token token)
{
	cout << "Syntax error! Token: " << token.getValue() << " unexpected at line " << position << endl;
}

void SyntaxAnalysis::printUndeclaredError(Token token)
{
	cout << "Semantic error! Token: " << token.getValue() << " undeclared at line " << position << endl;
}

void SyntaxAnalysis::printAlreadyDeclaredError(Token token, int position_) {
	cout << "Semantic error! Token: " << token.getValue() << " at line " << position << " already declared at line " << position_ << endl;
}

void SyntaxAnalysis::declared(Token token)
{
	if (token.getType() == T_R_ID) {
		for (Variable* var : reg_variables)
			if (var->getName() == token.getValue()){
				return;
			}
	}
	else if(token.getType() == T_ID) {
		for (Label* l : labels)
			if (l->getName() == token.getValue()) {
				return;
			}
	}
	else {
		for (Variable* var : memory_variables)
			if (var->getName() == token.getValue()){
				return;
			}
	}
	errorFound = true;
	printUndeclaredError(currentToken);
}

void SyntaxAnalysis::exists(Token token){
	if (token.getType() == T_R_ID) {
		for (Variable* var : reg_variables)
			if (var->getName() == token.getValue()) {
				errorFound = true;
				printAlreadyDeclaredError(token, var->getPosition());
			}
				
	}
	else if (token.getType() == T_ID) {
		for (Function* f : functions)
			if (f->getName() == token.getValue()) {
				errorFound = true;
				printAlreadyDeclaredError(token, f->getPosition());
			}
		for (Label* l : labels)
			if (l->getName() == token.getValue()) {
				errorFound = true;
				printAlreadyDeclaredError(token, l->getPosition());
			}
	}
	else {
		for (Variable* var : memory_variables)
			if (var->getName() == token.getValue()){
				errorFound = true;
				printAlreadyDeclaredError(token, var->getPosition());
			}
	}
}

Variable* SyntaxAnalysis::findVariableByName(Token t){
	if (t.getType() == T_R_ID) {
		for (Variable* v : reg_variables)
			if (v->getName() == t.getValue()) return v;
	}
	else {
		for (Variable* v : memory_variables)
			if (v->getName() == t.getValue()) return v;
	}

	return nullptr;
}

Label* SyntaxAnalysis::findLabelByName(Token t)
{
	for (Label* l : labels)
		if (l->getName() == t.getValue()) return l;

	return nullptr;
}

void SyntaxAnalysis::linkJumpInstructions(Instruction* i, Label* l)
{
		Instruction* realI;
		Instruction* jumpToInstruction = l->nextInstruction(instructions);
		for (Instruction* in : instructions) if (in->getPos() == i->getPos()) realI = in;
		realI->addSucc(jumpToInstruction);
		jumpToInstruction->addPred(realI);
}

void SyntaxAnalysis::linkWithPreviousInstruction(Instruction* i)
{
	if (previousInstruction) {
		i->addPred(previousInstruction);
		previousInstruction->addSucc(i);
	}
}

void SyntaxAnalysis::eat(TokenType t)
{
	if (!errorFound)
	{
		if (currentToken.getType() == t)
		{
			if(!secondPass)
				if (currentToken.getType() == T_R_ID || currentToken.getType() == T_M_ID) declared(currentToken);

			if(!secondPass) cout <<  position << " " << currentToken.getValue() << endl;
			currentToken = getNextToken();
		}
		else
		{
			printSyntaxError(currentToken);
			errorFound = true;
		}
	}
}


Token SyntaxAnalysis::getNextToken()
{
	if (tokenIterator == lexicalAnalysis.getTokenList().end())
		throw runtime_error("End of input file reached");
	return *tokenIterator++;
}


void SyntaxAnalysis::Q()
{
	if (!errorFound)
	{
		S();
		eat(T_SEMI_COL);
		L();
	}
}


void SyntaxAnalysis::S()
{
	if (!errorFound)
	{
		position++;
		if (currentToken.getType() == T_MEM) {
			eat(T_MEM);
			if (!secondPass) {
				exists(currentToken);
				memory_variables.push_back(new Variable(currentToken.getValue(), position, Variable::VariableType::MEM_VAR));
			}
			eat(T_M_ID);
			eat(T_NUM);
		}
		else if (currentToken.getType() == T_REG) {
			eat(T_REG);
			if (!secondPass) {
				exists(currentToken);
				reg_variables.push_back(new Variable(currentToken.getValue(), position, Variable::VariableType::REG_VAR));
			}
			eat(T_R_ID);
		}
		else if (currentToken.getType() == T_FUNC) {
			eat(T_FUNC);
			if (!secondPass) {
				exists(currentToken);
				functions.push_back(new Function(currentToken.getValue(), position));
			}
			eat(T_ID);
		}
		else if (currentToken.getType() == T_ID) {
			if (!secondPass) {
				exists(currentToken);
				labels.push_back(new Label(currentToken.getValue(), position));
			}
			eat(T_ID);
			eat(T_COL);
			E();
		}
		else {
			E();
		}
	}
}


void SyntaxAnalysis::L()
{
	if (!errorFound)
	{
		if (currentToken.getType() == T_END_OF_FILE) {
			return;
		}
		else {
			Q();
		}
	}
}


void SyntaxAnalysis::E()
{
	if (!errorFound)
	{
		Instruction* i = nullptr;
		Variables dest;
		Variables src;
		Variables use;
		Variables def;
		if (currentToken.getType() == T_ADD) {
			eat(T_ADD);
			dest.push_back(findVariableByName(currentToken));
			eat(T_R_ID);
			eat(T_COMMA);
			src.push_back(findVariableByName(currentToken));
			eat(T_R_ID);
			eat(T_COMMA);
			src.push_back(findVariableByName(currentToken));
			eat(T_R_ID);
			i = new Instruction(position, I_ADD, dest, src, dest, src);
		}
		else if (currentToken.getType() == T_ADDI) {
			eat(T_ADDI);
			dest.push_back(findVariableByName(currentToken));
			eat(T_R_ID);
			eat(T_COMMA);
			src.push_back(findVariableByName(currentToken));
			eat(T_R_ID);
			eat(T_COMMA);
			eat(T_NUM);
			i = new Instruction(position, I_ADDI, dest, src, dest, src);
		}
		else if (currentToken.getType() == T_SUB) {
			eat(T_SUB);
            dest.push_back(findVariableByName(currentToken));
			eat(T_R_ID);
			eat(T_COMMA);
			src.push_back(findVariableByName(currentToken));
			eat(T_R_ID);
			eat(T_COMMA);
			src.push_back(findVariableByName(currentToken));
			eat(T_R_ID);
			i = new Instruction(position, I_SUB, dest, src, dest, src);
		
		}
		else if (currentToken.getType() == T_LA) {
			eat(T_LA);
			dest.push_back(findVariableByName(currentToken));
			eat(T_R_ID);
			eat(T_COMMA);
			src.push_back(findVariableByName(currentToken));
			eat(T_M_ID);
			i = new Instruction(position, I_LA, dest, src, dest, use);
		}
		else if (currentToken.getType() == T_LW) {
			eat(T_LW);
			dest.push_back(findVariableByName(currentToken));
			eat(T_R_ID);
			eat(T_COMMA);
			eat(T_NUM);
			eat(T_L_PARENT);
			src.push_back(findVariableByName(currentToken));
			eat(T_R_ID);
			eat(T_R_PARENT);
			i = new Instruction(position, I_LW, dest, src, dest, src);
		}
		else if (currentToken.getType() == T_LI) {
			eat(T_LI);
			dest.push_back(findVariableByName(currentToken));
			eat(T_R_ID);
			eat(T_COMMA);
			eat(T_NUM);
			i = new Instruction(position, I_LI, dest, src, dest, src);
		}
		else if (currentToken.getType() == T_SW) {
			eat(T_SW);
            src.push_back(findVariableByName(currentToken));
			eat(T_R_ID);
			eat(T_COMMA);
			eat(T_NUM);
			eat(T_L_PARENT);
            dest.push_back(findVariableByName(currentToken));
			eat(T_R_ID);
			eat(T_R_PARENT);
			i = new Instruction(position, I_SW, dest, src, dest, src);
		}
		else if (currentToken.getType() == T_B) {
			eat(T_B);
			if (secondPass) declared(currentToken);
            Token label = currentToken;
			eat(T_ID);
			i = new Instruction(position, I_B, dest, src, dest, src);
            if (secondPass) linkJumpInstructions(i, findLabelByName(label));
		}
		else if (currentToken.getType() == T_BLTZ) {
			eat(T_BLTZ);
			src.push_back(findVariableByName(currentToken));
			eat(T_R_ID);
			eat(T_COMMA);
			if (secondPass) declared(currentToken);
            Token label = currentToken;
			eat(T_ID);
			i = new Instruction(position, I_BLTZ, dest, src, dest, src);
            if (secondPass) linkJumpInstructions(i, findLabelByName(label));
		}
		else if (currentToken.getType() == T_NOP) {
			eat(T_NOP);
			i = new Instruction(position, I_NOP, dest, src, dest, src);
		}
		else {
			printSyntaxError(currentToken);
			errorFound = true;
		}
		if (!secondPass) instructions.push_back(i);
		linkWithPreviousInstruction(i);
		previousInstruction = i;
	}
}