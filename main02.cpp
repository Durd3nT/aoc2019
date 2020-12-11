#include <iostream>
#include <fstream>
#include <string>
#include <vector>


void runIntCode(std::vector<int> & code) {
    for (size_t i = 0; i < code.size(); i+=4) {
        int opcode = code[i];
        int param1 = code[i+1];
        int param2 = code[i+2];
        int param3 = code[i+3];

        if (opcode == 1) // ALTERNATIVE: Expression templates
            code[param3] = code[param1] + code[param2];
        else if (opcode == 2)
            code[param3] = code[param1] * code[param2];
        else if (opcode == 99)
            break;
        else {
            std::cout << "ERROR: something went wrong - opcode = " << opcode;
            std::cout << " (opcode must be 1, 2 or 99)\n";
        }
    }
}


void findInput(const std::vector<int> & initCode,
               const int & desiredOut, std::vector<int> & input)
{
    for (int noun = 0; noun < 99; noun++) {
        for (int verb = 0; verb < 99; verb++) {
            std::vector<int> modCode(initCode);

            modCode[1] = noun;
            modCode[2] = verb;
            runIntCode(modCode);
            if (modCode[0] == desiredOut) {
                input.push_back(noun);
                input.push_back(verb);
                std::cout << "\n - - - PART 2 - - - \n";
                std::cout << "OUTPUT: " << modCode[0];
                std::cout << " with input (solution) " << 100*noun+verb << "\n";
                break;
            }
            if (noun == 12 && verb == 2) {
                std::cout << "\n - - - PART 1 - - - \n";
                std::cout << "Value at position 0: " << modCode[0] << "\n";
            }
        }
    }

}


// ----- MAIN -----
int main() {
    std::ifstream inFile("./input_files/in02.txt");

    std::vector<int> initCode;
    std::string val;

    if (inFile.is_open()) {
        while (getline(inFile, val, ',')) {
            initCode.push_back(std::stoi(val));
        }
    }

    int desiredOut = 19690720;
    std::vector<int> input;
    findInput(initCode, desiredOut, input);

}
