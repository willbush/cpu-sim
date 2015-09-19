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
    int pc, ac, x, y;
    std::string cpuEvents;
    static const int END_INSTRUCTION = 50;
    const int &READ_END_OF_PIPE, &WRITE_END_OF_PIPE;

    void waitForMemReadySignal();
    bool isReadySignal(char) const;
    int fetchInstruction();
    int readFromMemory(int);
    void writeToMemory(const int, const int );
    bool endNotReached(int) const;
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
    void jumpToAddress();
    void putPort();
    void runProcessor();
    void processInstruction(int);
};

#endif // SRC_CPU_H_
