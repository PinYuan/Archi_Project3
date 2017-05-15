#ifndef ARITHMETIC_H_INCLUDED
#define ARITHMETIC_H_INCLUDED

unsigned int signExtend( unsigned int dataIn, unsigned int dataLen ){
    unsigned int signbit;
    unsigned int mask = 0xFFFFFFFF;

    signbit = dataIn >> (dataLen-1);
    if(signbit)
        return ((mask >> dataLen) << dataLen) | dataIn;
    else
        return dataIn;
}
#endif // ARITHMETIC_H_INCLUDED
