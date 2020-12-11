#include <iostream>
#include <fstream>
#include <iterator>
#include <cmath>
#include <vector>


template <typename T>
void computeExtraFuel(T & massTot) {
    double mass(1);
    T massDiff(massTot);
    // std::cout << massDiff << "\n";

    while(mass > 0) {
        mass = double(massDiff)/3;
        mass = std::floor(mass)-2;
        if (mass > 0) {
            massTot += int(mass);
        }
        massDiff = mass;
    }
}


int computeFuel(std::vector<int> & modules, bool include_fuel) {
    int massTot = 0;
    std::vector<int>::iterator it = modules.begin();

    for (; it != modules.end(); ++it) {
        double mass = double(*it)/3;
        mass = std::floor(mass)-2;
        if (include_fuel) {
            computeExtraFuel(mass);
        }
        massTot += int(mass);
    }
    return massTot;
}


int main() {
    std::vector<int> modules;

    std::ifstream inFile("./input_files/in01.txt");
    if (inFile.is_open()) {
        // see cppreference:
        // default-constructor of istream_iterator is end-of-stream iterator
        std::istream_iterator<int> start(inFile), end;
        modules.assign(start, end);
    } else { std::cout << "ERROR: could not open file\n"; }

    bool include_fuel = false;
    int massTot = 0;
    massTot = computeFuel(modules, include_fuel);

    std::cout << "\n - - - PART 1 - - - \n";
    std::cout << "Fuel requirements (without including fuel to carry fuel): ";
    std::cout << massTot << "\n";

    include_fuel = true;
    massTot = computeFuel(modules, include_fuel);

    std::cout << "\n - - - PART 2 - - - \n";
    std::cout << "Total fuel requirements: " << massTot << "\n";
}
