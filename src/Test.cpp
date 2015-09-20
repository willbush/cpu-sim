#include "cute.h"
#include "Memory.h"
#include "Parser.h"
#include "Cpu.h"
#include "ide_listener.h"
#include "xml_listener.h"
#include "cute_runner.h"
#include "sys/wait.h"

struct CpuSimTest {
    int _forkResult;
    int _cpuToMem[2];
    int _memToCpu[2];

    // serves as the test setup
    CpuSimTest() {
        pipe(_cpuToMem);
        pipe(_memToCpu);
        _forkResult = fork();
    }

    // serves as the test tear down.
    ~CpuSimTest() {
    }

    void canLoadValueIntoAC() {
        const int len = 5;
        int instructions[len] = { 1, 50, 50, -1000, 30 }; // load 50 into AC and exit
        assertCpuEventsEqual("w LV50 e", createVector(instructions, len));
    }

    void canLoadValueIntoAcFromAddress() {
        const int len = 9;
        int instructions[len] = { 1, 77, 7, 20, 3, 20, 50, -1000, 30 };
        assertCpuEventsEqual("w LV77 SA20,77 LVFA20,77 e", createVector(instructions, len));
    }

    void canStoreIntoAddressAndLoadFromAddress() {
        const int len = 9;
        // load 50 into AC, store AC into address 100, load from 100 into the AC and exit
        int instructions[len] = { 1, 50, 7, 100, 2, 100, 50, -1000, 30 };
        assertCpuEventsEqual("w LV50 SA100,50 LA100,50 e", createVector(instructions, len));
    }

    void canIncrementandDecrementX() {
        const int len = 6;
        // increment registers x twice and y once
        int instructions[len] = { 25, 25, 26, 50, -1000, 30 };
        assertCpuEventsEqual("w x++ x++ x-- e", createVector(instructions, len));
    }

    void canLoadValueFromAddressPlusXintoAC() {
        const int len = 10;
        int instructions[len] = { 1, 666, 7, 200, 25, 4, 199, 50, -1000, 30 };
        assertCpuEventsEqual("w LV666 SA200,666 x++ LVFAPX200,666 e", createVector(instructions, len));
    }

    void canLoadValueFromAddressPlusYintoAC() {
        const int len = 10;
        int instructions[len] = { 1, 10, 16, 7, 20, 5, 10, 50, -1000, 30 };
        assertCpuEventsEqual("w LV10 CY10 SA20,10 LVFAPY20,10 e", createVector(instructions, len));
    }

    void canCopyToX() {
        const int len = 6;
        int instructions[len] = { 1, 7, 14, 50, -1000, 30 };
        assertCpuEventsEqual("w LV7 CX7 e", createVector(instructions, len));
    }

    void canCopyToY() {
        const int len = 6;
        int instructions[len] = { 1, 7, 16, 50, -1000, 30 };
        assertCpuEventsEqual("w LV7 CY7 e", createVector(instructions, len));
    }

    void canCopyXtoAC() {
        const int len = 9;
        int instructions[len] = { 1, 7, 14, 1, 0, 15, 50, -1000, 30 };
        assertCpuEventsEqual("w LV7 CX7 LV0 CX2AC7 e", createVector(instructions, len));
    }

    void canCopyYtoAC() {
        const int len = 9;
        int instructions[len] = { 1, 7, 16, 1, 0, 17, 50, -1000, 30 };
        assertCpuEventsEqual("w LV7 CY7 LV0 CY2AC7 e", createVector(instructions, len));
    }

    void canJumpToAddress() {
        const int len = 8;
        // jump pass garbage -1 to the end (50)
        int instructions[len] = { 20, 5, -1, -1, -1, 50, -1000, 30 };
        assertCpuEventsEqual("w JA5 e", createVector(instructions, len));
    }

    void canJumpToAddressIfAcIsZero() {
        const int len = 14;
        int instructions[len] = { 1, 1, 21, 5, 1, 0, 21, 11, -1, -1, -1, 50, -1000, 30 };
        assertCpuEventsEqual("w LV1 LV0 JA11 e", createVector(instructions, len));
    }

    void canJumpToAddressIfAcIsNotZero() {
        const int len = 14;
        int instructions[len] = { 1, 0, 22, 5, 1, 1, 22, 11, -1, -1, -1, 50, -1000, 30 };
        assertCpuEventsEqual("w LV0 LV1 JA11 e", createVector(instructions, len));
    }

    void canPutPortOne() {
        const int len = 7;
        int instructions[len] = { 1, 10, 9, 1, 50, -1000, 30 };
        assertCpuEventsEqual("w LV10 P1,10 e", createVector(instructions, len));
    }

