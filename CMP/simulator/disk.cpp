#include "disk.h"
#include <bits/stdc++.h>
using namespace std;

extern unsigned int pc;
extern unsigned int initialSp;
extern bool halt;

bool isEmpty(FILE *file){
    long savedOffset = ftell(file);
    fseek(file, 0, SEEK_END);

    if (ftell(file) == 0){
        return true;
    }

    fseek(file, savedOffset, SEEK_SET);
    return false;
}

Disk::Disk(string imageFile){
    FILE *fptr;
    fptr = fopen(imageFile.c_str() , "rb");
    if(!fptr){
		halt = true;
		printf("open file error\n");
		return;
	}

    fill(disk, disk+1024, '\0');

    Mode mode = (imageFile == "iimage.bin") ? INST : DATA;

    nums = 0;
    unsigned char c;

    if(mode == INST){
    	pc = 0;
	    for(int i=0;i<4;i++){
            c = fgetc(fptr);
            pc += c << (8*(3-i));
        }
        for(int i=0;i<4;i++){
            c = fgetc(fptr);
            nums += c << (8*(3-i));
        }
        for(int i=0;i<nums;i++){
            for(int j=0;j<4;j++){
                c = fgetc(fptr);
                disk[pc+4*i+j] = c;
            }
        }
    }
    else if(mode == DATA){
        initialSp = 0;
		if(!isEmpty(fptr)){
            for(int i=0;i<4;i++){
                c = fgetc(fptr);
                if(c != EOF) initialSp += c << (8*(3-i));
                else break;
            }
            for(int i=0;i<4;i++){
                c = fgetc(fptr);
                if(c != EOF) nums += c << (8*(3-i));
                else break;
            }
            for(int i=0;i<nums;i++){
                for(int j=0;j<4;j++){
                    c = fgetc(fptr);
                    if(c != EOF) disk[4*i+j] = c;
                    else break;
                }
            }
        }
    }
    fclose(fptr);
}
