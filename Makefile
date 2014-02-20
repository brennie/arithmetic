SRC=src/bigint.cpp src/main.cpp
CXXFLAGS=-std=c++11 -Iinclude -Wall -Wextra -Werror

all: arithmetic.exe

arithmetic.exe: $(SRC)
	g++ $(CXXFLAGS) $(SRC) -o arithmetic.exe
