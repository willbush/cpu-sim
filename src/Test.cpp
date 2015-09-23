#include "cute.h"
#include "Memory.h"
#include "Cpu.h"
#include "ide_listener.h"
#include "xml_listener.h"
#include "cute_runner.h"
#include "sys/wait.h"
#include "stdlib.h"

struct CpuSimTest {
    int _forkResult;
    int _cpuToMem[2];
    int _memToCpu[2];
    std::streambuf * old;
    std::stringstream coutBuffer;

    // serves as the test setup
    CpuSimTest() {
        // redirect cout buffer to my own to test against
        old = std::cout.rdbuf(coutBuffer.rdbuf());
        pipe(_cpuToMem);
        pipe(_memToCpu);
        _forkResult = fork();
    }

    // serves as the test tear down.
    ~CpuSimTest() {
        // reset cout back to original buffer
        std::cout.rdbuf(old);
    }

    void canLoadValueIntoAcFromAddress() {
        const int len = 9;
        std::string instructions[len] = { "1", "77", "7", "20", "3", "20", "50", ".1000", "30" };
        runCpu(createVector(instructions, len));
        ASSERT_EQUAL("", coutBuffer.str());
    }

    void canStoreIntoAddressAndLoadFromAddress() {
        const int len = 9;
        // load 50 into AC, store AC into address 100, load from 100 into the AC and exit
        std::string instructions[len] = { "1", "50", "7", "100", "2", "100", "50", ".1000", "30" };
        runCpu(createVector(instructions, len));
        ASSERT_EQUAL("", coutBuffer.str());
    }

    void canIncrementandDecrementX() {
        const int len = 6;
        // increment registers x twice and y once
        std::string instructions[len] = { "25", "25", "26", "50", ".1000", "30" };
        runCpu(createVector(instructions, len));
        ASSERT_EQUAL("", coutBuffer.str());
    }

    void canLoadValueFromAddressPlusXintoAC() {
        const int len = 10;
        std::string instructions[len] = { "1", "666", "7", "200", "25", "4", "199", "50", ".1000", "30" };
        runCpu(createVector(instructions, len));
        ASSERT_EQUAL("", coutBuffer.str());
    }

    void canLoadValueFromAddressPlusYintoAC() {
        const int len = 10;
        std::string instructions[len] = { "1", "10", "16", "7", "20", "5", "10", "50", ".1000", "30" };
        runCpu(createVector(instructions, len));
        ASSERT_EQUAL("", coutBuffer.str());
    }

    void canCopyToX() {
        const int len = 6;
        std::string instructions[len] = { "1", "7", "14", "50", ".1000", "30" };
        runCpu(createVector(instructions, len));
        ASSERT_EQUAL("", coutBuffer.str());
    }

    void canCopyToY() {
        const int len = 6;
        std::string instructions[len] = { "1", "7", "16", "50", ".1000", "30" };
        runCpu(createVector(instructions, len));
        ASSERT_EQUAL("", coutBuffer.str());
    }

    void canCopyXtoAC() {
        const int len = 9;
        std::string instructions[len] = { "1", "7", "14", "1", "0", "15", "50", ".1000", "30" };
        runCpu(createVector(instructions, len));
        ASSERT_EQUAL("", coutBuffer.str());
    }

    void canCopyYtoAC() {
        const int len = 9;
        std::string instructions[len] = { "1", "7", "16", "1", "0", "17", "50", ".1000", "30" };
        runCpu(createVector(instructions, len));
        ASSERT_EQUAL("", coutBuffer.str());
    }

    void canJumpToAddress() {
        const int len = 8;
        // jump pass garbage -1 to the end (50)
        std::string instructions[len] = { "20", "5", "99", "99", "99", "50", ".1000", "30" };
        runCpu(createVector(instructions, len));
        ASSERT_EQUAL("", coutBuffer.str());
    }

    void canJumpToAddressIfAcIsZero() {
        const int len = 14;
        std::string instructions[len] = { "1", "1", "21", "5", "1", "0", "21", "11", "99", "99", "99", "50", ".1000",
                "30" };
        runCpu(createVector(instructions, len));
        ASSERT_EQUAL("", coutBuffer.str());
    }

    void canJumpToAddressIfAcIsNotZero() {
        const int len = 14;
        std::string instructions[len] = { "1", "0", "22", "5", "1", "1", "22", "11", "99", "99", "99", "50", ".1000",
                "30" };
        runCpu(createVector(instructions, len));
        ASSERT_EQUAL("", coutBuffer.str());
    }

    void canPutPortOne() {
        const int len = 7;
        std::string instructions[len] = { "1", "10", "9", "1", "50", ".1000", "30" };
        runCpu(createVector(instructions, len));
        ASSERT_EQUAL("10", coutBuffer.str());
    }

