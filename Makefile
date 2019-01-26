## Simple C Makefile

## Define compiler and flags
CC=mpicc 
CXX=cc 
CCFLAGS=-g -std=c99 -Wall -Werror -lpthread -lm

all: build

makedist:
	mkdir -p dist

build-generator: makedist
	$(CC) ./src/generategraph.c -o ./dist/generategraph.out $(CCFLAGS)

build-bfs: makedist
	$(CC) ./src/main.c -o ./dist/bfs.out $(CCFLAGS)

build: build-bfs

run-generator: build-generator
	./dist/generategraph.out 16 16

run-bfs: build-bfs
	mpirun -n 4 ./dist/bfs.out
	# cat ./res/data.txt | ./dist/bfs.out

clean: 
	rm -f ./dist/*