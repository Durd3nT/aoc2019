CXX = g++
CXXFLAGS += -std=c++14
CXXFLAGS += -Wall -Wextra -Wpedantic


main09.exe: main09.cpp ../src/intCode.hpp
	$(CXX) $(CXXFLAGS) $< -o $@

run: main09.exe
	./main09.exe

clean:
	rm -v *.exe

.PHONY: run clean
