#include <iostream>
#include <string>
#include <vector>

// find password x:
// - x in [min, max]
// - contains two equal adjecent numbers (e.g., 22 in 122035)
// - left to right, digits never DECREASE


bool meetCriteria(const int & x, const bool two_adjecent_only) {
    std::string xs = std::to_string(x);

    // 1) is x 6 digits in length? --------------
    if (xs.size() != 6) { return false; }
    // ------------------------------------------

    std::vector<int> xi;
    for (size_t i = 0; i < xs.size(); i++) {
        xi.push_back(xs[i]-'0');
    }

    // 2) does x contain 2 and ONLY 2 equal adjecent digits?
    // note: 111122 still passes this criteron
    bool TWOadjecentEqual = false;
    int adjecentCount = 1;
    for (size_t i = 0; i < xi.size()-1; i++) {
        if (two_adjecent_only) {
            if (xi[i] == xi[i+1]) {
                adjecentCount++;
                if (i+1 == xi.size()-1 && adjecentCount == 2) {
                    TWOadjecentEqual = true;
                }
            } else if (adjecentCount == 2) {
                TWOadjecentEqual = true;
                adjecentCount = 1;
            }
            else if (adjecentCount > 2) { adjecentCount = 1; }
        } else {
            if (xi[i] == xi[i+1]) {
                adjecentCount++;
                TWOadjecentEqual = true;
            }
        }
    }
    if (!TWOadjecentEqual) { return false; }
    // ------------------------------------------

    // 3) do digits in x (left to right) NEVER decrease?
    bool digitDecrease = false;
    for (size_t i = 1; i < xi.size(); i++) {
        if (xi[i] < xi[i-1]) { digitDecrease = true; }
    }
    if (digitDecrease) { return false; }
    // ------------------------------------------

    return true;
}


int findAllPossibilities(const int & min, const int & max,
                         const bool two_adjecent_only)
{
    int n = 0;

    for (int i = min; i <= max; i++) {
        if (meetCriteria(i, two_adjecent_only)) { n++; }
    }

    return n;
}


int main() {
    int min = 387638; //172851;
    int max = 919123; //675869;

    bool two_adjecent_only = false;
    int n = findAllPossibilities(min, max, two_adjecent_only);
    std::cout << "\n - - - PART 1 - - - \n";
    std::cout << "There are " << n << " possible passwords\n";

    two_adjecent_only = true;
    n = findAllPossibilities(min, max, two_adjecent_only);

    std::cout << "\n - - - PART 2 - - - \n";
    std::cout << "Now, there are only " << n << " possible passwords\n";
}