    void canPutPortTwo() {
        const int len = 7;
        int instructions[len] = { 1, 1101, 9, 2, 50, -1000, 30 };
        assertCpuEventsEqual("w LV1101 P2,M e", createVector(instructions, len));
    }

    void canPush() {
        const int len = 9;
        int instructions[len] = { 1, 9, 27, 1, 10, 27, 50, -1000, 30 };
        assertCpuEventsEqual("w LV9 PU998,9 LV10 PU997,10 e", createVector(instructions, len));
    }

    void canPushAndPop() {
        const int len = 11;
        int instructions[len] = { 1, 9, 27, 1, 10, 27, 28, 28, 50, -1000, 30 };
        assertCpuEventsEqual("w LV9 PU998,9 LV10 PU997,10 PO997,10 PO998,9 e", createVector(instructions, len));
    }

    void canPutRandInAC() {
        const int len = 4;
        int instructions[len] = { 8, 50, -1000, 30 };
        assertCpuEventsEqual("w R e", createVector(instructions, len));
    }

    void canAddXtoAC() {
        const int len = 7;
        int instructions[len] = { 1, 4, 25, 10, 50, -1000, 30 };
        assertCpuEventsEqual("w LV4 x++ AX2AC1,4 e", createVector(instructions, len));
    }

    void canAddYtoAC() {
        const int len = 7;
        int instructions[len] = { 1, 4, 16, 11, 50, -1000, 30 };
        assertCpuEventsEqual("w LV4 CY4 AY2AC4,4 e", createVector(instructions, len));
    }

    void canAddTwoRandAndPrint() {
        const int len = 12;
        int instructions[len] = { 8, 14, 8, 16, 8, 10, 11, 9, 1, 50, -1000, 30 };
        assertCpuEventsEqual("w R CX87 R CY78 R AX2AC87,16 AY2AC78,103 P1,181 e", createVector(instructions, len));
    }

    void canCallAddress() {
        const int len = 5;
        int instructions[len] = { 23, 2, 50, -1000, 30 };
        assertCpuEventsEqual("w PU998,2 JA2 CA2 e", createVector(instructions, len));
    }

    void canReturnFromStack() {
        const int len = 8;
        int instructions[len] = { 23, 3, 50, 25, 25, 24, -1000, 30 };
        assertCpuEventsEqual("w PU998,2 JA3 CA3 x++ x++ PO998,2 JA2 RET2 e", createVector(instructions, len));
    }

    void canPrintHi() {
        const int len = 18;
        int instructions[len] = { 1, 72, 9, 2, 1, 73, 9, 2, 23, 11, 50, 1, 10, 9, 2, 24, -1000, 30 };
        assertCpuEventsEqual("w LV72 P2,H LV73 P2,I PU998,10 JA11 CA11 LV10 P2,\n PO998,10 JA10 RET10 e",
                createVector(instructions, len));
    }

    void canSubtractACbyX() {
        const int len = 7;
        int instructions[len] = { 1, 5, 14, 12, 50, -1000, 30 };
        assertCpuEventsEqual("w LV5 CX5 SUBX5 e", createVector(instructions, len));
    }

    void canSubtractACbyY() {
        const int len = 7;
        int instructions[len] = { 1, 5, 16, 13, 50, -1000, 30 };
        assertCpuEventsEqual("w LV5 CY5 SUBY5 e", createVector(instructions, len));
    }

    void canChangeSP2AC() {
        const int len = 6;
        int instructions[len] = { 1, 88, 18, 50, -1000, 30 };
        assertCpuEventsEqual("w LV88 CSP2AC88,88 e", createVector(instructions, len));
    }

    void canChangeAC2SP() {
        const int len = 6;
        int instructions[len] = { 1, 88, 19, 50, -1000, 30 };
        assertCpuEventsEqual("w LV88 CAC2SP999,999 e", createVector(instructions, len));
    }

    void canInterruptAndReturn() {
        const int len = 19;
        int instructions[len] = { 1, 99, 27, 29, 28, 50, -1000, 30, -1500, 19, 9, 1, 1, 10, 9, 2, 30 };
        assertCpuEventsEqual(
                "w LV99 PU998,99 PU997,4 PU1998,997 SM,1 CAC2SP1998,1998 P1,1998 LV10 P2,\n PO1998,997 PO997,4 SM,0 PO998,99 e",
                createVector(instructions, len));
    }

    void canPreventRecursiveInterrupts() {
        const int len = 11;
        int instructions[len] = { 1, 99, 27, 29, 50, -1000, 30, -1500, 29, 29, 30 };
        assertCpuEventsEqual("w LV99 PU998,99 PU997,4 PU1998,997 SM,1 PO1998,997 PO997,4 SM,0 e",
                createVector(instructions, len));
    }

