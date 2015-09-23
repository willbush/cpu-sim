#ifndef SRC_COMPUTERSIM_H_
#define SRC_COMPUTERSIM_H_
#include <string>
#include <vector>

class ComputerSim {
public:
    ComputerSim(const std::vector<std::string>&, const int);
    virtual ~ComputerSim();

private:
    bool isChild(int) const;
    bool isParent(int) const;
    void tryPipe(int*, int*) const;
    int tryFork() const;
};

#endif // SRC_COMPUTERSIM_H_
