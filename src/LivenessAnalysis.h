#ifndef __LIVENESS_ANALYSIS__
#define __LIVENESS_ANALYSIS__

#include "Types.h"
#include "IR.h"
#include "SyntaxAnalysis.h"


/**
 * This class performs liveness analysis
 */
class LivenessAnalysis {
public:
    LivenessAnalysis(SyntaxAnalysis&);

    //Method that implements liveness analysis algorithm
    void DoLivenessAnalysis();
private:
    Instructions& instructions;
};

#endif
