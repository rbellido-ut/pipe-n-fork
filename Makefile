COMPILER = g++
EXECUTABLE = pipe-n-fork
MAINSOURCE = pipe-fork.cpp
UTILSOURCE = pipe-fork-utils.cpp
FLAGS = -W -Wall
LIBRARIES = -lpthread
OBJECTS = pipe-fork-utils.o pipe-fork.o

pipe-fork: $(OBJECTS)
	$(COMPILER) $(FLAGS) -o $(EXECUTABLE) $(OBJECTS) $(LIBRARIES)

run:
	./$(EXECUTABLE)

clean:
	rm -f $(EXECUTABLE) *.o

pipe-fork-utils.o:
	$(COMPILER) -c $(UTILSOURCE)

pipe-fork.o:
	$(COMPILER) -c $(MAINSOURCE)
