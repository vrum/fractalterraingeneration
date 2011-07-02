CC=g++
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=main.cpp SimplexNoise.cpp colorizer.cpp color.cpp shader.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=SimplexNoise

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
