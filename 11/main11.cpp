#include <fstream>
#include <string>
#include <vector>
#include <map>

#include "intCode.hpp"


template<typename T>
void loadData(const std::string & data_path, std::vector<T> & data) {
	static_assert(std::is_same<int, T>::value ||
				  std::is_same<long, T>::value ||
				  std::is_same<long long, T>::value,
				  "ERROR: function loadData() must be called with template\
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
    }
}



template<typename T>
void initRobot(const std::vector<T> initCode, std::vector<int> & gridDim)
{
	// robot moving on grid, robot's instructions (movements) given by intCode
	// each point on grid can be black or white, the robot with change the
	// of each grid point and move according to its intcode instructions
	// - output[1] of intcode is color to paint the panel (0 black, 1 white)
	// - output[2] of intcode is direction to move next (0 turn 90 degrees left,
	//   1 turn 90 degrees right)
	// --------------------------

    bool stopAtOutput = false;
    bool stopAtInput = true;
    bool printInOut = false;

	intCode<T> IC(initCode, stopAtOutput, stopAtInput, printInOut);

    // all panels start out black, therefore input is 1 at first
    std::vector<T> input{0};
    std::vector<T> output(2);
    bool halted = false;

    // initialize 1x1 grid, false == black, true == white
    std::map<std::vector<int>, bool> grid;
    grid.insert({std::vector<int>{0,0}, false});
    int dir = 0; // in degrees, 0 pointing up, 90 pointing right
	std::vector<int> gridPos{0,0};

    while (!halted) {
        halted = IC.runIntCode(input);
        output = IC.getOutput();

		// change color of grid tile depending on output of intCode
		if (output[0] == 0 && grid[gridPos] == 1) {
			grid[gridPos] = false;
		} else if (output[0] == 1 && grid[gridPos] == 0) {
			grid[gridPos] = true;
		}

		// rotate robot
        if (output[1] == 1) {
            dir = (dir == 270 ? 0 : (dir+90));
        } else if (output[1] == 0) {
            dir = (dir == 0 ? 270 : (dir-90));
        }

		// move robot forward
        if (dir == 0) { gridPos[1]++; }
        else if (dir == 90) { gridPos[0]++; }
        else if (dir == 180) { gridPos[1]--; }
        else if (dir == 270) { gridPos[0]--; }
        else {
            std::cerr << "ERROR: direction can only be along the axes!\n";
            break;
        }

		auto current = grid.find(gridPos);

		// get color of tile currently on
		if (current != grid.end()) {
            input[0] = current->second;
        } else {
			grid.insert({gridPos, false});
			input[0] = 0;
		}

    } // WHILE: !halted

	// compute grid dimensions, i.e., number of painted grid points, and size
	// of grid both in x- and y-direction
	int yMax = -(1<<20);
	int yMin = 1<<20;
	for (const auto & p: grid) {
		if (p.first[1] > yMax)
			yMax = p.first[1];
		if (p.first[1] < yMin)
			yMin = p.first[1];
	}

	gridDim.push_back(grid.size());
	gridDim.push_back((--grid.end())->first[0] - grid.begin()->first[0]);
	gridDim.push_back(yMax - yMin);
}


int main() {
    std::string data_path = "in11.txt";
    std::vector<long> initCode;

	loadData(data_path, initCode);

	std::vector<int> gridDim;

    initRobot(initCode, gridDim);

	std::cout << "Number of painted points is " << gridDim[0] << "\n";
}
