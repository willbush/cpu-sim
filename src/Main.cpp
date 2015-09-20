#include "ComputerSim.h"
#include "Parser.h"
#include "stdlib.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;

bool isCorrectArgCount(int);
int tryParseTimer(const string&);
vector<int> parseSourceFile(fstream&, const string&);

//int main(int argc, char const *argv[]) {
//    if (!isCorrectArgCount(argc))
//        exit(EXIT_FAILURE);
//
//    const string filepath = argv[1];
//    const string intervalTimerStr = argv[2];
//    const int intervaltimer = tryParseTimer(intervalTimerStr);
//
//    fstream fs(filepath.c_str());
//    if (!fs) {
//        cerr << "source file: " << filepath << " not found\n";
//        exit(EXIT_FAILURE);
//    }
//    vector<int> program = parseSourceFile(fs, filepath);
//
//    ComputerSim c(program, intervaltimer);
//}

bool isCorrectArgCount(int argc) {
    const int requiredArgs = 3;
    if (argc != requiredArgs) {
        cerr << "Please pass the source file name and timer interrupt interval as arguments\n";
        cerr << "e.g. ./CPUsim source.txt 10" << endl;
        return false;
    }
    return true;
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

vector<int> parseSourceFile(fstream& sourceFile, const string& path) {
    vector<int> program;
    sourceFile.open(path.c_str(), ios::in);
    string line;

    while(getline(sourceFile, line)) {
        int codeInput;
        if (Parser::tryPraseLine(line, codeInput))
            program.push_back(codeInput);
    }
    sourceFile.close();
    return program;
}