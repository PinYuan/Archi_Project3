#ifndef PAGETABLE_H_INCLUDED
#define PAGETABLE_H_INCLUDED

class PageTable{
public:
    int pageSize;
    int entryNum;

    int hit;
    int miss;

    unsigned int* physicalPageNumber;
    bool* valid;

    PageTable(){};
    PageTable(int pageSize);

    unsigned int readPageTable(unsigned int virtualPageNumber);

    bool checkValid(unsigned int virtualPageNumber);

    void updatePageTable(unsigned int virtualPageNumber, unsigned int physicalPageNumber);
};

#endif // PAGETABLE_H_INCLUDED
