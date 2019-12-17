CXX = g++
CXXFLAGS += -std=c++14
CXXFLAGS += -Wall -Wextra -Wpedantic

main04.exe: main04.cpp
	$(CXX) $(CXXFLAGS) main04.cpp -o main04.exe

run: main04.exe
	./main04.exe

clean:
	rm -v *.exe

.PHONY: run clean
