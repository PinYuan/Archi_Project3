#ifndef ERRORDETECT_H_INCLUDED
#define ERRORDETECT_H_INCLUDED
using namespace std;

class ErrorDetect{
private:
    int mult;//mult(u)
    int mf;//mfhi mflo
    int cycle;
public:
    ErrorDetect();
    void setCycle(int cycle);
    int memoryAddressOverflow(unsigned int opCode, unsigned int address);
    int dataMisaligned(unsigned int opCode, unsigned int address);
};

#endif // ERRORDETECT_H_INCLUDED
