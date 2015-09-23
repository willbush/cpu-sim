#ifndef SRC_CPU_H_
#define SRC_CPU_H_

class Cpu {
public:

    Cpu(const int&, const int&, const int);
    virtual ~Cpu();

private:
    static const int USER_STACK_START_ADDRESS = 1000;
    static const int SYSTEM_STACK_START_ADDRESS = 2000;
    static const int SYSTEM_START_ADDRESS = 1000;
    static const int TIMER_INTERRUPT_HANDLER_ADDRESS = 1000;
    static const int INTERRUPT_HANDLER_ADDRESS = 1500;
    static const int END_INSTRUCTION = 50;
    int _ir, _pc, _ac, _x, _y, _sp;
    unsigned int _interruptInterval;
    const int &READ_END_OF_PIPE, &WRITE_END_OF_PIPE;
    bool _inSystemMode, _interruptEnabled;

    void waitForMemReadySignal();
    bool isReadySignal(char) const;
    int fetchInstruction();
    void processInstruction();
    int readFromMemory(int);
    void writeToMemory(const int, const int );
    bool endNotReached() const;
    void sendEndCommandToMemory();
    void jumpToAddress(const int);
    void putPort();
    void runProcessor();
    void push(const int);
    int pop();
    void putRandInAC();
    void callAddress();
    void interrupt(const int);
    void returnFromInterrupt();
    void checkForAccessVioloation(int);
    unsigned int checkAndUpdateTimer(unsigned int);
};

#endif // SRC_CPU_H_
