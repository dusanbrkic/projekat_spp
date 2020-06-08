cmake_minimum_required(VERSION 3.15)
project(src)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(src
        Constants.h
        FiniteStateMachine.cpp
        FiniteStateMachine.h
        IR.h
        LexicalAnalysis.cpp
        LexicalAnalysis.h
        main.cpp
        SyntaxAnalysis.cpp
        SyntaxAnalysis.h
        Token.cpp
        Token.h
        Types.h
        LivenessAnalysis.h
        LivenessAnalysis.cpp
        ResourceAllocation.h
        ResourceAllocation.cpp MipsCode.cpp MipsCode.h)
