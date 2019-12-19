CXX = g++
CXXFLAGS += -std=c++14
CXXFLAGS += -Wall -Wextra -Wpedantic

main06.exe: main06.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

run: main06.exe
	./main06.exe

clean:
	rm -v *.exe

.PHONY: run clean
