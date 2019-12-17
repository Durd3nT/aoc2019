CXX = g++
CXXFLAGS += -std=c++14
CXXFLAGS += -Wall -Wextra -Wpedantic

main02.exe: main02.cpp
	$(CXX) $(CXXFLAGS) main02.cpp -o main02.exe

run: main02.exe
	./main02.exe

clean:
	rm -v *.exe

.PHONY: run clean
