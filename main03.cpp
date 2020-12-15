#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <limits>
#include <cmath>
#include <cassert>



enum direction {up, down, left, right};

std::ostream & operator<<(std::ostream & out, const direction & dir) {
    switch (dir) {
        case up:
            out << "up";
            break;
        case down:
            out << "down";
            break;
        case left:
            out << "left";
            break;
        case right:
            out << "right";
            break;
        default:
            out << "No fit";
    }
    return out;
}


struct segment {
    direction dir;
    int dist;
};

struct wire {
    /* edge_coords stores coordinates (x,y) of all edges of a wire, i.e., of
       points where the wire changes direction.
       cum_len stores the length of a wire up to each coordinate
       edge_coords[i]
    */

    std::vector<std::vector<int>> edge_coords;
    std::vector<long unsigned> cum_len;
};

struct intersection {
    /* Stores:
       - coords: (x,y) of intersction
       - man_dist: manhatten distance to origin
       - len_to_inter: length of all wires up to intersection
    */

    std::vector<int> coords;
    unsigned man_dist;
    std::vector<unsigned> len_to_inter;
};


// --- FUNCTIONS ---
template <typename S>
void loadData(const std::string & data_path,
              std::vector<std::vector<S>> & wire_segs)
{
    /*
    Load CSVs from format <char><int>, e.g., R719, into a vector of objects of
    type segment, storing the first char (direction, e.g., R) in segment.dir
    and the rest of the string (distance, e.g., 719) in segment.dist
    */

    assert(wire_segs.empty());
    std::ifstream infile(data_path);

    std::string line;
    std::string walk;
    int wire_count = 0;

    if (infile.is_open()) {
        while (getline(infile, line, '\n')) {
            wire_segs.resize(wire_count + 1);

            std::istringstream lineStream(line);
            while (getline(lineStream, walk, ',')) {
                S seg;

                switch (walk[0]) {
                    case 'U':
                        seg.dir = up;
                        break;
                    case 'D':
                        seg.dir = down;
                        break;
                    case 'L':
                        seg.dir = left;
                        break;
                    case 'R':
                        seg.dir = right;
                        break;
                    default:
                        std::cout << "ERROR: direction letter " << walk[0];
                        std::cout << " not supported!\n";
                }

                std::string dist = walk.substr(1);
                seg.dist = std::stoi(dist);
                wire_segs[wire_count].push_back(seg);
            }
            wire_count++;
        }
    } else {
        std::cout << "Error: could not open file " << data_path << "\n";
    }
}


template <typename S, typename W>
void getGridCoord(const std::vector<std::vector<S>> & wire_segs,
                  std::vector<W> & wires)
{
    /*
    Store the wires in vector of object of type wire.
    Each object of type wire contains a coordinate vector of all edges (i.e.,
    changes in direction) of the wire in the coordinate (x0,y0), (x1, y1), ...
    Each object of type wire further contains a vector containing its
    cumulative length up to the respectuve coordinate.
    Coordinates are obtained by iterating through the directions and distances
    of each wire's segments.
    */

    assert(wires.empty() && "wires vector must be empty");

    wires.resize(wire_segs.size());

    for (size_t i = 0; i < wire_segs.size(); i++) { // loop over wires
        int x = 0;
        int y = 0;
        int l = 0;

        wires[i].edge_coords.push_back(std::vector<int>{x, y});
        wires[i].cum_len.push_back(l);

        for (size_t j = 0; j < wire_segs[i].size(); j++) {
            switch (wire_segs[i][j].dir) {
                case up:
                    y += wire_segs[i][j].dist;
                    break;
                case down:
                    y -= wire_segs[i][j].dist;
                    break;
                case left:
                    x -= wire_segs[i][j].dist;
                    break;
                case right:
                    x += wire_segs[i][j].dist;
                    break;
            }
            l += wire_segs[i][j].dist;

            wires[i].edge_coords.push_back(std::vector<int>{x, y});
            wires[i].cum_len.push_back(l);
        }
    }
}


bool inbetween(const int & x, const int & a, const int & b) {
    /* Is x in [a,b] or [b,a]?
    */

    return ( (x > a && x < b) || (x < a && x > b) );
}


