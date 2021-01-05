.POSIX:
.PHONY: run clean
CC= gcc
CFLAGS= -g -Wall -Wextra -Wpedantic -Wuninitialized -Wundef -Wcast-align -Wstrict-overflow=2 -Wwrite-strings -Wno-format-nonliteral
OBJECTS= controller.o processor.o
LFLAGS= -lasan

all: $(OBJECTS)
	$(CC) -o controller controller.o $(LFLAGS)
	$(CC) -o processor processor.o $(LFLAGS)

controller.o: controller.c
	$(CC) $(CFLAGS) -c controller.c

processor.o: processor.c
	$(CC) $(CFLAGS) -c processor.c

clean:
	rm *.o controller processor
