#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <cassert>

#include "./src/intCode.hpp"




template<typename T>
std::vector<T> operator+(const std::vector<T> & b, const std::vector<T> & c) {
    std::vector<T> a;
    for (size_t i=0; i < b.size(); i++) {
        a.push_back(b[i] + c[i]);
    }

    return a;
}

template<typename T>
std::vector<T> & operator+=(std::vector<T> & a, const std::vector<T> & b) {
    for (size_t i=0; i < a.size(); i++) {
        a[i] += b[i];
    }

    return a;
}



/* --------------------- */
/* - - - Functions - - - */
/* --------------------- */

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
T run_droid(const std::vector<T> & initCode)
{
    std::map<int, std::vector<int>> dir_to_trans{
        {1, std::vector<int>{0, 1}},  // north
        {2, std::vector<int>{0, -1}}, // south
        {3, std::vector<int>{-1, 0}}, // west
        {4, std::vector<int>{1, 0}}   // east
    };

    std::map<int, int> turn_left{
        {1, 3},
        {2, 4},
        {3, 2},
        {4, 1}
    };

    std::map<int, int> turn_right{
        {1, 4},
        {2, 3},
        {3, 1},
        {4, 2}
    };


    bool stopAtOutput = true;
    bool stopAtInput = true;
    bool printInOut = false;

    T input = 1;
    T output;
    bool halted = false;

	intCode<T> IC(initCode, stopAtOutput, stopAtInput, printInOut);

    T dist_to_target = 0;
    std::vector<T> coords{0, 0};

    std::map<std::vector<T>, int> area_map;
    area_map.insert({std::vector<T>{0,0}, 1});


    while (!halted) {
        halted = IC.runIntCode(input);
        output = IC.getSingleOutput();

        if (output == 0) {
            area_map.insert({coords + dir_to_trans[input], output});

            input = turn_left[input];
        } else if (output == 1) {
            coords += dir_to_trans[input];
            if (area_map.find(coords) != area_map.end()) {
                dist_to_target--;
            } else {
                area_map.insert({coords, output});
                dist_to_target++;
            }

            input = turn_right[input];
        } else if (output == 2) {
            coords += dir_to_trans[input];
            area_map.insert({coords, output});
            dist_to_target++;

            input = turn_right[input];
            break;
        }
    }

    return dist_to_target;
}




// ----------------
// - - - MAIN - - -
// ----------------

int main() {
    std::string data_path = "./input_files/in15.txt";
    std::vector<int> initCode;

    loadData(data_path, initCode);

    int shortest_dist = run_droid(initCode);

    std::cout << "\n - - - PART I - - -\n";
    std::cout << "Final score: " << shortest_dist << "\n";
}
