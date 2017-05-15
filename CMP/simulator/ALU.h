#ifndef ALU_H_INCLUDED
#define ALU_H_INCLUDED
#include <bits/stdc++.h>
using namespace std;

class ALU{
public:
    unsigned int ALUResult;
    unsigned long long int ALUResult64;

    void ALUoperater(unsigned int data1, unsigned int data2, unsigned int ALUControl);
};

#endif // ALU_H_INCLUDED
