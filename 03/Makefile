CXX = g++
CXXFLAGS += -std=c++14
CXXFLAGS += -Wall -Wextra -Wpedantic

main03.exe: main03.cpp
	$(CXX) $(CXXFLAGS) main03.cpp -o main03.exe

run: main03.exe
	./main03.exe

clean:
	rm -v *.exe

.PHONY: run clean
