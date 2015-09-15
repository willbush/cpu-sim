#include "cute.h"
#include "Memory.h"
#include "ide_listener.h"
#include "xml_listener.h"
#include "cute_runner.h"
#include "CPUsimTest.h"
#include "sys/wait.h"

void canLoad7intoAC() {
    int cpuToMem[2];
    int memToCpu[2];
    pipe(cpuToMem);
    pipe(memToCpu);
    int forkResult = fork();

    if (forkResult == 0) {
        int& cpuReadEnd = cpuToMem[0];
        int& cpuWriteEnd = memToCpu[1];
        int program[2] = { 10, 7 };
        Memory m(cpuReadEnd, cpuWriteEnd, program);
        _exit(EXIT_SUCCESS);
    } else {
        int& memWriteEnd = cpuToMem[1];
        int& memReadEnd = memToCpu[0];
        char readInstruction = 'R';
        char endInstruction = 'E';
        int readAddress = 0;
        write(memWriteEnd, &readInstruction, sizeof(char));
        write(memWriteEnd, &readAddress, sizeof(int));
        int value;
        read(memReadEnd, &value, sizeof(int));
        ASSERT_EQUAL(10, value);

        char writeInstruction = 'W';
        int writeAddress = 0;
        int writeVal = 7;
        write(memWriteEnd, &writeInstruction, sizeof(char));
        write(memWriteEnd, &writeAddress, sizeof(int));
        write(memWriteEnd, &writeVal, sizeof(int));
        write(memWriteEnd, &readInstruction, sizeof(char));
        write(memWriteEnd, &readAddress, sizeof(int));
        read(memReadEnd, &value, sizeof(int));
        ASSERT_EQUAL(7, value);

        write(memWriteEnd, &endInstruction, sizeof(char));
        waitpid(-1, NULL, 0); // wait for child
    }
}

void runSuite(int argc, char const *argv[]) {
    cute::xml_file_opener xmlfile(argc, argv);
    cute::xml_listener<cute::ide_listener<> > lis(xmlfile.out);
    cute::suite s;
    s.push_back(CUTE(canLoad7intoAC));
    cute::makeRunner(lis, argc, argv)(s, "CPUsimTest");
}

int main(int argc, char const *argv[]) {
    runSuite(argc, argv);
}
