CXX = g++
CXXFLAGS += -std=c++14
CXXFLAGS += -Wall -Wextra -Wpedantic

main05.exe: main05.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

run: main05.exe
	./main05.exe

clean:
	rm -v *.exe

.PHONY: run clean
