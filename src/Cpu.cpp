#include "Cpu.h"
#include "stdio.h"
#include <unistd.h>

int ac;

Cpu::Cpu(const int& memReadEnd, const int& memWriteEnd) {
    ac = 7;
    int value = 0;
    int myValue = 0;
    int size = sizeof(value);
    write(memWriteEnd, &myValue, size);
    read(memReadEnd, &myValue, size);
}

Cpu::~Cpu() {
}

int Cpu::getAC() const {
    return ac;
}
