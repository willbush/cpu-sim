#ifndef SRC_COMPUTERSIM_H_
#define SRC_COMPUTERSIM_H_
#include <string>

class ComputerSim {
public:
    ComputerSim(const int* program);
    virtual ~ComputerSim();

private:
    bool isChild(int) const;
    bool isParent(int) const;
    void tryPipe(int*, int*) const;
    int tryFork() const;
    void printThenExitFailure(const std::string) const;
};

#endif
