#include "Cpu.h"
#include "stdlib.h"
#include <unistd.h>
#include <stdexcept>
#include <iostream>

Cpu::Cpu(const int& readEndOfPipe, const int& writeEndOfPipe, const int interruptInterval)
        : READ_END_OF_PIPE(readEndOfPipe), WRITE_END_OF_PIPE(writeEndOfPipe) {
    _interruptInterval = interruptInterval;
    _ir = _pc = _ac = _x = _y = 0;
    _sp = USER_STACK_START_ADDRESS;
    _inSystemMode = false;
    _interruptEnabled = true;

    waitForMemReadySignal();
    runProcessor();
    sendEndCommandToMemory();
}

Cpu::~Cpu() {
}

void Cpu::waitForMemReadySignal() {
    char valueFromPipe;
    read(READ_END_OF_PIPE, &valueFromPipe, sizeof(char));
    if (!isReadySignal(valueFromPipe)) {
        std::string errorMsg = "ERROR: First read value from the pipe to "
                "the memory was not a ready state signal as it should be.\n";
        sendEndCommandToMemory();
        throw std::runtime_error(errorMsg);
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
        _ac = fetchInstruction();
        break;
    case 2:
        _ac = readFromMemory(fetchInstruction());
        break;
    case 3:
        _ac = readFromMemory(readFromMemory(fetchInstruction()));
        break;
    case 4:
        _ac = readFromMemory(fetchInstruction() + _x);
        break;
    case 5:
        _ac = readFromMemory(fetchInstruction() + _y);
        break;
    case 6:
        _ac = readFromMemory(_sp + _x);
        break;
    case 7:
        writeToMemory(fetchInstruction(), _ac);
        break;
    case 8:
        putRandInAC();
        break;
    case 9:
        putPort();
        break;
    case 10:
        _ac = _ac + _x;
        break;
    case 11:
        _ac = _ac + _y;
        break;
    case 12:
        _ac = _ac - _x;
        break;
    case 13:
        _ac = _ac - _y;
        break;
    case 14:
        _x = _ac;
        break;
    case 15:
        _ac = _x;
        break;
    case 16:
        _y = _ac;
        break;
    case 17:
        _ac = _y;
        break;
    case 18:
        _sp = _ac;
        break;
    case 19:
        _ac = _sp;
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
    case 24: // return from stack
        jumpToAddress(pop());
        break;
    case 25:
        _x++;
        break;
    case 26:
        _x--;
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
        if (_inSystemMode)
            returnFromInterrupt();
        break;
    }
}

unsigned int Cpu::checkAndUpdateTimer(unsigned int timer) {
    // The return from interrupt instruction is a system instruction and must not increment the timer.
    const int iRet = 30;
    if (_interruptEnabled && _ir != iRet) {
        // running the timer while running the interrupt handler can cause an infinite loop
        timer++;
        if (timer >= _interruptInterval) {
            interrupt(TIMER_INTERRUPT_HANDLER_ADDRESS);
            timer = 0; // reset
        }
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
    if (!_inSystemMode && address >= SYSTEM_START_ADDRESS) {
        sendEndCommandToMemory();
        throw std::runtime_error("ERROR: Attempted to access a protected memory location.\n");
    }
}

bool Cpu::endNotReached() const {
    return _ir != END_INSTRUCTION;
}

bool Cpu::isReadySignal(char command) const {
    return command == 'r';
}

void Cpu::sendEndCommandToMemory() {
    char endCommand = 'E';
    write(WRITE_END_OF_PIPE, &endCommand, sizeof(char)); // tell memory I want it to end
}

void Cpu::jumpToAddress(const int address) {
    _pc = address;
}

void Cpu::putPort() {
    const int port = fetchInstruction();
    if (port == 1) {
        std::cout << _ac << std::flush;
    } else if (port == 2) {
        char value = _ac;
        std::cout << value << std::flush;
    }
}

void Cpu::callAddress() {
    const int address = fetchInstruction();
    push(_pc);
    jumpToAddress(address);
}

void Cpu::push(const int value) {
    writeToMemory(--_sp, value);
}

int Cpu::pop() {
    return readFromMemory(_sp++);
}

void Cpu::putRandInAC() {
    const int maxBound = 100;
    const int minBound = 1;
    srand(time(NULL));
    // random integer on interval [1, 100]
    _ac = rand() % maxBound + minBound;
}

void Cpu::interrupt(const int interruptHandlerAddress) {
    _inSystemMode = true;
    _interruptEnabled = false;
    const int pcCopy = _pc;
    const int spCopy = _sp;

    // set program counter to interrupt handler and stack pointer to system stack
    _pc = interruptHandlerAddress;
    _sp = SYSTEM_STACK_START_ADDRESS;

    // save user stack pointer and program counter to system stack
    push(spCopy);
    push(pcCopy);
}

void Cpu::returnFromInterrupt() {
    _pc = pop();
    _sp = pop();
    _inSystemMode = false;
    _interruptEnabled = true;
}
