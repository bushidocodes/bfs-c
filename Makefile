## Distributed BFS — Makefile

CC     = mpicc
CFLAGS = -g -std=c17 -Wall -Wextra -I./src

## aml.c is third-party (Graph500); compile with fewer warnings
AML_CFLAGS = -g -std=c99 -I./src

BFS_SRCS = src/main.c src/globals.c src/bfs.c src/queue.c
BFS_OBJS = $(patsubst src/%.c,dist/%.o,$(BFS_SRCS))
AML_OBJ  = dist/aml.o

all: build

dist:
	mkdir -p dist

res:
	mkdir -p res

## Compile aml separately with relaxed flags (third-party code)
$(AML_OBJ): src/aml.c src/aml.h | dist
	$(CC) $(AML_CFLAGS) -c $< -o $@

## Compile project sources
dist/%.o: src/%.c | dist
	$(CC) $(CFLAGS) -c $< -o $@

build-bfs: $(AML_OBJ) $(BFS_OBJS)
	$(CC) $^ -o dist/bfs.out -lpthread -lm

build-generator: dist
	$(CC) $(CFLAGS) src/generategraph.c src/rand_uint64.c -o dist/generategraph.out -lm

build: build-bfs

run-generator: build-generator res
	./dist/generategraph.out 8 16

run-bfs: build-bfs
	mpirun -n 2 ./dist/bfs.out --mca orte_base_help_aggregate 0

run: run-generator run-bfs

clean:
	rm -f ./dist/*

.PHONY: all build build-bfs build-generator run run-bfs run-generator clean
