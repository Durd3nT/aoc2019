#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <utility>
#include <map>


int loadData(const std::string & data_path, std::vector<std::string> & data) {
    // load all objects into std::vector data.
    // original data is of form AAA)MMM, meaning object MMM orbits object AAA,
    // followed by BBB)NNN, CCC)OOO, ...
    // This is stored in data as (AAA, BBB, CCC, ..., MMM, NNN, OOO, ...)
    // so the first dim strings in data are the objects that are (directly)
    // orbited by the last dim strings in data.

    std::ifstream infile(data_path);

    std::string line, object;
    std::vector<std::string> orbit;
    int dim = 0;

    while (getline(infile, line, '\n')) {
        std::istringstream lineStream(line);
        int i = 0;

        while (getline(lineStream, object, ')')) {
            i++;
            if (i==1) { data.push_back(object); }
            else if (i==2) { orbit.push_back(object); }
        }

        dim++;
    }

    data.insert(data.end(), orbit.begin(), orbit.end());
    return dim;
}



void findAllOrbits(const std::vector<std::string> & objects, const int dim,
                std::multimap<int, std::string> & orbitMap,
                int orbCount, std::string & innerObj)
{
    // Get number of direct and indirect orbits for every object.
    // (indirect) Orbit structure is tree like.
    // The function findAllOrbits() calls itself over and again until arriving
    // at the outermost orbit, then going to the last branch split of the tree
    // and again advancing until the end of the next branch, and so on.
    //
    // The pairs (number of (in)direct orbits, object) gets stored in the
    // std::multimap orbitMap
    
    int orbitCount = orbCount;
    std::vector<int> indices;

    orbitMap.insert(std::pair<int, std::string>(orbitCount, innerObj));

    for (int i = 0; i < dim; i++) {
        if (objects[i] == innerObj) { indices.push_back(i+dim); }
    }

    orbitCount++;
    for (size_t i = 0; i < indices.size(); i++) {
        innerObj = objects[indices[i]];

        findAllOrbits(objects, dim, orbitMap, orbitCount, innerObj);
    }
}


int getChecksum(const std::multimap<int, std::string> & orbitMap) {
    int sum = 0;

    // sum of all indirect orbits
    std::multimap<int, std::string>::const_iterator it = orbitMap.begin();
    for(; it != orbitMap.end(); ++it) {
        sum += it->first;
    }

    return sum;
}



// ################
// ----- MAIN -----
// ################

int main() {
    std::string data_path = "./in06.txt";

    std::vector<std::string> objects;
    int dim = loadData(data_path, objects);

    std::multimap<int, std::string> orbitMap;
    int orbitCount = 0;
    std::string innermostObj = "COM";

    findAllOrbits(objects, dim, orbitMap, orbitCount, innermostObj);

    int checksum = getChecksum(orbitMap);

    std::cout << "Sum of all direct and indirect orbits is " << checksum << "\n";
}
