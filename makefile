CXX = g++
CXXFLAGS = -std=c++11 -Wall -O2
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system

SRC = main.cpp ComplexPlane.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = mandelbrot

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp ComplexPlane.h
	$(CXX) $(CXXFLAGS) -c $<

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)
