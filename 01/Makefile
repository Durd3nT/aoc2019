CXX = g++
CXXFLAGS += -std=c++14
CXXFLAGS += -Wall -Wextra -Wpedantic

main.exe: main.cpp
	$(CXX) $(CXXFLAGS) main.cpp -o main.exe

run: main.exe
	./main.exe

clean:
	rm -v *.exe

.PHONY: run clean
