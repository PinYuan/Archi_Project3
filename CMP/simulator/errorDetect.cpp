#include <bits/stdc++.h>
#include "errorDetect.h"

#define LW 0x23
#define LH 0x21
#define LHU 0x25
#define LB 0x20
#define LBU 0x24
#define SW 0x2b
#define SH 0x29
#define SB 0x28

using namespace std;

ErrorDetect::ErrorDetect(){
    cycle = 0;
    mult = mf = 0;
}
void ErrorDetect::setCycle(int cycle){
    this->cycle = cycle;
}
int ErrorDetect::memoryAddressOverflow(unsigned int opCode, unsigned int address){
    int halt = 0;
    unsigned int maxAddress = 0;
    if(opCode == LW || opCode == SW)//word
        maxAddress = address+3;
    else if(opCode == LH || opCode == LHU || opCode == SH)//half
        maxAddress = address+1;
    else//byte
        maxAddress = address;
    if(address > 1023 || maxAddress > 1023)
        halt = 1;
    return halt;
}
int ErrorDetect::dataMisaligned(unsigned int opCode, unsigned int address){
    int halt = 0;
    if(opCode == LW || opCode == SW){//word lw sw
        if(address%4 != 0)
            halt = 1;
    }
    else if(opCode == LH || opCode == LHU || opCode == SH){//half lh lhu sh
        if(address%2 != 0)
            halt = 1;
    }
    return halt;
}