    void canPutPortTwo() {
        const int len = 7;
        std::string instructions[len] = { "1", "1101", "9", "2", "50", ".1000", "30" };
        runCpu(createVector(instructions, len));
        ASSERT_EQUAL("M", coutBuffer.str());
    }

    void canPush() {
        const int len = 9;
        std::string instructions[len] = { "1", "9", "27", "1", "10", "27", "50", ".1000", "30" };
        runCpu(createVector(instructions, len));
        ASSERT_EQUAL("", coutBuffer.str());
    }

    void canPushAndPop() {
        const int len = 11;
        std::string instructions[len] = { "1", "9", "27", "1", "10", "27", "28", "28", "50", ".1000", "30" };
        runCpu(createVector(instructions, len));
        ASSERT_EQUAL("", coutBuffer.str());
    }

    void canPutRandInAC() {
        const int len = 4;
        std::string instructions[len] = { "8", "50", "1000", "30" };
        runCpu(createVector(instructions, len));
        ASSERT_EQUAL("", coutBuffer.str());
    }

    void canAddXtoAC() {
        const int len = 7;
        std::string instructions[len] = { "1", "4", "25", "10", "50", ".1000", "30" };
        runCpu(createVector(instructions, len));
        ASSERT_EQUAL("", coutBuffer.str());
    }

    void canAddYtoAC() {
        const int len = 7;
        std::string instructions[len] = { "1", "4", "16", "11", "50", ".1000", "30" };
        runCpu(createVector(instructions, len));
        ASSERT_EQUAL("", coutBuffer.str());
    }

    void canPrintRandomInt() {
        const int len = 6;
        std::string instructions[len] = { "8", "9", "1", "50", ".1000", "30" };
        runCpu(createVector(instructions, len));
        int actual = atoi(coutBuffer.str().c_str());
        ASSERT_GREATER_EQUAL(actual, 1);
        ASSERT_LESS_EQUAL(actual, 100);
    }

    void canAddThreeRandAndPrint() {
        const int len = 12;
        std::string instructions[len] = { "8", "14", "8", "16", "8", "10", "11", "9", "1", "50", ".1000", "30" };
        runCpu(createVector(instructions, len));
        int actual = atoi(coutBuffer.str().c_str());
        ASSERT_GREATER_EQUAL(actual, 1);
        ASSERT_LESS_EQUAL(actual, 300);
    }

    void canCallAddress() {
        const int len = 5;
        std::string instructions[len] = { "23", "2", "50", ".1000", "30" };
        runCpu(createVector(instructions, len));
        ASSERT_EQUAL("", coutBuffer.str());
    }

    void canReturnFromStack() {
        const int len = 8;
        std::string instructions[len] = { "23", "3", "50", "25", "25", "24", ".1000", "30" };
        runCpu(createVector(instructions, len));
        ASSERT_EQUAL("", coutBuffer.str());
    }

    void canPrintHi() {
        const int len = 18;
        std::string instructions[len] = { "1", "72", "9", "2", "1", "73", "9", "2", "23", "11", "50", "1", "10", "9",
                "2", "24", ".1000", "30" };
        runCpu(createVector(instructions, len));
        ASSERT_EQUAL("HI\n", coutBuffer.str());
    }

    void canSubtractACbyX() {
        const int len = 7;
        std::string instructions[len] = { "1", "5", "14", "12", "50", ".1000", "30" };
        runCpu(createVector(instructions, len));
        ASSERT_EQUAL("", coutBuffer.str());
    }

    void canSubtractACbyY() {
        const int len = 7;
        std::string instructions[len] = { "1", "5", "16", "13", "50", ".1000", "30" };
        runCpu(createVector(instructions, len));
        ASSERT_EQUAL("", coutBuffer.str());
    }

    void canChangeSP2AC() {
        const int len = 6;
        std::string instructions[len] = { "1", "88", "18", "50", ".1000", "30" };
        runCpu(createVector(instructions, len));
        ASSERT_EQUAL("", coutBuffer.str());
    }

    void canChangeAC2SP() {
        const int len = 6;
        std::string instructions[len] = { "1", "88", "19", "50", ".1000", "30" };
        runCpu(createVector(instructions, len));
        ASSERT_EQUAL("", coutBuffer.str());
    }

    void canInterruptAndReturn() {
        const int len = 19;
        std::string instructions[len] = { "1", "99", "27", "29", "28", "50", ".1000", "30", ".1500", "19", "9", "1",
                "1", "10", "9", "2", "30" };
        runCpu(createVector(instructions, len));
        ASSERT_EQUAL("1998\n", coutBuffer.str());
    }

