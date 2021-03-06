#ifndef INTCODE_HPP
#define INTCODE_HPP


#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cassert>


/*
An Intcode is a series of instruction sets consisting of two to four integers,
the first of which is the instruction (containing operation code and parameter
modes). The other integers are the set's parametes on which the operation is
performed

--- CONSTRUCTOR ---
- no default constructor
- constructor must be provided at least one argument, namely the int code

# intCode(std::vector<T> code, bool stopAtOutput = false,
          bool stopAtInput = false, bool printInOut = false,
          size_t pos = 0)
- Arguments:
    code - int code containing all instruction sets, the program if you want
    stopAtOutput - false (default): lets runIntCode() continue program
                   execution after each output instruction
                   true: lets runIntCode() return false after each output
                   instruction
    stopAtInput - false (default): lets runIntCode() continue program execution
                  after each input instruction
                  true: lets runIntCode() return false after each input
                  instruction
    printInOut - false (default): lets runIntCode() not print any output
                  generated by the program
                  true: lets runIntCode() print each output that the program
                  generates
    pos - lets runIntCode() start at a position in the program (series of
          instruction sets) different from 0 (default)


--- FUNCTIONS ---

# bool runIntCode(const std::vector<T> & IN)
- Arguments:
    IN (optional): If no input vector is provided, an empty vector is passed as
    arguement via a helper function
- Returns:
    boolean that is true IFF the int program was halted (only happens at the
    finish of the program), else returns false


--- WORKING PRINCIPLE ---

- Int program is initialized with constructor that sets all member variables
- runIntCode() loops through the instrucion sets (incrementing the position
  count by the number of integers in each respective set)
- for each instruction set
    - reads instruction (operation code + parameter modes of all parameters)
    - sets operation code dependent step size (increment of position count,
      i.e, where is the next instruction that must be read after this one)
    - sets all parameter modes AND if the index of any of the parameters to be
      read lies outside the range of the code size, the function
      setParameterMode() appends as many 0's (zeros) to the code vector as
      necessary in order to avoid false memory access.
    - finally executes operation (depending on operation code) on the
      previously set parameters
    - OPTIONAL: - program execution stops at output if stopAtOutput_ = true,
                - program execution stops at input if stopAtInput_ = true AND
                  pos_ is not 0 AND stopped_ = false (set to true when hitting
                  an input instruction and stopAtInput_ = true) (otherwise,
                  program execution would be caught in an infinite loop of
                  stopping at the same input instruction over and over)
                - program prints all outputs if printInOut_ = true
*/

template<typename T>
class intCode {
    public:
        static_assert(std::is_same<int, T>::value ||
                      std::is_same<long, T>::value ||
                      std::is_same<long long, T>::value,
                      "ERROR: class intCode must be called with template\
                       parameter of type <signed int>, i.e., int, long, etc.");

        // Ctor
        intCode() = delete;
        intCode(std::vector<T> code, bool stopAtOutput = false,
                bool stopAtInput = false, bool printInOut = false,
                size_t pos = 0) :
                code_(code), stopAtOutput_(stopAtOutput),
                stopAtInput_(stopAtInput), printInOut_(printInOut),
                pos_(pos), input_(0), inputCount_(0), relBase_(0),
                stopped_(false)
        {
            lastOut_.resize(0);
        }

        // Getters
        size_t getPosition() const { return pos_; }
        std::vector<T> getCode() const { return code_; }
        std::vector<T> getOutput() const { return lastOut_; }
        T getSingleOutput() const { return lastOut_[0]; }

        // Public Member
        bool runIntCode(const std::vector<T> &);
        bool runIntCode(const T &);
        bool runIntCode();

    private:
        std::vector<T> code_;
        bool stopAtOutput_;
        bool stopAtInput_;
        bool printInOut_;
        size_t pos_; // where in int code am I
        T input_;
        size_t inputCount_;
        T relBase_;
        std::vector<T> lastOut_;
        std::vector<int> paraMode_;
        bool stopped_;

        // Private Member
        void getParameterMode();
        void setParameterMode(std::vector<T *> &);
        void modify1();
        void modify2();
        void modify3();
        void modify4();
        void modify5();
        void modify6();
        void modify7();
        void modify8();
        void modify9();

