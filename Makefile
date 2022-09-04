CC=g++
CFLAGS=-c -Werror -pedantic-errors -Wall -Wextra -Wpedantic -Wfloat-equal -Wundef -Wextra-semi -Wshadow -Wpointer-arith -Wcast-align -Wstrict-overflow=5 -Wwrite-strings -Wcast-qual -Wswitch-default -Wduplicated-branches -Wduplicated-cond -Wconversion -Wunreachable-code -Wnarrowing -Wredundant-decls -Wlogical-op
LDFLAGS=-lsfml-window -lsfml-graphics -lsfml-system -lpthread -fsanitize=address
SOURCES=main.cpp decoration.cpp blocks.cpp Buffon.cpp integral.cpp Mandelbrot.cpp scattering.cpp series.cpp spigot.cpp Viete.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=project

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) -o $(EXECUTABLE) $(OBJECTS) $(LDFLAGS)

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

decoration.o: decoration.cpp
	$(CC) $(CFLAGS) decoration.cpp

blocks.o: blocks.cpp
	$(CC) $(CFLAGS) blocks.cpp

Buffon.o: Buffon.cpp
	$(CC) $(CFLAGS) Buffon.cpp

integral.o: integral.cpp
	$(CC) $(CFLAGS) integral.cpp

Mandelbrot.o: Mandelbrot.cpp
	$(CC) $(CFLAGS) Mandelbrot.cpp

scattering.o: scattering.cpp
	$(CC) $(CFLAGS) scattering.cpp

series.o: series.cpp
	$(CC) $(CFLAGS) series.cpp

spigot.o: spigot.cpp
	$(CC) $(CFLAGS) spigot.cpp

Viete.o: Viete.cpp
	$(CC) $(CFLAGS) Viete.cpp

clean:
	rm -rf *.o project