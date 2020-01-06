#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <cmath>
#include <numeric>
#include <algorithm>
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


// ##############
// --- PART 1 ---
// ##############
template<typename T>
bool sightBlocked(const std::vector<std::vector<T>> & coords,
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

    std::vector<T> conn(2);
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

    std::vector<T> s(2);
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


size_t getMaxAsts(const std::vector<int> & astCount) {
    size_t max = 0;

    for (size_t i = 0; i < astCount.size(); i++) {
        if (astCount[i] > astCount[max]) { max = i; }
    }

    return max;
}


// ##############
// --- PART 2 ---
// ##############
void cartToPolar(const std::vector<std::vector<int>> & coords,
                 const size_t station,
                 std::vector<std::vector<double>> & coordsPol)
{
    // shift origin of coordinate system to station
    // transform to polar coordinates

    for (size_t i = 0; i < coords.size(); i++) {
        // atan2(y,x) : R -> [-PI , PI] with phi=0 on +x axis and positive phi
        // running counter clockwise
        // atan2(x,y) for phi = 0 on +y axis and positive phi running clockwise
        // atan2(x,-y) necessary since coordinate +y axis is pointing down
        double phi = atan2((coords[i][0]-coords[station][0]),
                    -(coords[i][1]-coords[station][1]));
        // phi in [0, 2 PI] instead of [-PI, PI]
        phi = (phi >= 0 ? phi : (2*M_PI + phi));
        double r = sqrt(
            (coords[i][0]-coords[station][0])*(coords[i][0]-coords[station][0])
            + (coords[i][1]-coords[station][1])*(coords[i][1]-coords[station][1]));

        coordsPol[i] = std::vector<double>{phi,r};
    }
}


void vaporizeAsts(const std::vector<std::vector<int>> & coords,
                  const size_t station, std::vector<size_t> & vapIdx)
{
    int N = coords.size();
    std::vector<bool> vaporized(N, false);
    int vapCount = 0;

    // convert coords to polar coordinates with phi = 0 pointing up along
    // +y axis and increasing clockwise. coordsPol[i][0] = phi, ..[i][1] = r
    std::vector<std::vector<double>> coordsPol(coords.size());
    cartToPolar(coords, station, coordsPol);

    // create vector of indices, sort such that coordsPol are in order of
    // ascending azimuthal angle phi if accessed with indices in order of
    // sorted vector idx
    std::vector<size_t> idx(N);
    std::iota(idx.begin(), idx.end(), 0);
    std::sort(idx.begin(), idx.end(),
              [& coordsPol](size_t i1, size_t i2) {
                  return coordsPol[i1][0] < coordsPol[i2][0];
              });

    // erase station itself from index list
    idx.erase(idx.begin());

    // sort vector idx such that coordsPol[idx[i]][1] gives increasing order of
    // radii r WITHIN series of equal azimuthal angles phi (coordsPol[idx[i]][0])
    size_t ir = 0;
    while (ir < idx.size()-1) {
        size_t end = 0;
        size_t start = ir;
        while (coordsPol[idx[ir]][0] == coordsPol[idx[ir+1]][0] ) {
            end++;
            ir++;
        }
        end++;
        std::sort(idx.begin()+start, idx.begin()+start+end,
              [& coordsPol](size_t i1, size_t i2) {
                  return coordsPol[i1][1] < coordsPol[i2][1];
              });
        ir++;
    }

    // vaporize all asteroids (N-1 without station)
    while (vapCount < N-1) {
        // remove from vector idx all vaporized asteroids
        idx.erase(std::remove_if(idx.begin(), idx.end(),
                                 [& vaporized](size_t i)
                                 { return vaporized[i]; }),
                  idx.end());

        if (idx.size() > 0) {
            vapIdx[vapCount] = idx[0];
            vaporized[idx[0]] = true;
            vapCount++;
        }

        for (size_t i = 1; i < idx.size(); i++) {
            if (coordsPol[idx[i]][0] == coordsPol[idx[i-1]][0]) {
                continue;
            } else {
                vapIdx[vapCount] = idx[i];
                vaporized[idx[i]] = true;
                vapCount++;
            }
        }
    } // WHILE: vapCount < N-1
}


// ##############
// ###  MAIN  ###
// ##############
int main() {
    std::string data_path = "in10.txt";
    std::vector<std::vector<int>> coords;

    loadData(data_path, coords);

    std::cout << "\n - - - PART 1 - - - \n";

    std::vector<int> astCount(coords.size());
    getAstsInSight(coords, astCount);

    size_t station = getMaxAsts(astCount);

    std::cout << "Asteroid with maximum number of other asteroids in sight:\n";
    std::cout << "(X,Y) = (" << coords[station][0] << ","
              << coords[station][1] << ") with " << astCount[station]
              << " asteroids in sight\n";

    std::cout << "\n - - - PART 2 - - - \n";


    std::vector<size_t> vapIdx(coords.size());

    vaporizeAsts(coords, station, vapIdx);

    std::cout << "Asteroid " << 200 << " vaporized at "
    << coords[vapIdx[199]][0] << "," << coords[vapIdx[199]][1] << "\t-  ";
    std::cout << "100*X + Y = "
    << coords[vapIdx[199]][0]*100+coords[vapIdx[199]][1] << "\n";


}
