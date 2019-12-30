CXX = g++
CXXFLAGS += -std=c++14
CXXFLAGS += -Wall -Wextra -Wpedantic


main08.exe: main08.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

run: main08.exe
	./main08.exe

clean:
	rm -v *.exe

.PHONY: run clean
