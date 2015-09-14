#ifndef SRC_COMPUTERSIM_H_
#define SRC_COMPUTERSIM_H_
#include <string>

class ComputerSim {
public:
    ComputerSim(const int);

    virtual ~ComputerSim();

    int getIntAfterPipe() const;

private:
    int intAfterPipe;

    bool isChild(int) const;
    bool isParent(int) const;
    void tryPipe(int*, int*) const;
    int tryFork() const;
    void printThenExitFailure(const std::string) const;
};

#endif
