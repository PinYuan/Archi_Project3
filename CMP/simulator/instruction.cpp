#include <bits/stdc++.h>
#include "instruction.h"
using namespace std;

extern unsigned int pc;
extern unsigned int illegal;

map<int, string> instFunc{
    {0x20, "add"}, {0x21, "addu"}, {0x22 ,"sub"}, {0x24, "and"}, {0x25, "or"}, {0x26, "xor"},
    {0x27, "nor"}, {0x28, "nand"}, {0x2A, "slt"}, {0x00, "sll"}, {0x02, "srl"}, {0x03, "sra"},
    {0x08, "jr"}, {0x18, "mult"}, {0x19, "multu"}, {0x10, "mfhi"}, {0x12, "mflo"},
};
map<int, string> instOpcode{
    {0x08, "addi"}, {0x09, "addiu"}, {0x23, "lw"}, {0x21, "lh"}, {0x25, "lhu"}, {0x20, "lb"},
    {0x24, "lbu"}, {0x2b, "sw"}, {0x29, "sh"}, {0x28, "sb"}, {0x0f, "lui"}, {0x0c, "andi"},
    {0x0d, "ori"},{0x0e, "nori"}, {0x0a, "slti"}, {0x04, "beq"}, {0x05, "bne"}, {0x07, "bgtz"},

    {0x02, "j"}, {0x03, "jal"}
};
Instruction::Instruction(void){
    completeInst = opCode = regRs = regRt = regRd =
    shamt = func = immediate = targetAddr = 0;
    type = '\0';
    name = "nop";
}
Instruction::Instruction(unsigned int instruction){
    completeInst = instruction;
    opCode = instruction >> 26;
    regRs = (instruction >> 21)%32;
    regRt = (instruction >> 16)%32;
    regRd = (instruction >> 11)%32;
    shamt = (instruction >> 6)%32;
    func = instruction%64;
    immediate = instruction%65536;
    targetAddr = instruction%67108864;

    if(opCode == 63){
        type = 'S';
        name = "halt";
    }
    else if(opCode == 2 || opCode == 3){
        type = 'J';
        name = instOpcode[opCode];
    }
    else if(opCode == 0){
        type = 'R';
        if(func == 0 && regRt == 0 && regRd == 0 && shamt == 0){
            type = 'S';
            name = "NOP";
        }
        else if(instFunc.find(func) != instFunc.end())
            name = instFunc[func];
        else{
            printf("illegal instruction found at 0x%X\n", pc);
            illegal = 1;
        }
    }
    else{
        type = 'I';
        if(instOpcode.find(opCode) != instOpcode.end())
            name = instOpcode[opCode];
        else{
            printf("illegal instruction found at 0x%X\n", pc);
            illegal = 1;
        }
    }
}
