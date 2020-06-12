#include <iostream>
#include <iomanip>

#include "SyntaxAnalysis.h"

using namespace std;


SyntaxAnalysis::SyntaxAnalysis(LexicalAnalysis &lex)
        : lexicalAnalysis(lex), errorFound(false), tokenIterator(lexicalAnalysis.getTokenList().begin()), position(0),
          secondPass(false), previousInstruction(nullptr) {
}

void SyntaxAnalysis::fillVariableIndexes(Variables &v) {
    int index = 0;
    for (Variable *var : v) {
        var->index = index;
        index++;
    }
}

bool SyntaxAnalysis::Do() {
    currentToken = getNextToken();
    Q();
    if (!errorFound) {
        fillVariableIndexes(reg_variables);
        DoSecondPass();
    }

    return !errorFound;
}

void SyntaxAnalysis::DoSecondPass() {
    //checks if there is a main function
    bool has_main = false;
    for (Function *f : functions) {
        if (f->getName() == "main")
            has_main = true;
        int b = 1;
        bool find = false;
        // creates labels from functions
        if (!instructions.empty()) {
            while (true) {
                for (Instruction *i : instructions) {
                    if (i->getPos() == f->getPosition() + b) {
                        labels.push_back(new Label(f->getName(), f->getPosition() + b));
                        find = true;
                        break;
                    }
                }
                if (find) break;
                if (b > instructions.back()->getPos()) {
                    labels.push_back(new Label(f->getName(), f->getPosition() + 1));
                    break;
                }
                b++;
            }
        } else {
            labels.push_back(new Label(f->getName(), f->getPosition()));
        }
    }
    if (!has_main) {
        errorFound = true;
        printMissingMainFunctionError();
        return;
    }
    tokenIterator = lexicalAnalysis.getTokenList().begin();
    currentToken = getNextToken();
    position = 0;
    secondPass = true;
    Q();
}

void SyntaxAnalysis::printMissingMainFunctionError() {
    cout << "Semantic error! Missing 'main' function!" << endl;
}

void SyntaxAnalysis::printSyntaxError(Token token) {
    cout << "Syntax error! Token: " << token.getValue() << " unexpected at line " << position << endl;
}

void SyntaxAnalysis::printUndeclaredError(Token token) {
    cout << "Semantic error! Token: " << token.getValue() << " undeclared at line " << position << endl;
}

void SyntaxAnalysis::printAlreadyDeclaredError(Token token, int position_) {
    cout << "Semantic error! Token: " << token.getValue() << " at line " << position << " already declared at line "
         << position_ << endl;
}

void SyntaxAnalysis::declared(Token token) {
    if (token.getType() == T_R_ID) {
        for (Variable *var : reg_variables)
            if (var->getName() == token.getValue()) {
                return;
            }
    } else if (token.getType() == T_ID) {
        for (Label *l : labels)
            if (l->getName() == token.getValue()) {
                return;
            }
        for (Function *f : functions)
            if (f->getName() == token.getValue()) {
                return;
            }
    } else {
        for (Variable *var : memory_variables)
            if (var->getName() == token.getValue()) {
                return;
            }
    }
    errorFound = true;
    printUndeclaredError(currentToken);
}

void SyntaxAnalysis::exists(Token token) {
    if (token.getType() == T_R_ID) {
        for (Variable *var : reg_variables)
            if (var->getName() == token.getValue()) {
                errorFound = true;
                printAlreadyDeclaredError(token, var->getPosition());
            }

    } else if (token.getType() == T_ID) {
        for (Function *f : functions)
            if (f->getName() == token.getValue()) {
                errorFound = true;
                printAlreadyDeclaredError(token, f->getPosition());
            }
        for (Label *l : labels)
            if (l->getName() == token.getValue()) {
                errorFound = true;
                printAlreadyDeclaredError(token, l->getPosition());
            }
    } else {
        for (Variable *var : memory_variables)
            if (var->getName() == token.getValue()) {
                errorFound = true;
                printAlreadyDeclaredError(token, var->getPosition());
            }
    }
}

Variable *SyntaxAnalysis::findVariableByName(Token t) {
    if (t.getType() == T_R_ID) {
        for (Variable *v : reg_variables)
            if (v->getName() == t.getValue()) return v;
    } else {
        for (Variable *v : memory_variables)
            if (v->getName() == t.getValue()) return v;
    }

    return nullptr;
}

