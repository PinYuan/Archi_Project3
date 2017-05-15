#ifndef TRACE_H_INCLUDED
#define TRACE_H_INCLUDED
#include "instruction.h"

void initTrace();
void printTrace(int cycle, Instruction inst);

#endif // TRACE_H_INCLUDED
