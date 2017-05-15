#ifndef CACHE_H_INCLUDED
#define CACHE_H_INCLUDED

class Cache{
public:
    class CacheBlock{
    public:
        unsigned char* data;
        unsigned int tag;
        bool valid;
        unsigned int MRU;
    };
    class CacheSet{
    public:
        CacheBlock* cacheBlock;
    };
    int blockSize;
    int cacheSize;
    int setAssociativity;
    int blockNum;
    int setNum;

    int hit;
    int miss;

    CacheSet* cacheSet;

    Cache(){};
    Cache(int cacheSize, int blockSize,int setAssociativity);

    unsigned char readCache(unsigned cacheIndex, unsigned int physicalAddressTag, unsigned int blockOffset);
    void writeCache(unsigned cacheIndex, unsigned int physicalAddressTag, unsigned int blockOffset, unsigned char data);

    bool isInCache(unsigned cacheIndex, unsigned int physicalAddressTag);
    unsigned int findUsableBlock(unsigned cacheIndex);

    void updateMRU(unsigned cacheIndex, unsigned int physicalAddressTag);
    void deleteIEntry(unsigned int physicalPageNumber);
    void deleteDEntry(unsigned int physicalPageNumber);

    void printCache();
};

#endif // CACHE_H_INCLUDED
