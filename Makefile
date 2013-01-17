COMPILER = gcc
EXECUTABLE = pipe-n-fork
SOURCE = main.c
FLAGS = -W -Wall

compile:
	$(COMPILER) $(FLAGS) -o $(EXECUTABLE) $(SOURCE)

default: compile run

run:
	./$(EXECUTABLE)

clean:
	rm pipe-n-fork
