CXX = g++
CXXFLAGS = -std=c++17

SRC = graphStates.cpp Heuristics_Solution.cpp
TARGETS = $(SRC:.cpp=)
TARGETS := $(addprefix output/, $(TARGETS))

all: run

run: $(TARGETS)
	for exe in $(TARGETS); do ./$$exe; done

output/%: %.cpp
	mkdir -p output
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f output/*