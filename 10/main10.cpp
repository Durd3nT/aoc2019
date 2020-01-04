#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cassert>


void loadData(const std::string & data_path,
              std::vector<std::vector<int>> & data)
{
    std::ifstream inFile(data_path);

    std::string line;
    int y = 0;

    while (getline(inFile, line, '\n')) {

        int x = 0;
        while(x < line.size()) {
            if (line[x] == '#') {
                std::vector<int> coordPair{x,y};
                data.push_back(coordPair);
            }
            x++;
        }
        y++;
    }
}


bool sightBlocked(const std::vector<std::vector<int>> & coords,
                  size_t lookedFrom, size_t lookedAt)
{
    assert(lookedAt < coords.size() && lookedFrom < coords.size());
    /* how to check if asteroid is blocked by others:
    - get vector connecting two ast, ast_lookedAt - ast_lookedFrom
    - for all ast_i with coord in [coord(ast_viewedFrom), coord(ast_lookedAt)]
        compute vector ast_viewedFrom - ast_i
        check, if ast_viewedFrom - ast_lookedAt is a multiple of
        ast_viewedFrom - ast_i
        if yes -> blocked
    */

    std::vector<int> conn(2);
    conn[0] = coords[lookedAt][0] - coords[lookedFrom][0];
    conn[1] = coords[lookedAt][1] - coords[lookedFrom][1];

    size_t start, end;
    if (lookedFrom < lookedAt) {
        start = lookedFrom;
        end = lookedAt;
    } else if (lookedFrom > lookedAt) {
        start = lookedAt;
        end = lookedFrom;
    }

    std::vector<int> s(2);
    for (size_t i = start+1; i < end; i++) {
        // compute connecting vector ast_lookedFrom - ast_i
        // ast_i is between ast_lookedFrom and ast_lookedAt
        s[0] = coords[i][0] - coords[lookedFrom][0];
        s[1] = coords[i][1] - coords[lookedFrom][1];

        if (conn[0] == 0 && s[0] == 0) {
            // check if conn AND s are parallel to y-axis
            return true;
        } else if (conn[1] == 0 && s[1] == 0) {
            // check if conn AND s are parallel to x-axis
            return true;
        } else if ( double(conn[0])/s[0] == double(conn[1])/s[1] ) {
            // check if conn is multiple of s
            return true;
        }
    }

    return false;
}


void getAstsInSight(const std::vector<std::vector<int>> & coords,
                    std::vector<int> & astCount)
{
    assert(astCount.size() == coords.size());

    for (size_t i = 0; i < coords.size(); i++) {
        for (size_t j = 0; j < coords.size(); j++) {
            if ( i != j && !sightBlocked(coords, i, j)) { astCount[i]++; }

        }
    }
}


int getMaxAsts(const std::vector<int> & astCount) {
    int max = 0;

    for (size_t i = 0; i < astCount.size(); i++) {
        if (astCount[i] > astCount[max]) { max = i; }
    }

    return max;
}



int main() {
    std::string data_path = "in10.txt";
    std::vector<std::vector<int>> coords;

    loadData(data_path, coords);

    std::vector<int> astCount(coords.size());
    getAstsInSight(coords, astCount);

    int indexMax = getMaxAsts(astCount);

    std::cout << "Asteroid with maximum number of other asteroids in sight:\n";
    std::cout << "(X,Y) = (" << coords[indexMax][0] << ","
              << coords[indexMax][1] << ") with " << astCount[indexMax]
              << " asteroids in sight\n";
}
