CC=g++
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=main.cpp CellNoise.cpp equation.cpp sortedList.cpp color.cpp colorizer.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=CellNoise

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
