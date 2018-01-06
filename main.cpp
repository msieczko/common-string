#include <iostream>
#include <iomanip>
#include <regex>
#include <fstream>
#include "data/RandomStringGenerator.h"
#include "algorithm/CommonStringFinder.h"
#include "algorithm/ExecutionTimeClock.h"

using namespace std;

void static printUsage(const char* name);
int t1Mode(vector<string> &args);
int t2Mode(vector<string> &args);
void solveOnce(StringSet &set, bool withInteractiveMode);
void peekFunction(const char* key, const StringSet &set, const ulong* matchingLetters,
                  const std::list<ulong>* matchingStrings, ulong currentStrIndex, bool keyChanged);
int t3Mode(vector<string> &args);

static const char* const kName = "AAL";
static const char* const kFileError = "File error";
static const int kError = 1;

int main(int argc, char* argv[]) {
    vector<string> args(argv + 1, argv + argc + !argc);
    int ret;
    if (args.empty()) {
        printUsage(kName);
        return kError;
    }

    if (args[0] == "-t1") {
        ret = t1Mode(args);
    } else if (args[0] == "-t2") {
        ret = t2Mode(args);
    } else if (args[0] == "-t3") {
        ret = t3Mode(args);
    } else {
        ret = kError;
    }

    if (ret == kError) {
        printUsage(kName);
    }
    return ret;
}

void static printUsage(const char* name) {
    cerr << "Usage:\n"
         << name << " -t1 [-i]                       \tRead data from stdin and print results to stdout\n"
         << name << " -t2 [-i] <gen_options>         \tGenerate random data and print results to stdout\n"
         << name
         << " -t3 <gen_options> <test_params>\tGenerate random data, measure execution time and print results to stdout\n"
         << "Optional parameter:\n"
         << "\t-i Run heuristic in interactive mode\n"
         << "Generator options:\n"
         << "\t-n STRING_LEN \tLength of generated strings\n"
         << "\t-m NUM_STRINGS\tNumber of generated strings\n"
         << "Test parameters:\n"
         << "\t-k NUM_GEN_SET     \tNumber of different generator settings\n"
         << "\t-step_n INC_STR_LEN\tIncrement of the length of generated strings\n"
         << "\t-step_m INC_NUM_STR\tIncrement of the number of generated strings\n"
         << "\t-r NUM_RUNS        \tNumber of algorithm runs per each generator setting\n"
         << endl;
}

int t1Mode(vector<string> &args) {
    bool withInteractiveMode = false;
    if (args.size() == 2) {
        if (args[1] == "-i") {
            withInteractiveMode = true;
        } else {
            return kError;
        }
    } else if (args.size() > 2) {
        return kError;
    }

    string firstLine;
    getline(cin, firstLine);
    if (!regex_match(firstLine, regex("^[10*]+$"))) {
        cout << kFileError << endl;
        return kError;
    }
    ulong stringLength = firstLine.length();
    regex format("^[10*]{" + to_string(stringLength) + "}$");
    vector<string> readData;

    for (string line; getline(cin, line);) {
        if (!regex_match(line, format)) {
            cout << kFileError << endl;
            return kError;
        }
        readData.push_back(line);
    }

    ulong numStrings = readData.size();
    StringSet set(stringLength, numStrings);
    char** data = set.getData();
    for (ulong str = 0; str < numStrings; ++str) {
        readData[str].copy(data[str], stringLength);
    }

    solveOnce(set, withInteractiveMode);
}

int t2Mode(vector<string> &args) {
    bool withInteractiveMode = false;
    switch (args.size()) {
        case 5:break;
        case 6:
            if (args[1] == "-i") {
                withInteractiveMode = true;
            } else {
                return kError;
            }
            break;
        default:return kError;
    }

    // Generator options start index in args array
    const int goi = withInteractiveMode ? 2 : 1;

    bool nSet = false;
    bool mSet = false;
    ulong stringLength = 0;
    ulong numStrings = 0;

    try {
        for (int i = goi; i < goi+3; i += 2) {
            if (args[i] == "-n" && !nSet) {
                stringLength = stoul(args[i + 1]);
                nSet = true;
            } else if (args[i] == "-m" && !mSet) {
                numStrings = stoul(args[i + 1]);
                mSet = true;
            } else {
                return kError;
            }
        }
    } catch (const logic_error &e) {
        return kError;
    }

    RandomStringGenerator rsg;
    StringSet set = rsg.generateStringSet(stringLength, numStrings);

    solveOnce(set, withInteractiveMode);
}

void solveOnce(StringSet &set, bool withInteractiveMode) {
    cout << "Input:" << endl;
    cout << set << endl << endl;

    CommonStringFinder csf;
    CommonStringFinder::Result result;
    if (withInteractiveMode) {
        cout << "HEURISTIC:" << endl;
        result = csf.heuristicInteractive(set, peekFunction);
    } else {
        result = csf.heuristic(set);
    }
    cout << "Heuristic solution:   " << result << endl;
    cout << "Brute-force solution: " << csf.bruteForce(set) << endl;
}

void peekFunction(const char* key, const StringSet &set, const ulong* matchingLetters,
                  const std::list<ulong>* matchingStrings, ulong currentStrIndex, bool keyChanged) {
    char** data = set.getData();
    auto stringLength = set.getStringLength();
    cout << "Key: " << string(key, stringLength) << (keyChanged ? " (new)\n" : "\n");
    cout << setw(3) << left << currentStrIndex << ": " << string(data[currentStrIndex], stringLength) << endl;
    cout << "mL:  ";
    for (auto i = 0; i < currentStrIndex + 1; ++i) {
        cout << i << "(" << matchingLetters[i] << ")  ";
    }
    cout << "\nmS:\n";
    for (auto i = 0; i < stringLength; ++i) {
        cout << i << "(" << key[i] << "): [";
        for (auto it = matchingStrings[i].begin(); it != matchingStrings[i].end(); ++it) {
            if (it != matchingStrings[i].begin()) {
                cout << ", ";
            }
            cout << *it;
        }
        cout << "]\n";
    }
    cin.get();
}

int t3Mode(vector<string> &args) {

}
