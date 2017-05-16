#include "processor.h"
#include "disk.h"
#include "memory.h"
#include "pageTable.h"
#include "TLB.h"
#include "cache.h"
#include "errorDetect.h"
#include "instruction.h"
#include "ALU.h"
#include "regFile.h"
#include "arithmetic.h"

#include <bits/stdc++.h>

#define ADD 0x20
#define ADDU 0x21
#define SUB 0x22
#define AND 0x24
#define OR 0x25
#define XOR 0x26
#define NOR 0x27
#define NAND 0x28
#define SLT 0x2A
#define SLL 0x00
#define SRL 0x02
#define SRA 0x03
#define JR 0x08
#define MULT 0x18
#define MULTU 0x19
#define MFHI 0x10
#define MFLO 0x12

#define ADDI 0x08
#define ADDIU 0x09
#define LW 0x23
#define LH 0x21
#define LHU 0x25
#define LB 0x20
#define LBU 0x24
#define SW 0x2b
#define SH 0x29
#define SB 0x28
#define LUI 0x0f
#define ANDI 0x0c
#define ORI 0x0d
#define NORI 0x0e
#define SLTI 0x0a
#define BEQ 0x04
#define BNE 0x05
#define BGTZ 0x07

#define J 0x02
#define JAL 0x03
using namespace std;

extern Disk IDisk;
extern Memory IMemory;
extern PageTable IPageTable;
extern TLB ITLB;
extern Cache ICache;

extern Disk DDisk;
extern Memory DMemory;
extern PageTable DPageTable;
extern TLB DTLB;
extern Cache DCache;

extern ErrorDetect ED;
extern RegisterFile RF;
extern ALU ALU1;

extern unsigned int pc;
extern bool halt;
extern unsigned int HI;
extern unsigned int LO;
extern bool traceMEM[6];

