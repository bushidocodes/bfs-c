## Simple C Makefile

## Define compiler and flags
CC=gcc 
CXX=gcc 
CCFLAGS= -std=c11 -Wall -fopenmp 

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
	./dist/generategraph.out 5 4 ./res/5x4_edgelist.txt
	./dist/generategraph.out 6 16 ./res/6x16_edgelist.txt
	./dist/generategraph.out 16 16 ./res/16x16_edgelist.txt
	# ./dist/generategraph.out 26 16 ./res/26x16_edgelist.txt

run-bfs: build-bfs
	# mpirun -n 2 ./dist/bfs.out --mca orte_base_help_aggregate 0
	cat ./res/5x4_edgelist.txt | ./dist/bfs.out > ./res/5x4_results.txt
	cat ./res/6x16_edgelist.txt | ./dist/bfs.out > ./res/6x16_results.txt
	cat ./res/16x16_edgelist.txt | ./dist/bfs.out > ./res/16x16_results.txt
	# cat ./res/26x16_edgelist.txt | ./dist/bfs.out > ./res/26x16_results.txt

run: run-generator run-bfs

clean: 
	rm -f ./dist/*