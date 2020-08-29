#include <fstream>
#include <algorithm>

#include "./src/intCode.hpp"


void readData(const std::string & data_path, std::vector<int> & data) {
    std::ifstream inFile(data_path);

    std::string val;
    if (inFile.is_open()) {
        while (getline(inFile, val, ',')) {
            data.push_back(std::stoi(val));
        }
        inFile.close();
    }
}



void ampSeries(std::vector<int> & initCode) {
    // find phase settings for 5 amplifiers running int code (initCode)
    // corresponding to the maximum output signal (output of int code) of
    // the last amplifier.
    // -------------------

    std::vector<int> phase{0,1,2,3,4};
    int maxOut = 0;
    std::vector<int> maxPhase;

    do {
        std::vector<int> input(2);
        std::vector<int> out{0};

        for (size_t i = 0; i < phase.size(); i++) {
            intCode<int> IC(initCode);

            input[0] = phase[i];
            input[1] = out[0];

            IC.runIntCode(input);
            out = IC.getOutput();
        }

        if (out[0] > maxOut) {
            maxOut = out[0];
            maxPhase = phase;
        }
    } while (std::next_permutation(phase.begin(), phase.end()));


    std::cout << " -- -- -- -- -- -- -- -- -- -- -- -- -- \n";
    std::cout << " --- AMPLIFIER SERIES --- \n";
    std::cout << "Max output: " << maxOut << " with phase ";
    for (auto & i: maxPhase)
        std::cout << i;
    std::cout << "\n";
}


void ampFeedbackLoop(std::vector<int> & initCode) {
    std::vector<int> phase{5,6,7,8,9};
    int maxOut = 0;
    std::vector<int> maxPhase;

    bool stopAtOutput = true;

    do {
        std::vector<intCode<int>> amps;
        bool halted = false;

        std::vector<int> input(2);
        std::vector<int> out{0};

        // initialize amplifiers until first output
        // after first run of for loop, out is output of amp 5, input for amp 1
        for (size_t i = 0; i < phase.size(); i++) {
            intCode<int> IC(initCode, stopAtOutput, false, false);

            input[0] = phase[i];
            input[1] = out[0];

            halted = IC.runIntCode(input);
            out = IC.getOutput();

            amps.push_back(IC); // push_back() copies, so no scope issues
        }

        input.resize(1);

        int loop = 0;
        while (!halted) {
            for (size_t i = 0; i < amps.size(); i++) {
                input[0] = out[0];
                halted = amps[i].runIntCode(input);
                std::vector<int> tempOut = amps[i].getOutput(); // NOTE SEG FAULT
                out.swap(tempOut);
            }
            loop++;
        }

        if (out[0] > maxOut) {
            maxOut = out[0];
            maxPhase = phase;
        }
    } while (std::next_permutation(phase.begin(), phase.end()));


    std::cout << " -- -- -- -- -- -- -- -- -- -- -- -- -- \n";
    std::cout << " --- AMPLIFIER FEEDBACK LOOP --- \n";
    std::cout << "Max output: " << maxOut << " with phase ";
    for (auto & i: maxPhase)
        std::cout << i;
    std::cout << "\n";
}




int main() {
    std::string data_path = "./input_files/in07.txt";
    std::vector<int> initCode;

    readData(data_path, initCode);

    ampSeries(initCode);

    ampFeedbackLoop(initCode);

}
