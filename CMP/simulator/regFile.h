#ifndef REGFILE_H_INCLUDED
#define REGFILE_H_INCLUDED

class RegisterFile{
private:
    unsigned int registers[32];
    unsigned int oldRegisters[32];
    unsigned int oldHI, oldLO;
public:
    unsigned int readData1, readData2;

    RegisterFile();

    void readWrite(unsigned int readRegister1, unsigned int readRegister2, unsigned int writeRegister
                   , unsigned int writeData, bool regWrite);

    void outputRegister(int cycle);
};

#endif // REGFILE_H_INCLUDED