Label *SyntaxAnalysis::findLabelByName(Token t) {
    for (Label *l : labels)
        if (l->getName() == t.getValue()) return l;

    return nullptr;
}

void SyntaxAnalysis::linkJumpInstructions(Instruction *i, Label *l) {
    Instruction *realI = nullptr;
    Instruction *jumpToInstruction = l->nextInstruction(instructions);
    for (Instruction *in : instructions) if (in->getPos() == i->getPos()) realI = in;
    if (realI == nullptr & jumpToInstruction == nullptr) {
        return;
    } else {
        realI->addSucc(jumpToInstruction);
        jumpToInstruction->addPred(realI);
    }
}

void SyntaxAnalysis::linkWithPreviousInstruction(Instruction *i) {
    if (previousInstruction) {
        i->addPred(previousInstruction);
        previousInstruction->addSucc(i);
    }
}

void SyntaxAnalysis::eat(TokenType t) {
    if (!errorFound) {
        if (currentToken.getType() == t) {
            if (!secondPass)
                if (currentToken.getType() == T_R_ID || currentToken.getType() == T_M_ID) declared(currentToken);
            currentToken = getNextToken();
        } else {
            printSyntaxError(currentToken);
            errorFound = true;
        }
    }
}


Token SyntaxAnalysis::getNextToken() {
    if (tokenIterator == lexicalAnalysis.getTokenList().end())
        throw runtime_error("End of input file reached");
    return *tokenIterator++;
}


void SyntaxAnalysis::Q() {
    if (!errorFound) {
        S();
        eat(T_SEMI_COL);
        L();
    }
}


void SyntaxAnalysis::S() {
    if (!errorFound) {
        position++;
        if (currentToken.getType() == T_MEM) {
            eat(T_MEM);
            if (!secondPass) {
                exists(currentToken);
                memory_variables.push_back(
                        new Variable(currentToken.getValue(), position, Variable::VariableType::MEM_VAR));
            }
            eat(T_M_ID);
            memory_variables.back()->value = currentToken.getValue();
            eat(T_NUM);
        } else if (currentToken.getType() == T_REG) {
            eat(T_REG);
            if (!secondPass) {
                exists(currentToken);
                reg_variables.push_back(
                        new Variable(currentToken.getValue(), position, Variable::VariableType::REG_VAR));
            }
            eat(T_R_ID);
        } else if (currentToken.getType() == T_FUNC) {
            eat(T_FUNC);
            if (!secondPass) {
                exists(currentToken);
                functions.push_back(new Function(currentToken.getValue(), position));
            }
            eat(T_ID);
        } else if (currentToken.getType() == T_ID) {
            if (!secondPass) {
                exists(currentToken);
                labels.push_back(new Label(currentToken.getValue(), position));
            }
            eat(T_ID);
            eat(T_COL);
            E();
        } else {
            E();
        }
    }
}


void SyntaxAnalysis::L() {
    if (!errorFound) {
        if (currentToken.getType() == T_END_OF_FILE) {
            return;
        } else {
            Q();
        }
    }
}