    void canLoadFromSPplusX() {
        const int len = 7;
        int instructions[len] = { 1, -999, 14, 6, 50, -1000, 30 };
        assertCpuEventsEqual("w LV-999 CX-999 LSP+X0,1 e", createVector(instructions, len));
    }

    // should print out ABCDEFGHIJKLMNOPQRSTUVWXYZ12345678910
    void acceptanceTest() {
        const int len = 1000;
        int instructions[len] = { 1, 0, 14, 4, 32, 21, 12, 9, 2, 25, 20, 3, 1, 0, 16, 5, 59, 21, 27, 9, 1, 1, 1, 11, 16,
                20, 15, 1, 10, 9, 2, 50, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84,
                85, 86, 87, 88, 89, 90, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 0, -1000, 30 };
        std::string exptected = "w LV0 CX0 LVFAPX32,65 P2,A x++ JA3 LVFAPX33,66 P2,B x++ PU998,10 PU1998,998 SM,1 "
                "PO1998,998 PO998,10 SM,0 JA3 LVFAPX34,67 P2,C x++ JA3 LVFAPX35,68 P2,D x++ PU998,10 PU1998,998 "
                "SM,1 PO1998,998 PO998,10 SM,0 JA3 LVFAPX36,69 P2,E x++ JA3 LVFAPX37,70 P2,F x++ PU998,10 PU1998,998 "
                "SM,1 PO1998,998 PO998,10 SM,0 JA3 LVFAPX38,71 P2,G x++ JA3 LVFAPX39,72 P2,H x++ PU998,10 PU1998,998 "
                "SM,1 PO1998,998 PO998,10 SM,0 JA3 LVFAPX40,73 P2,I x++ JA3 LVFAPX41,74 P2,J x++ PU998,10 PU1998,998 "
                "SM,1 PO1998,998 PO998,10 SM,0 JA3 LVFAPX42,75 P2,K x++ JA3 LVFAPX43,76 P2,L x++ PU998,10 PU1998,998 "
                "SM,1 PO1998,998 PO998,10 SM,0 JA3 LVFAPX44,77 P2,M x++ JA3 LVFAPX45,78 P2,N x++ PU998,10 PU1998,998 "
                "SM,1 PO1998,998 PO998,10 SM,0 JA3 LVFAPX46,79 P2,O x++ JA3 LVFAPX47,80 P2,P x++ PU998,10 PU1998,998 "
                "SM,1 PO1998,998 PO998,10 SM,0 JA3 LVFAPX48,81 P2,Q x++ JA3 LVFAPX49,82 P2,R x++ PU998,10 PU1998,998 "
                "SM,1 PO1998,998 PO998,10 SM,0 JA3 LVFAPX50,83 P2,S x++ JA3 LVFAPX51,84 P2,T x++ PU998,10 PU1998,998 "
                "SM,1 PO1998,998 PO998,10 SM,0 JA3 LVFAPX52,85 P2,U x++ JA3 LVFAPX53,86 P2,V x++ PU998,10 PU1998,998 "
                "SM,1 PO1998,998 PO998,10 SM,0 JA3 LVFAPX54,87 P2,W x++ JA3 LVFAPX55,88 P2,X x++ PU998,10 PU1998,998 "
                "SM,1 PO1998,998 PO998,10 SM,0 JA3 LVFAPX56,89 P2,Y x++ JA3 LVFAPX57,90 P2,Z x++ PU998,10 PU1998,998 "
                "SM,1 PO1998,998 PO998,10 SM,0 JA3 LVFAPX58,0 JA12 LV0 CY0 LVFAPY59,1 P1,1 LV1 AY2AC0,1 PU998,24 PU1998,998 "
                "SM,1 PO1998,998 PO998,24 SM,0 CY1 JA15 LVFAPY60,2 P1,2 LV1 AY2AC1,1 CY2 JA15 LVFAPY61,3 PU998,17 PU1998,998 "
                "SM,1 PO1998,998 PO998,17 SM,0 P1,3 LV1 AY2AC2,1 CY3 JA15 LVFAPY62,4 P1,4 LV1 PU998,23 PU1998,998 SM,1 "
                "PO1998,998 PO998,23 SM,0 AY2AC3,1 CY4 JA15 LVFAPY63,5 P1,5 LV1 AY2AC4,1 CY5 JA15 PU998,15 PU1998,998 "
                "SM,1 PO1998,998 PO998,15 SM,0 LVFAPY64,6 P1,6 LV1 AY2AC5,1 CY6 JA15 LVFAPY65,7 P1,7 PU998,21 PU1998,998 "
                "SM,1 PO1998,998 PO998,21 SM,0 LV1 AY2AC6,1 CY7 JA15 LVFAPY66,8 P1,8 LV1 AY2AC7,1 CY8 PU998,25 PU1998,998 "
                "SM,1 PO1998,998 PO998,25 SM,0 JA15 LVFAPY67,9 P1,9 LV1 AY2AC8,1 CY9 JA15 LVFAPY68,10 PU998,19 PU1998,998 "
                "SM,1 PO1998,998 PO998,19 SM,0 P1,10 LV1 AY2AC9,1 CY10 JA15 LVFAPY69,0 JA27 LV10 P2,\n PU998,32 PU1998,998 SM,1 e";
        assertCpuEventsEqual(exptected, createVector(instructions, len));
    }

private:
    std::vector<int> createVector(int array[], const int len) {
        std::vector<int> v(&array[0], &array[0] + len);
        return v;
    }

