#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "constants.h"
#include "graph.c"
#include "queue.c"

void process_vertex_early(uint64_t vertex);
void process_vertex_late(uint64_t vertex);
void process_edge(uint64_t source, uint64_t destination);
void initialize_search(graph *g);
void bfs(graph *g, uint64_t start);

/* BFS Stuff */
bool is_discovered[MAXV + 1];
uint64_t has_parent[MAXV + 1];

/* Initialize our data structures */
void initialize_search(graph *g)
{
    for (uint64_t i = 1; i <= g->number_vertices; i++)
    {
        is_discovered[i] = false;
        has_parent[i] = 0;
    }
}

void bfs(graph *g, uint64_t start)
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

/* Use recursion to reverse the ancestry chain defined by the has_parent data structures to find the shortest path */

void find_path(uint64_t start, uint64_t end, uint64_t parents[])
{
    /* Base Case: Roots are either expressed by being their own parent or being -1 depending on the graph implementation */
    if ((start == end) || (end == -1))
        printf("\n%lu", start);
    else
    {
        find_path(start, has_parent[end], has_parent);
        printf(" %lu", end);
    }
}

void print_parents(graph *g)
{
    for (uint64_t i = 1; i <= g->number_vertices; i++)
    {
        printf("%lu has parent %lu\n", i, has_parent[i]);
    }
}
