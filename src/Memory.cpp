#include "Memory.h"
#include <unistd.h>
#include <stdlib.h>

Memory::Memory(const int& readEndOfPipe, const int& writeEndOfPipe, const std::vector<std::string>& program)
        : READ_END_OF_PIPE(readEndOfPipe), WRITE_END_OF_PIPE(writeEndOfPipe) {

    initializeMemory(program);
    sendReadySignal();
    listenForCpuCommands();
}

Memory::~Memory() {
}

void Memory::initializeMemory(const std::vector<std::string>& program) {
    unsigned int memIndex, programIndex;
    memIndex = programIndex = 0;

    while (programIndex < program.size()) {
        std::string value = program[programIndex];
        if (value[0] == '.') { // period represent a load address
            std::string subStr = value.substr(1, value.size() - 1); // remove period
            memIndex = atoi(subStr.c_str());
            programIndex++; // move to next instruction
        }
        memory[memIndex++] = atoi(program[programIndex++].c_str());
    }
}

void Memory::sendReadySignal() const {
    const char readySignal = 'r';
    write(WRITE_END_OF_PIPE, &readySignal, sizeof(readySignal));
}

void Memory::listenForCpuCommands() {
    char command;
    do {
        read(READ_END_OF_PIPE, &command, sizeof(char));
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
    read(READ_END_OF_PIPE, &address, sizeof(int));
    return memory[address];
}

void Memory::sendToCpu(int valueFromMem) const {
    write(WRITE_END_OF_PIPE, &valueFromMem, sizeof(valueFromMem));
}

void Memory::writeToMemory() {
    int address, valueToWrite, size = sizeof(int);
    read(READ_END_OF_PIPE, &address, size);
    read(READ_END_OF_PIPE, &valueToWrite, size);
    memory[address] = valueToWrite;
}

bool Memory::cpuHasCommands(int command) const {
    return command != 'E';
}