    void canPreventRecursiveInterrupts() {
        const int len = 11;
        std::string instructions[len] = { "1", "99", "27", "29", "50", ".1000", "30", ".1500", "29", "29", "30" };
        runCpu(createVector(instructions, len));
        ASSERT_EQUAL("", coutBuffer.str());
    }

    void canLoadFromSPplusX() {
        const int len = 11;
        std::string instructions[len] = { "1", "7", "27", "1", "5", "27", "25", "6", "50", ".1000", "30" };
        runCpu(createVector(instructions, len));
        ASSERT_EQUAL("", coutBuffer.str());
    }

    void canPrintData() {
        const int len = 100;
        std::string instructions[len] = { "1", "0", "14", "4", "32", "21", "12", "9", "2", "25", "20", "3", "1", "0",
                "16", "5", "59", "21", "27", "9", "1", "1", "1", "11", "16", "20", "15", "1", "10", "9", "2", "50",
                "65", "66", "67", "68", "69", "70", "71", "72", "73", "74", "75", "76", "77", "78", "79", "80", "81",
                "82", "83", "84", "85", "86", "87", "88", "89", "90", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
                "10", "0", ".1000", "30" };
        runCpu(createVector(instructions, len));
        ASSERT_EQUAL("ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678910\n", coutBuffer.str());
    }

    void memAccessAndStackTest() {
        const int len = 100;
        std::string instructions[len] = { "19", "9", "1", "1", "10", "9", "2", "27", "19", "9", "1", "1", "10", "9",
                "2", "28", "19", "9", "1", "1", "10", "9", "2", "29", "2", "1000", "50", ".1000", "30", ".1500", "19",
                "9", "1", "1", "10", "9", "2", "27", "19", "9", "1", "1", "10", "9", "2", "28", "19", "9", "1", "1",
                "10", "9", "2", "30" };
        ASSERT_THROWSM("ERROR: Attempted to access a protected memory location.",
                runCpu(createVector(instructions, len)), std::runtime_error);
        ASSERT_EQUAL("1000\n999\n1000\n1998\n1997\n1998\n", coutBuffer.str());
    }

    void canPrintHappyFace() {
        const int len = 240;
        std::string instructions[len] = { ".0", "23", "15", "23", "30", "23", "51", "23", "86", "23", "103", "23", "142",
                "23", "163", "50", "1", "4", "27", "23", "206", "28", "1", "6", "27", "23", "178", "28", "23", "220",
                "24", "1", "32", "9", "2", "1", "47", "9", "2", "1", "9", "27", "23", "206", "28", "1", "92", "9", "2",
                "23", "220", "24", "1", "47", "9", "2", "1", "32", "9", "2", "9", "2", "9", "2", "23", "225", "1", "32",
                "9", "2", "9", "2", "23", "225", "1", "32", "9", "2", "9", "2", "1", "92", "9", "2", "23", "220", "24",
                "1", "124", "9", "2", "1", "11", "27", "23", "206", "28", "1", "124", "9", "2", "23", "220", "24", "1",
                "92", "9", "2", "1", "32", "9", "2", "9", "2", "9", "2", "1", "92", "9", "2", "1", "4", "27", "23",
                "192", "28", "1", "47", "9", "2", "1", "32", "9", "2", "9", "2", "1", "47", "9", "2", "23", "220", "24",
                "1", "32", "9", "2", "1", "92", "9", "2", "1", "9", "27", "23", "206", "28", "1", "47", "9", "2", "23",
                "220", "24", "1", "4", "27", "23", "206", "28", "1", "6", "27", "23", "178", "28", "23", "220", "24",
                "1", "1", "14", "6", "14", "1", "45", "9", "2", "26", "15", "22", "183", "24", "1", "1", "14", "6",
                "14", "1", "95", "9", "2", "26", "15", "22", "197", "24", "1", "1", "14", "6", "14", "1", "32", "9",
                "2", "26", "15", "22", "211", "24", "1", "10", "9", "2", "24", "1", "45", "9", "2", "1", "42", "9", "2",
                "24", ".1000", "30" };
        std::string expected = "    ------\n /         \\\n/   -*  -*  \\\n|           |\n\\   \\____/  /\n \\         /\n    ------\n";
        runCpu(createVector(instructions, len));
        ASSERT_EQUAL(expected, coutBuffer.str());
    }

    void interruptAcceptanceTest() {
        const int len = 70;
        std::string instructions[len] = { "1", "10", "14", "1", "65", "9", "2", "1", "10", "9", "2", "29", "26", "15",
                "22", "3", "50", ".1000", "27", "15", "27", "17", "27", "2", "1700", "14", "25", "15", "7", "1700", "28",
                "16", "28", "14", "28", "30", ".1500", "27", "15", "27", "17", "27", "2", "1700", "9", "1", "1", "10",
                "9", "2", "28", "16", "28", "14", "28", "30", ".1700", "0" };
        std::string expected = "A\n0\nA\n1\nA\n2\nA\n2\nA\n3\nA\n4\nA\n4\nA\n5\nA\n6\nA\n6\n";
        runCpu(createVector(instructions, len));
        ASSERT_EQUAL(expected, coutBuffer.str());
    }

private:

