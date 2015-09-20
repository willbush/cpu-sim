#include "ComputerSim.h"
#include "Memory.h"
#include "Cpu.h"
#include "stdlib.h"
#include "sys/wait.h"
#include "stdio.h"
#include <unistd.h>
#include <iostream>
#include <sstream>

ComputerSim::ComputerSim(const std::vector<int>& program, const int timerInterval) {
    int cpuToMem[2];
    int memToCpu[2];
    tryPipe(cpuToMem, memToCpu);
    int forkResult = tryFork();

    if (isChild(forkResult)) {
        Memory m(cpuToMem[0], memToCpu[1], program);
        _exit(EXIT_SUCCESS);
    } else if (isParent(forkResult)) {
        Cpu c(memToCpu[0], cpuToMem[1], timerInterval);
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