void SyntaxAnalysis::E() {
    if (!errorFound) {
        Instruction *i = nullptr;
        Variables dest;
        Variables src;
        Variables use;
        Variables def;
        std::string literal = "0";
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
            i = new Instruction(position, I_ADD, dest, src, dest, src, literal, "");
        } else if (currentToken.getType() == T_ADDI) {
            eat(T_ADDI);
            dest.push_back(findVariableByName(currentToken));
            eat(T_R_ID);
            eat(T_COMMA);
            src.push_back(findVariableByName(currentToken));
            eat(T_R_ID);
            eat(T_COMMA);
            literal = currentToken.getValue();
            eat(T_NUM);
            i = new Instruction(position, I_ADDI, dest, src, dest, src, literal, "");
        } else if (currentToken.getType() == T_SUB) {
            eat(T_SUB);
            dest.push_back(findVariableByName(currentToken));
            eat(T_R_ID);
            eat(T_COMMA);
            src.push_back(findVariableByName(currentToken));
            eat(T_R_ID);
            eat(T_COMMA);
            src.push_back(findVariableByName(currentToken));
            eat(T_R_ID);
            i = new Instruction(position, I_SUB, dest, src, dest, src, literal, "");

        } else if (currentToken.getType() == T_LA) {
            eat(T_LA);
            dest.push_back(findVariableByName(currentToken));
            eat(T_R_ID);
            eat(T_COMMA);
            src.push_back(findVariableByName(currentToken));
            eat(T_M_ID);
            i = new Instruction(position, I_LA, dest, src, dest, use, literal, "");
        } else if (currentToken.getType() == T_LW) {
            eat(T_LW);
            dest.push_back(findVariableByName(currentToken));
            eat(T_R_ID);
            eat(T_COMMA);
            literal = currentToken.getValue();
            eat(T_NUM);
            eat(T_L_PARENT);
            src.push_back(findVariableByName(currentToken));
            eat(T_R_ID);
            eat(T_R_PARENT);
            i = new Instruction(position, I_LW, dest, src, dest, src, literal, "");
        } else if (currentToken.getType() == T_LI) {
            eat(T_LI);
            dest.push_back(findVariableByName(currentToken));
            eat(T_R_ID);
            eat(T_COMMA);
            literal = currentToken.getValue();
            eat(T_NUM);
            i = new Instruction(position, I_LI, dest, src, dest, src, literal, "");
        } else if (currentToken.getType() == T_SW) {
            eat(T_SW);
            src.push_back(findVariableByName(currentToken));
            eat(T_R_ID);
            eat(T_COMMA);
            literal = currentToken.getValue();
            eat(T_NUM);
            eat(T_L_PARENT);
            dest.push_back(findVariableByName(currentToken));
            eat(T_R_ID);
            eat(T_R_PARENT);
            i = new Instruction(position, I_SW, dest, src, dest, src, literal, "");
        } else if (currentToken.getType() == T_B) {
            eat(T_B);
            if (secondPass) declared(currentToken);
            Token label = currentToken;
            eat(T_ID);
            i = new Instruction(position, I_B, dest, src, dest, src, literal, label.getValue());
            if (secondPass && !errorFound) linkJumpInstructions(i, findLabelByName(label));
        } else if (currentToken.getType() == T_BLTZ) {
            eat(T_BLTZ);
            src.push_back(findVariableByName(currentToken));
            eat(T_R_ID);
            eat(T_COMMA);
            if (secondPass) declared(currentToken);
            Token label = currentToken;
            eat(T_ID);
            i = new Instruction(position, I_BLTZ, dest, src, dest, src, literal, label.getValue());
            if (secondPass && !errorFound) linkJumpInstructions(i, findLabelByName(label));
        } else if (currentToken.getType() == T_NOP) {
            eat(T_NOP);
            i = new Instruction(position, I_NOP, dest, src, dest, src, literal, "");
        } else if (currentToken.getType() == T_BEQ) {
            eat(T_BEQ);
            src.push_back(findVariableByName(currentToken));
            eat(T_R_ID);
            eat(T_COMMA);
            src.push_back(findVariableByName(currentToken));
            eat(T_R_ID);
            eat(T_COMMA);
            if (secondPass) declared(currentToken);
            Token label = currentToken;
            eat(T_ID);
            i = new Instruction(position, I_BEQ, dest, src, dest, src, literal, label.getValue());
            if (secondPass && !errorFound) linkJumpInstructions(i, findLabelByName(label));
        } else if (currentToken.getType() == T_ABS) {
            eat(T_ABS);
            dest.push_back(findVariableByName(currentToken));
            eat(T_R_ID);
            eat(T_COMMA);
            src.push_back(findVariableByName(currentToken));
            eat(T_R_ID);
            i = new Instruction(position, I_ABS, dest, src, dest, src, literal, "");
        } else if (currentToken.getType() == T_AND) {
            eat(T_AND);
            dest.push_back(findVariableByName(currentToken));
            eat(T_R_ID);
            eat(T_COMMA);
            src.push_back(findVariableByName(currentToken));
            eat(T_R_ID);
            eat(T_COMMA);
            src.push_back(findVariableByName(currentToken));
            eat(T_R_ID);
            i = new Instruction(position, I_AND, dest, src, dest, src, literal, "");
        } else {
            printSyntaxError(currentToken);
            errorFound = true;
            return;
        }
        if (!secondPass) instructions.push_back(i);
        linkWithPreviousInstruction(i);
        previousInstruction = i;
    }
}