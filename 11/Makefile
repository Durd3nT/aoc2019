CXX = g++
CXXFLAGS += -std=c++14
CXXFLAGS += -Wall -Wextra -Wpedantic


main11.exe: main11.cpp ../src/intCode.hpp
	$(CXX) $(CXXFLAGS) $< -o $@

run: main11.exe
	./main11.exe

clean:
	rm -v *.exe

.PHONY: run clean
