#include "ComputerSim.h"
#include "Memory.h"
#include "Cpu.h"
#include "sys/wait.h"
#include <unistd.h>
#include <stdexcept>

ComputerSim::ComputerSim(const std::vector<std::string>& program, const int timerInterval) {
    int cpuToMem[2];
    int memToCpu[2];
    tryPipe(cpuToMem, memToCpu);
    int forkResult = tryFork();

    if (isChild(forkResult)) {
        Memory m(cpuToMem[0], memToCpu[1], program);
    } else if (isParent(forkResult)) {
        Cpu c(memToCpu[0], cpuToMem[1], timerInterval);
        waitpid(-1, NULL, 0); // wait for child
    }
}

ComputerSim::~ComputerSim() {
}

void ComputerSim::tryPipe(int* cpuToMem, int* memToCpu) const {
    if (pipe(cpuToMem) < 0 || pipe(memToCpu) < 0)
        throw std::runtime_error("ERROR: pipe failed");
}

int ComputerSim::tryFork() const {
    int forkResult = fork();
    if (forkResult == -1)
        throw std::runtime_error("ERROR: fork failed");

    return forkResult;
}

bool ComputerSim::isChild(int forkResult) const {
    return forkResult == 0;
}

bool ComputerSim::isParent(int forkResult) const {
    return forkResult > 0;
}
