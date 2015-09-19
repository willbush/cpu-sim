#ifndef SRC_COMPUTERSIM_H_
#define SRC_COMPUTERSIM_H_
#include <string>

class ComputerSim {
public:
    ComputerSim(const int*);
    virtual ~ComputerSim();

private:
    bool isChild(int) const;
    bool isParent(int) const;
    void tryPipe(int*, int*) const;
    int tryFork() const;
    void printErrThenExitFailure(const std::string) const;
};

#endif // SRC_COMPUTERSIM_H_
