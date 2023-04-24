# the compiler: gcc for C program, define as g++ for C++
CC = mpic++

# compiler flags:
#  -g     - this flag adds debugging information to the executable file
#  -Wall  - this flag is used to turn on most compiler warnings
CFLAGS  = --prefix /usr/local/share/OpenMPI
# The build target 
TARGET = parsplit
CXXOPTS = cxxopts.hpp

all: $(TARGET)

$(TARGET): $(TARGET).cpp
	$(CC) -o $(TARGET) $(TARGET).cpp

clean:

