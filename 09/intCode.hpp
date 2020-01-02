#ifndef INTCODE_HPP
#define INTCODE_HPP


#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <type_traits>


// Type Traits
// template<typename T> struct intCode_type<T> { typedef T type; }
// template<> struct intCode_type<int> { typedef int type; }


template<typename T>
class intCode {
    public:
        static_assert(std::is_same<int, T>::value ||
                      std::is_same<long, T>::value ||
                      std::is_same<long long, T>::value,
                      "ERROR: class intCode must be called with template\
                       parameter of type <signed int>, i.e., int, long, etc.");

        // Ctor
        intCode(std::vector<T> code, bool stopAtOutput = false,
                size_t pos = 0)
        {
            code_ = code;
            lastOut_ = 0;
            stopAtOutput_ = stopAtOutput;
            pos_ = pos;
            input_ = 0;
            inputCount_ = 0;
            relBase_ = 0;
        }

        // Getters
        size_t getPosition() { return pos_; }
        std::vector<T> getCode() { return code_; }
        T getOutput() { return lastOut_; }

        // Public Member
        bool runIntCode(const std::vector<T> &);

    private:
        size_t pos_; // where in int code am I
        size_t inputCount_;
        T relBase_;
        T lastOut_;
        T input_;
        std::vector<T> code_;
        std::vector<int> paraMode_;
        bool stopAtOutput_;

        // Private Member
        void getParameterMode();
        void setParaMode(std::vector<T *> &);
        void modify1();
        void modify2();
        void modify3();
        void modify4();
        void modify5();
        void modify6();
        void modify7();
        void modify8();
        void modify9();

};


template<typename T>
void intCode<T>::setParaMode(std::vector<T *> & params)
{
    // set pointers to integers depending on parameter mode
    // 0 - position mode
    //     (parameter interpreted as position of value)
    // 1 - immediate mode
    //     (parameter interpreted as value)
    // 2 - relative mode
    //     (parameter is interpreted as position, value is at
    //     position + relative base)

    for (size_t i = 0; i < params.size(); i++) {
        if (*(paraMode_.begin()+1+i) == 0) { // position mode
            params[i] = &( *(code_.begin() + *(code_.begin() + pos_+1+i) ) );
        } else if (*(paraMode_.begin()+1+i) == 1) { // immediate mode
            params[i] = &( *(code_.begin() + pos_+1+i) );
        } else if (*(paraMode_.begin()+1+i) == 2) { // relative mode
            params[i] = &( *(code_.begin() + *(code_.begin() + pos_+1+i) + relBase_) );
        }
    }
}


template<typename T>
void intCode<T>::modify1()
{
    std::vector<T *> params(3);
    setParaMode(params);

    *params[2] = (*params[0]) + (*params[1]);
}


template<typename T>
void intCode<T>::modify2()
{
    std::vector<T *> params(3);
    setParaMode(params);

    *params[2] = (*params[0]) * (*params[1]);
}


template<typename T>
void intCode<T>::modify3()
{
    std::vector<T *> params(1);
    setParaMode(params);

    *params[0] = input_;
}


template<typename T>
void intCode<T>::modify4()
{
    std::vector<T *> params(1);
    setParaMode(params);

    lastOut_ = *params[0];
}


template<typename T>
void intCode<T>::modify5()
{
    std::vector<T *> params(2);
    setParaMode(params);

    if (*params[0] != 0) { pos_ = *params[1]; }
    else { pos_ += 3; }
}


template<typename T>
void intCode<T>::modify6()
{
    std::vector<T *> params(2);
    setParaMode(params);

    if (*params[0] == 0) { pos_ = *params[1]; }
    else { pos_ += 3; }
}


template<typename T>
void intCode<T>::modify7()
{
    std::vector<T *> params(3);
    setParaMode(params);

    if (*params[0] < *params[1]) { *params[2] = 1; } else { *params[2] = 0; }
}


template<typename T>
void intCode<T>::modify8()
{
    std::vector<T *> params(3);
    setParaMode(params);

    if (*params[0] == *params[1]) { *params[2] = 1; } else { *params[2] = 0; }
}


template<typename T>
void intCode<T>::modify9()
{
    std::vector<T *> params(1);
    setParaMode(params);

    relBase_ += *params[0];
}


template<typename T>
void intCode<T>::getParameterMode() {
    // from first integer in instruction (of form ABCDE):
    // - last two digits (DE) give opcode
    // - other digits (ABC - read from right to left, so CBA) give parameter
    //   modes of parameters in instruction. Parameter mode can be 1 (immediate
    //   mode, meaning the parameter is taken by value), or 0 (position mode,
    //   meaning the parameter gives the position/index of the value)
    //   If no parameter modes are provided, they are set to default - 0
    // -------------------------

    paraMode_.resize(0);

    std::string xs = std::to_string(code_[pos_]);
    if (xs.size() > 2) {
        std::string ops = xs.substr(xs.size()-2);
        paraMode_.push_back(std::stoi(ops));

        for (int i = xs.size()-3; i >= 0; i--) {
            paraMode_.push_back(xs[i]-'0');
        }
    } else {
        paraMode_.push_back(code_[pos_]);
    }
}


template<typename T>
bool intCode<T>::runIntCode(const std::vector<T> & in) {
    //
    // start gives starting position in int code (default = 0).
    // return true IFF program was halted (opcode 99), false otherwise
    // -----------------

    size_t step = 0;

    while (pos_ < code_.size()) {
        getParameterMode();
        int opcode = paraMode_[0];

        if (opcode == 1 || opcode == 2 || opcode == 7 || opcode == 8) {
            step = 4;
            paraMode_.resize(step);
        } else if (opcode == 3 || opcode == 4 || opcode == 9) {
            step = 2;
            paraMode_.resize(step);
        } else if (opcode == 5 || opcode == 6) {
            step = 3;
            paraMode_.resize(step);
        }


        if (opcode == 1) { // ALTERNATIVE: Expression templates
            modify1();
        } else if (opcode == 2) {
            modify2();
        } else if (opcode == 3) {
            inputCount_++;
            if (inputCount_ <= in.size()) {
                input_ = in[inputCount_-1];
            } else {
                std::cout << " Too few input arguments provided, type ";
                std::cout << "Input here: ";
                std::cin >> input_;
            }
            modify3();
        } else if (opcode == 4) {
            modify4();
            std::cout << lastOut_ << "\n";
            if (stopAtOutput_) {
                inputCount_ = 0;
                pos_ += step;
                return false;
            }
        } else if (opcode == 5) {
            modify5();
        } else if (opcode == 6) {
            modify6();
        } else if (opcode == 7) {
            modify7();
        } else if (opcode == 8) {
            modify8();
        } else if (opcode == 9) {
            modify9();
        } else if (opcode == 99) {
            return true; // program came to an end
        }
        else {
            std::cout << "ERROR: something went wrong - opcode = " << opcode;
            std::cout << "(must be in (1, 2, ..., 8, 99))\n";
            return false;
        }

        if (opcode != 5 && opcode != 6) { pos_ += step; }
    } // WHILE pos_ < code_.size()

    return false;
}


#endif // INTCODE_HPP
