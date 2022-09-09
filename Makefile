CC=g++
CFLAGS=-Werror -pedantic-errors -Wall -Wextra -Wpedantic -Wfloat-equal -Wundef -Wextra-semi -Wshadow -Wpointer-arith
CFLAGS+=-Wcast-align -Wstrict-overflow=5 -Wwrite-strings -Wcast-qual -Wswitch-default -Wduplicated-branches -Wduplicated-cond
CFLAGS+=-Wconversion -Wunreachable-code -Wnarrowing -Wredundant-decls -Wlogical-op -flto -fsanitize=address,undefined
LDFLAGS=-lsfml-window -lsfml-graphics -lsfml-system -lpthread
SOURCES=main.cpp decoration.cpp blocks.cpp Buffon.cpp integral.cpp Mandelbrot.cpp scattering.cpp series.cpp spigot.cpp Viete.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=project

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(EXECUTABLE) $(OBJECTS) $(LDFLAGS)

$(OBJECTS): $(SOURCES)
	$(CC) -c $(CFLAGS) $(SOURCES)

clean:
	rm $(OBJECTS) $(EXECUTABLE)