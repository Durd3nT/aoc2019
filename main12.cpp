#include <iostream>
#include <fstream>
#include <string>
#include <vector>

// --------------------
// --- Vector CLASS ---
// --------------------
// template<typename T>
// class vec3 {
//     private:
//         using value_t = T;
//         std::vector<value_t> comp;
//
//     public:
//         // ----- SPECIAL MEMBERS -----
//         vec3() { comp.resize(3); }
//
//         vec3(value_t a, value_t b, value_t c) {
//             comp.resize(3);
//             comp[0] = a;
//             comp[1] = b;
//             comp[2] = c;
//         }
//
//         vec3(const vec3 &) = default;
//         vec3 & operator=(const vec3 &) = default;
//
//         // ----- OPERATORS -----
//         inline vec3 operator-() const;
//         inline vec3 & operator+=(const vec3 & v);
//         inline vec3 & operator-=(const vec3 & v);
//         inline value_t operator[](int i) const { return comp[i]; }
//         inline value_t & operator[](int i) { return comp[i]; }
//
//         inline size_t size() const { return comp.size(); }
// };
//
// // ----- Operators -----
// template<typename T>
// inline vec3<T> vec3<T>::operator-() const {
//     return vec3(-comp[0], -comp[1], -comp[2]);
// }
//
// template<typename T>
// inline vec3<T> & vec3<T>::operator+=(const vec3 & v) {
//     comp[0] += v.comp[0];
//     comp[1] += v.comp[1];
//     comp[2] += v.comp[2];
//     return *this;
// }
//
// template<typename T>
// inline vec3<T> & vec3<T>::operator-=(const vec3 & v) {
//     comp[0] -= v.comp[0];
//     comp[1] -= v.comp[1];
//     comp[2] -= v.comp[2];
//     return *this;
// }




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

    while(getline(infile, line, '\n')) {
        moon<T> m;

        size_t str_pos = 0;

        while((str_pos = line.find('=', str_pos)) != std::string::npos) {
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
void simulate(std::vector<moon<T>> & moons, unsigned Tmax) {
    for (unsigned i = 0; i < Tmax; i++) {
        step(moons);
    }
}


int main() {
    std::string dataPath = "./input_files/in12.txt";

    std::vector<moon<int>> moons;

    // read initial positions
    loadData(dataPath, moons);

    for (auto & i: moons) {
        i.vel_.resize(3,0);
    }

    std::cout << "\n - - - PART I - - -\n";
    unsigned Tmax = 1000;
    simulate(moons, Tmax);

    int Etot = computeEnergy(moons);
    std::cout << "Total energy of all moons: " << Etot << "\n";


}
