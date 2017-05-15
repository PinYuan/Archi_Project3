#include "memory.h"
#include "disk.h"
#include "pageTable.h"
#include "TLB.h"
#include "cache.h"
#include <bits/stdc++.h>
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

Memory::Memory(int memorySize, int pageSize){
    this->memorySize = memorySize;
    this->pageSize = pageSize;
    this->pageNum = memorySize/pageSize;
    this->wordsNum = memorySize/4;

    memoryEntries = new MemoryEntry[pageNum];
    for(int i=0; i<pageNum; i++){
        memoryEntries[i].valid = false;
        memoryEntries[i].lastRefCycle = 0;
        memoryEntries[i].memory = new unsigned char[pageSize];
        //initial memory
        fill(memoryEntries[i].memory, memoryEntries[i].memory+pageSize,'\0');
    }

}
void Memory::readMemory(unsigned int physicalPageNumber){
    for(int i=0; i<pageSize; i++){
        printf("%02X", memoryEntries[physicalPageNumber].memory[i]);
    }
    printf("\n");
}
void Memory::writeMemory(unsigned int physicalPageNumber, unsigned int pageOffset, unsigned char data){
    memoryEntries[physicalPageNumber].memory[pageOffset] = data;
}
void Memory::moveFromIMEMToICache(unsigned int cacheIndex, unsigned int physicalAddressTag,
                                unsigned int physicalPageNumber, unsigned int pageOffset,
                                unsigned int blockOffset){
    int usableCacheBlockIndex = ICache.findUsableBlock(cacheIndex);
    //cout << "usableCacheBlockIndex "<< usableCacheBlockIndex<< endl;
    ICache.cacheSet[cacheIndex].cacheBlock[usableCacheBlockIndex].valid = true;
    ICache.cacheSet[cacheIndex].cacheBlock[usableCacheBlockIndex].tag = physicalAddressTag;
    for(int i=0; i<ICache.blockSize; i++){
        ICache.cacheSet[cacheIndex].cacheBlock[usableCacheBlockIndex].data[i] = memoryEntries[physicalPageNumber].memory[pageOffset-blockOffset+i];
    }
}
void Memory::moveFromIDiskToIMem(unsigned int physicalPageNumber, unsigned int pageOffset, int pc){
    memoryEntries[physicalPageNumber].valid = true;
    for(int i=0; i<pageSize; i++){
        memoryEntries[physicalPageNumber].memory[i] = IDisk.disk[pc - pageOffset + i];
    }
}
void Memory::moveFromDMEMToDCache(unsigned int cacheIndex, unsigned int physicalAddressTag, unsigned int physicalPageNumber,
                            unsigned int pageOffset, unsigned int blockOffset){
    int usableCacheBlockIndex = DCache.findUsableBlock(cacheIndex);
    DCache.cacheSet[cacheIndex].cacheBlock[usableCacheBlockIndex].valid = true;
    DCache.cacheSet[cacheIndex].cacheBlock[usableCacheBlockIndex].tag = physicalAddressTag;
    for(int i=0; i<DCache.blockSize; i++){
        DCache.cacheSet[cacheIndex].cacheBlock[usableCacheBlockIndex].data[i] = memoryEntries[physicalPageNumber].memory[pageOffset-blockOffset+i];
    }
}
void Memory::moveFromDDiskToDMem(unsigned int physicalPageNumber, unsigned int pageOffset, unsigned int dataAddress){
    memoryEntries[physicalPageNumber].valid = true;
    for(int i=0; i<pageSize; i++){
        memoryEntries[physicalPageNumber].memory[i] = DDisk.disk[dataAddress - pageOffset + i];
    }
}
unsigned int Memory::findUsableEntry(){
    unsigned int index;
    unsigned int minCycle = 500000;
    for(int i=0; i<pageNum; i++){
        if(!memoryEntries[i].valid)
            return (unsigned int)i;
        else{
            if(memoryEntries[i].lastRefCycle < minCycle){
                minCycle = memoryEntries[i].lastRefCycle;
                index = i;
            }
        }
    }
    return index;
}
void Memory::updateLastCycle(unsigned int physicalPageNumber, int cycle){
    memoryEntries[physicalPageNumber].lastRefCycle = cycle;
}
