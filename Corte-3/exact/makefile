CXX = g++
CXXFLAGS = -std=c++17

TARGET = output/exact_solution
SRC = Exact_Solution.cpp

all: run

$(TARGET): $(SRC)
	mkdir -p output
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)