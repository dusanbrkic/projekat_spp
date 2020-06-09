//
// Created by Dusan on 06-Jun-20.
//

#ifndef SRC_RESOURCEALLOCATION_H
#define SRC_RESOURCEALLOCATION_H

#include "IR.h"
#include "SyntaxAnalysis.h"
#include <map>

/**
 * This class represent interference graph and performs resource allocation
 */
class InterferenceGraph {
public:
    InterferenceGraph(SyntaxAnalysis&);
    void printMatrix();
    void DoInterferenceGraph();
    struct ResourceAllocationError : std::runtime_error {
        ResourceAllocationError() : runtime_error("\nException! Could not allocate resources!\n"){}
    };
private:
    void fillInterferenceGraph();
    std::stack<Variable*> simplify(bool);
    //performs resource allocation
    void doResourceAllocation();

    std::vector<std::vector<char>> matrix;
    Instructions& instructions;
	Variables& variables;
    int size;
    std::stack<Variable *> s;
};

#endif //SRC_RESOURCEALLOCATION_H