bool intersect(std::vector<std::vector<int>>::const_iterator coord1,
               std::vector<std::vector<int>>::const_iterator coord2)
{
    /* Do the two lines starting at *coord1 and *coord2, respectively,
       intersect?
    */

    if ( inbetween((*coord2)[0], (*coord1)[0], (*(coord1 + 1))[0])
         && inbetween((*coord1)[1], (*coord2)[1], (*(coord2 + 1))[1]) ) {
        return true;
    } else if ( inbetween((*coord1)[0], (*coord2)[0], (*(coord2 + 1))[0])
         && inbetween((*coord2)[1], (*coord1)[1], (*(coord1 + 1))[1]) ) {
        return true;
    } else { return false; }
}


void computeIntersection(std::vector<std::vector<int>>::const_iterator coord1,
                         std::vector<std::vector<int>>::const_iterator coord2,
                         int & x, int & y)
{
    /* Compute intersection between lines starting at *coord1 and *coord2,
       respectively, store coordinates of intersection in x and y.
    */

    if ((*coord1)[0] == (*(coord1 + 1))[0]) {
        x = (*coord1)[0];
        y = (*coord2)[1];
    } else {
        x = (*coord2)[0];
        y = (*coord1)[1];
    }
}


template <typename W, typename I>
void get_all_intersections(const std::vector<W> & wires,
                           std::vector<I> & inters)
{
    /*
    Finds the coordinates of all intersections by looping through the
    edge-coordinates of both cables and checking whether they cross.
    Confirmed intersections are stored in a vector of objects of type
    intersection, with coordinates of the intersection in intersection.coords,
    as well as manhatten distance and cumulative length up to that
    intersection.
    */

    assert(inters.empty() && "inters vector must be empty");

    for (size_t i = 0; i < wires[0].edge_coords.size()-1; i++) {
        for (size_t j = 0; j < wires[1].edge_coords.size()-1; j++) {
            // compute intersection
            std::vector<std::vector<int>>::const_iterator it0 = wires[0].edge_coords.begin();
            std::vector<std::vector<int>>::const_iterator it1 = wires[1].edge_coords.begin();

            if (intersect(it0 + i, it1 + j)) {
                int x = 0;
                int y = 0;
                computeIntersection(it0 + i, it1 + j, x, y);

                I inter;
                inter.coords = std::vector<int>{x, y};
                inter.man_dist = std::abs(x) + std::abs(y);
                inter.len_to_inter.push_back(wires[0].cum_len[i]
                                             + std::abs((*(it0 + i))[0] - x)
                                             + std::abs((*(it0 + i))[1] - y));
                inter.len_to_inter.push_back(wires[1].cum_len[j]
                                             + std::abs((*(it1 + j))[0] - x)
                                             + std::abs((*(it1 + j))[1] - y));

                inters.push_back(inter);
            }
        }
    }
}


template <typename I>
unsigned find_closest_intersection(const std::vector<I> & inters) {
    /* Find intersection with minimum manhatten distance.
    */

    unsigned min = std::numeric_limits<int>::max();

    for (size_t i = 0; i < inters.size(); i++) {
        if (inters[i].man_dist < min && inters[i].man_dist != 0) {
            min = inters[i].man_dist;
        }
    }

    return min;
}


template <typename I>
unsigned find_min_comb_len(const std::vector<I> & inters) {
    /* Find intersection with minimum combined cable length up to the
       intersection.
    */

    unsigned min = std::numeric_limits<int>::max();

    for (size_t i = 0; i < inters.size(); i++) {
        unsigned com_len = (inters[i].len_to_inter[0] + inters[i].len_to_inter[1]);
        if (com_len < min && inters[i].man_dist != 0) {
            min = com_len;
        }
    }

    return min;
}


// - - - - - - - - -
// ----- MAIN -----
// - - - - - - - - -
int main() {
    std::string data_path = "./input_files/in03.txt";

    std::vector<std::vector<segment>> wire_segs;
    loadData<segment>(data_path, wire_segs);

    std::vector<wire> wires;
    getGridCoord<segment, wire>(wire_segs, wires);

    std::vector<intersection> inters;
    get_all_intersections<wire, intersection>(wires, inters);

    unsigned closest = find_closest_intersection(inters);

    std::cout << "\n - - - PART 1 - - - \n";
    std::cout << "Closest intersection is ";
    std::cout << closest << " away\n";

    unsigned min_com_len = find_min_comb_len(inters);

    std::cout << "\n - - - PART 2 - - - \n";
    std::cout << "Minimum combined cable length to intersection is ";
    std::cout << min_com_len << "\n";

}
