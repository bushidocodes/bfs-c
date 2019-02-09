#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "constants.h"
#include "graph.c"
#include "queue.c"

// void initialize_search(graph *g);
void bfs(graph *g, uint64_t start, bool is_discovered[MAXV + 1], uint64_t has_parent[MAXV + 1]);

void bfs(graph *g, uint64_t start, bool is_discovered[MAXV + 1], uint64_t has_parent[MAXV + 1])
{
    queue *q;
    uint64_t vertex;
    uint64_t neighbor_count = 0;

    // Initialize queue
    q = malloc(sizeof(queue));
    reset(q, false);

    // Add start to queue
    enqueue(start, q);

    // And set to discovered
    is_discovered[start] = true;
    // And set self as parent
    has_parent[start] = start;

    while (len(q) > 0)
    {
        vertex = dequeue(q);
        uint64_t neighbors[MAXV] = {UINT64_C(0)};
        neighbor_count = getNeighbors(g, vertex, neighbors);

        for (uint64_t i = 0; i < neighbor_count; i++)
        {
            // and then I need to make 60 - 65 atomic...
            if (is_discovered[neighbors[i]] == false)
            {
                enqueue(neighbors[i], q);
                is_discovered[neighbors[i]] = true;
                has_parent[neighbors[i]] = vertex;
            }
        }
    }
}

void print_parents(graph *g, uint64_t has_parent[MAXV + 1])
{
    for (uint64_t i = 1; i <= g->number_vertices; i++)
    {
        printf("%lu has parent %lu\n", i, has_parent[i]);
    }
}
