#include "cute.h"
#include "ide_listener.h"
#include "xml_listener.h"
#include "cute_runner.h"
#include "CPUsimTest.h"
#include "ComputerSim.h"

void canIncrementInt() {
    ComputerSim cs(10);
    ASSERT_EQUAL(11, cs.getIntAfterPipe());
}

void runSuite(int argc, char const *argv[]) {
    cute::xml_file_opener xmlfile(argc, argv);
    cute::xml_listener<cute::ide_listener<> > lis(xmlfile.out);
    cute::suite s;
    s.push_back(CUTE(canIncrementInt));
    cute::makeRunner(lis, argc, argv)(s, "CPUsimTest");
}

int main(int argc, char const *argv[]) {
    runSuite(argc, argv);
}
