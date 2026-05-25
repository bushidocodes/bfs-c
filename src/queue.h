#pragma once
#include <stdbool.h>
#include "constants.h"

typedef struct queue {
    unsigned long data[MAXV];
    int start;
    int end;
} queue;

void enqueue(unsigned long x, queue *q);
unsigned long dequeue(queue *q);
int len(queue *q);
void reset(queue *q, bool should_zero_data);
