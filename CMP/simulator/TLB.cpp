#include "TLB.h"
#include <bits/stdc++.h>
using namespace std;

TLB::TLB(int pageSize){
    this->pageSize = pageSize;
    this->entryNum = 1024/pageSize/4;

    this->hit = this->miss = 0;

    physicalPageNumber = new unsigned int[entryNum];
    tag = new unsigned int[entryNum];
    lastRefCycle = new unsigned int[entryNum];
    valid = new bool[entryNum];

    fill(physicalPageNumber, physicalPageNumber+entryNum, 0);
    fill(tag, tag+entryNum, 0);
    fill(lastRefCycle, lastRefCycle+entryNum, 0);
    fill(valid, valid+entryNum, false);
}

unsigned int TLB::readTLB(unsigned int virtualPageNumber){
    for(int i=0 ; i<entryNum; i++){
		if(valid[i] && (tag[i] == virtualPageNumber)){
			return physicalPageNumber[i];
		}
	}
}

bool TLB::isInTLB(unsigned int virtualPageNumber){
    for(int i=0 ; i<entryNum ; i++){
		if(valid[i] && (tag[i] == virtualPageNumber)){
			return true;
		}
	}
	return false;
}

unsigned int TLB::findUsableEntry(){
    unsigned int index;
    unsigned int minCycle = 500000;
    for(int i=0; i<entryNum; i++){
        if(!valid[i]){
            return (unsigned int )i;
        }
        else{
            if(lastRefCycle[i] < minCycle){
                minCycle = lastRefCycle[i];
                index = i;
            }
        }
    }
    return index;
}

void TLB::updateLastCycle(unsigned int virtualPageNumber, int cycle){
    for(int i=0 ; i<entryNum; i++){
		if(valid[i] && (tag[i] == virtualPageNumber)){
			lastRefCycle[i] = cycle;
		}
	}
}

void TLB::updateTLB(unsigned int virtualPageNumber, unsigned int physicalPageNumber, int index , int cycle){
    this->physicalPageNumber[index] = physicalPageNumber;
    lastRefCycle[index] = cycle;
    tag[index] = virtualPageNumber;
    valid[index] = true;
}

void TLB::deleteEntry(unsigned int physicalPageNumber){
    for(int i=0; i<entryNum; i++){
        if(this->physicalPageNumber[i] == physicalPageNumber){
			valid[i] = false;
		}
    }
}
