#include "Memory.h"
#include <unistd.h>

Memory::Memory(const int& cpuReadEnd, const int& cpuWriteEnd,
        const int* program)
        : CPU_READ_END(cpuReadEnd), CPU_WRITE_END(cpuWriteEnd) {
    initializeMemory(program);
    listenForCpuCommands();
}

Memory::~Memory() {
}

void Memory::initializeMemory(const int* program) {
    const int programLen = sizeof(program) / sizeof(program[0]);

    for (int i = 0; i < programLen; i++)
        memory[i] = program[i];
}

void Memory::listenForCpuCommands() {
    int command;
    do {
        read(CPU_READ_END, &command, sizeof(char));
        performCommand(command);
    } while (cpuHasCommands(command));
}

void Memory::performCommand(char command) {
    if (isReadCommand(command))
        sendToCpu(readFromMemory());
    else if (isWriteCommand(command))
        writeToMemory();
}

bool Memory::isReadCommand(char command) const {
    return command == 'R';
}

bool Memory::isWriteCommand(char command) const {
    return command == 'W';
}

int Memory::readFromMemory() const {
    int address;
    read(CPU_READ_END, &address, sizeof(int));
    return memory[address];
}

void Memory::sendToCpu(int valueFromMem) const {
    write(CPU_WRITE_END, &valueFromMem, sizeof(valueFromMem));
}

void Memory::writeToMemory() {
    int address, valueToWrite, size = sizeof(int);
    read(CPU_READ_END, &address, size);
    read(CPU_READ_END, &valueToWrite, size);
    memory[address] = valueToWrite;
}

bool Memory::cpuHasCommands(int command) const {
    return command != 'E';
}
