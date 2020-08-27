#include <iostream>
#include <fstream>
#include <iterator>
#include <cmath>
#include <vector>


template <typename T>
void computeExtraFuel(T & massTot) {
    double mass(1);
    T massDiff(massTot);
    std::cout << massDiff << "\n";

    while(mass > 0) {
        mass = double(massDiff)/3;
        mass = std::floor(mass)-2;
        if (mass > 0) {
            massTot += int(mass);
        }
        massDiff = mass;
    }
}


void computeFuel(int & massTot, std::vector<int> & modules) {
    std::vector<int>::iterator it = modules.begin();
    for (; it != modules.end(); ++it) {
        double mass = double(*it)/3;
        mass = std::floor(mass)-2;
        computeExtraFuel(mass);
        massTot += int(mass);
    }
}


int main() {
    std::vector<int> modules;

    std::ifstream inFile("in01.txt");
    if (inFile.is_open()) {
        // see cppreference:
        // default-constructor of istream_iterator is end-of-stream iterator
        std::istream_iterator<int> start(inFile), end;
        modules.assign(start, end);
    } else { std::cout << "ERROR: could not open file\n"; }

    int massTot(0);

    computeFuel(massTot, modules);

    std::cout << "Total mass including fuel for fuel = " << massTot << "\n";
}
