SRC=src/bigint.cpp src/tests.cpp
CXXFLAGS=-std=c++11 -Iinclude -Wall -Wextra -Werror

all: tests.exe

tests.exe: $(SRC)
	g++ $(CXXFLAGS) $(SRC) -o tests.exe
