#ifndef INTCODE_HPP
#define INTCODE_HPP


#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cassert>


const int ADD = 1;
const int MULTIPLY = 2;
const int INPUT = 3;
const int OUTPUT = 4;
const int JUMPTRUE = 5;
const int JUMPFALSE = 6;
const int LESS = 7;
const int EQUAL = 8;
const int ADJUSTBASE = 9;
const int HALT = 99;

const int POSITION = 0;
const int IMMEDIATE = 1;
const int RELATIVE = 2;



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
                bool printOutput = false,size_t pos = 0)
        {
            code_ = code;
            lastOut_ = 0;
            stopAtOutput_ = stopAtOutput;
            printOutput_ = printOutput;
            pos_ = pos;
            input_ = 0;
            inputCount_ = 0;
            relBase_ = 0;
        }
        ~intCode() = default;

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
        bool printOutput_;

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
    // - position mode (0)
    //   (parameter interpreted as position of value)
    // - immediate mode (1)
    //   (parameter interpreted as value)
    // - relative mode (2)
    //   (parameter is interpreted as position, value is at
    //   position + relative base)

    assert(paraMode_.size() == params.size()+1);
    assert(pos_+1+params.size() > 0 ||
        *(code_.begin() + pos_+1+params.size()) > 0 ||
        *(code_.begin() + pos_+1+params.size()) + relBase_ > 0);

    while (pos_+1+params.size() > code_.size() ||
           unsigned(*(code_.begin() + pos_+1+params.size())) > code_.size() ||
           unsigned(*(code_.begin() + pos_+1+params.size()) + relBase_) > code_.size())
    {
        code_.push_back(0);
    }

    for (size_t i = 0; i < params.size(); i++) {
        if (*(paraMode_.begin()+1+i) == POSITION) {
            params[i] = &( *(code_.begin() + *(code_.begin() + pos_+1+i) ) );
        } else if (*(paraMode_.begin()+1+i) == IMMEDIATE) {
            params[i] = &( *(code_.begin() + pos_+1+i) );
        } else if (*(paraMode_.begin()+1+i) == RELATIVE) {
            params[i] = &( *(code_.begin() + *(code_.begin() + pos_+1+i)
                                           + relBase_) );
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
        // opcode are LAST 2 digits of instruction
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
    int opcode = 0;

    while (opcode != HALT) {
        getParameterMode();
        opcode = paraMode_[0];

        if (opcode == 1 || opcode == 2 || opcode == 7 || opcode == 8) {
            step = 4;
        } else if (opcode == 3 || opcode == 4 || opcode == 9) {
            step = 2;
        } else if (opcode == 5 || opcode == 6) {
            step = 3;
        }

        // fill all remaining parameter modes with 0
        paraMode_.resize(step);

        if (opcode == ADD) { // ALTERNATIVE: Expression templates
            modify1();
        } else if (opcode == MULTIPLY) {
            modify2();
        } else if (opcode == INPUT) {
            inputCount_++;
            if (inputCount_ <= in.size()) {
                input_ = in[inputCount_-1];
            } else {
                std::cout << " Too few input arguments provided, type ";
                std::cout << "Input here: ";
                std::cin >> input_;
            }
            modify3();
        } else if (opcode == OUTPUT) {
            modify4();
            if (printOutput_) {
                std::cout << lastOut_ << "\n";
            }
            if (stopAtOutput_) {
                inputCount_ = 0;
                pos_ += step;
                return false;
            }
        } else if (opcode == JUMPTRUE) {
            modify5();
        } else if (opcode == JUMPFALSE) {
            modify6();
        } else if (opcode == LESS) {
            modify7();
        } else if (opcode == EQUAL) {
            modify8();
        } else if (opcode == ADJUSTBASE) {
            modify9();
        } else if (opcode > 9 && opcode != 99) {
            std::cout << "ERROR: something went wrong - opcode = " << opcode;
            std::cout << "(must be in (1, 2, ..., 8, 99))\n";
            return false;
        }

        if (opcode != 5 && opcode != 6) { pos_ += step; }
    } // WHILE pos_ < code_.size()

    return true;
}


#endif // INTCODE_HPP
