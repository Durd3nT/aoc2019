#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <cassert>

#include "./src/intCode.hpp"

/* ---------------------------- */
/* - - - Class Directuion - - - */
/* ---------------------------- */
class direction
{
public:
    enum dir_t {invalid = 0, north = 1, south = 2, west = 3, east = 4};
    direction() : d_(invalid), orig_d_(invalid) {}
    direction(dir_t d) : d_(d), orig_d_(d) {}
    direction(unsigned d) {
        switch(d)
        {
            case 1: d_ = north; break;
            case 2: d_ = south; break;
            case 3: d_ = west; break;
            case 4: d_ = east; break;
            default: std::cout << "ERROR: " << d << " is invalid value\n";
        }
        orig_d_ = d_;
    }
    direction(const direction & dir) = default;

    unsigned operator()() const { return d_; }
    direction & operator=(const int rhs);
    direction & operator=(const direction & rhs);
    direction & operator++();
    direction operator++(int);
    bool operator==(const direction & rhs);

    template<typename T>
    void move(T & x, T & y);
    direction get_rev_dir();
    void try_new_dir();

private:
    dir_t d_;
    dir_t orig_d_;
};

/* - - - MEMBER - - - */
direction & direction::operator=(const int rhs)
{
    if (d_ == rhs){
        return *this;
    } else {
        direction new_dir(rhs);
        d_ = new_dir.d_;
        orig_d_ = d_;
    }
    return *this;
}


direction & direction::operator=(const direction & rhs)
{
    if (this == &rhs) {
        return *this;
    } else {
        d_ = rhs.d_;
        orig_d_ = d_;
    }
    return *this;
}


direction direction::operator++(int)
{
    direction tmp = *this;
    switch (d_)
    {
        case north:   d_ = east;  break;
        case south:   d_ = west;  break;
        case west:    d_ = north; break;
        case east:    d_ = south; break;
        case invalid: std::cout << "Current direction is INVALID\n";
    }
    return tmp;
}


bool direction::operator==(const direction & rhs)
{
    return (d_ == rhs());
}


template<typename T>
void direction::move(T & x, T & y)
{
    switch (d_)
    {
        case north:   y++; break;
        case south:   y--; break;
        case west:    x--; break;
        case east:    x++; break;
        case invalid: std::cout << "Current direction is INVALID\n";
    }
}


direction direction::get_rev_dir()
{
    dir_t rev_dir;
    switch (d_)
    {
        case north:   rev_dir = south; break;
        case south:   rev_dir = north; break;
        case west:    rev_dir = east;  break;
        case east:    rev_dir = west;  break;
        case invalid: std::cout << "Current direction is INVALID\n";
    }
    return rev_dir;
}


// void direction::try_new_dir()
// {
//     d_++;
// }



/* - - - NON-MEMBER - - - */
std::ostream& operator<<(std::ostream& os, direction dir)
{
    switch (dir())
    {
        case 0: os << "INVALID"; break;
        case 1: os << "north";   break;
        case 2: os << "south";   break;
        case 3: os << "west";    break;
        case 4: os << "east";    break;
        default: os.setstate(std::ios_base::failbit);
    }
    return os;
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
    std::map<std::vector<T>, int> area_map;
    area_map.insert({std::vector<T>{0,0}, 1});
    std::vector<std::vector<T>> path;

    bool stopAtOutput = true;
    bool stopAtInput = true;
    bool printInOut = false;

    // T input = 1;
    direction input_dir(1);
    T output;
    bool halted = false;

	intCode<T> IC(initCode, stopAtOutput, stopAtInput, printInOut);

    T dist_to_target = 0;
    T x = 0;
    T y = 0;
    std::vector<T> coords(2);

    int j = 0;

    while (j<5 && output != 2 && !halted) {
        direction rev_dir;
        halted = IC.runIntCode(input_dir());
        output = IC.getSingleOutput();

        // if (area_map.find(std::vector<T>{x, y}) != area_map.end()) {
        //
        // } else {
        //
        // }

        if (output == 1) {
            input_dir.move(x, y);
            rev_dir = input_dir.get_rev_dir();

            area_map.insert({std::vector<T>{x, y}, output});
            path.push_back(std::vector<T>{x, y});
        } else if (output == 0) {
            switch (input_dir())
            {
                case 1: area_map.insert({std::vector<T>{x, y+1}, output}); break;
                case 2: area_map.insert({std::vector<T>{x, y-1}, output}); break;
                case 3: area_map.insert({std::vector<T>{x-1, y}, output}); break;
                case 4: area_map.insert({std::vector<T>{x+1, y}, output}); break;
            }

            input_dir++;
        }

        j++;
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

    int final_score = run_droid(initCode);
    // direction dir1(1);
    // std::cout << dir1 << "\n";
    // direction dir2(west);
    // std::cout << dir1 << " " << dir2 << "   " << "\n";

    std::cout << "\n - - - PART I - - -\n";
    // std::cout << "Final score: " << final_score << "\n";
}
