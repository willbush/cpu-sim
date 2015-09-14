#include "Memory.h"
#include "stdio.h"
#include <unistd.h>

Memory::Memory(const int& cpuReader, const int& cpuWriter) {
    int myVal;
    read(cpuReader, &myVal, sizeof(myVal));
    myVal++;
    write(cpuWriter, &myVal, sizeof(myVal));
    printf("child read: %d\n", myVal);
}

Memory::~Memory() {
}
