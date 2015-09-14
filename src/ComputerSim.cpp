#include "ComputerSim.h"
#include "Memory.h"
#include "Cpu.h"
#include "stdlib.h"
#include "sys/wait.h"
#include "stdio.h"
#include <unistd.h>
#include <iostream>

int intAfterPipe;

ComputerSim::ComputerSim(const int value) {
    int myValue = value;
    int cpuToMem[2];
    int memToCpu[2];
    tryPipe(cpuToMem, memToCpu);
    int forkResult = tryFork();

    if (isChild(forkResult)) {
        int& cpuReader = cpuToMem[0];
        int& cpuWriter = memToCpu[1];
        Memory m(cpuReader, cpuWriter);
        _exit(EXIT_SUCCESS);
    } else if (isParent(forkResult)) {
        int size = sizeof(value);
        write(cpuToMem[1], &myValue, size);
        read(memToCpu[0], &myValue, size);
        intAfterPipe = myValue;
    }
    waitpid(-1, NULL, 0);
}

ComputerSim::~ComputerSim() {
}

void ComputerSim::tryPipe(int* cpuToMem, int* memToCpu) const {
    if (pipe(cpuToMem) < 0 || pipe(memToCpu) < 0)
        printThenExitFailure("pipe failed");
}

int ComputerSim::tryFork() const {
    int forkResult = fork();
    if (forkResult == -1)
        printThenExitFailure("fork failed");

    return forkResult;
}

void ComputerSim::printThenExitFailure(const std::string errorMsg) const {
    std::cerr << errorMsg << std::endl;
    exit(EXIT_FAILURE);
}

bool ComputerSim::isChild(int forkResult) const {
    return forkResult == 0;
}

bool ComputerSim::isParent(int forkResult) const {
    return forkResult > 0;
}

int ComputerSim::getIntAfterPipe() const {
    return intAfterPipe;
}
