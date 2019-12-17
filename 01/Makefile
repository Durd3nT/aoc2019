CXX = g++
CXXFLAGS += -std=c++14
CXXFLAGS += -Wall -Wextra -Wpedantic

main01.exe: main01.cpp
	$(CXX) $(CXXFLAGS) main01.cpp -o main01.exe

run: main01.exe
	./main01.exe

clean:
	rm -v *.exe

.PHONY: run clean
