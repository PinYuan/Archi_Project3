#include "cache.h"
#include "memory.h"
#include <bits/stdc++.h>

using namespace std;

extern Memory IMemory;
extern Memory DMemory;

Cache::Cache(int cacheSize, int blockSize,int setAssociativity){
    this->cacheSize = cacheSize;
    this->blockSize = blockSize;
    this->setAssociativity = setAssociativity;
    this->blockNum = cacheSize/blockSize;
    this->setNum = blockNum/setAssociativity;

    this->hit = this->miss = 0;

    cacheSet = new CacheSet[setNum];
    for(int i=0; i<setNum; i++){
        cacheSet[i].cacheBlock = new CacheBlock[setAssociativity];
    }
    for(int i=0; i<setNum; i++){
        for(int j=0; j<setAssociativity; j++){
            cacheSet[i].cacheBlock[j].data = new unsigned char[blockSize];
            //initial cache
            fill(cacheSet[i].cacheBlock[j].data, cacheSet[i].cacheBlock[j].data+blockSize, '\0');
            cacheSet[i].cacheBlock[j].tag = 0;
            cacheSet[i].cacheBlock[j].valid = false;
            cacheSet[i].cacheBlock[j].MRU = 0;
        }
    }
}
unsigned char Cache::readCache(unsigned cacheIndex, unsigned int physicalAddressTag, unsigned int blockOffset){
    for(int i=0; i<setAssociativity; i++){
		if((cacheSet[cacheIndex].cacheBlock[i].valid) &&
            (cacheSet[cacheIndex].cacheBlock[i].tag == physicalAddressTag)){
			return cacheSet[cacheIndex].cacheBlock[i].data[blockOffset];
		}
	}
}
void Cache::writeCache(unsigned cacheIndex, unsigned int physicalAddressTag, unsigned int blockOffset, unsigned char data){
    for(int i=0; i<setAssociativity; i++){
		if((cacheSet[cacheIndex].cacheBlock[i].valid) &&
            (cacheSet[cacheIndex].cacheBlock[i].tag == physicalAddressTag)){
            cacheSet[cacheIndex].cacheBlock[i].data[blockOffset] = data;
		}
	}
}
bool Cache::isInCache(unsigned int cacheIndex, unsigned int physicalAddressTag){
    for(int i=0; i<setAssociativity; i++){
		if((cacheSet[cacheIndex].cacheBlock[i].valid) &&
            (cacheSet[cacheIndex].cacheBlock[i].tag == physicalAddressTag)){
			return true;
		}
	}
	return false;
}

unsigned int Cache::findUsableBlock(unsigned int cacheIndex){
    int MRUZero = 0;
    bool findZero = false;
    for(int i=0; i<setAssociativity; i++){
        if(!(cacheSet[cacheIndex].cacheBlock[i].valid)){
             return (unsigned int)i;
        }
        else if(cacheSet[cacheIndex].cacheBlock[i].MRU == 0 && !findZero){
                MRUZero = i;
                findZero = true;
        }
    }
    return MRUZero;
}

void Cache::updateMRU(unsigned cacheIndex, unsigned int physicalAddressTag){
    for(int i=0; i<setAssociativity; i++){
        if((cacheSet[cacheIndex].cacheBlock[i].valid) &&
            (cacheSet[cacheIndex].cacheBlock[i].tag == physicalAddressTag)){
			cacheSet[cacheIndex].cacheBlock[i].MRU = 1;
		}
    }
    //check if all one
    bool allOne = true;
    for(int i=0; i<setAssociativity; i++){
        if(!cacheSet[cacheIndex].cacheBlock[i].valid || cacheSet[cacheIndex].cacheBlock[i].MRU == 0){
            allOne = false;
            break;
        }
    }
    //if all one ,reset
    if(allOne){
        for(int i=0; i<setAssociativity; i++){
            cacheSet[cacheIndex].cacheBlock[i].MRU = 0;
            if(cacheSet[cacheIndex].cacheBlock[i].tag == physicalAddressTag)
                cacheSet[cacheIndex].cacheBlock[i].MRU = 1;
        }
    }
}
void Cache::deleteIEntry(unsigned int physicalPageNumber){
    //belong to this page, all need to be delete
    for(int i=0; i<setNum; i++){
        for(int j=0; j<setAssociativity; j++){
            //???
            unsigned int pageOffset = i*blockSize%IMemory.pageSize;
            unsigned int ppn = (cacheSet[i].cacheBlock[j].tag*blockSize*setNum + pageOffset)/IMemory.pageSize;
            if(ppn == physicalPageNumber){
                cacheSet[i].cacheBlock[j].valid = false;
                cacheSet[i].cacheBlock[j].MRU = 0;
            }
        }
    }
}
void Cache::deleteDEntry(unsigned int physicalPageNumber){
    //belong to this page, all need to be delete
    for(int i=0; i<setNum; i++){
        for(int j=0; j<setAssociativity; j++){
            //???
            unsigned int pageOffset = i*blockSize%DMemory.pageSize;
            unsigned int ppn = (cacheSet[i].cacheBlock[j].tag*blockSize*setNum + pageOffset)/DMemory.pageSize;
            if(ppn == physicalPageNumber){
                cacheSet[i].cacheBlock[j].valid = false;
                cacheSet[i].cacheBlock[j].MRU = 0;
            }
        }
    }
}
