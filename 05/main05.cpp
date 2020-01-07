#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <vector>

#include "intCode.hpp"


// ############
// --- MAIN ---
// ############
int main() {

    std::ifstream inFile("in05.txt");

    std::vector<int> initCode;
    std::string val;

    if (inFile.is_open()) {
        while (getline(inFile, val, ',')) {
            initCode.push_back(std::stoi(val));
        }
    }

    bool stopAtOutput = false;
    bool printOutput = true;

    {
        std::cout << "\n - - - PART 1 - - - \n";
        std::cout << "-> air con diagnostics code\n";
        std::vector<int> input{1};
        intCode<int> IC(initCode, stopAtOutput, printOutput);
        IC.runIntCode(input);
    }

    {
        std::cout << "\n - - - PART 2 - - - \n";
        std::cout << "-> thermal radiator controller diagnostics code\n";
        std::vector<int> input{5};
        intCode<int> IC(initCode, stopAtOutput, printOutput);
        IC.runIntCode(input);
    }
}
