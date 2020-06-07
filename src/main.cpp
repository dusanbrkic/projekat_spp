#include <iostream>
#include "LexicalAnalysis.h"
#include "SyntaxAnalysis.h"
#include "LivenessAnalysis.h"
#include "ResourceAllocation.h"

using namespace std;

int main() {
    try {
        std::string fileName = "..\\..\\examples\\simple.mavn";
        bool retVal = false;

        LexicalAnalysis lex;

        if (!lex.readInputFile(fileName))
            throw runtime_error("\nException! Failed to open input file!\n");

        lex.initialize();

        retVal = lex.Do();

        if (retVal) {
            cout << "Lexical analysis finished successfully!" << endl;
        } else {
            lex.printLexError();
            throw runtime_error("\nException! Lexical analysis failed!\n");
        }

        SyntaxAnalysis syn(lex);

        retVal = syn.Do();

        if (retVal) {
            cout << "Syntax analysis finished successfully!" << endl;
        } else {
            throw runtime_error("\nException! Syntax analysis failed!\n");
        }

        LivenessAnalysis livenessAnalysis(syn);
        livenessAnalysis.DoLivenessAnalysis();

        InterferenceGraph ig(syn);
        ig.DoInterferenceGraph();
    }
    catch (runtime_error e) {
        cout << e.what() << endl;
        return 1;
    }

    return 0;
}