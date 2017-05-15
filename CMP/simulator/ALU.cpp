#include <bits/stdc++.h>
#include "ALU.h"
using namespace std;
extern unsigned int HI;
extern unsigned int LO;

void ALU::ALUoperater(unsigned int data1, unsigned int data2, unsigned int ALUControl){
    if(ALUControl == 0){//add
        ALUResult = data1 + data2;
    }
    else if(ALUControl == 1){//sub
        ALUResult = data1 - data2;
    }
    else if(ALUControl == 2){//and
        ALUResult = data1 & data2;
    }
    else if(ALUControl == 3){//or
        ALUResult = data1 | data2;
    }
    else if(ALUControl == 4){//xor
        ALUResult = data1 ^ data2;
    }
    else if(ALUControl == 5){//nor
        ALUResult = ~(data1 | data2);
    }
    else if(ALUControl == 6){//nand
        ALUResult = ~(data1 & data2);
    }
    else if(ALUControl == 7){//slt
        unsigned int signBit1 = data1 >> 31;
        unsigned int signBit2 = data2 >> 31;

        if(signBit1 == 0 && signBit2 == 0)//+ +
            ALUResult =  data1 < data2;
        else if(signBit1 == 1 && signBit2 == 0)//- +
            ALUResult = 1;
        else if(signBit1 == 0 && signBit2 == 1)//+ -
            ALUResult = 0;
        else
            ALUResult = data1 < data2;
    }
    else if(ALUControl == 8){//sll
        ALUResult = data1 << data2;
    }
    else if(ALUControl == 9){//srl
        ALUResult = data1 >> data2;
    }
    else if(ALUControl == 10){//sra
        bitset<32> data1Bit(data1);
        bool neg = data1Bit[31]?1:0;
        data1Bit >>= data2;
        if(neg){
            for(int i=0;i<data2;i++)
                data1Bit.set(31-i,1);
        }
        ALUResult = data1Bit.to_ulong();
    }
    else if(ALUControl == 11 || ALUControl == 12){//mult multu
        bitset<32> rs(data1), rt(data2);
        bitset<64> ALUResultBit64(0);
        if(ALUControl == 11){//mult
            //turn - => +
            bitset<32> absRs(0);
            bitset<32> absRt(0);
            if(rs[31] == 1){
                absRs = bitset<32>(rs.flip().to_ulong()+1);
                rs.flip();
            }
            else absRs = rs;
            if(rt[31] == 1){
                absRt = bitset<32>(rt.flip().to_ulong()+1);
                rt.flip();
            }
            else absRt = rt;
            for(int i=0;i<32;i++){
                if(absRt[i] == 1){
                    bitset<64> temp = bitset<64>(absRs.to_ulong());
                    temp <<= i;
                    ALUResultBit64 = bitset<64>(ALUResultBit64.to_ullong()+temp.to_ullong());
                }
            }
            if(rs[31] ^ rt[31])
                ALUResultBit64 = bitset<64>(ALUResultBit64.flip().to_ullong()+1);
        }
        else{//multu
            for(int i=0;i<32;i++){
                if(rt[i] == 1){
                    bitset<64> temp = bitset<64>(rs.to_ulong());
                    temp <<= i;
                    ALUResultBit64 = bitset<64>(ALUResultBit64.to_ullong()+temp.to_ullong());
                }
            }
        }
        ALUResult64 = ALUResultBit64.to_ullong();
    }
}
