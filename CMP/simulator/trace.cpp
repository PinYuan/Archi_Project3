#include "trace.h"
#include "instruction.h"
#include <bits/stdc++.h>

using namespace std;

extern bool traceMEM[6];

void initTrace(){
    FILE* fptrTrace;
    fptrTrace = fopen("trace.rpt", "w");
    if(!fptrTrace){printf("open trace file error\n");return;}
    fclose(fptrTrace);
}
void printTrace(int cycle, Instruction inst){
    FILE* fptr;
    fptr = fopen("trace.rpt", "a");
    if(!fptr){printf("open trace file error\n");return;}

    fprintf(fptr, "%d, %-4s : ", cycle, inst.name.c_str());

    if(traceMEM[1]) fprintf(fptr, "ITLB  ");
    if(traceMEM[0]) fprintf(fptr, "ICache  ");

    if(traceMEM[2]) fprintf(fptr, "Disk  ");
    fprintf(fptr, ";  ");
    if(traceMEM[4]) fprintf(fptr, "DTLB  ");
    if(traceMEM[3]) fprintf(fptr, "DCache  ");
    if(traceMEM[5]) fprintf(fptr, "Disk  ");
    fprintf(fptr, "\n");
    fclose(fptr);
}
