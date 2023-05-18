CC=gcc
CFLAGS=-Wall -g 

all: gen.o main

gen.o: gen.c
	$(CC) $(CFLAGS) -c $^ -o $@

libbmp.o: libbmp.c
	$(CC) $(CFLAGS) -c $^ -o $@

main: main.c gen.o libbmp.o
	$(CC) $(CFLAGS) $^ -o $@ 

clean:
	rm *.o main