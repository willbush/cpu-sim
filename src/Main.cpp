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
bool fileExist(const string&);
vector<string> parseSourceFile(const string&);
bool tryPraseLine(const string&, string&);
void tryRunComputerSim(const vector<string>&, const int);

int main(int argc, char const *argv[]) {
    checkArgCount(argc);
    const string filepath = argv[1];
    const string timerIntervalStr = argv[2];
    const int timerInterval = tryParseTimer(timerIntervalStr);

    checkTimerInterval(timerInterval);
    checkIfFileExist(filepath);

    vector<string> program = parseSourceFile(filepath);
    tryRunComputerSim(program, timerInterval);
}

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

bool fileExist(const string& path) {
    return ifstream(path.c_str());
}

vector<string> parseSourceFile(const string& path) {
    fstream sourceFile;
    vector<string> program;
    sourceFile.open(path.c_str(), ios::in);
    string line;

    while (getline(sourceFile, line)) {
        string parsedOutput;
        if (tryPraseLine(line, parsedOutput))
            program.push_back(parsedOutput);
    }
    sourceFile.close();
    return program;
}

bool tryPraseLine(const string& line, string& outputValue) {
    int value;
    bool isParsed = false;

    if (line[0] == '.') {
        string subStr = line.substr(1, line.size() - 1); // remove period
        istringstream input(subStr);
        if (input >> value) { // get first integer value on the line
            ostringstream oss;
            oss << "." << value;
            outputValue = oss.str();
            isParsed = true;
        }
    } else {
        istringstream input(line);
        if (input >> value) {
            ostringstream oss;
            oss << value;
            outputValue = oss.str();
            isParsed = true;
        }
    }
    return isParsed;
}

void tryRunComputerSim(const vector<string>& program, const int timerInterval) {
    try {
        ComputerSim c(program, timerInterval);
    } catch (exception &e) {
        cout << e.what() << endl;
        exit(EXIT_FAILURE);
    }
}
