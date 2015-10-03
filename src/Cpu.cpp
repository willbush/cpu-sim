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
    case 1: // load value
        _ac = fetchInstruction();
        break;
    case 2: // load value at the address into the AC
        _ac = readFromMemory(fetchInstruction());
        break;
    case 3: // Use the AC value as an address to a another value.
        // this new value is also used as an address to the final value.
        _ac = readFromMemory(readFromMemory(fetchInstruction()));
        break;
    case 4: // load the value at address + X into the AC
        _ac = readFromMemory(fetchInstruction() + _x);
        break;
    case 5: // load the value at the address + Y into the AC
        _ac = readFromMemory(fetchInstruction() + _y);
        break;
    case 6: // from from sp + X into the AC
        _ac = readFromMemory(_sp + _x);
        break;
    case 7: // store the AC value into the following address
        writeToMemory(fetchInstruction(), _ac);
        break;
    case 8: // Get a random integer from interval [1, 100]
        putRandInAC();
        break;
    case 9: // write the AC to the screen as an integer if following value is 1
        // or as an char if the following value is 2
        putPort();
        break;
    case 10: // add the value in X to the AC
        _ac = _ac + _x;
        break;
    case 11: // add the value in Y to the AC
        _ac = _ac + _y;
        break;
    case 12: // subtract the value in X from the AC
        _ac = _ac - _x;
        break;
    case 13: // subtract the value in Y from the AC
        _ac = _ac - _y;
        break;
    case 14: // Copy AC to X
        _x = _ac;
        break;
    case 15: // Copy X to AC
        _ac = _x;
        break;
    case 16: // Copy AC to Y
        _y = _ac;
        break;
    case 17: // Copy Y to AC
        _ac = _y;
        break;
    case 18: // Copy AC to SP
        _sp = _ac;
        break;
    case 19: // Copy SP to AC
        _ac = _sp;
        break;
    case 20: // Jump to following address
        jumpToAddress(fetchInstruction());
        break;
    case 21: // jump to address if AC == 0
        if (_ac == 0)
            jumpToAddress(fetchInstruction());
        else
            _pc++;
        break;
    case 22: // jump to address if AC != 0
        if (_ac != 0)
            jumpToAddress(fetchInstruction());
        else
            _pc++;
        break;
    case 23: // push return address onto the stack and jump to the following address
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
    case 29: // Set system mode, switch stack, push SP and PC, set new SP and PC
        if (_interruptEnabled)
            interrupt(INTERRUPT_HANDLER_ADDRESS);
        break;
    case 30: // restore registers and set to user mode
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
