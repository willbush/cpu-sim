#include "ComputerSim.h"
#include "stdlib.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;

void checkArgCount(int);
int tryParseTimer(const string&);
void checkTimerInterval(const int);
void checkIfFileExist(const string&);
bool fileExist(const std::string&);
vector<int> parseSourceFile(const string&);
bool tryPraseLine(const std::string&, int&);

//int main(int argc, char const *argv[]) {
//    checkArgCount(argc);
//    const string filepath = argv[1];
//    const string timerIntervalStr = argv[2];
//    const int timerInterval = tryParseTimer(timerIntervalStr);
//
//    checkTimerInterval(timerInterval);
//    checkIfFileExist(filepath);
//
//    vector<int> program = parseSourceFile(filepath);
//    ComputerSim c(program, timerInterval);
//}

void checkArgCount(int argc) {
    const int requiredArgs = 3;
    if (argc != requiredArgs) {
        cerr << "Please pass the source file name and timer interrupt interval as arguments\n";
        cerr << "e.g. ./CPUsim source.txt 10" << endl;
        exit(EXIT_FAILURE);
    }
}

int tryParseTimer(const string& intervalTimerStr) {
    istringstream ss(intervalTimerStr);
    int intervalTimer;
    if (!(ss >> intervalTimer)) {
        cerr << "Failed to parse " << intervalTimerStr << " into an integer.\n";
        cerr << "Please ensure your arguments are in the following format:\n";
        cerr << "e.g. ./CPUsim source.txt 10" << endl;
        exit(EXIT_FAILURE);
    }
    return intervalTimer;
}


void checkTimerInterval(const int timerInterval) {
    if (timerInterval <= 0) {
        cerr << "timer interval: " << timerInterval << " must be greater than 0\n";
        exit(EXIT_FAILURE);
    }
}

void checkIfFileExist(const string& filepath) {
    if (!fileExist(filepath)) {
        cerr << "source file: " << filepath << " not found\n";
        exit(EXIT_FAILURE);
    }
}

bool fileExist(const std::string& path) {
    return ifstream(path.c_str());
}

vector<int> parseSourceFile(const string& path) {
    fstream sourceFile;
    vector<int> program;
    sourceFile.open(path.c_str(), ios::in);
    string line;

    while (getline(sourceFile, line)) {
        int codeInput;
        if (tryPraseLine(line, codeInput))
            program.push_back(codeInput);
    }
    sourceFile.close();
    return program;
}

bool tryPraseLine(const std::string& line, int& outputValue) {
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
