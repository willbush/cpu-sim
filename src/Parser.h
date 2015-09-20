#ifndef SRC_PARSER_H_
#define SRC_PARSER_H_
#include <string>

// mimicking static class like behavior
class Parser {
public:
    static bool tryPraseLine(const std::string&, int& value);
private:
    Parser() {};
};

#endif // SRC_PARSER_H_
