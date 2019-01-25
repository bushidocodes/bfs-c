## Simple C Makefile

## Define compiler and flags
CC=cc 
CXX=cc 
CCFLAGS=-g -std=c99 -Wall -Werror -lpthread

all: build

makedist:
	mkdir -p dist

build: makedist
	$(CC) ./src/main.c -o ./dist/a.out $(CCFLAGS)

run: build
	cat ./res/data.txt | ./dist/a.out

clean: 
	rm -f ./dist/*