    void assertCpuEventsEqual(const std::string& expected, const std::vector<int>& program) {
        if (_forkResult == 0) {
            Memory m(_cpuToMem[0], _memToCpu[1], program);
            _exit(EXIT_SUCCESS);
        } else {
            Cpu c(_memToCpu[0], _cpuToMem[1], 10);
            ASSERT_EQUAL(expected, c.getEvents());
            waitpid(-1, NULL, 0); // wait for child
        }
    }
};

struct ParserTest {
    ParserTest() {
    }

    ~ParserTest() {
    }

    void canIgnoreWhitespace() {
        int value = -1;
        int result = Parser::tryPraseLine(" \n", value);
        ASSERT_EQUAL(false, result);
        ASSERT_EQUAL(-1, value);
    }

    void canIgnoreOnlyCommet() {
        int value = -1;
        int result = Parser::tryPraseLine("// 10 useful comment\n", value);
        ASSERT_EQUAL(false, result);
        ASSERT_EQUAL(-1, value);
    }

    void canIgnoreCommet() {
        int value;
        int result = Parser::tryPraseLine("23 //useful comment\n", value);
        ASSERT_EQUAL(true, result);
        ASSERT_EQUAL(23, value);
    }

    void canParseLoadAddress() {
        int value;
        int result = Parser::tryPraseLine(".1000 //useful comment\n", value);
        ASSERT_EQUAL(true, result);
        ASSERT_EQUAL(-1000, value);
    }
};

void runSuite(int argc, char const *argv[]) {
    cute::xml_file_opener xmlfile(argc, argv);
    cute::xml_listener<cute::ide_listener<> > lis(xmlfile.out);
    cute::suite s;
    s.push_back(CUTE_SMEMFUN(CpuSimTest, canLoadValueIntoAC));
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
    s.push_back(CUTE_SMEMFUN(CpuSimTest, canPutPortOne));
    s.push_back(CUTE_SMEMFUN(CpuSimTest, canPutPortTwo));
    s.push_back(CUTE_SMEMFUN(CpuSimTest, canPush));
    s.push_back(CUTE_SMEMFUN(CpuSimTest, canPushAndPop));
    s.push_back(CUTE_SMEMFUN(CpuSimTest, canPutRandInAC));
    s.push_back(CUTE_SMEMFUN(CpuSimTest, canAddXtoAC));
    s.push_back(CUTE_SMEMFUN(CpuSimTest, canAddYtoAC));
    s.push_back(CUTE_SMEMFUN(CpuSimTest, canAddTwoRandAndPrint));
    s.push_back(CUTE_SMEMFUN(CpuSimTest, canCallAddress));
    s.push_back(CUTE_SMEMFUN(CpuSimTest, canReturnFromStack));
    s.push_back(CUTE_SMEMFUN(CpuSimTest, canPrintHi));
    s.push_back(CUTE_SMEMFUN(CpuSimTest, canSubtractACbyX));
    s.push_back(CUTE_SMEMFUN(CpuSimTest, canSubtractACbyY));
    s.push_back(CUTE_SMEMFUN(CpuSimTest, canChangeSP2AC));
    s.push_back(CUTE_SMEMFUN(CpuSimTest, canChangeAC2SP));
    s.push_back(CUTE_SMEMFUN(CpuSimTest, canInterruptAndReturn));
    s.push_back(CUTE_SMEMFUN(CpuSimTest, canLoadFromSPplusX));
    s.push_back(CUTE_SMEMFUN(CpuSimTest, canPreventRecursiveInterrupts));
    s.push_back(CUTE_SMEMFUN(CpuSimTest, acceptanceTest));

    s.push_back(CUTE_SMEMFUN(ParserTest, canIgnoreWhitespace));
    s.push_back(CUTE_SMEMFUN(ParserTest, canIgnoreCommet));
    s.push_back(CUTE_SMEMFUN(ParserTest, canParseLoadAddress));
    s.push_back(CUTE_SMEMFUN(ParserTest, canIgnoreOnlyCommet));

    cute::makeRunner(lis, argc, argv)(s, "CPUsimTest");
}

int main(int argc, char const *argv[]) {
    runSuite(argc, argv);
}
