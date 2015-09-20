#include "Parser.h"
#include <sstream>

bool Parser::tryPraseLine(const std::string& line, int& outputValue) {
    int value;
    bool isParsed = false;

    if (line[0] == '.') {
        // remove period
        std::string subStr = line.substr(1, line.size() - 1);
        std::istringstream input(subStr);
        if (input >> value) {
            outputValue = -value;
            isParsed = true;
        }
    } else {
        std::istringstream input(line);
        if (input >> value) {
            outputValue = value;
            isParsed = true;
        }
    }
    return isParsed;
}
