SRC=src/bigint.cpp src/add.cpp src/sub.cpp src/mul.cpp src/div.cpp src/mod.cpp src/shift.cpp src/compare.cpp src/tests.cpp
OBJECTS=$(SRC:.cpp=.o)
CXXFLAGS=-std=c++11 -Iinclude -Wall -Wextra -Werror -g

all: tests.exe

tests.exe: $(OBJECTS)
	g++ -o tests.exe $(OBJECTS)
