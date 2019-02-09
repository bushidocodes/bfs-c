#include <limits.h>
#include <omp.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "constants.h"
#include "graph.c"
#include "queue.c"

// void initialize_search(graph *g);
void bfs(graph *g, uint64_t start, uint64_t has_parent[MAXV + 1]);

void bfs(graph *g, uint64_t start, uint64_t has_parent[MAXV + 1])
{
    uint8_t level[MAXV] = {UINT8_MAX};
    uint8_t current_level = 0;
    uint64_t neighbor_count, neighbor_count1 = 0;

    // And set self as parent
    has_parent[start] = start;
    level[start] = 0;
#pragma omp parallel for
    for (uint64_t current_vertex = 1; current_vertex < g->number_vertices; current_vertex++)
    {
        if (level[current_vertex] == current_level)
        {
            uint64_t neighbors[MAXV] = {UINT64_C(0)};
            neighbor_count = getNeighbors(g, current_vertex, neighbors);

            for (uint64_t i = 0; i < neighbor_count; i++)
            {
                if (level[i] == UINT8_MAX)
                {
                    level[i] = current_level + 1;
                    has_parent[i] = current_vertex;
                }
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
