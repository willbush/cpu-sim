#include "ComputerSim.h"
#include "Memory.h"
#include "Cpu.h"
#include "stdlib.h"
#include "sys/wait.h"
#include "stdio.h"
#include <unistd.h>
#include <iostream>

ComputerSim::ComputerSim(const int* program) {
    int cpuToMem[2];
    int memToCpu[2];
    tryPipe(cpuToMem, memToCpu);
    int forkResult = tryFork();

    if (isChild(forkResult)) {
        int& cpuReadEnd = cpuToMem[0];
        int& cpuWriteEnd = memToCpu[1];
//        Memory m(cpuReadEnd, cpuWriteEnd, program); //TODO fix programLen
        _exit(EXIT_SUCCESS);
    } else if (isParent(forkResult)) {
        int& memWriteEnd = cpuToMem[1];
        int& memReadEnd = memToCpu[0];
        Cpu c(memReadEnd, memWriteEnd);
        waitpid(-1, NULL, 0); // wait for child
    }
}

ComputerSim::~ComputerSim() {
}

void ComputerSim::tryPipe(int* cpuToMem, int* memToCpu) const {
    if (pipe(cpuToMem) < 0 || pipe(memToCpu) < 0)
        printErrThenExitFailure("pipe failed");
}

int ComputerSim::tryFork() const {
    int forkResult = fork();
    if (forkResult == -1)
        printErrThenExitFailure("fork failed");

    return forkResult;
}

void ComputerSim::printErrThenExitFailure(const std::string errorMsg) const {
    std::cerr << errorMsg << std::endl;
    exit(EXIT_FAILURE);
}

bool ComputerSim::isChild(int forkResult) const {
    return forkResult == 0;
}

bool ComputerSim::isParent(int forkResult) const {
    return forkResult > 0;
}
