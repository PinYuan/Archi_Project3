#ifndef PROCESSOR_H_INCLUDED
#define PROCESSOR_H_INCLUDED

#include "instruction.h"

unsigned int fetchInst(int cycle);
unsigned char getData(unsigned int dataAddress, int cycle, int offset);
void writeBack(unsigned int dataAddress, unsigned char data, int cycle, int offset);
void execute(Instruction inst, int cycle);

#endif // PROCESSOR_H_INCLUDED
