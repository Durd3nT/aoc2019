#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <limits>



// ##########################################################################

// --- FUNCTIONS ---

void loadData(const std::string & data_path, std::vector<char> & direction,
              std::vector<int> & distance, std::vector<int> & lineLength)
{
    // load CSVs of format <char><int>, e.g., R999. Store char (R) in
    // std::vector direction, and integer (999) in std::vector distance

    std::ifstream infile(data_path);

    int lineCount = 0;
    std::string line;
    std::string walk;

    while (getline(infile, line, '\n')) {
        lineLength.push_back(0);
        std::istringstream lineStream(line); // convert string line to istream

        while (getline(lineStream, walk, ',')) {
            lineLength[lineCount]++;
            direction.push_back(walk[0]); // string position 0 is direction
            std::string dir = walk.substr(1); // from position 1 to end
            distance.push_back(std::stoi(dir));
        }
        lineCount++;
    }
}


void getGridCoord(const std::vector<char> & dir, const std::vector<int> & dist,
                  const int & edges, std::vector<int> & coord,
                  std::vector<int> & cableLen)
{
    // for each cable, store grid coordinates of each edge (where a cable
    // changes its direction) in std::vector coord.
    // coord is layed out as follows:
    // coord = (coord_cable1 , coord_cable2)
    // with coord_cablei = (x1, y1, x2, ...)

    for (int j = 0; j < 2; j++) { // loop over cables
        int X = 0;
        int Y = 0;
        int L = 0;

        for (int i = 0; i < edges; i++) { // loop over cable edges
            if (dir[i + j*edges] == 'R')
                X += dist[i + j*edges];
            else if (dir[i + j*edges] == 'L')
                X -= dist[i + j*edges];
            else if (dir[i + j*edges] == 'U')
                Y += dist[i + j*edges];
            else if (dir[i + j*edges] == 'D')
                Y -= dist[i + j*edges];

            coord[2*i + 2*j*edges] = X;
            coord[2*i+1 + 2*j*edges] = Y;

            L += dist[i + j*edges];
            cableLen[i + j*edges] = L;
        } // for i < edgeCount
    } // for j < number of cables
}


bool vertical(const int & x1, const int & x2)
{
    // is line horizontal between x1 and x2 horizontal?
    return x1 == x2;
}


bool parallel(std::vector<int>::const_iterator start1,
              std::vector<int>::const_iterator start2)
{
    // are lines with x-coordinates *start1, etc., respectively, parallel?
    return (vertical(*start1, *(start1+2)) == vertical(*start2, *(start2+2)));
}


bool inbetween(const int & x, const int & a, const int & b) {
    // is x in [a,b] or [b,a]?
    return ( (x > a && x < b) || (x < a && x > b) );
}


bool intersect(std::vector<int>::const_iterator start1,
               std::vector<int>::const_iterator start2)
{
    // do the two lines starting at *start1 and *start2, respectively,
    // intersect?
    if ( inbetween(*start2, *start1, *(start1+2))
         && inbetween(*(start1+1), *(start2+1), *(start2+3)) ) {
        return true;
    } else if ( inbetween(*start1, *start2, *(start2+2))
         && inbetween(*(start2+1), *(start1+1), *(start1+3)) ) {
        return true;
    } else { return false; }
}


void computeIntersection(std::vector<int>::const_iterator start1,
                         std::vector<int>::const_iterator start2,
                         int & x, int & y)
{
    // - compute intersection between lines starting at *start1 and *start2,
    // respectively, store coordinates of intersection in x and y
    if (vertical(*start1, *(start1+2))) {
        x = (*start1);
        y = (*(start2+1));
    } else {
        x = (*start2);
        y = (*(start1+1));
    }
}


int getSigDelay(std::vector<int>::const_iterator start1,
                std::vector<int>::const_iterator start2,
                const int & cableLen1, const int & cableLen2,
                int & x, int & y)
{
    // return combined steps both cables take to reach an intersection
    int len1 = cableLen1 + std::abs(*start1 - x) + std::abs(*(start1+1) - y);
    int len2 = cableLen2 + std::abs(*start2 - x) + std::abs(*(start2+1) - y);

    return (len1 + len2);
}


void getAllIntersections(const std::vector<int> & coord,
                         const std::vector<int> & cableLen,
                         const int & edges, std::vector<int> & inter,
                         int & minComLen)
{
    // - For both cables, loop through coordinates of all their edges (where
    // the cable changes direction) and compare all line elements of one cable
    // (straight sections of cable) to all line elements of other cable to
    // to check for intersections of the two cables.
    // - Store x- and y-grid-coordinates of each intersection in std::vector inter
    // - Additionally, get minimum combined length of both cables to reach an
    // intersection.

    for (int i = 0; i < edges-1; i++) { // loop over cable edges
        for (int j = 0; j < edges-1; j++) { // loop over cable edges

            if ( !parallel(coord.begin()+2*i, coord.begin()+2*j+2*edges) ) {
                bool cablesCross = intersect(coord.begin()+2*i,
                                             coord.begin()+2*j+2*edges);
                // compute intersection
                if (cablesCross) {
                    int x = 0;
                    int y = 0;
                    computeIntersection(coord.begin()+2*i,
                                        coord.begin()+2*j+2*edges,
                                        x, y);
                    inter.push_back(x);
                    inter.push_back(y);

                    // compute combined cable length to intersection
                    int combinedLen = 0;
                    combinedLen = getSigDelay(coord.begin()+2*i,
                                        coord.begin()+2*j+2*edges,
                                        cableLen[i], cableLen[j+edges],
                                        x, y);
                    if (combinedLen < minComLen || minComLen == 0) {
                        minComLen = combinedLen;
                    }
                } // IF: cablesCross
            } // IF: cables are NOT parallel
        } // FOR j < edges-1
    } // FOR i < edges-1
}


int findClosestIntersection(const std::vector<int> & inter) {
    size_t n = inter.size()/2;
    int min = std::numeric_limits<int>::max();

    for (size_t i = 0; i < n; i++) {
        int dist = inter[2*i] + inter[2*i+1];
        if (dist < min) { min = dist; }
    }

    return min;
}

// ##########################################################################


// ################
// ----- MAIN -----
// ################
int main() {
    std::string data_path = "./input_files/in03.txt";

    std::vector<char> direction;
    std::vector<int> distance, edgeCount;

    loadData(data_path, direction, distance, edgeCount);

    if (edgeCount[0] == edgeCount[1]) {
        std::vector<int> gridCoord;
        std::vector<int> cableLen;
        // store x,y coords next to each other, as they will be accessed
        // together and for both cables in the same vector
        gridCoord.resize(2*direction.size()); // (x1, y1, x2, y2, ...)
        cableLen.resize(direction.size()); // (len at (x1,y1), at (x2,y2), ...)
        getGridCoord(direction, distance, edgeCount[0], gridCoord, cableLen);

        std::vector<int> inter;
        int minComLen = 0;
        getAllIntersections(gridCoord, cableLen, edgeCount[0],
                            inter, minComLen);
        int closestIntersection = findClosestIntersection(inter);

        std::cout << "Closest intersection is ";
        std::cout << closestIntersection << " away\n";
        std::cout << "Minimum combined cable length to intersection is ";
        std::cout << minComLen << "\n";
    } else { std::cout << "ERROR: lines have different length\n"; }
}
