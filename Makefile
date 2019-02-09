## Simple C Makefile

## Define compiler and flags
CC=gcc 
CXX=gcc 
CCFLAGS=-g -std=c11 -Wall -fopenmp -lpthread -lm -lrt

all: build

makeres:
	mkdir -p res

makedist:
	mkdir -p dist

build-generator: makedist
	$(CC) ./src/generategraph.c -o ./dist/generategraph.out $(CCFLAGS)

build-bfs: makedist
	$(CC) ./src/main.c -o ./dist/bfs.out $(CCFLAGS)

build: build-bfs

run-generator: build-generator makeres
	./dist/generategraph.out 16 16

run-bfs: build-bfs
	# mpirun -n 2 ./dist/bfs.out --mca orte_base_help_aggregate 0
	cat ./res/data.txt | ./dist/bfs.out

run: run-generator run-bfs

clean: 
	rm -f ./dist/*