CC=g++
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=CellNoise.cpp color.cpp main.cpp 
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=CellNoise

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
