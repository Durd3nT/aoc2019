#ifndef INTCODE_HPP
#define INTCODE_HPP


#include <iostream>
#include <fstream>
#include <string>
#include <vector>


void setParaMode(std::vector<int *> & params,
                 std::vector<int>::iterator p_it,
                 std::vector<int>::iterator code_it, const int & pos)
{
    // set pointers to integers depending on parameter mode (1 or 0, position
    // or immediate, respectively)

    for (size_t i = 0; i < params.size(); i++) {
        if (*(p_it+i) == 1) {
            params[i] = &(*(code_it+pos+i));
        } else {
            params[i] = &(*(code_it + *(code_it+pos+i)));
        }
    }
}


// make template parameter for opcode (policies / expression templates)
void modify1(std::vector<int>::iterator p_it,
             std::vector<int>::iterator code_it, const int & pos)
{
    std::vector<int *> params(3);
    setParaMode(params, p_it, code_it, pos);

    *params[2] = (*params[0]) + (*params[1]);
}


void modify2(std::vector<int>::iterator p_it,
             std::vector<int>::iterator code_it, const int & pos)
{
    std::vector<int *> params(3);
    setParaMode(params, p_it, code_it, pos);

    *params[2] = (*params[0]) * (*params[1]);
}


void modify3(std::vector<int>::iterator p_it,
             std::vector<int>::iterator code_it, const int & pos)
{
    int input = 0;
    std::vector<int *> params(1);
    setParaMode(params, p_it, code_it, pos);

    std::cin >> input;
    *params[0] = input;

    // std::cout << "Input: " << input << "\n";
}


int modify4(std::vector<int>::iterator p_it,
             std::vector<int>::iterator code_it, const int & pos)
{
    std::vector<int *> params(1);
    setParaMode(params, p_it, code_it, pos);

    // std::cout << "Output: " << *params[0] << "\n";

    return *params[0];
}


size_t modify5(std::vector<int>::iterator p_it,
             std::vector<int>::iterator code_it, const int & pos)
{
    std::vector<int *> params(2);
    setParaMode(params, p_it, code_it, pos);

    if (*params[0] != 0) { return *params[1]; }
    else { return pos + 2; }
}


size_t modify6(std::vector<int>::iterator p_it,
             std::vector<int>::iterator code_it, const int & pos)
{
    std::vector<int *> params(2);
    setParaMode(params, p_it, code_it, pos);

    if (*params[0] == 0) { return *params[1]; }
    else { return pos + 2; }
}


void modify7(std::vector<int>::iterator p_it,
             std::vector<int>::iterator code_it, const int & pos)
{
    std::vector<int *> params(3);
    setParaMode(params, p_it, code_it, pos);

    if (*params[0] < *params[1]) { *params[2] = 1; } else { *params[2] = 0; }
}


void modify8(std::vector<int>::iterator p_it,
             std::vector<int>::iterator code_it, const int & pos)
{
    std::vector<int *> params(3);
    setParaMode(params, p_it, code_it, pos);

    if (*params[0] == *params[1]) { *params[2] = 1; } else { *params[2] = 0; }
}



void getParameterMode(const int & x, std::vector<int> & paraMode) {
    // from first integer in instruction (of form ABCDE):
    // - last two digits (DE) give opcode
    // - other digits (ABC - read from right to left, so CBA) give parameter
    //   modes of parameters in instruction. Parameter mode can be 1 (immediate
    //   mode, meaning the parameter is taken by value), or 0 (position mode,
    //   meaning the parameter gives the position/index of the value)
    //   If no parameter modes are provided, they are set to default - 0

    std::string xs = std::to_string(x);
    if (xs.size() > 2) {
        std::string ops = xs.substr(xs.size()-2);
        paraMode.push_back(std::stoi(ops));
        // 1002
        for (int i = xs.size()-3; i >= 0; i--) {
            paraMode.push_back(xs[i]-'0');
        }
    } else {
        paraMode.push_back(x);
    }
}


bool runIntCode(std::vector<int> code, int & out, const int start = 0) {
    // out must be passed by referenc in order to write out without exiting
    // runIntCode().
    // start gives starting position in int code (default = 0).
    // -----------------

    size_t step = 4;
    size_t i = start; // start iterating through intCode

    while (i < code.size()) {
        std::vector<int> paraMode;
        getParameterMode(code[i], paraMode);
        int opcode = paraMode[0];

        if (opcode == 1 || opcode == 2 || opcode == 7 || opcode == 8) {
            step = 4;
            paraMode.resize(step);
        } else if (opcode == 3 || opcode == 4) {
            step = 2;
            paraMode.resize(step);
        } else if (opcode == 5 || opcode == 6) {
            step = 3;
            paraMode.resize(step);
        }


        if (opcode == 1) { // ALTERNATIVE: Expression templates
            modify1(paraMode.begin()+1, code.begin(), i+1);
        } else if (opcode == 2) {
            modify2(paraMode.begin()+1, code.begin(), i+1);
        } else if (opcode == 3) {
            modify3(paraMode.begin()+1, code.begin(), i+1);
        } else if (opcode == 4) {
            out = modify4(paraMode.begin()+1, code.begin(), i+1);
        } else if (opcode == 5) {
            i = modify5(paraMode.begin()+1, code.begin(), i+1);
        } else if (opcode == 6) {
            i = modify6(paraMode.begin()+1, code.begin(), i+1);
        } else if (opcode == 7) {
            modify7(paraMode.begin()+1, code.begin(), i+1);
        } else if (opcode == 8) {
            modify8(paraMode.begin()+1, code.begin(), i+1);
        } else if (opcode == 99) {
            std::cout << " --- PROGRAM HALTED (op 99) --- \n";
            break;
        }
        else {
            std::cout << "ERROR: something went wrong - opcode = " << opcode;
            std::cout << "(must be in (1, 2, ..., 8, 99))\n";
        }

        if (opcode != 5 && opcode != 6) { i += step; }
    }
}


#endif // INTCODE_HPP
