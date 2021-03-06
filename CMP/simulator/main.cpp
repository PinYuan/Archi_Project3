#include <bits/stdc++.h>
#include "ALU.h"
#include "instruction.h"
#include "disk.h"
#include "memory.h"
#include "cache.h"
#include "pageTable.h"
#include "TLB.h"
#include "regFile.h"
#include "errorDetect.h"
#include "processor.h"
#include "trace.h"

using namespace std;
unsigned int pc;
unsigned int initialSp;
unsigned int HI;
unsigned int LO;
bool illegal;
bool traceMEM[6];//0 ICache, 1 ITLB, 2 IDisk, 3 DCache, 4 DTLB, 5 DDisk

Disk IDisk = Disk("iimage.bin");
Memory IMemory;
PageTable IPageTable;
TLB ITLB;
Cache ICache;

Disk DDisk = Disk("dimage.bin");
Memory DMemory;
PageTable DPageTable;
TLB DTLB;
Cache DCache;

ErrorDetect ED;
RegisterFile RF;
ALU ALU1;

bool halt;

void printHitMiss();

int main(int argc , char *argv[])
{
    ios::sync_with_stdio(false);
    cin.tie(0);

    int IMemorySize, DMemorySize, IMemoryPageSize, DMemoryPageSize,
        totalICacheSize, ICacheBlockSize, ICacheSetAssociativity,
        totalDCacheSize, DCacheBlockSize, DCacheSetAssociativity;

    //assign command line argument
    IMemorySize = 64;
    DMemorySize = 32;
    IMemoryPageSize =  8;
    DMemoryPageSize = 16;
    totalICacheSize = 16;
    ICacheBlockSize = 4;
    ICacheSetAssociativity = 4;
    totalDCacheSize = 16;
    DCacheBlockSize = 4;
    DCacheSetAssociativity = 1;
	
	if(argc == 2){
        IMemorySize = atoi(argv[1]);
    }
    else if(argc == 3){
        IMemorySize = atoi(argv[1]);
        DMemorySize = atoi(argv[2]);
    }
    else if(argc == 4){
        IMemorySize = atoi(argv[1]);
        DMemorySize = atoi(argv[2]);
        IMemoryPageSize =  atoi(argv[3]);
    }
    else if(argc == 5){
        IMemorySize = atoi(argv[1]);
        DMemorySize = atoi(argv[2]);
        IMemoryPageSize =  atoi(argv[3]);
        DMemoryPageSize = atoi(argv[4]);
    }
    else if(argc == 6){
        IMemorySize = atoi(argv[1]);
        DMemorySize = atoi(argv[2]);
        IMemoryPageSize =  atoi(argv[3]);
        DMemoryPageSize = atoi(argv[4]);
        totalICacheSize = atoi(argv[5]);
    }
    else if(argc == 7){
        IMemorySize = atoi(argv[1]);
        DMemorySize = atoi(argv[2]);
        IMemoryPageSize =  atoi(argv[3]);
        DMemoryPageSize = atoi(argv[4]);
        totalICacheSize = atoi(argv[5]);
        ICacheBlockSize = atoi(argv[6]);
    }
    else if(argc == 8){
        IMemorySize = atoi(argv[1]);
        DMemorySize = atoi(argv[2]);
        IMemoryPageSize =  atoi(argv[3]);
        DMemoryPageSize = atoi(argv[4]);
        totalICacheSize = atoi(argv[5]);
        ICacheBlockSize = atoi(argv[6]);
        ICacheSetAssociativity = atoi(argv[7]);
    }
    else if(argc == 9){
        IMemorySize = atoi(argv[1]);
        DMemorySize = atoi(argv[2]);
        IMemoryPageSize =  atoi(argv[3]);
        DMemoryPageSize = atoi(argv[4]);
        totalICacheSize = atoi(argv[5]);
        ICacheBlockSize = atoi(argv[6]);
        ICacheSetAssociativity = atoi(argv[7]);
        totalDCacheSize = atoi(argv[8]);
    }
    else if(argc == 10){
        IMemorySize = atoi(argv[1]);
        DMemorySize = atoi(argv[2]);
        IMemoryPageSize =  atoi(argv[3]);
        DMemoryPageSize = atoi(argv[4]);
        totalICacheSize = atoi(argv[5]);
        ICacheBlockSize = atoi(argv[6]);
        ICacheSetAssociativity = atoi(argv[7]);
        totalDCacheSize = atoi(argv[8]);
        DCacheBlockSize = atoi(argv[9]);
    }
    else if(argc == 11){
        IMemorySize = atoi(argv[1]);
        DMemorySize = atoi(argv[2]);
        IMemoryPageSize =  atoi(argv[3]);
        DMemoryPageSize = atoi(argv[4]);
        totalICacheSize = atoi(argv[5]);
        ICacheBlockSize = atoi(argv[6]);
        ICacheSetAssociativity = atoi(argv[7]);
        totalDCacheSize = atoi(argv[8]);
        DCacheBlockSize = atoi(argv[9]);
        DCacheSetAssociativity = atoi(argv[10]);
    }

    halt = false;
    illegal = false;

    IMemory = Memory(IMemorySize, IMemoryPageSize);
    IPageTable = PageTable(IMemoryPageSize);
    ITLB = TLB(IMemoryPageSize);
    ICache = Cache(totalICacheSize, ICacheBlockSize, ICacheSetAssociativity);

    DMemory = Memory(DMemorySize, DMemoryPageSize);
    DPageTable = PageTable(DMemoryPageSize);
    DTLB = TLB(DMemoryPageSize);
    DCache = Cache(totalDCacheSize, DCacheBlockSize, DCacheSetAssociativity);

	 //no image file
    if(halt) return 0;

    int cycle = 0;

    RF.outputRegister(cycle);

    initTrace();

    while(1){
        cycle++;

        //reset trace
        fill(traceMEM, traceMEM+6, false);

        Instruction instruction(fetchInst(cycle));
        execute(instruction, cycle);

        printTrace(cycle, instruction);

        if(halt) break;
        RF.outputRegister(cycle);
    }
    //report.rpt
    printHitMiss();
}
void printHitMiss(){
    FILE* fptr;
    fptr = fopen("report.rpt", "w");
    if(!fptr){printf("open report file error\n");return;}
    fprintf(fptr, "ICache :\n");
    fprintf(fptr, "# hits: %d\n", ICache.hit);
    fprintf(fptr, "# misses: %d\n", ICache.miss);
    fprintf(fptr, "\n");

    fprintf(fptr, "DCache :\n");
    fprintf(fptr, "# hits: %d\n", DCache.hit);
    fprintf(fptr, "# misses: %d\n", DCache.miss);
    fprintf(fptr, "\n");

    fprintf(fptr, "ITLB :\n");
    fprintf(fptr, "# hits: %d\n", ITLB.hit);
    fprintf(fptr, "# misses: %d\n", ITLB.miss);
    fprintf(fptr, "\n");

    fprintf(fptr, "DTLB :\n");
    fprintf(fptr, "# hits: %d\n", DTLB.hit);
    fprintf(fptr, "# misses: %d\n", DTLB.miss);
    fprintf(fptr, "\n");

    fprintf(fptr, "IPageTable :\n");
    fprintf(fptr, "# hits: %d\n", IPageTable.hit);
    fprintf(fptr, "# misses: %d\n", IPageTable.miss);
    fprintf(fptr, "\n");

    fprintf(fptr, "DPageTable :\n");
    fprintf(fptr, "# hits: %d\n", DPageTable.hit);
    fprintf(fptr, "# misses: %d\n", DPageTable.miss);
    fprintf(fptr, "\n");
    fclose(fptr);
}
