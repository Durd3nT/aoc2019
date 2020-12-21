#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <cassert>




typedef std::map<std::string, long> map_si;


void read_file(const std::string & path,
               std::map<std::string, map_si> & reaction_map,
               map_si & product_map, map_si & leftover_map)
{
    assert(reaction_map.empty());

    std::ifstream in_file(path);

    if (in_file.is_open()) {
        std::string line;
        std::string arrow = " => ";
        std::string sep = ", ";

        while (getline(in_file, line, '\n')) {
            std::istringstream lineStream(line);
            std::string subline;
            map_si ingreds;

            size_t pos_space;
            size_t pos_sep = line.find(sep);
            while (pos_sep != std::string::npos) {
                subline = line.substr(0, pos_sep);

                pos_space = subline.find(' ');
                ingreds.insert({subline.substr(pos_space+1),
                                std::stoi(subline.substr(0, pos_space))});

                line.erase(0, pos_sep + sep.size());
                pos_sep = line.find(sep);
            }

            size_t pos_arrow = line.find(arrow);
            subline = line.substr(0, pos_arrow);
            pos_space = subline.find(' ');
            ingreds.insert({subline.substr(pos_space+1),
                            std::stoi(subline.substr(0, pos_space))});

            subline = line.substr(pos_arrow + arrow.size());
            pos_space = subline.find(' ');

            product_map.insert({subline.substr(pos_space+1),
                                std::stoi(subline.substr(0, pos_space))});
            leftover_map.insert({subline.substr(pos_space+1), 0});
            reaction_map.insert({subline.substr(pos_space+1), ingreds});
        }
    } else {
        std::cout << "Error: could not open file " << path << "\n";
    }
}



void reset_leftover(map_si & leftover) {
    for (auto & x: leftover) {
        x.second = 0;
    }
}



long get_total_ore_req(const std::map<std::string, map_si> & reaction_map,
                       const map_si & product_map,
                       const std::string & prod, const long & quantity,
                       map_si & leftover)
{
    std::map<std::string, map_si>::const_iterator prod_it = reaction_map.find(prod);

    long n = quantity / product_map.find(prod)->second;
    if ((quantity % product_map.find(prod)->second) > leftover[prod]) {
        n += (quantity % product_map.find(prod)->second != 0);
        leftover[prod] += product_map.find(prod)->second - (quantity % product_map.find(prod)->second);
    } else {
        leftover[prod] -= (quantity % product_map.find(prod)->second);
    }

    long ore_count = 0;
    map_si::const_iterator ingred_it = prod_it->second.begin();

    while (ingred_it != prod_it->second.end()) {
        if (ingred_it->first == "ORE") {
            ore_count += n*(ingred_it->second);
        } else {
            ore_count += get_total_ore_req(reaction_map, product_map,
                                           ingred_it->first,
                                           n*(ingred_it->second),
                                           leftover);
        }

        ingred_it++;
    }

    return ore_count;
}


long get_max_fuel(const std::map<std::string, map_si> & reaction_map,
                  const map_si & product_map,
                  const long & tot_ore,
                  const map_si & leftover_init)
{
    map_si leftover(leftover_init);
    long ore_count = get_total_ore_req(reaction_map, product_map,
                                  "FUEL", 1, leftover);
    reset_leftover(leftover);
    long max_fuel = tot_ore / ore_count; // initial guess
    long step = max_fuel;

    while (step != 1) {
        reset_leftover(leftover);

        if (step > 2){
            step = step / 2 + (step % 2 != 0);
        } else {
            step = 1;
        }

        if (ore_count > tot_ore) {
            max_fuel -= step;
        } else if (ore_count <= tot_ore) {
            max_fuel += step;
        }

        ore_count = get_total_ore_req(reaction_map, product_map,
                                      "FUEL", max_fuel, leftover);
    }

    while (ore_count > tot_ore) {
        max_fuel -= step;
        ore_count = get_total_ore_req(reaction_map, product_map,
                                      "FUEL", max_fuel, leftover);
    }

    return max_fuel;
}



// ----------------
// - - - MAIN - - -
// ----------------

int main() {
    std::string data_path = "./input_files/in14.txt";
    std::map<std::string, map_si> reaction_map;
    map_si product_map;
    map_si leftover;
    map_si quant_map;

    read_file(data_path, reaction_map, product_map, leftover);

    {
        long ore_count = get_total_ore_req(reaction_map, product_map,
                                           "FUEL", 1, leftover);

        std::cout << "\n - - - PART I - - -\n";
        std::cout << "Total required ORE for 1 FUEL: " << ore_count << "\n";
    }

    {
        reset_leftover(leftover);
        long tot_ore_collected = 1e12;

        long max_fuel = get_max_fuel(reaction_map, product_map,
                                     tot_ore_collected, leftover);

        std::cout << "\n - - - PART II - - -\n";
        std::cout << "Total amount of FUEL from collected ORE: " << max_fuel << "\n";

        long ore_count = get_total_ore_req(reaction_map, product_map,
                                           "FUEL", max_fuel, leftover);
        std::cout << "using " << ore_count << " ORE\n";
    }
}
