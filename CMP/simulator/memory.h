#ifndef MEMORY_H_INCLUDED
#define MEMORY_H_INCLUDED
#include <bits/stdc++.h>
using namespace std;

class Memory{
public:
    class MemoryEntry{
    public:
        unsigned char* memory;
        bool valid;
        unsigned int lastRefCycle;
    };
    int wordsNum;
    int memorySize;
    int pageSize;
    int pageNum;

    MemoryEntry* memoryEntries;

    Memory(){};
    Memory(int memorySize, int pageSize);

    //additional
    void readMemory(unsigned int physicalPageNumber);
    void writeMemory(unsigned int physicalPageNumber, unsigned int pageOffset, unsigned char data);

    void moveFromIMEMToICache(unsigned int cacheIndex, unsigned int physicalAddressTag, unsigned int physicalPageNumber,
                            unsigned int pageOffset, unsigned int blockOffset);
    void moveFromIDiskToIMem(unsigned int physicalPageNumber, unsigned int pageOffset, int pc);
    void moveFromDMEMToDCache(unsigned int cacheIndex, unsigned int physicalAddressTag, unsigned int physicalPageNumber,
                            unsigned int pageOffset, unsigned int blockOffset);
    void moveFromDDiskToDMem(unsigned int physicalPageNumber, unsigned int pageOffset, unsigned int dataAddress);

    unsigned int findUsableEntry();

    void updateLastCycle(unsigned int physicalPageNumber, int cycle);
};


#endif // MEMORY_H_INCLUDED
