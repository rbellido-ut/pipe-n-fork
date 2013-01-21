COMPILER = g++
EXECUTABLE = pipe-n-fork
SOURCE = pipe-fork.cpp
FLAGS = -W -Wall
LIBRARIES = -lpthread

compile:
	$(COMPILER) $(FLAGS) -o $(EXECUTABLE) $(SOURCE) $(LIBRARIES)

default: compile run

run:
	./$(EXECUTABLE)

clean:
	rm pipe-n-fork
