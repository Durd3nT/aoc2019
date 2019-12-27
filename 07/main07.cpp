#include <fstream>
#include <algorithm>

#include "intCode.hpp"


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

    // create output file for amplifier signals (int code output)
    std::ofstream oFile;
    oFile.open("outIntCode.txt");
    oFile << 0 << "\n" << 0;
    oFile.close();

    std::vector<int> phase{0,1,2,3,4}; // or with std::iota
    int maxOut = 0;
    std::vector<int> maxPhase;

    std::ifstream inout;

    do {
        int out = 0;

            for (size_t i = 0; i < phase.size(); i++) {
                std::cout << " ---- AMP " << i << " ---- \n";
                oFile.open("outIntCode.txt");
                oFile << phase[i] << "\n" << out;
                oFile.close();

                // redirect cin to file to read from that file at run time
                inout.open("outIntCode.txt");
                // save old buf (standard input) and redirect to inout buffer
                auto cinbuf = std::cin.rdbuf(inout.rdbuf());

                runIntCode(initCode, out);

                inout.close();
            }

        if (out > maxOut) {
            maxOut = out;
            maxPhase = phase;
        }
    } while (std::next_permutation(phase.begin(), phase.end()));


    std::cout << " -- -- -- -- -- -- -- -- -- -- -- -- -- \n";
    std::cout << "Max output: " << maxOut << " with phase ";
    for (auto & i: maxPhase)
        std::cout << i;
    std::cout << "\n";
}





int main() {
    std::string data_path = "in07.txt";
    std::vector<int> initCode;

    readData(data_path, initCode);

    ampSeries(initCode);

}
