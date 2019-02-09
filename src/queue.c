#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "constants.h"

/* Array Based Queue */

typedef struct queue
{
    uint64_t data[MAXV]; /* Array that stores the queue */
    uint64_t start;      /* idx of the start of the queue */
    uint64_t end;        /* ids of the end of the queue (exclusive) */
} queue;

void enqueue(uint64_t x, queue *q)
{
    if (q->end >= MAXV)
    {
        printf("Error: Queue is full");
        /* TODO: Implement queue cleanup...*/
        return;
    }
    q->data[q->end - 1] = x;
    q->end++; // Not thread-safe... Not atomic
}

uint64_t dequeue(queue *q)
{
    if (q->start == q->end - 1)
    {
        printf("Error: Queue is empty\n");
        exit(99);
    }
    uint64_t result = q->data[q->start];
    q->start++;
    return result;
}

uint64_t len(queue *q)
{
    return (q->end - 1) - q->start;
}

void reset(queue *q, bool should_zero_data)
{
    if (should_zero_data)
    {
        for (uint64_t i = 0; i <= q->end; i++)
        {
            q->data[i] = 0;
        }
    }
    q->start = 0;
    q->end = 1;
}
