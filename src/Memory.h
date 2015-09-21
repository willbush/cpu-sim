#ifndef SRC_MEMORY_H_
#define SRC_MEMORY_H_
#include <vector>
#include <string>

class Memory {
public:
    Memory(const int&, const int&, const std::vector<std::string>&);
    virtual ~Memory();

private:
    static const int SPACE_AVAILABLE = 2000;
    static const int TIMER_INTERRUPT_HANDLER_START_ADDRESS = 1000;
    static const int INTERRUPT_HANDLER_START_ADDRESS = 1500;
    const int &READ_END_OF_PIPE, &WRITE_END_OF_PIPE;
    int memory[SPACE_AVAILABLE];

    void initializeMemory(const std::vector<std::string>&);
    void writeToMemory();
    void listenForCpuCommands();
    void performCommand(char);
    void sendReadySignal() const;
    int readFromMemory() const;
    void sendToCpu(int) const;
    bool cpuHasCommands(int) const;
    bool isReadCommand(char) const;
    bool isWriteCommand(char) const;
};

#endif // SRC_MEMORY_H_
