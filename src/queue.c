#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "constants.h"

/* Array Based Queue */

typedef struct queue
{
    int data[MAXV]; /* Array that stores the queue */
    int start;      /* idx of the start of the queue */
    int end;        /* ids of the end of the queue (exclusive) */
} queue;

void enqueue(int x, queue *q)
{
    if (q->end >= MAXV)
    {
        printf("Error: Queue is full");
        /* TODO: Implement queue cleanup...*/
        return;
    }
    q->data[q->end - 1] = x;
    q->end++;
}

int dequeue(queue *q)
{
    if (q->start == q->end - 1)
    {
        printf("Error: Queue is empty\n");
        return -1;
    }
    int result = q->data[q->start];
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
        for (int i = 0; i <= q->end; i++)
        {
            q->data[i] = 0;
        }
    }
    q->start = 0;
    q->end = 1;
}

// TODO: Turn this into unit test fodder
// int main(void)
// {
//     queue *q;
//     q = malloc(sizeof(queue));
//     reset(q, false); /* I can use reset to initialize the start and end ints */
//     enqueue(1, q);
//     printf("Length: %d\n", len(q));
//     enqueue(2, q);
//     printf("Length: %d\n", len(q));
//     enqueue(3, q);
//     printf("Length: %d\n", len(q));
//     enqueue(4, q);
//     printf("Length: %d\n", len(q));
//     enqueue(5, q);
//     printf("Length: %d\n", len(q));
//     printf("Dequeued %d\n", dequeue(q));
//     printf("Length: %d\n", len(q));
//     printf("Dequeued %d\n", dequeue(q));
//     printf("Length: %d\n", len(q));

//     printf("Dequeued %d\n", dequeue(q));
//     printf("Length: %d\n", len(q));

//     printf("Dequeued %d\n", dequeue(q));
//     printf("Length: %d\n", len(q));

//     free(q);
//     return 0;
// }