unsigned int fetchInst(int cycle){
    unsigned int inst = 0;
    unsigned int virtualPageNumber = pc/IMemory.pageSize;

    //TLB hit
    if(ITLB.isInTLB(virtualPageNumber)){
        unsigned int physicalPageNumber = ITLB.readTLB(virtualPageNumber);

        unsigned int pageOffset = pc%IMemory.pageSize;
        unsigned int physicalAddr = physicalPageNumber*IMemory.pageSize + pageOffset;
        unsigned int blockAddr = physicalAddr/ICache.blockSize;
        unsigned int physicalAddrTag = blockAddr/ICache.setNum;
        unsigned int cacheIndex = blockAddr%ICache.setNum;//belong to which set
        unsigned int blockOffset = physicalAddr%ICache.blockSize;

        //cache hit
        if(ICache.isInCache(cacheIndex, physicalAddrTag)){
            for(int i=0; i<4; i++) inst += ICache.readCache(cacheIndex, physicalAddrTag, blockOffset+i) << (8*(3-i));
            ICache.updateMRU(cacheIndex, physicalAddrTag);
            //update memory?
            traceMEM[0] = true;
            ICache.hit++;
        }
        //memory hit
        else{
            IMemory.moveFromIMEMToICache(cacheIndex, physicalAddrTag, physicalPageNumber, pageOffset, blockOffset);
            for(int i=0; i<4; i++) inst += ICache.readCache(cacheIndex, physicalAddrTag, blockOffset+i) << (8*(3-i));

            ICache.updateMRU(cacheIndex, physicalAddrTag);
            //update memory?
            ICache.miss++;
        }
        ITLB.updateLastCycle(virtualPageNumber, cycle);

        traceMEM[1] = true;
        ITLB.hit++;
    }
    //TLB miss
    else{
        //page table hit
        if(IPageTable.checkValid(virtualPageNumber)){
            unsigned int physicalPageNumber = IPageTable.readPageTable(virtualPageNumber);

            unsigned int pageOffset = pc%IMemory.pageSize;
            unsigned int physicalAddr = physicalPageNumber*IMemory.pageSize + pageOffset;
            unsigned int blockAddr = physicalAddr/ICache.blockSize;
            unsigned int physicalAddrTag = blockAddr/ICache.setNum;
            unsigned int cacheIndex = blockAddr%ICache.setNum;//belong to which set
            unsigned int blockOffset = physicalAddr%ICache.blockSize;

            //cache hit
            if(ICache.isInCache(cacheIndex, physicalAddrTag)){
                for(int i=0; i<4; i++) inst += ICache.readCache(cacheIndex, physicalAddrTag, blockOffset+i) << (8*(3-i));

                ICache.updateMRU(cacheIndex, physicalAddrTag);
                IMemory.updateLastCycle(physicalPageNumber, cycle);
                traceMEM[0] = true;
                ICache.hit++;
            }
            //memory hit
            else{
                IMemory.moveFromIMEMToICache(cacheIndex, physicalAddrTag, physicalPageNumber, pageOffset, blockOffset);
                for(int i=0; i<4; i++) inst += ICache.readCache(cacheIndex, physicalAddrTag, blockOffset+i) << (8*(3-i));

                ICache.updateMRU(cacheIndex, physicalAddrTag);
                IMemory.updateLastCycle(physicalPageNumber, cycle);
                ICache.miss++;
            }
            int index = ITLB.findUsableEntry();
            ITLB.updateTLB(virtualPageNumber, physicalPageNumber, index, cycle);
            IPageTable.hit++;
        }
        //page fault
        else{
            for(int i=0; i<4; i++) inst += IDisk.disk[pc+i] << (8*(3-i));
            unsigned int physicalPageNumber = IMemory.findUsableEntry();

            unsigned int pageOffset = pc%IMemory.pageSize;
            unsigned int physicalAddr = physicalPageNumber*IMemory.pageSize + pageOffset;
            unsigned int blockAddr = physicalAddr/ICache.blockSize;
            unsigned int physicalAddrTag = blockAddr/ICache.setNum;
            unsigned int cacheIndex = blockAddr%ICache.setNum;//belong to which set
            unsigned int blockOffset = physicalAddr%ICache.blockSize;

            //if there is a master, remove original master from cache and TLB
            if(IMemory.memoryEntries[physicalPageNumber].valid){
                ICache.deleteIEntry(physicalPageNumber);
                ITLB.deleteEntry(physicalPageNumber);
            }
            //move data to mem
            IMemory.moveFromIDiskToIMem(physicalPageNumber, pageOffset, pc);
            ///DEBUG CHECK PAGE CONTEXT
            //IMemory.readMemory(physicalPageNumber);

            //update memory last cycle
            IMemory.updateLastCycle(physicalPageNumber, cycle);

            //move data to cache
            IMemory.moveFromIMEMToICache(cacheIndex, physicalAddrTag, physicalPageNumber, pageOffset,
                                         blockOffset);
            ICache.updateMRU(cacheIndex, physicalAddrTag);
            //update PTE
            IPageTable.updatePageTable(virtualPageNumber, physicalPageNumber);

            //update TLB
            int index = ITLB.findUsableEntry();
            ITLB.updateTLB(virtualPageNumber, physicalPageNumber, index, cycle);

            traceMEM[2] = true;

            ICache.miss++;
            IPageTable.miss++;
        }
        ITLB.miss++;
    }
    return inst;
}
unsigned char getData(unsigned int dataAddress, int cycle, int offset){
    unsigned char data = 0;
    unsigned int virtualPageNumber = dataAddress/DMemory.pageSize;

    //TLB hit
    if(DTLB.isInTLB(virtualPageNumber)){
        unsigned int physicalPageNumber = DTLB.readTLB(virtualPageNumber);

        unsigned int pageOffset = dataAddress%DMemory.pageSize;
        unsigned int physicalAddr = physicalPageNumber*DMemory.pageSize + pageOffset;
        unsigned int blockAddr = physicalAddr/DCache.blockSize;
        unsigned int physicalAddrTag = blockAddr/DCache.setNum;
        unsigned int cacheIndex = blockAddr%DCache.setNum;//belong to which set
        unsigned int blockOffset = physicalAddr%DCache.blockSize;

        //cache hit
        if(DCache.isInCache(cacheIndex, physicalAddrTag)){
            data = DCache.readCache(cacheIndex, physicalAddrTag, blockOffset);
            
			//update memory?
            if(offset == 0){
           		DCache.updateMRU(cacheIndex, physicalAddrTag);
            	DMemory.updateLastCycle(physicalPageNumber, cycle);	
			
			    traceMEM[3] = true;
                DCache.hit++;
            }
        }
        //memory hit
        else{
            DMemory.moveFromDMEMToDCache(cacheIndex, physicalAddrTag, physicalPageNumber, pageOffset, blockOffset);
            data = DCache.readCache(cacheIndex, physicalAddrTag, blockOffset);
          

            if(offset == 0){
            	DCache.updateMRU(cacheIndex, physicalAddrTag);
            	DMemory.updateLastCycle(physicalPageNumber, cycle);

				DCache.miss++;
        	}
		}
        if(offset == 0){
        	DTLB.updateLastCycle(virtualPageNumber, cycle);
            traceMEM[4] = true;
            DTLB.hit++;
        }
    }
    //TLB miss
    else{
        //page table hit
        if(DPageTable.checkValid(virtualPageNumber)){
            unsigned int physicalPageNumber = DPageTable.readPageTable(virtualPageNumber);

            unsigned int pageOffset = dataAddress%DMemory.pageSize;
            unsigned int physicalAddr = physicalPageNumber*DMemory.pageSize + pageOffset;
            unsigned int blockAddr = physicalAddr/DCache.blockSize;
            unsigned int physicalAddrTag = blockAddr/DCache.setNum;
            unsigned int cacheIndex = blockAddr%DCache.setNum;//belong to which set
            unsigned int blockOffset = physicalAddr%DCache.blockSize;
            //cache hit
            if(DCache.isInCache(cacheIndex, physicalAddrTag)){
                data = DCache.readCache(cacheIndex, physicalAddrTag, blockOffset);
                if(offset == 0){
                	DCache.updateMRU(cacheIndex, physicalAddrTag);
                	DMemory.updateLastCycle(physicalPageNumber, cycle);
                    
					traceMEM[3] = true;
                    DCache.hit++;
                }
            }
            //memory hit
            else{
                DMemory.moveFromDMEMToDCache(cacheIndex, physicalAddrTag, physicalPageNumber, pageOffset, blockOffset);
                data = DCache.readCache(cacheIndex, physicalAddrTag, blockOffset);
               
                if(offset == 0){
					DCache.updateMRU(cacheIndex, physicalAddrTag);
					DMemory.updateLastCycle(physicalPageNumber, cycle);
				
					DCache.miss++;
            	}
			}
            if(offset == 0){
            	int index = DTLB.findUsableEntry();
            	DTLB.updateTLB(virtualPageNumber, physicalPageNumber, index, cycle);
	
				DPageTable.hit++;
        	}
		}
        //page fault
        else{
            data = DDisk.disk[dataAddress];
            unsigned int physicalPageNumber = DMemory.findUsableEntry();

            unsigned int pageOffset = dataAddress%DMemory.pageSize;
            unsigned int physicalAddr = physicalPageNumber*DMemory.pageSize + pageOffset;
            unsigned int blockAddr = physicalAddr/DCache.blockSize;
            unsigned int physicalAddrTag = blockAddr/DCache.setNum;
            unsigned int cacheIndex = blockAddr%DCache.setNum;//belong to which set
            unsigned int blockOffset = physicalAddr%DCache.blockSize;

            //if there is a master, remove original master from cache and TLB
            if(DMemory.memoryEntries[physicalPageNumber].valid){
                DCache.deleteDEntry(physicalPageNumber);
                DTLB.deleteEntry(physicalPageNumber);
            }
            //move data to mem
            DMemory.moveFromDDiskToDMem(physicalPageNumber, pageOffset, dataAddress);
            //move data to cache
            DMemory.moveFromDMEMToDCache(cacheIndex, physicalAddrTag, physicalPageNumber, pageOffset,
                                         blockOffset);

            if(offset == 0){
                //update memory last cycle
                DMemory.updateLastCycle(physicalPageNumber, cycle);
                //update PTE
                DPageTable.updatePageTable(virtualPageNumber, physicalPageNumber);
                //update TLB
                int index = DTLB.findUsableEntry();
                DTLB.updateTLB(virtualPageNumber, physicalPageNumber, index, cycle);
                //update cache
                DCache.updateMRU(cacheIndex, physicalAddrTag);

				traceMEM[5] = true;
                DCache.miss++;
                DPageTable.miss++;
            }
        }
        if(offset == 0)DTLB.miss++;
    }
    return data;
}
void writeBack(unsigned int dataAddress, unsigned char data, int cycle, int offset){
    unsigned int virtualPageNumber = dataAddress/DMemory.pageSize;

    //TLB hit
    if(DTLB.isInTLB(virtualPageNumber)){
        unsigned int physicalPageNumber = DTLB.readTLB(virtualPageNumber);

        unsigned int pageOffset = dataAddress%DMemory.pageSize;
        unsigned int physicalAddr = physicalPageNumber*DMemory.pageSize + pageOffset;
        unsigned int blockAddr = physicalAddr/DCache.blockSize;
        unsigned int physicalAddrTag = blockAddr/DCache.setNum;
        unsigned int cacheIndex = blockAddr%DCache.setNum;//belong to which set
        unsigned int blockOffset = physicalAddr%DCache.blockSize;
        //cache hit
        if(DCache.isInCache(cacheIndex, physicalAddrTag)){
            DCache.writeCache(cacheIndex, physicalAddrTag, blockOffset, data);
            //write to memory
            DMemory.writeMemory(physicalPageNumber, pageOffset, data);
            
            if(offset == 0){
          		DCache.updateMRU(cacheIndex, physicalAddrTag);
           		DMemory.updateLastCycle(physicalPageNumber, cycle);     

				traceMEM[3] = true;
                DCache.hit++;
            }
        }
        //memory hit
        else{
            //write to memory
            DMemory.writeMemory(physicalPageNumber, pageOffset, data);
            //move data to cache
            DMemory.moveFromDMEMToDCache(cacheIndex, physicalAddrTag, physicalPageNumber, pageOffset, blockOffset);
          
            if(offset == 0){
		  		DCache.updateMRU(cacheIndex, physicalAddrTag);		
            	DMemory.updateLastCycle(physicalPageNumber, cycle);
				DCache.miss++;
        	}
		}
        DDisk.disk[dataAddress] = data;
        if(offset == 0){
        	DTLB.updateLastCycle(virtualPageNumber, cycle);
            traceMEM[4] = true;
            DTLB.hit++;
        }
    }
    //TLB miss
    else{
        //PTE hit
        if(DPageTable.checkValid(virtualPageNumber)){
            unsigned int physicalPageNumber = DPageTable.readPageTable(virtualPageNumber);

            unsigned int pageOffset = dataAddress%DMemory.pageSize;
            unsigned int physicalAddr = physicalPageNumber*DMemory.pageSize + pageOffset;
            unsigned int blockAddr = physicalAddr/DCache.blockSize;
            unsigned int physicalAddrTag = blockAddr/DCache.setNum;
            unsigned int cacheIndex = blockAddr%DCache.setNum;//belong to which set
            unsigned int blockOffset = physicalAddr%DCache.blockSize;
            //cache hit
            if(DCache.isInCache(cacheIndex, physicalAddrTag)){
                DCache.writeCache(cacheIndex, physicalAddrTag, blockOffset, data);
                //write to memory
                DMemory.writeMemory(physicalPageNumber, pageOffset, data);
                
                if(offset == 0){
               		DCache.updateMRU(cacheIndex, physicalAddrTag);
               		DMemory.updateLastCycle(physicalPageNumber, cycle);     
					
					traceMEM[3] = true;
                    DCache.hit++;
                }
            }
            //memory hit
            else{
                //write to memory
                DMemory.writeMemory(physicalPageNumber, pageOffset, data);
                //move data to cache
                DMemory.moveFromDMEMToDCache(cacheIndex, physicalAddrTag, physicalPageNumber, pageOffset, blockOffset);
                if(offset == 0){
                	DCache.updateMRU(cacheIndex, physicalAddrTag);
                	DMemory.updateLastCycle(physicalPageNumber, cycle);
				
					DCache.miss++;
            	}
			}
            DDisk.disk[dataAddress] = data;
            if(offset == 0){
				int index = DTLB.findUsableEntry();
				DTLB.updateTLB(virtualPageNumber, physicalPageNumber, index, cycle);
			
				DPageTable.hit++;
        	}
		}
        //page fault
        else{
            DDisk.disk[dataAddress] = data;
            unsigned int physicalPageNumber = DMemory.findUsableEntry();

            unsigned int pageOffset = dataAddress%DMemory.pageSize;
            unsigned int physicalAddr = physicalPageNumber*DMemory.pageSize + pageOffset;
            unsigned int blockAddr = physicalAddr/DCache.blockSize;
            unsigned int physicalAddrTag = blockAddr/DCache.setNum;
            unsigned int cacheIndex = blockAddr%DCache.setNum;//belong to which set
            unsigned int blockOffset = physicalAddr%DCache.blockSize;

            if(DMemory.memoryEntries[physicalPageNumber].valid){
                //remove original one from cache
                DCache.deleteDEntry(physicalPageNumber);
                //remove from TLB
                DTLB.deleteEntry(physicalPageNumber);
            }
            DMemory.moveFromDDiskToDMem(physicalPageNumber, pageOffset, dataAddress);

            DMemory.moveFromDMEMToDCache(cacheIndex, physicalAddrTag, physicalPageNumber, pageOffset, blockOffset);
            
			if(offset == 0){
   				DMemory.updateLastCycle(physicalPageNumber, cycle);
                DCache.updateMRU(cacheIndex, physicalAddrTag);
                DPageTable.updatePageTable(virtualPageNumber, physicalPageNumber);
                int index = DTLB.findUsableEntry();
                DTLB.updateTLB(virtualPageNumber, physicalPageNumber, index, cycle);

				
				traceMEM[5] = true;
                DCache.miss++;
                DPageTable.miss++;
            }
        }
        if(offset == 0)DTLB.miss++;
    }
}
void execute(Instruction inst, int cycle){
    if(inst.type == 'S' && inst.name == "halt"){
        halt = true;
    }
    else if(inst.type == 'S' && inst.name == "sll"){
		pc += 4;
        return;
    }
    else if(inst.type == 'J'){
        if(inst.opCode == JAL){//ra = pc + 4
            RF.readWrite(0, 0, 31, pc + 4, 1);//reg write
        }
        pc = (((pc+4) & 0xf0000000) | (inst.targetAddr << 2));
    }
    else if(inst.type == 'R'){
        RF.readWrite(inst.regRs, inst.regRt, 0, 0, 0);//reg read
        if(inst.func == ADD || inst.func == ADDU){
            ALU1.ALUoperater(RF.readData1, RF.readData2, 0);
            RF.readWrite(0, 0, inst.regRd, ALU1.ALUResult, 1);
        }
        else if(inst.func == SUB){
            ALU1.ALUoperater(RF.readData1, RF.readData2, 1);
            bitset<32> data2ChangeSign(RF.readData2);
            RF.readWrite(0, 0, inst.regRd, ALU1.ALUResult, 1);
        }
        else if(inst.func == AND){
            ALU1.ALUoperater(RF.readData1, RF.readData2, 2);
            RF.readWrite(0, 0, inst.regRd, ALU1.ALUResult, 1);
        }
        else if(inst.func == OR){
            ALU1.ALUoperater(RF.readData1, RF.readData2, 3);
            RF.readWrite(0, 0, inst.regRd, ALU1.ALUResult, 1);
        }
        else if(inst.func == XOR){
            ALU1.ALUoperater(RF.readData1, RF.readData2, 4);
            RF.readWrite(0, 0, inst.regRd, ALU1.ALUResult, 1);
        }
        else if(inst.func == NOR){
            ALU1.ALUoperater(RF.readData1, RF.readData2, 5);
            RF.readWrite(0, 0, inst.regRd, ALU1.ALUResult, 1);
        }
        else if(inst.func == NAND){
            ALU1.ALUoperater(RF.readData1, RF.readData2, 6);
            RF.readWrite(0, 0, inst.regRd, ALU1.ALUResult, 1);
        }
        else if(inst.func == SLT){
            ALU1.ALUoperater(RF.readData1, RF.readData2, 7);
            RF.readWrite(0, 0, inst.regRd, ALU1.ALUResult, 1);
        }
        else if(inst.func == SLL){
            ALU1.ALUoperater(RF.readData2, inst.shamt, 8);
            RF.readWrite(0, 0, inst.regRd, ALU1.ALUResult, 1);
        }
        else if(inst.func == SRL){
            ALU1.ALUoperater(RF.readData2, inst.shamt, 9);
            RF.readWrite(0, 0, inst.regRd, ALU1.ALUResult, 1);
        }
        else if(inst.func == SRA){
            ALU1.ALUoperater(RF.readData2, inst.shamt, 10);
            RF.readWrite(0, 0, inst.regRd, ALU1.ALUResult, 1);
        }
        else if(inst.func == JR){
            pc = RF.readData1;
        }
        else if(inst.func == MULT){
            ALU1.ALUoperater(RF.readData1, RF.readData2, 11);
            HI = (ALU1.ALUResult64 >> 32)%4294967296;
            LO = ALU1.ALUResult64%4294967296;
        }
        else if(inst.func == MULTU){
            ALU1.ALUoperater(RF.readData1, RF.readData2, 12);
            HI = (ALU1.ALUResult64 >> 32)%4294967296;
            LO = ALU1.ALUResult64%4294967296;
        }
        else if(inst.func == MFHI){
            RF.readWrite(0, 0, inst.regRd, HI, 1);
        }
        else if(inst.func == MFLO){
            RF.readWrite(0, 0, inst.regRd, LO, 1);
        }
        else{//unrecognized instruction
            printf("illegal instruction found at 0x%X\n", pc);
            return;
        }
        //pc = pc + 4
        if(inst.func != JR){//skip jr
            pc += 4;
        }
    }
    else if(inst.type == 'I'){
        if(inst.opCode == BEQ || inst.opCode == BNE || inst.opCode == BGTZ){
            bool satisfy = false;
            unsigned int signExtenedImme = signExtend(inst.immediate, 16);
            RF.readWrite(inst.regRs, inst.regRt, 0, 0, 0);//reg read

            if(inst.opCode == BEQ)
                satisfy = (RF.readData1 == RF.readData2)?true:false;
            else if(inst.opCode == BNE)
                satisfy = (RF.readData1 != RF.readData2)?true:false;
            else if(inst.opCode == BGTZ)
                satisfy = ((RF.readData1>>31) >0 && RF.readData1!=0)?true:false;

            pc = (satisfy)?(pc+4+(signExtenedImme << 2)):(pc+4);
        }
        else{
            if(inst.opCode == LW || inst.opCode == LH || inst.opCode == LHU || inst.opCode == LB || inst.opCode ==  LBU){//load
                unsigned int signExtendImme = signExtend(inst.immediate, 16);
                RF.readWrite(inst.regRs, inst.regRt, 0, 0, 0);//reg read
                ALU1.ALUoperater(signExtendImme, RF.readData1, 0);//$s + C(un/signed)

                ///error detect
                if(ED.memoryAddressOverflow(inst.opCode, ALU1.ALUResult) || ED.dataMisaligned(inst.opCode, ALU1.ALUResult)){
                    halt = true;
                    return;
                }

                if(inst.opCode == LW){
                    unsigned char data[4];
                    for(int i=0; i<4; i++){
                        data[i] = getData(ALU1.ALUResult+i, cycle, i);
                    }
                    RF.readWrite(0, 0, inst.regRt, (data[0]<<24 | data[1]<<16 | data[2]<<8 | data[3]), 1);
                }
                else if(inst.opCode == LH || inst.opCode == LHU){
                    unsigned char data[2];
                    for(int i=0; i<2; i++){
                        data[i] = getData(ALU1.ALUResult+i, cycle, i);
                    }
                    if(inst.opCode == LH){
                        if(data[0]>>7 == 1)
                            RF.readWrite(0, 0, inst.regRt, (0XFFFF0000|(data[0]<<8 | data[1])), 1);
                        else
                            RF.readWrite(0, 0, inst.regRt, (data[0]<<8 | data[1]), 1);
                    }
                    else
                        RF.readWrite(0, 0, inst.regRt, (data[0]<<8 | data[1]), 1);
                }
                else if(inst.opCode == LB || inst.opCode == LBU){
                    unsigned char data;
                    data = getData(ALU1.ALUResult, cycle, 0);
                    if(inst.opCode == LB){
                        if(data>>7 == 1)
                            RF.readWrite(0, 0, inst.regRt, (0XFFFFFF00|data<<8), 1);
                        else
                            RF.readWrite(0, 0, inst.regRt, data, 1);
                    }
                    else
                        RF.readWrite(0, 0, inst.regRt, data, 1);
                }
            }
            else if(inst.opCode == SW || inst.opCode == SH || inst.opCode ==  SB){//store
                unsigned int signExtenedImme = signExtend(inst.immediate, 16);
                RF.readWrite(inst.regRs, inst.regRt, 0, 0, 0);
                ALU1.ALUoperater(signExtenedImme, RF.readData1, 0);//$s + C(signed)

                ///error detect

                if(ED.memoryAddressOverflow(inst.opCode, ALU1.ALUResult) || ED.dataMisaligned(inst.opCode, ALU1.ALUResult)){
                    halt = true;
                    return;
                }
                ///TODO write back
                if(inst.opCode == SW){
                    unsigned char data[4];
                    //printf("%08x\n", RF.readData2);
                    data[0] = (RF.readData2 & 0XFF000000)>>24;
                    data[1] = (RF.readData2 & 0X00FF0000)>>16;
                    data[2] = (RF.readData2 & 0X0000FF00)>>8;
                    data[3] = (RF.readData2 & 0X000000FF);
                    for(int i=0; i<4; i++){
                       // printf("%2x", data[i]);
                        writeBack(ALU1.ALUResult+i, data[i], cycle , i);
                    }
                   // printf("\n");
                }
                else if(inst.opCode == SH){
                    unsigned char data[2];
                    data[0] = (RF.readData2 & 0X0000FF00)>>8;
                    data[1] = (RF.readData2 & 0X000000FF);
                    for(int i=0; i<2; i++)
                        writeBack(ALU1.ALUResult+i, data[i], cycle , i);
                }
                else if(inst.opCode == SB){
                    unsigned char data;
                    data = (RF.readData2 & 0X000000FF);
                    writeBack(ALU1.ALUResult, data, cycle , 0);
                }
            }
            else if(inst.opCode == ADDI || inst.opCode == ADDIU){//addi addiu
                RF.readWrite(inst.regRs, inst.regRt, 0, 0, 0);//reg read
                ALU1.ALUoperater(RF.readData1, signExtend(inst.immediate, 16),  0);
                RF.readWrite(0, 0, inst.regRt, ALU1.ALUResult, 1);//reg write
            }
            else if(inst.opCode == LUI || inst.opCode == ANDI || inst.opCode == ORI || inst.opCode == NORI || inst.opCode == SLTI){
                RF.readWrite(inst.regRs, inst.regRt, 0, 0, 0);//reg read
                //directly assign alu control
                if(inst.opCode == LUI)
                    ALU1.ALUoperater(inst.immediate, 16, 8);//shift <<
                else if(inst.opCode == ANDI)
                    ALU1.ALUoperater(RF.readData1, inst.immediate, 2);//andi
                else if(inst.opCode == ORI)
                    ALU1.ALUoperater(RF.readData1, inst.immediate, 3);//ori
                else if(inst.opCode == NORI)
                    ALU1.ALUoperater(RF.readData1, inst.immediate, 5);//nori
                else if(inst.opCode == SLTI)
                    ALU1.ALUoperater(RF.readData1, signExtend(inst.immediate, 16), 7);//slti
                RF.readWrite(0, 0, inst.regRt, ALU1.ALUResult, 1);//reg write
            }
            else{//unrecognized instruction
                printf("illegal instruction found at 0x%X\n", pc);
                return;
            }
            pc += 4;
        }
    }
}
