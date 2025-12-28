CXX = g++
CXXFLAGS = -Wall -Wextra -fPIC -std=c++11 -g
LDFLAGS = -L. -lmandelbrot
TARGET = main
LIB = libmandelbrot.so
SRC = mandelbrot.cpp
HDR = mandelbrot.hpp
MAIN = main.cpp

all: $(LIB) $(TARGET)

$(LIB): $(SRC) $(HDR)
	$(CXX) $(CXXFLAGS) -shared -o $@ $(SRC)

$(TARGET): $(MAIN) $(HDR)
	$(CXX) $(CXXFLAGS) -o $@ $(MAIN) -L. -lmandelbrot

run: all
	LD_LIBRARY_PATH=. ./main
clean:
	rm -f $(TARGET) $(LIB)

# -Wall        : Enable most common compiler warnings
# -Wextra      : Enable additional (stricter) compiler warnings
# -fPIC        : Generate position-independent code (required for shared libraries)
# -L.          : Add the current directory to the library search path
# -lmandelbrot : Link against libmandelbrot.so 
# -shared      : Build a shared (dynamic) library instead of an executable	