    std::vector<std::string> createVector(std::string array[], const int len) {
        std::vector<std::string> v(&array[0], &array[0] + len);
        return v;
    }

    void runCpu(const std::vector<std::string>& program) {
        if (_forkResult == 0) {
            Memory m(_cpuToMem[0], _memToCpu[1], program);
            _exit(EXIT_SUCCESS);
        } else {
            Cpu c(_memToCpu[0], _cpuToMem[1], 10);
            waitpid(-1, NULL, 0); // wait for child
        }
    }
};

void runSuite(int argc, char const *argv[]) {
    cute::xml_file_opener xmlfile(argc, argv);
    cute::xml_listener<cute::ide_listener<> > lis(xmlfile.out);
    cute::suite s;
    s.push_back(CUTE_SMEMFUN(CpuSimTest, canPrintData));
    s.push_back(CUTE_SMEMFUN(CpuSimTest, memAccessAndStackTest));
    s.push_back(CUTE_SMEMFUN(CpuSimTest, canPrintHappyFace));
    s.push_back(CUTE_SMEMFUN(CpuSimTest, interruptAcceptanceTest));
    s.push_back(CUTE_SMEMFUN(CpuSimTest, canPutPortOne));
    s.push_back(CUTE_SMEMFUN(CpuSimTest, canPutPortTwo));
    s.push_back(CUTE_SMEMFUN(CpuSimTest, canAddThreeRandAndPrint));
    s.push_back(CUTE_SMEMFUN(CpuSimTest, canPrintHi));
    s.push_back(CUTE_SMEMFUN(CpuSimTest, canInterruptAndReturn));
	s.push_back(CUTE_SMEMFUN(CpuSimTest, canLoadValueIntoAcFromAddress));
	s.push_back(CUTE_SMEMFUN(CpuSimTest, canStoreIntoAddressAndLoadFromAddress));
	s.push_back(CUTE_SMEMFUN(CpuSimTest, canIncrementandDecrementX));
	s.push_back(CUTE_SMEMFUN(CpuSimTest, canLoadValueFromAddressPlusXintoAC));
	s.push_back(CUTE_SMEMFUN(CpuSimTest, canLoadValueFromAddressPlusYintoAC));
	s.push_back(CUTE_SMEMFUN(CpuSimTest, canCopyToX));
	s.push_back(CUTE_SMEMFUN(CpuSimTest, canCopyToY));
	s.push_back(CUTE_SMEMFUN(CpuSimTest, canCopyXtoAC));
	s.push_back(CUTE_SMEMFUN(CpuSimTest, canCopyYtoAC));
	s.push_back(CUTE_SMEMFUN(CpuSimTest, canJumpToAddress));
	s.push_back(CUTE_SMEMFUN(CpuSimTest, canJumpToAddressIfAcIsZero));
	s.push_back(CUTE_SMEMFUN(CpuSimTest, canJumpToAddressIfAcIsNotZero));
	s.push_back(CUTE_SMEMFUN(CpuSimTest, canPush));
	s.push_back(CUTE_SMEMFUN(CpuSimTest, canPushAndPop));
	s.push_back(CUTE_SMEMFUN(CpuSimTest, canPutRandInAC));
	s.push_back(CUTE_SMEMFUN(CpuSimTest, canAddXtoAC));
	s.push_back(CUTE_SMEMFUN(CpuSimTest, canAddYtoAC));
	s.push_back(CUTE_SMEMFUN(CpuSimTest, canCallAddress));
	s.push_back(CUTE_SMEMFUN(CpuSimTest, canReturnFromStack));
	s.push_back(CUTE_SMEMFUN(CpuSimTest, canSubtractACbyX));
	s.push_back(CUTE_SMEMFUN(CpuSimTest, canSubtractACbyY));
	s.push_back(CUTE_SMEMFUN(CpuSimTest, canChangeSP2AC));
	s.push_back(CUTE_SMEMFUN(CpuSimTest, canChangeAC2SP));
	s.push_back(CUTE_SMEMFUN(CpuSimTest, canPreventRecursiveInterrupts));
	s.push_back(CUTE_SMEMFUN(CpuSimTest, canLoadFromSPplusX));
	s.push_back(CUTE_SMEMFUN(CpuSimTest, canPrintRandomInt));

    cute::makeRunner(lis, argc, argv)(s, "CPUsimTest");
}

int main(int argc, char const *argv[]) {
    runSuite(argc, argv);
}
