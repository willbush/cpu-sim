#include "Cpu.h"
#include "stdio.h"
#include "stdlib.h"
#include <unistd.h>
#include <iostream>
#include <sstream>

Cpu::Cpu(const int& readEndOfPipe, const int& writeEndOfPipe, const int interruptInterval)
        : READ_END_OF_PIPE(readEndOfPipe), WRITE_END_OF_PIPE(writeEndOfPipe) {
    _interruptInterval = interruptInterval;
    _ir = _pc = _ac = _x = _y = 0;
    _sp = USER_STACK_START_ADDRESS;
    _inSystemMode = false;
    _interruptEnabled = true;
    _cpuEvents = "";

    waitForMemReadySignal();
    runProcessor();
    sendEndCommandToMemory();
}

Cpu::~Cpu() {
}

void Cpu::waitForMemReadySignal() {
    _cpuEvents += "w ";
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
    unsigned int timer = 0;

    do {
        _ir = fetchInstruction();
        processInstruction();
        timer = checkAndUpdateTimer(timer);
    } while (endNotReached());
}

void Cpu::processInstruction() {
    switch (_ir) {
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
        if (_ac == 0)
            jumpToAddress(fetchInstruction());
        else
            _pc++;
        break;
    case 22:
        if (_ac != 0)
            jumpToAddress(fetchInstruction());
        else
            _pc++;
        break;
    case 23:
        callAddress();
        break;
    case 24:
        returnFromStack();
        break;
    case 25:
        _cpuEvents += "x++ ";
        _x++;
        break;
    case 26:
        _x--;
        _cpuEvents += "x-- ";
        break;
    case 27:
        push(_ac);
        break;
    case 28:
        _ac = pop();
        break;
    case 29:
        if (_interruptEnabled)
            interrupt(INTERRUPT_HANDLER_ADDRESS);
        break;
    case 30:
        returnFromInterrupt();
        break;
    }
}

unsigned int Cpu::checkAndUpdateTimer(unsigned int timer) {
    if (timer++ >= _interruptInterval && _interruptEnabled) {
        interrupt(TIMER_INTERRUPT_HANDLER_ADDRESS);
        timer = 0; // reset
    }
    return timer;
}

int Cpu::fetchInstruction() {
    return readFromMemory(_pc++);
}

int Cpu::readFromMemory(int address) {
    checkForAccessVioloation(address);
    char readCommand = 'R';
    write(WRITE_END_OF_PIPE, &readCommand, sizeof(char)); // tell memory I want to read
    write(WRITE_END_OF_PIPE, &address, sizeof(int)); // tell memory the address
    int valueFromPipe;
    read(READ_END_OF_PIPE, &valueFromPipe, sizeof(int)); // get the value from memory
    return valueFromPipe;
}

void Cpu::writeToMemory(const int address, const int value) {
    checkForAccessVioloation(address);
    char writeCommand = 'W';
    write(WRITE_END_OF_PIPE, &writeCommand, sizeof(char)); // tell memory I want to write
    write(WRITE_END_OF_PIPE, &address, sizeof(int)); // tell memory the address
    write(WRITE_END_OF_PIPE, &value, sizeof(int)); // tell memory the value
}

void Cpu::checkForAccessVioloation(int address) {
    if (!_inSystemMode && address >= SYSTEM_START_ADDRESS)
        printMemoryAccessErrAndExit();
    else if (_inSystemMode && address < SYSTEM_START_ADDRESS)
        printMemoryAccessErrAndExit();
}

void Cpu::printMemoryAccessErrAndExit() {
    std::cerr << "ERROR: Attempted to access a protected memory location.\n";
    sendEndCommandToMemory();
    _exit(EXIT_FAILURE);
}

bool Cpu::endNotReached() const {
    return _ir != END_INSTRUCTION;
}

bool Cpu::isReadySignal(char command) const {
    return command == 'r';
}

void Cpu::sendEndCommandToMemory() {
    _cpuEvents += "e";
    char endCommand = 'E';
    write(WRITE_END_OF_PIPE, &endCommand, sizeof(char)); // tell memory I want it to end
}

std::string Cpu::getEvents() const {
    return _cpuEvents;
}

void Cpu::loadValue() {
    const int value = fetchInstruction();
    std::ostringstream oss;
    oss << "LV" << value << " ";
    _cpuEvents += oss.str();
    _ac = value;
}

void Cpu::loadValueFromAddress() {
    const int address = fetchInstruction();
    const int value = readFromMemory(address);
    _ac = value;

    std::ostringstream oss;
    oss << "LVFA" << address << "," << value << " ";
    _cpuEvents += oss.str();
}

void Cpu::storeAddress() {
    const int address = fetchInstruction();
    std::ostringstream oss;
    oss << "SA" << address << "," << _ac << " ";
    _cpuEvents += oss.str();
    writeToMemory(address, _ac);
}

void Cpu::loadAddress() {
    const int address = fetchInstruction();
    std::ostringstream oss;
    const int value = readFromMemory(address);
    _ac = value;
    oss << "LA" << address << "," << _ac << " ";
    _cpuEvents += oss.str();
}

