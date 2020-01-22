CXX = g++
CXXFLAGS += -std=c++14
CXXFLAGS += -Wall -Wextra -Wpedantic


main07.exe: main07.cpp ../src/intCode.hpp
	$(CXX) $(CXXFLAGS) $< -o $@

run: main07.exe
	./main07.exe

clean:
	rm -v *.exe

.PHONY: run clean
