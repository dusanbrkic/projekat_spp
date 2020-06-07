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

std::stack<Variable *> InterferenceGraph::simplify() {
    Variables::iterator iterator = variables.begin();
    std::map<Variable *, int> m;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (matrix[i][j] == '1') {
                if (m.find(*iterator) == m.end())
                    m.insert(std::pair<Variable *, int>(*iterator, 0));
                else m[*iterator]++;
            }
        }
        iterator++;
    }

    std::multimap<int, Variable *> flipped_map;
    for (std::map<Variable *, int>::iterator i = m.begin(); i != m.end(); i++)
        flipped_map.insert(std::pair<int, Variable *>(i->second, i->first));

    std::multimap<int, Variable *>::reverse_iterator i = flipped_map.rbegin();
    while (i->first >= __REG_NUMBER__) {
        i++;
        if (i == flipped_map.rend())
            throw InterferenceGraph::ResourceAllocationError();
    }
    s.push(i->second);
    for (int k = 0; k < size; k++) {
        matrix.at(i->second->index).at(k) = '0';
        matrix.at(k).at(i->second->index) = '0';
    }
    for (int h = 0; h < size; h++)
        for (int g = 0; g < size; g++)
            if (matrix[h][g] != '0')
                simplify();
    return s;
}

void InterferenceGraph::DoInterferenceGraph() {
    fillInterferenceGraph(instructions);
    simplify();
    fillInterferenceGraph(instructions);
}
