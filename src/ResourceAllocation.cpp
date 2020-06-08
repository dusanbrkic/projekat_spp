//
// Created by Dusan on 06-Jun-20.
//
#include "ResourceAllocation.h"

void InterferenceGraph::fillInterferenceGraph(Instructions &instructions) {
    for (Instruction *i : instructions) {
        for (Variable *v : i->m_def) {
            for (Variable *out : i->m_out) {
                matrix[v->index][out->index] = '1';
                matrix[out->index][v->index] = '1';
            }
        }
    }
}

InterferenceGraph::InterferenceGraph(SyntaxAnalysis &syn) : variables(syn.reg_variables),
                                                            size(syn.reg_variables.size()),
                                                            matrix(syn.reg_variables.size(),
                                                                   std::vector<char>(syn.reg_variables.size(), '0')),
                                                            instructions(syn.instructions), s() {}

void InterferenceGraph::printMatrix() {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            std::cout << matrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

std::stack<Variable *> InterferenceGraph::simplify(bool firstPass) {
    Variables::iterator iterator;
    std::map<Variable *, int> m;

    // puts '0'-s on main diagonal (creates adjacency matrix)
    if(firstPass) for (int i = 0; i < size; i++) matrix[i][i] = '0';

    //insert all non deleted vertices to map
    iterator = variables.begin();
    for (int i = 0; i < size; i++) {
        if (matrix[i][i] != 'd')
            m.insert(std::pair<Variable *, int>(*iterator, 0));
        iterator++;
    }

    iterator = variables.begin();

    //increment degrees of vertices if they have adjacent vertices
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (matrix[i][j] == '1')
                m[*iterator]++;
        }
        iterator++;
    }

    //flips map to <value, key>, multimap can have more exact keys
    std::multimap<int, Variable *> flipped_map;
    for (std::map<Variable *, int>::iterator i = m.begin(); i != m.end(); i++)
        flipped_map.insert(std::pair<int, Variable *>(i->second, i->first));

    // multimap is sorted, starting from the end (the greatest value)
    std::multimap<int, Variable *>::reverse_iterator i = flipped_map.rbegin();
    // next section removes the vertex if it has the greatest degree in graph of less than __REG_NUMBER__
    while (i->first >= __REG_NUMBER__) {
        i++;
        if (i == flipped_map.rend())
            throw InterferenceGraph::ResourceAllocationError();
    }
    //push vertex to the stack
    s.push(i->second);
    //remove the vertex from matrix
    for (int k = 0; k < size; k++) {
        matrix.at(i->second->index).at(k) = 'd';
        matrix.at(k).at(i->second->index) = 'd';
    }
    // if matrix has non deleted elements --> recursion
    for (int h = 0; h < size; h++)
        for (int g = 0; g < size; g++)
            if (matrix[h][g] != 'd')
                simplify(false);
    return s;
}


void InterferenceGraph::doResourceAllocation() {
    Variable *currentVariable;
    Variables save;
    int color;
    bool find;

    while (!s.empty()) {
        currentVariable = s.top();
        s.pop();

        save.push_back(currentVariable);

        if (save.size() == 1) {
            // first variable on stack
            currentVariable->m_assignment = (Regs) 1;
        } else {
            // there are other variables, need to assign color
            Variables temp;
            // get variable from stack which are interference with currentVariable
            // put them in temp list
            for (Variable *el : save)
                if (matrix[currentVariable->index][el->index] == __INTERFERENCE__)
                    temp.push_back(el);

            // find diffrent color
            for (color = 1; color <= __REG_NUMBER__; color++) {
                find = true;
                for (Variable *el : temp) {
                    if (color == el->m_assignment) {
                        find = false;
                    }
                }
                if (find) {
                    currentVariable->m_assignment = (Regs) color;
                    break;
                }
            }
        }
    }
}

void InterferenceGraph::DoInterferenceGraph() {
    fillInterferenceGraph(instructions);
    simplify(true);
    fillInterferenceGraph(instructions);
    doResourceAllocation();
}