        // Static Constants
        static const int ADD = 1;
        static const int MULTIPLY = 2;
        static const int INPUT = 3;
        static const int OUTPUT = 4;
        static const int JUMPTRUE = 5;
        static const int JUMPFALSE = 6;
        static const int LESS = 7;
        static const int EQUAL = 8;
        static const int ADJUSTBASE = 9;
        static const int HALT = 99;

        static const int POSITION = 0;
        static const int IMMEDIATE = 1;
        static const int RELATIVE = 2;
};


// ------------------------
// --- MEMBER FUNCTIONS ---
// ------------------------

// --- PUBLIC ---

template<typename T>
bool intCode<T>::runIntCode(const std::vector<T> & in) {
    // return true IFF program was halted (opcode 99), false otherwise
    // -----------------

    size_t step = 0;
    int opcode = 0;

    if (stopAtInput_) {
        lastOut_.resize(0);
        inputCount_ = 0;
    } else if (stopAtOutput_) {
        inputCount_ = 0;
    }


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

        if (opcode == ADD) {
            modify1();
        } else if (opcode == MULTIPLY) {
            modify2();
        } else if (opcode == INPUT) {
            if (pos_ > 0 && stopAtInput_ && !stopped_) {
                stopped_ = true;
                return false;
            }

            inputCount_++;
            if (inputCount_ <= in.size()) {
                input_ = in[inputCount_-1];
            } else {
                std::cout << "Too few input arguments provided, type "
                          << "Input here: ";
                std::cin >> input_;
            }

            modify3();
            stopped_ = false;

            if (printInOut_) {
                std::cout << "in: " << input_ << "\n";
            }
        } else if (opcode == OUTPUT) {
            /* if stopAtOutput_ only resize lastOut_ if another output
               instruction is issued. otherwise, it will be resized at last
               call of runIntCode before HALT, erasing the last output.
            */

            if (stopAtOutput_) { lastOut_.resize(0); }

            modify4();

            if (printInOut_) {
                std::cout << "out: " << lastOut_.back() << "\n";
            }

            if (stopAtOutput_) {
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

    if (printInOut_) {
        std::cout << "99 - HALTED\n";
    }

    return true;
}


template<typename T>
bool intCode<T>::runIntCode(const T & in) {
    // Allows for passing of single input as scalar, turns it into vector and
    // passes it to intCode<T>::runIntCode(const std::vector<T> & in)

	std::vector<T> inVec;
    inVec.push_back(in);
    bool res = runIntCode(inVec);

	return res;
}


template<typename T>
bool intCode<T>::runIntCode() {
    // Default argument for std::vector<T> input, in case no input vector is
    // provided. Prompts a manual input via std::cin for any input instruction

	std::vector<T> emptyVec;
    bool res = runIntCode(emptyVec);

	return res;
}


// --- PRIVATE ---

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
void intCode<T>::setParameterMode(std::vector<T *> & params)
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
    setParameterMode(params);

    *params[2] = (*params[0]) + (*params[1]);
}


template<typename T>
void intCode<T>::modify2()
{
    std::vector<T *> params(3);
    setParameterMode(params);

    *params[2] = (*params[0]) * (*params[1]);
}


template<typename T>
void intCode<T>::modify3()
{
    std::vector<T *> params(1);
    setParameterMode(params);

    *params[0] = input_;
}


template<typename T>
void intCode<T>::modify4()
{
    std::vector<T *> params(1);
    setParameterMode(params);

    lastOut_.push_back(*params[0]);
}


template<typename T>
void intCode<T>::modify5()
{
    std::vector<T *> params(2);
    setParameterMode(params);

    if (*params[0] != 0) { pos_ = *params[1]; }
    else { pos_ += 3; }
}


template<typename T>
void intCode<T>::modify6()
{
    std::vector<T *> params(2);
    setParameterMode(params);

    if (*params[0] == 0) { pos_ = *params[1]; }
    else { pos_ += 3; }
}


template<typename T>
void intCode<T>::modify7()
{
    std::vector<T *> params(3);
    setParameterMode(params);

    if (*params[0] < *params[1]) { *params[2] = 1; }
    else { *params[2] = 0; }
}


template<typename T>
void intCode<T>::modify8()
{
    std::vector<T *> params(3);
    setParameterMode(params);

    if (*params[0] == *params[1]) { *params[2] = 1; }
    else { *params[2] = 0; }
}


template<typename T>
void intCode<T>::modify9()
{
    std::vector<T *> params(1);
    setParameterMode(params);

    relBase_ += *params[0];
}


#endif // INTCODE_HPP
