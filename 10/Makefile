CXX = g++
CXXFLAGS += -std=c++14
CXXFLAGS += -Wall -Wextra -Wpedantic


main10.exe: main10.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

run: main10.exe
	./main10.exe

clean:
	rm -v *.exe

.PHONY: run clean
