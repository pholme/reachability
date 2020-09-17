SRC = .
CFLAGS = -Ofast -march=native
LDFLAGS = -Ofast -march=native
CC = gcc

OBJ1 = o/reachability.o o/read.o

all : reachability

reachability: $(OBJ1)
	$(CC) $(LDFLAGS) -o $@ $^ -lm

o/reachability.o : $(SRC)/reachability.c $(SRC)/reachability.h $(SRC)/Makefile
	$(CC) $(CFLAGS) -c $(SRC)/reachability.c -o $@

o/read.o : $(SRC)/read.c $(SRC)/reachability.h $(SRC)/Makefile
	$(CC) $(CFLAGS) -c $(SRC)/read.c -o $@
