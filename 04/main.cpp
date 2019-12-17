#include <iostream>
#include <string>

// find password x:
// - x in [min, max]
// - contains two equal adjecent numbers (e.g., 22 in 122035)
// - left to right, digits never DECREASE


bool meetCriteria(const int & x) {

    return false;
}


int findAllPossibilities(const int & min, const int & max) {
    int n = 0;

    for (int i = min; i <= max; i++) {
        if (meetCriteria(i)) { n++; }
    }

    return n;
}


int main() {
    int min = 172851;
    int max = 675869;

    findAllPossibilities(min, max);

}
