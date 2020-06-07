//
// Created by Dusan on 06-Jun-20.
//

#ifndef SRC_RESOURCEALLOCATION_H
#define SRC_RESOURCEALLOCATION_H

#include "IR.h"
#include "SyntaxAnalysis.h"
#include <map>

class InterferenceGraph {
public:
    InterferenceGraph(SyntaxAnalysis&);
    void printMatrix();
    void DoInterferenceGraph();
    struct ResourceAllocationError : std::runtime_error {
        ResourceAllocationError() : runtime_error("Could not allocate resources!"){}
    };
private:
    void fillInterferenceGraph(Instructions&);
    std::stack<Variable*> simplify();

    std::vector<std::vector<char>> matrix;
    Instructions& instructions;
	Variables& variables;
    int size;
    std::stack<Variable *> s;
};

#endif //SRC_RESOURCEALLOCATION_H
