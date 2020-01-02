#include <fstream>
#include <algorithm>

#include "intCode.hpp"


template<typename T>
void readData(const std::string & data_path, std::vector<T> & data) {
    std::ifstream inFile(data_path);

    std::string val;
    if (inFile.is_open()) {
        while (getline(inFile, val, ',')) {
            data.push_back(std::stoi(val));
        }
        inFile.close();
    }
}




int main() {
    std::string data_path = "in09.txt";
    std::vector<long> initCode;

    readData(data_path, initCode);

    std::vector<long> input{2};
    intCode<long> IC(initCode);
    IC.runIntCode(input);
}
