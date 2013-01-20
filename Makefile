COMPILER = g++
EXECUTABLE = pipe-n-fork
SOURCE = pipe-fork.cpp
FLAGS = -W -Wall

compile:
	$(COMPILER) $(FLAGS) -o $(EXECUTABLE) $(SOURCE)

default: compile run

run:
	./$(EXECUTABLE)

clean:
	rm pipe-n-fork
