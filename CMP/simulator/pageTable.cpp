#include "pageTable.h"
#include <bits/stdc++.h>
using namespace std;

PageTable::PageTable(int pageSize){
    this->pageSize = pageSize;
    this->entryNum = 1024/pageSize;//page_table_entries = disk_size / page_size

    this->hit = this->miss = 0;

    physicalPageNumber = new unsigned int[entryNum];
    valid = new bool [entryNum];

    fill(physicalPageNumber, physicalPageNumber+entryNum, 0);
    fill(valid, valid+entryNum, false);
}
unsigned int PageTable::readPageTable(unsigned int virtualPageNumber){
    return physicalPageNumber[virtualPageNumber];
}
bool PageTable::checkValid(unsigned int virtualPageNumber){
    return valid[virtualPageNumber];
}
void PageTable::updatePageTable(unsigned int virtualPageNumber, unsigned int physicalPageNumber){
    //original physical page number data should be invalid
    for(int i=0; i<entryNum; i++){
        if(this->physicalPageNumber[i] == physicalPageNumber){
            valid[i] = false;
            break;
        }
    }
    //move into memory
    this->physicalPageNumber[virtualPageNumber] = physicalPageNumber;
    valid[virtualPageNumber] = true;
}
