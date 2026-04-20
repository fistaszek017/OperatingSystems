CC     = gcc
CFLAGS = -Wall -Wextra -O2
LIBS   = -lm

all: program1 program2

program1: program1.c
	$(CC) $(CFLAGS) -o program1 program1.c $(LIBS)

program2: program2.c
	$(CC) $(CFLAGS) -o program2 program2.c $(LIBS)

run: all
	./program2 &
	./program1

clean:
	rm -f program1 program2 /tmp/fifo_to_calc /tmp/fifo_to_result

.PHONY: all clean run