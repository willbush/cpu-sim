#include "Cpu.h"
#include "stdio.h"
#include "stdlib.h"
#include <unistd.h>
#include <iostream>
#include <sstream>

Cpu::Cpu(const int& readEndOfPipe, const int& writeEndOfPipe)
        : READ_END_OF_PIPE(readEndOfPipe), WRITE_END_OF_PIPE(writeEndOfPipe) {
    pc = ac = x = y = 0;
    cpuEvents = "";

    waitForMemReadySignal();
    runProcessor();
    sendEndCommandToMemory();
}

Cpu::~Cpu() {
}

void Cpu::waitForMemReadySignal() {
    cpuEvents += "w ";
    char valueFromPipe;
    read(READ_END_OF_PIPE, &valueFromPipe, sizeof(char));
    if (!isReadySignal(valueFromPipe)) {
        std::cerr << "First read value from the pipe to "
                "the memory was not a ready state signal as it should be.\n";
        _exit(EXIT_FAILURE);
    }
}

void Cpu::runProcessor() {
    int ir;
    do {
        ir = fetchInstruction();
        processInstruction(ir);
    } while (endNotReached(ir));
}

void Cpu::processInstruction(const int ir) {
    switch (ir) {
    case 1:
        loadValue();
        break;
    case 2:
        loadAddress();
        break;
    case 3:
        loadValueFromAddress();
        break;
    case 4:
        loadValueFromAddressPlusX();
        break;
    case 5:
        loadValueFromAddressPlusY();
        break;
    case 7:
        storeAddress();
        break;
    case 9:
        putPort();
        break;
    case 14:
        copyACtoX();
        break;
    case 15:
        copyXtoAC();
        break;
    case 16:
        copyACtoY();
        break;
    case 17:
        copyYtoAC();
        break;
    case 20:
        jumpToAddress();
        break;
    case 21:
        if (ac == 0)
            jumpToAddress();
        else
            pc++;
        break;
    case 22:
        if (ac != 0)
            jumpToAddress();
        else
            pc++;
        break;
    case 25:
        cpuEvents += "x++ ";
        x++;
        break;
    case 26:
        x--;
        cpuEvents += "x-- ";
        break;
    }
}

int Cpu::fetchInstruction() {
    return readFromMemory(pc++);
}

int Cpu::readFromMemory(int address) {
    char readCommand = 'R';
    write(WRITE_END_OF_PIPE, &readCommand, sizeof(char)); // tell memory I want to read
    write(WRITE_END_OF_PIPE, &address, sizeof(int)); // tell memory the address
    int valueFromPipe;
    read(READ_END_OF_PIPE, &valueFromPipe, sizeof(int)); // get the value from memory
    return valueFromPipe;
}

void Cpu::writeToMemory(const int address, const int value) {
    char writeCommand = 'W';
    write(WRITE_END_OF_PIPE, &writeCommand, sizeof(char)); // tell memory I want to write
    write(WRITE_END_OF_PIPE, &address, sizeof(int)); // tell memory the address
    write(WRITE_END_OF_PIPE, &value, sizeof(int)); // tell memory the value
}

bool Cpu::endNotReached(int instruction) const {
    return instruction != END_INSTRUCTION;
}

bool Cpu::isReadySignal(char command) const {
    return command == 'r';
}

void Cpu::sendEndCommandToMemory() {
    cpuEvents += "e";
    char endCommand = 'E';
    write(WRITE_END_OF_PIPE, &endCommand, sizeof(char)); // tell memory I want it to end
}

std::string Cpu::getEvents() const {
    return cpuEvents;
}

void Cpu::loadValue() {
    const int value = fetchInstruction();
    std::ostringstream oss;
    oss << "LV" << value << " ";
    cpuEvents += oss.str();
    ac = value;
}

void Cpu::loadValueFromAddress() {
    const int address = fetchInstruction();
    const int value = readFromMemory(address);
    ac = value;

    std::ostringstream oss;
    oss << "LVFA" << address << "," << value << " ";
    cpuEvents += oss.str();
}

void Cpu::storeAddress() {
    const int address = fetchInstruction();
    std::ostringstream oss;
    oss << "SA" << address << "," << ac << " ";
    cpuEvents += oss.str();
    writeToMemory(address, ac);
}

void Cpu::loadAddress() {
    const int address = fetchInstruction();
    std::ostringstream oss;
    const int value = readFromMemory(address);
    ac = value;
    oss << "LA" << address << "," << ac << " ";
    cpuEvents += oss.str();
}

void Cpu::loadValueFromAddressPlusX() {
    const int address = fetchInstruction() + x;
    const int value = readFromMemory(address);
    ac = value;

    std::ostringstream oss;
    oss << "LVFAPX" << address << "," << value << " ";
    cpuEvents += oss.str();
}

void Cpu::loadValueFromAddressPlusY() {
    const int address = fetchInstruction() + y;
    const int value = readFromMemory(address);
    ac = value;

    std::ostringstream oss;
    oss << "LVFAPY" << address << "," << value << " ";
    cpuEvents += oss.str();
}

void Cpu::copyACtoX() {
    x = ac;
    std::ostringstream oss;
    oss << "CX" << x << " ";
    cpuEvents += oss.str();
}

void Cpu::copyACtoY() {
    y = ac;
    std::ostringstream oss;
    oss << "CY" << y << " ";
    cpuEvents += oss.str();
}

void Cpu::copyXtoAC() {
    ac = x;
    std::ostringstream oss;
    oss << "CX2AC" << ac << " ";
    cpuEvents += oss.str();
}

void Cpu::copyYtoAC() {
    ac = y;
    std::ostringstream oss;
    oss << "CY2AC" << ac << " ";
    cpuEvents += oss.str();
}

void Cpu::jumpToAddress() {
    pc = fetchInstruction();
    std::ostringstream oss;
    oss << "JA" << pc << " ";
    cpuEvents += oss.str();
}

void Cpu::putPort() {
    const int port = fetchInstruction();
    std::ostringstream oss;
    if (port == 1) {
        std::cout << ac;
        oss << "P1," << ac << " ";
    } else if (port == 2) {
        char value = ac;
        std::cout << value;
        oss << "P2," << value << " ";
    }
    cpuEvents += oss.str();
}
