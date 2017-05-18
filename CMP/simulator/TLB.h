#ifndef TLB_H_INCLUDED
#define TLB_H_INCLUDED

class TLB{
public:
    int pageSize;
    int entryNum;

    int* physicalPageNumber;
    int* tag;
    unsigned int* lastRefCycle;
    bool* valid;

    int hit;
    int miss;

    TLB(){};
    TLB(int pageSize);

    unsigned int readTLB(unsigned int virtualPageNumber);

    bool isInTLB(unsigned int virtualPageNumber);
    unsigned int findUsableEntry();

    void updateLastCycle(unsigned int virtualPageNumber, int cycle);
    void updateTLB(unsigned int virtualPageNumber, unsigned int physicalPageNumber, int index , int cycle);
    void deleteEntry(unsigned int physicalPageNumber);
};

#endif // TLB_H_INCLUDED
