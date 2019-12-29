#ifndef INTCODE_HPP
#define INTCODE_HPP


#include <iostream>
#include <fstream>
#include <string>
#include <vector>



class intCode {
    private:
        size_t pos_; // where in int code am I
        std::vector<int> code_;
        std::vector<int> paraMode_;
        int lastOut_;
        bool stopAtOutput_;
        int input_;
        size_t inputCount_;

        // Private Member
        void getParameterMode();
        void setParaMode(std::vector<int *> &);
        void modify1();
        void modify2();
        void modify3();
        void modify4();
        void modify5();
        void modify6();
        void modify7();
        void modify8();

    public:
        // Ctor
        intCode(std::vector<int> code, int in, bool stopAtOutput = false,
                size_t pos = 0)
        {
            code_ = code;
            lastOut_ = in;
            stopAtOutput_ = stopAtOutput;
            pos_ = pos;
            input_ = 0;
            inputCount_ = 0;
        }

        // Getters
        size_t getPosition() { return pos_; }
        std::vector<int> getCode() { return code_; }
        int getOutput() { return lastOut_; }

        // Public Member
        bool runIntCode(const std::vector<int> &);
};



void intCode::setParaMode(std::vector<int *> & params)
{
    // set pointers to integers depending on parameter mode (1 or 0, position
    // or immediate, respectively)

    for (size_t i = 0; i < params.size(); i++) {
        if (*(paraMode_.begin()+1+i) == 1) {
            params[i] = &(*(code_.begin()+pos_+1+i));
        } else {
            params[i] = &(*(code_.begin() + *(code_.begin()+pos_+1+i)));
        }
    }
}


// make template parameter for opcode (policies / expression templates)
void intCode::modify1()
{
    std::vector<int *> params(3);
    setParaMode(params);

    *params[2] = (*params[0]) + (*params[1]);
}


void intCode::modify2()
{
    std::vector<int *> params(3);
    setParaMode(params);

    *params[2] = (*params[0]) * (*params[1]);
}


void intCode::modify3()
{
    std::vector<int *> params(1);
    setParaMode(params);

    *params[0] = input_;
}


void intCode::modify4()
{
    std::vector<int *> params(1);
    setParaMode(params);

    lastOut_ = *params[0];
}


void intCode::modify5()
{
    std::vector<int *> params(2);
    setParaMode(params);

    if (*params[0] != 0) { pos_ = *params[1]; }
    else { pos_ += 3; }
}


void intCode::modify6()
{
    std::vector<int *> params(2);
    setParaMode(params);

    if (*params[0] == 0) { pos_ = *params[1]; }
    else { pos_ += 3; }
}


void intCode::modify7()
{
    std::vector<int *> params(3);
    setParaMode(params);

    if (*params[0] < *params[1]) { *params[2] = 1; } else { *params[2] = 0; }
}


void intCode::modify8()
{
    std::vector<int *> params(3);
    setParaMode(params);

    if (*params[0] == *params[1]) { *params[2] = 1; } else { *params[2] = 0; }
}



void intCode::getParameterMode() {
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


bool intCode::runIntCode(const std::vector<int> & in) {
    // out must be passed by referenc in order to write out without exiting
    // runIntCode().
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
        } else if (opcode == 3 || opcode == 4) {
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
