#include <stdio.h>
#include "queue.h"

void enqueue(unsigned long x, queue *q)
{
    if (q->end >= MAXV)
    {
        printf("Error: Queue is full");
        return;
    }
    q->data[q->end - 1] = x;
    q->end++;
}

unsigned long dequeue(queue *q)
{
    if (q->start == q->end - 1)
    {
        printf("Error: Queue is empty\n");
        return (unsigned long)-1;
    }
    unsigned long result = q->data[q->start];
    q->start++;
    return result;
}

int len(queue *q)
{
    return (q->end - 1) - q->start;
}

void reset(queue *q, bool should_zero_data)
{
    if (should_zero_data)
    {
        for (int i = 0; i < q->end - 1; i++)
        {
            q->data[i] = 0;
        }
    }
    q->start = 0;
    q->end = 1;
}
