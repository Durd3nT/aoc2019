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
                std::multimap<std::string, int> & orbitMap,
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

    orbitMap.insert(std::pair<std::string, int>(innerObj, orbitCount));

    for (int i = 0; i < dim; i++) {
        if (objects[i] == innerObj) { indices.push_back(i+dim); }
    }

    orbitCount++;
    for (size_t i = 0; i < indices.size(); i++) {
        innerObj = objects[indices[i]];
        findAllOrbits(objects, dim, orbitMap, orbitCount, innerObj);
    }
}


int getChecksum(const std::multimap<std::string, int> & orbitMap) {
    int sum = 0;

    // sum of all indirect orbits
    std::multimap<std::string, int>::const_iterator it = orbitMap.begin();
    for(; it != orbitMap.end(); ++it) {
        sum += it->second;
    }

    return sum;
}


int getOrbitalDist(const std::vector<std::string> & objects, const int dim,
                   const std::multimap<std::string, int> & orbitMap,
                   const std::string & o1, const std::string & o2)
{
    // Get distance between objects that o1 and o2 are orbiting. Hence,
    // o1 and o2 are found in the second half of std::vector objects.
    // NOTE: o1 and o2 may have different distances (numbers of indirect orbits)
    //       between them and the branching at which their two branches join.

    std::vector<int> index(2);
    int diff = 0;
    std::string obj1, obj2;

    auto orbit1 = orbitMap.find(o1);
    auto orbit2 = orbitMap.find(o2);

    // regardless of number of indirect orbits of o1 and o2, respectively,
    // redefine obj1 to always be the furthest away from COM
    if (orbit1->second < orbit2->second) {
        obj1 = o2;
        obj2 = o1;
        diff = (orbit2->second) - (orbit1->second);
    } else {
        obj1 = o1;
        obj2 = o2;
        diff = (orbit1->second) - (orbit2->second);
    }

    for (int i = 0; i < diff; i++) {
        for (int i = dim; i < 2*dim; i++) {
            if (objects[i] == obj1) { index[0] = i-dim; }
        }
        obj1 = objects[index[0]];
    } // FOR: i < diff

    int count = diff;

    while (obj1 != obj2) {
        count += 2;

        for (int i = dim; i < 2*dim; i++) {
            if (objects[i] == obj1) { index[0] = i-dim; }
            else if (objects[i] == obj2) { index[1] = i-dim; }
        }

        obj1 = objects[index[0]];
        obj2 = objects[index[1]];
    }
    count -= 2; // subtract first two steps from o1 (YOU) and o2 (SAN) inward

    return count;
}



// ################
// ----- MAIN -----
// ################

int main() {
    std::string data_path = "./input_files/in06.txt";

    std::vector<std::string> objects;
    int dim = loadData(data_path, objects);

    std::multimap<std::string, int> orbitMap;
    int orbitCount = 0;
    std::string innermostObj = "COM";

    findAllOrbits(objects, dim, orbitMap, orbitCount, innermostObj);

    int checksum = getChecksum(orbitMap);
    int distSanta = getOrbitalDist(objects, dim, orbitMap, "YOU", "SAN");

    std::cout << "Sum of all direct and indirect orbits: " << checksum << "\n";
    std::cout << "Orbital distance to Santa: " << distSanta << "\n";
}