void Cpu::loadValueFromAddressPlusX() {
    const int address = fetchInstruction() + _x;
    const int value = readFromMemory(address);
    _ac = value;

    std::ostringstream oss;
    oss << "LVFAPX" << address << "," << value << " ";
    _cpuEvents += oss.str();
}

void Cpu::loadValueFromAddressPlusY() {
    const int address = fetchInstruction() + _y;
    const int value = readFromMemory(address);
    _ac = value;

    std::ostringstream oss;
    oss << "LVFAPY" << address << "," << value << " ";
    _cpuEvents += oss.str();
}

void Cpu::copyACtoX() {
    _x = _ac;
    std::ostringstream oss;
    oss << "CX" << _x << " ";
    _cpuEvents += oss.str();
}

void Cpu::copyACtoY() {
    _y = _ac;
    std::ostringstream oss;
    oss << "CY" << _y << " ";
    _cpuEvents += oss.str();
}

void Cpu::copyXtoAC() {
    _ac = _x;
    std::ostringstream oss;
    oss << "CX2AC" << _ac << " ";
    _cpuEvents += oss.str();
}

void Cpu::copyYtoAC() {
    _ac = _y;
    std::ostringstream oss;
    oss << "CY2AC" << _ac << " ";
    _cpuEvents += oss.str();
}

void Cpu::jumpToAddress(const int address) {
    _pc = address;
    std::ostringstream oss;
    oss << "JA" << _pc << " ";
    _cpuEvents += oss.str();
}

void Cpu::putPort() {
    const int port = fetchInstruction();
    std::ostringstream oss;
    if (port == 1) {
        std::cout << _ac;
        oss << "P1," << _ac << " ";
    } else if (port == 2) {
        char value = _ac;
        std::cout << value;
        oss << "P2," << value << " ";
    }
    _cpuEvents += oss.str();
}

void Cpu::callAddress() {
    const int address = fetchInstruction();
    push(_pc);
    jumpToAddress(address);
    std::ostringstream oss;
    oss << "CA" << address << " ";
    _cpuEvents += oss.str();
}

void Cpu::returnFromStack() {
    const int address = pop();
    jumpToAddress(address);
    std::ostringstream oss;
    oss << "RET" << address << " ";
    _cpuEvents += oss.str();
}

void Cpu::push(const int value) {
    writeToMemory(--_sp, value);
    std::ostringstream oss;
    oss << "PU" << _sp << "," << value << " ";
    _cpuEvents += oss.str();
}

int Cpu::pop() {
    const int spCopy = _sp;
    const int poppedValue = readFromMemory(_sp++);
    std::ostringstream oos;
    oos << "PO" << spCopy << "," << poppedValue << " ";
    _cpuEvents += oos.str();
    return poppedValue;
}

void Cpu::putRandInAC() {
    const int maxBound = 100;
    const int minBound = 1;
    // random integer on interval [1, 100]
    _ac = rand() % maxBound + minBound;
    _cpuEvents += "R ";
}

void Cpu::addXtoAC() {
    std::ostringstream oos;
    oos << "AX2AC" << _x << "," << _ac << " ";
    _cpuEvents += oos.str();
    _ac = _ac + _x;
}

void Cpu::addYtoAC() {
    std::ostringstream oos;
    oos << "AY2AC" << _y << "," << _ac << " ";
    _cpuEvents += oos.str();
    _ac = _ac + _y;
}

void Cpu::subXfromAC() {
    _ac = _ac - _x;
    std::ostringstream oss;
    oss << "SUBX" << _x << " ";
    _cpuEvents += oss.str();
}

void Cpu::subYfromAC() {
    _ac = _ac - _y;
    std::ostringstream oss;
    oss << "SUBY" << _y << " ";
    _cpuEvents += oss.str();
}

void Cpu::changeSP2AC() {
    _sp = _ac;
    std::ostringstream oss;
    oss << "CSP2AC" << _sp << "," << _ac << " ";
    _cpuEvents += oss.str();
}

void Cpu::changeAC2SP() {
    _ac = _sp;
    std::ostringstream oss;
    oss << "CAC2SP" << _ac << "," << _sp << " ";
    _cpuEvents += oss.str();
}

void Cpu::interrupt(const int interruptHandlerAddress) {
    push(_pc);
    _inSystemMode = true;
    _interruptEnabled = false;
    _pc = interruptHandlerAddress;
    const int spCopy = _sp;
    _sp = SYSTEM_STACK_START_ADDRESS;
    push(spCopy);
    std::ostringstream oss;
    oss << "SM," << _inSystemMode << " ";
    _cpuEvents += oss.str();
}

void Cpu::returnFromInterrupt() {
    _sp = pop();
    _inSystemMode = false;
    _interruptEnabled = true;
    _pc = pop();

    std::ostringstream oss;
    oss << "SM," << _inSystemMode << " ";
    _cpuEvents += oss.str();
}

void Cpu::loadFromSpPlusX() {
    _ac = readFromMemory(_sp + _x);
    std::ostringstream oss;
    oss << "LSP+X" << (_sp + _x) << "," << _ac << " ";
    _cpuEvents += oss.str();
}
