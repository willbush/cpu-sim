#include "Cpu.h"
#include "stdio.h"
#include "stdlib.h"
#include <unistd.h>
#include <iostream>
#include <sstream>

Cpu::Cpu(const int& readEndOfPipe, const int& writeEndOfPipe)
        : READ_END_OF_PIPE(readEndOfPipe), WRITE_END_OF_PIPE(writeEndOfPipe) {
    ir = pc = ac = x = y = 0;
    sp = USER_STACK_START_ADDRESS;
    inSystemMode = false;
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
        std::cerr << "ERROR: First read value from the pipe to "
                "the memory was not a ready state signal as it should be.\n";
        sendEndCommandToMemory();
        _exit(EXIT_FAILURE);
    }
}

void Cpu::runProcessor() {
    do {
        ir = fetchInstruction();
        processInstruction();
    } while (endNotReached());
}

void Cpu::processInstruction() {
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
    case 6:
        loadFromSpPlusX();
        break;
    case 7:
        storeAddress();
        break;
    case 8:
        putRandInAC();
        break;
    case 9:
        putPort();
        break;
    case 10:
        addXtoAC();
        break;
    case 11:
        addYtoAC();
        break;
    case 12:
        subXfromAC();
        break;
    case 13:
        subYfromAC();
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
    case 18:
        changeSP2AC();
        break;
    case 19:
        changeAC2SP();
        break;
    case 20:
        jumpToAddress(fetchInstruction());
        break;
    case 21:
        if (ac == 0)
            jumpToAddress(fetchInstruction());
        else
            pc++;
        break;
    case 22:
        if (ac != 0)
            jumpToAddress(fetchInstruction());
        else
            pc++;
        break;
    case 23:
        callAddress();
        break;
    case 24:
        returnFromStack();
        break;
    case 25:
        cpuEvents += "x++ ";
        x++;
        break;
    case 26:
        x--;
        cpuEvents += "x-- ";
        break;
    case 27:
        push(ac);
        break;
    case 28:
        ac = pop();
        break;
    case 29:
        interrupt();
        break;
    case 30:
        returnFromInterrupt();
        break;
    }
}

int Cpu::fetchInstruction() {
    return readFromMemory(pc++);
}

int Cpu::readFromMemory(int address) {
    checkAddressAgainstMode(address);
    char readCommand = 'R';
    write(WRITE_END_OF_PIPE, &readCommand, sizeof(char)); // tell memory I want to read
    write(WRITE_END_OF_PIPE, &address, sizeof(int)); // tell memory the address
    int valueFromPipe;
    read(READ_END_OF_PIPE, &valueFromPipe, sizeof(int)); // get the value from memory
    return valueFromPipe;
}

void Cpu::writeToMemory(const int address, const int value) {
    checkAddressAgainstMode(address);
    char writeCommand = 'W';
    write(WRITE_END_OF_PIPE, &writeCommand, sizeof(char)); // tell memory I want to write
    write(WRITE_END_OF_PIPE, &address, sizeof(int)); // tell memory the address
    write(WRITE_END_OF_PIPE, &value, sizeof(int)); // tell memory the value
}

void Cpu::checkAddressAgainstMode(int address) {
    if (!inSystemMode && address >= 1000)
        printMemoryAccessErrAndExit();
    else if (inSystemMode && address < 1000)
        printMemoryAccessErrAndExit();
}

void Cpu::printMemoryAccessErrAndExit() {
    std::cerr << "ERROR: Attempted to access a protected memory location.\n";
    sendEndCommandToMemory();
    _exit(EXIT_FAILURE);
}

bool Cpu::endNotReached() const {
    return ir != END_INSTRUCTION;
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

void Cpu::jumpToAddress(const int address) {
    pc = address;
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

void Cpu::callAddress() {
    const int address = fetchInstruction();
    push(pc);
    jumpToAddress(address);
    std::ostringstream oss;
    oss << "CA" << address << " ";
    cpuEvents += oss.str();
}

void Cpu::returnFromStack() {
    const int address = pop();
    jumpToAddress(address);
    std::ostringstream oss;
    oss << "RET" << address << " ";
    cpuEvents += oss.str();
}

void Cpu::push(const int value) {
    writeToMemory(--sp, value);
    std::ostringstream oss;
    oss << "PU" << sp << "," << value << " ";
    cpuEvents += oss.str();
}

int Cpu::pop() {
    const int spCopy = sp;
    const int poppedValue = readFromMemory(sp++);
    std::ostringstream oos;
    oos << "PO" << spCopy << "," << poppedValue << " ";
    cpuEvents += oos.str();
    return poppedValue;
}

void Cpu::putRandInAC() {
    const int maxBound = 100;
    const int minBound = 1;
    // random integer on interval [1, 100]
    ac = rand() % maxBound + minBound;
    cpuEvents += "R ";
}

void Cpu::addXtoAC() {
    std::ostringstream oos;
    oos << "AX2AC" << x << "," << ac << " ";
    cpuEvents += oos.str();
    ac = ac + x;
}

void Cpu::addYtoAC() {
    std::ostringstream oos;
    oos << "AY2AC" << y << "," << ac << " ";
    cpuEvents += oos.str();
    ac = ac + y;
}

void Cpu::subXfromAC() {
    ac = ac - x;
    std::ostringstream oss;
    oss << "SUBX" << x << " ";
    cpuEvents += oss.str();
}

void Cpu::subYfromAC() {
    ac = ac - y;
    std::ostringstream oss;
    oss << "SUBY" << y << " ";
    cpuEvents += oss.str();
}

void Cpu::changeSP2AC() {
    sp = ac;
    std::ostringstream oss;
    oss << "CSP2AC" << sp << "," << ac << " ";
    cpuEvents += oss.str();
}

void Cpu::changeAC2SP() {
    ac = sp;
    std::ostringstream oss;
    oss << "CAC2SP" << ac << "," << sp << " ";
    cpuEvents += oss.str();
}

void Cpu::interrupt() {
    push(pc);
    inSystemMode = true;
    pc = INTERRUPT_HANDLER_ADDRESS;
    const int spCopy = sp;
    sp = SYSTEM_STACK_START_ADDRESS;
    push(spCopy);
    std::ostringstream oss;
    oss << "SM," << inSystemMode << " ";
    cpuEvents += oss.str();
}

void Cpu::returnFromInterrupt() {
    sp = pop();
    inSystemMode = false;
    pc = pop();

    std::ostringstream oss;
    oss << "SM," << inSystemMode << " ";
    cpuEvents += oss.str();
}

void Cpu::loadFromSpPlusX() {
    ac = readFromMemory(sp + x);
    std::ostringstream oss;
    oss << "LSP+X" << (sp + x) << "," << ac << " ";
    cpuEvents += oss.str();
}
