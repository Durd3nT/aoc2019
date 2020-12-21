#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "./src/intCode.hpp"



template<typename T>
void loadData(const std::string & data_path, std::vector<T> & data) {
	static_assert(std::is_same<int, T>::value ||
				  std::is_same<long, T>::value ||
				  std::is_same<long long, T>::value,
				  "ERROR: function loadData() must be called with template \
parameter of type <signed int>, i.e., int, long, etc.");


	std::ifstream inFile(data_path);

    std::string val;
    if (inFile.is_open()) {
        while (getline(inFile, val, ',')) {
            if (std::is_same<T, int>::value) {
                data.push_back(std::stoi(val));
            } else if (std::is_same<T, long>::value) {
                data.push_back(std::stol(val));
            } else if (std::is_same<T, long long>::value) {
                data.push_back(std::stoll(val));
            }
        }
        inFile.close();
    } else {
        std::cout << "Error: could not open file " << data_path << "\n";
    }
}


template <typename T>
unsigned get_tile_coords(const std::vector<T> & output,
                         std::vector<T> & paddle_coords,
                         std::vector<T> & ball_coords)
{
    unsigned count_blocks = 0;
    for (size_t i = 0; i < output.size(); i+=3) {
        if (output[i+2] == 3) {
            paddle_coords.push_back(output[i]);
        } else if (output[i+2] == 4) {
            ball_coords.push_back(output[i]);
        } else if (output[i+2] == 2) {
            count_blocks++;
        }
    }

    return count_blocks;
}


template <typename T>
T run_arcade(const std::vector<T> & initCode)
{
    std::vector<T> modCode(initCode);
    modCode[0] = 2;

    bool stopAtOutput = false;
    bool stopAtInput = true;
    bool printInOut = false;

    std::vector<T> input(1);
    std::vector<T> output;
    bool halted = false;

	intCode<T> IC(modCode, stopAtOutput, stopAtInput, printInOut);

    halted = IC.runIntCode();
    output = IC.getOutput();

    std::vector<T> paddle_coords;
    std::vector<T> ball_coords;
    unsigned num_blocks = get_tile_coords(output, paddle_coords, ball_coords);

    std::cout << "\n - - - PART I - - -\n";
    std::cout << "There are " << num_blocks << " blocks" << "\n";
    if (paddle_coords[0] < ball_coords[0]) {
        input[0] = 1;
    } else if (paddle_coords[0] > ball_coords[0]) {
        input[0] = -1;
    } else if (paddle_coords[0] == ball_coords[0]) {
        input[0] = 0;
    }

    int score = 0;
    while (!halted) {
        halted = IC.runIntCode(input);
        output = IC.getOutput();

        for (size_t i = 0; i < output.size(); i+=3) {
            if (output[i+2] == 3) {
                paddle_coords[0] = output[i];
            } else if (output[i+2] == 4) {
                ball_coords[0] = output[i];
            } else if (output[i] == -1 && output[i+1] == 0) {
                score = output[i+2];
            }
        }

        if (paddle_coords[0] < ball_coords[0]) {
            input[0] = 1;
        } else if (paddle_coords[0] > ball_coords[0]) {
            input[0] = -1;
        } else if (paddle_coords[0] == ball_coords[0]) {
            input[0] = 0;
        }
    }

    return score;
}



// ----------------
// - - - MAIN - - -
// ----------------

int main() {
    std::string data_path = "./input_files/in13.txt";
    std::vector<int> initCode;

    loadData(data_path, initCode);

    int final_score = run_arcade(initCode);

    std::cout << "\n - - - PART II - - -\n";
    std::cout << "Final score: " << final_score << "\n";
}
