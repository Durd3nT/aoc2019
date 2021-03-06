#include <fstream>
#include <algorithm>

#include "./src/intCode.hpp"


template<typename T>
void readData(const std::string & data_path, std::vector<T> & data) {
    std::ifstream inFile(data_path);

    std::string val;
    if (inFile.is_open()) {
        while (getline(inFile, val, ',')) {
            if (std::is_same<T, int>::value) {
                data.push_back(std::stoi(val));
            } else if (std::is_same<T, long>::value) {
                data.push_back(std::stol(val));
            }
        }
        inFile.close();
    }
}




int main() {
    std::string data_path = "./input_files/in09.txt";
    std::vector<long> initCode;

    std::vector<long> test0{109,1,204,-1,1001,100,1,100,1008,100,16,101,1006,101,0,99};
    std::vector<long> test1{1102,34915192,34915192,7,4,7,99,0};
    std::vector<long> test2{104,1125899906842624,99};

    readData(data_path, initCode);

    bool stopAtOutput = false;
    bool stopAtInput = false;
    bool printInOut = true;

    {
        std::cout << "\n - - - PART 1 - - - \n";

        std::vector<long> input{1};
        intCode<long> IC(initCode, stopAtOutput, stopAtInput, printInOut);
        IC.runIntCode(input);
    }

    {
        std::cout << "\n - - - PART 2 - - - \n";

        std::vector<long> input{2};
        intCode<long> IC(initCode, stopAtOutput, stopAtInput, printInOut);
        IC.runIntCode(input);
    }
}
