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
    uint8_t level[MAXV + 1];
    memset(level, UINT8_MAX, (MAXV + 1) * sizeof(uint8_t));
    uint8_t current_level = 0;
    uint64_t neighbor_count = 0;
    bool should_advance = true;

    // And set self as parent
    has_parent[start] = start;
    level[start] = 0;
    // #pragma omp parallel for
    while (should_advance)
    {
        should_advance = false;
        for (uint64_t current_vertex = 1; current_vertex < g->number_vertices; current_vertex++)
        {
            if (level[current_vertex] == current_level)
            {
                uint64_t neighbors[MAXV] = {UINT64_C(0)};
                neighbor_count = getNeighbors(g, current_vertex, neighbors);

                for (uint64_t i = 0; i < neighbor_count; i++)
                {
                    if (level[neighbors[i]] == UINT8_MAX)
                    {
                        should_advance = true;
                        level[neighbors[i]] = current_level + 1;
                        has_parent[neighbors[i]] = current_vertex;
                    }
                }
            }
        }
        current_level++;
    }
}

void print_parents(graph *g, uint64_t has_parent[MAXV + 1])
{
    for (uint64_t i = 1; i <= g->number_vertices; i++)
    {
        printf("%lu has parent %lu\n", i, has_parent[i]);
    }
}
