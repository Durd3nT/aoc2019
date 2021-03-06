CXX = g++-9
CXXFLAGS += -std=c++17
CXXFLAGS += -Wall -Wextra -Wpedantic


main12.exe: main12.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

run: main12.exe
	./main12.exe

clean:
	rm -v *.exe

.PHONY: run clean
