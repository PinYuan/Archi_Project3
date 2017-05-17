CMP:main.o processor.o ALU.o errorDetect.o instruction.o cache.o memory.o disk.o TLB.o pageTable.o regFile.o trace.o
	g++ -std=c++11 -g -o CMP main.o processor.o ALU.o errorDetect.o instruction.o cache.o memory.o disk.o TLB.o pageTable.o regFile.o trace.o
main.o:main.cpp
	g++ -std=c++11 -g -c main.cpp
processor.o:processor.cpp
	g++ -std=c++11 -g -c processor.cpp
ALU.o:ALU.cpp
	g++ -std=c++11 -g -c ALU.cpp
errorDetect.o:errorDetect.cpp
	g++ -std=c++11 -g -c errorDetect.cpp
instruction.o:instruction.cpp
	g++ -std=c++11 -g -c instruction.cpp
cache.o:cache.cpp
	g++ -std=c++11 -g -c cache.cpp
memory.o:memory.cpp
	g++ -std=c++11 -g -c memory.cpp
disk.o:disk.cpp
	g++ -std=c++11 -g -c disk.cpp
TLB.o:TLB.cpp
	g++ -std=c++11 -g -c TLB.cpp
pageTable.o:pageTable.cpp
	g++ -std=c++11 -g -c pageTable.cpp
regFile.o:regFile.cpp
	g++ -std=c++11 -g -c regFile.cpp
trace.o:trace.cpp
	g++ -std=c++11 -g -c trace.cpp
clean:
	rm -f  main.o processor.o ALU.o errorDetect.o instruction.o cache.o memory.o disk.o TLB.o pageTable.o regFile.o trace.o

