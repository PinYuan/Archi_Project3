#ifndef DISK_H_INCLUDED
#define DISK_H_INCLUDED
#include <bits/stdc++.h>
using namespace std;

enum Mode{INST, DATA};

class Disk{
public:
    unsigned char disk[1024];
    int nums;
    Disk(){};
    Disk(string imageFile);
};


#endif // DISK_H_INCLUDED
