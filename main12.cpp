#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <numeric>



template<typename T>
struct moon {
    std::vector<T> pos_;
    std::vector<T> vel_;
};


template<typename T>
void loadData(const std::string & dataPath, std::vector<moon<T>> & data)
{
    std::ifstream infile(dataPath);

    std::string line;

    while (getline(infile, line, '\n')) {
        moon<T> m;

        size_t str_pos = 0;

        while ((str_pos = line.find('=', str_pos)) != std::string::npos) {
            size_t end = line.find(',', str_pos);

            if (end == std::string::npos) {
                end = line.find('>', str_pos);
            }

            std::string num = line.substr(str_pos+1, end-str_pos-1);
            m.pos_.push_back(std::stoi(num));

            str_pos++;
        } // WHILE find('=')

        data.push_back(m);
    } // WHILE getline '\n'
}



template<typename T>
void updateVelocity(moon<T> & m1, moon<T> & m2) {
    for (size_t i = 0; i < m1.pos_.size(); i++) {
        if (m1.pos_[i] < m2.pos_[i]) {
            m1.vel_[i]++;
            m2.vel_[i]--;
        } else if (m1.pos_[i] > m2.pos_[i]) {
            m1.vel_[i]--;
            m2.vel_[i]++;
        }
    }
}


template<typename T>
void updatePosition(moon<T> & m) {
    for (size_t i = 0; i < m.pos_.size(); i++) {
        m.pos_[i] += m.vel_[i];
    }
}


template<typename T>
int computeEnergy(const std::vector<moon<T>> & moons) {
    int sum = 0;
    for (const auto & m: moons) {
        int Epot = 0;
        int Ekin = 0;

        for (size_t i = 0; i < m.pos_.size(); i++) {
            Epot += std::abs(m.pos_[i]);
            Ekin += std::abs(m.vel_[i]);
        }

        sum += (Epot*Ekin);
    }

    return sum;
}


template<typename T>
void step(std::vector<moon<T>> & moons) {
    for (size_t i = 0; i < moons.size()-1; i++) {
        for (size_t j = i+1; j < moons.size(); j++) {
            updateVelocity(moons[i], moons[j]);
        }
    }

    for (size_t i = 0; i < moons.size(); i++) {
        updatePosition(moons[i]);
    }
}


template<typename T>
void simulate(std::vector<moon<T>> & moons, const unsigned Tmax) {
    for (unsigned i = 0; i < Tmax; i++) {
        step(moons);
    }
}


template<typename T>
long long get_periodicity(std::vector<moon<T>> & moons) {
    std::vector<std::vector<int>> moons_init_pos;
    for (const auto & m: moons) {
        moons_init_pos.push_back(m.pos_);
    }

    std::vector<bool> coords_periodic(3, false);
    std::vector<int> coords_periodicity(3, 0);

    unsigned t = 0;
    while (!coords_periodic[0] || !coords_periodic[1] || !coords_periodic[2]) {
        step(moons);
        t++;

        for (size_t k = 0; k < coords_periodic.size(); k++) {
            if (!coords_periodic[k]) {
                bool equal = true;
                for (size_t m = 0; m < moons.size(); m++) {
                    if (moons[m].pos_[k] != moons_init_pos[m][k]) {
                        equal = false;
                        break;
                    }
                }
                if (equal) {
                    coords_periodicity[k] = t+1;
                    std::cout << "coord " << k << " periodic at " << t+1 << "\n";
                    coords_periodic[k] = true;
                }
            }
        }
    }

    return std::lcm<long long>(std::lcm<long long>(coords_periodicity[0],
                                                   coords_periodicity[1]),
                               coords_periodicity[2]);
}


int main() {
    std::string dataPath = "./input_files/in12.txt";

    std::vector<moon<int>> moons_init, moons;

    // read initial positions
    loadData(dataPath, moons_init);

    for (auto & m: moons_init) {
        m.vel_.resize(3,0);
    }

    moons = moons_init;

    std::cout << "\n - - - PART I - - -\n";
    unsigned Tmax = 1000;
    simulate(moons, Tmax);

    int Etot = computeEnergy(moons);
    std::cout << "Total energy of all moons: " << Etot << "\n";

    std::cout << "\n - - - PART II - - -\n";
    std::vector<moon<int>> moons_periodic = moons_init;

    long long periodicity = get_periodicity(moons_periodic);
    std::cout << "All moons return to their initial position at time step ";
    std::cout << periodicity << "\n";
}
