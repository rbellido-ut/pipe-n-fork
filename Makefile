COMPILER = g++
EXECUTABLE = pipe-n-fork
SOURCE = pipe-fork.cpp
FLAGS = -W -Wall
LIBRARIES = -lpthread

pipe-fork:
	$(COMPILER) $(FLAGS) -o $(EXECUTABLE) $(SOURCE) $(LIBRARIES)

run:
	./$(EXECUTABLE)

clean:
	rm $(EXECUTABLE)
