#ifndef SRC_CPU_H_
#define SRC_CPU_H_
#include <string>

class Cpu {
public:

    Cpu(const int&, const int&);
    virtual ~Cpu();
    int getAC();
    std::string getEvents() const;

private:
    static const int STACK_START_ADDRESS = 999;
    unsigned int ir, pc, ac, x, y, sp;
    std::string cpuEvents;
    static const int END_INSTRUCTION = 50;
    const int &READ_END_OF_PIPE, &WRITE_END_OF_PIPE;

    void waitForMemReadySignal();
    bool isReadySignal(char) const;
    int fetchInstruction();
    void processInstruction();
    int readFromMemory(int);
    void writeToMemory(const int, const int );
    bool endNotReached() const;
    void sendEndCommandToMemory();
    void loadValue();
    void loadValueFromAddress();
    void loadValueFromAddressPlusX();
    void loadValueFromAddressPlusY();
    void loadAddress();
    void storeAddress();
    void copyACtoX();
    void copyACtoY();
    void copyXtoAC();
    void copyYtoAC();
    void jumpToAddress(const int);
    void putPort();
    void runProcessor();
    void push(const int);
    int pop();
    void addXtoAC();
    void addYtoAC();
    void putRandInAC();
    void callAddress();
    void returnFromStack();
};

#endif // SRC_CPU_H_
