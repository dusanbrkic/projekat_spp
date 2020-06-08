//
// Created by Dusan on 07-Jun-20.
//

#include "MipsCode.h"

MipsCode::MipsCode(SyntaxAnalysis &analysis, std::string string) : syntax(analysis), fileName(string) {}

void MipsCode::writeMipsCode() {
    std::ofstream output(fileName);

    //sort labels by position
    Label *minPointer = syntax.labels.front();
    for (Labels::iterator li = syntax.labels.begin(); li != syntax.labels.end(); li++) {
        for (Labels::iterator li2 = syntax.labels.begin(); li2 != syntax.labels.end(); li2++) {
            if ((*li2)->pos <= (*li)->pos) minPointer = *li2;
        }
        std::swap(**li, *minPointer);
    }

    Functions::iterator fi = syntax.functions.begin();
    Labels::iterator li = syntax.labels.begin();
    Instructions::iterator ii = syntax.instructions.begin();

    while (fi != syntax.functions.end()) {
        //func name
        output << ".globl " << (*fi)->getName() << std::endl;
        fi++;
    }
    //memory variables
    output << std::endl << ".data" << std::endl << std::endl;
    for (auto el : syntax.memory_variables)
        output << el->getName() << ":" << std::setw(LEFT_ALIGN) << ".word " << el->value << std::endl;
    output << std::endl;

    //instructions and labels
    output << ".text" << std::endl;
    //if there are no instructions
    if (*ii == nullptr) {
        while (li != syntax.labels.end()) {
            output << (*li)->getName() << ":" << std::endl;
            li++;
        }
        output.close();
        return;
    }
    //main
    while (true) {
        if (li != syntax.labels.end() && ii != syntax.instructions.end()) {
            if ((*li)->pos <= (*ii)->m_position) {
                output << (*li)->getName() << ":" << std::endl;
                li++;
            } else {
                (*ii)->writeInfo(output);
                ii++;
            }
        } else if (li != syntax.labels.end()) {
            while (li != syntax.labels.end()) {
                output << (*li)->getName() << ":" << std::endl;
                li++;
            }
        } else if (ii != syntax.instructions.end()) {
            while (ii != syntax.instructions.end()) {
                (*ii)->writeInfo(output);
                ii++;
            }
        } else {
            break;
        }
    }


    output.close();
}


