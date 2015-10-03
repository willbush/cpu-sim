#include "ComputerSim.h"
#include "Memory.h"
#include "Cpu.h"
#include "sys/wait.h"
#include <unistd.h>
#include <stdexcept>

/*
 * The program is passed as a vector of strings which is parsed from the text file back in main.
 * I do this in part due to the single responsibility principle. However, the main reason is that
 * it allows me to easily write unit test which exercise the functionality of the CPU.
 * The unit test are likely not include with this assignment.
 */
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
