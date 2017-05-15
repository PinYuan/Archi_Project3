#include <bits/stdc++.h>
#include "regFile.h"
using namespace std;

extern unsigned int pc;
extern unsigned int initialSp;
extern unsigned int HI;
extern unsigned int LO;

RegisterFile::RegisterFile(){
    FILE* fptr;
    fptr = fopen("snapshot.rpt", "w");
    fclose(fptr);

    for(int i=0;i<32;i++){
        if(i == 29){//sp
            oldRegisters[i] = registers[i] = initialSp;
            continue;
        }
        oldRegisters[i] = registers[i] = 0;
    }
    HI = oldHI = 0;
    LO = oldLO = 0;
}

void RegisterFile::readWrite(unsigned int readRegister1, unsigned int readRegister2, unsigned int writeRegister
               , unsigned int writeData, bool regWrite){
    if(regWrite == true){
        if(writeRegister != 0)
            registers[writeRegister] = writeData;
    }
    else{
        readData1 = registers[readRegister1];
        readData2 = registers[readRegister2];
    }
}

void RegisterFile::outputRegister(int cycle){
    FILE* fptr;
    fptr = fopen("snapshot.rpt", "a");
    if(!fptr){printf("open file error\n");return;}

    if(cycle == 0){//all print
        fprintf(fptr, "cycle %d\n", cycle);
        //reg
        for(int i=0;i<32;i++){
            fprintf(fptr, "$%02d: 0x%08X\n", i, registers[i]);
        }

        //HI LO
        fprintf(fptr, "$HI: 0x%08X\n", HI);
        fprintf(fptr, "$LO: 0x%08X\n", LO);
        //pc
        fprintf(fptr, "PC: 0x%08X\n\n\n", pc);
    }
    else{//only print change
        fprintf(fptr, "cycle %d\n", cycle);

        //reg check diff
        for(int i=0;i<32;i++){
            if(oldRegisters[i] != registers[i]){
                fprintf(fptr, "$%02d: 0x%08X\n", i, registers[i]);
                oldRegisters[i] = registers[i];
            }
        }

        //HI LO
        if(oldHI != HI){
            fprintf(fptr, "$HI: 0x%08X\n", HI);
            oldHI = HI;
        }
        if(oldLO != LO){
            fprintf(fptr, "$LO: 0x%08X\n", LO);
            oldLO = LO;
        }
        //pc
        fprintf(fptr, "PC: 0x%08X\n\n\n", pc);
    }
    fclose(fptr);
}


