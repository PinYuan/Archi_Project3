#ifndef INSTRUCTION_H_INCLUDED
#define INSTRUCTION_H_INCLUDED
#include <bits/stdc++.h>
using namespace std;

class Instruction{
public:
    unsigned int completeInst;
    unsigned int opCode;
    unsigned int regRs;
    unsigned int regRt;
    unsigned int regRd;
    unsigned int shamt;
    unsigned int func;
    unsigned int immediate;
    unsigned int targetAddr;
    char type;
    string name;

    Instruction(void);
    Instruction(unsigned int instruction);
};

#endif // INSTRUCTION_H_INCLUDED
