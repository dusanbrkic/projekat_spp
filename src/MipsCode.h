//
// Created by Dusan on 07-Jun-20.
//

#ifndef SRC_MIPSCODE_H
#define SRC_MIPSCODE_H

#include "SyntaxAnalysis.h"
#include <iomanip>

/**
 * This class creates MIPS code
 */
class MipsCode {
public:
    MipsCode(SyntaxAnalysis&, std::string);

    // writes MIPS code to file
    void writeMipsCode();
private:
    SyntaxAnalysis& syntax;
    std::string fileName;
};

#endif //SRC_MIPSCODE_H
