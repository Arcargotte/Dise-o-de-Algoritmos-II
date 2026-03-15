CXX = g++
CXXFLAGS = -std=c++17

TARGET = output/aco
SRC = aco.cpp

all: run

$(TARGET): $(SRC)
	mkdir -p output
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)