#ifndef SRC_MEMORY_H_
#define SRC_MEMORY_H_

class Memory {
public:
    Memory(const int&, const int&, const int* program);
    virtual ~Memory();

private:
    static const int SPACE_AVAILABLE = 2000;
    const int &CPU_READ_END, &CPU_WRITE_END;
    int memory[SPACE_AVAILABLE];
    void initializeMemory(const int*);
    int readFromMemory() const;
    void writeToMemory();
    void listenForCpuCommands();
    void performCommand(char);
    void sendToCpu(int) const;
    bool cpuHasCommands(int command) const;
    bool isReadCommand(char command) const;
    bool isWriteCommand(char command) const;
};

#endif
