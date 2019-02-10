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
void bfs(graph *g, uint32_t start, uint32_t has_parent[]);

void bfs(graph *g, uint32_t start, uint32_t has_parent[])
{
    uint8_t *level = malloc((g->number_vertices + 1) * sizeof(uint8_t));
    memset(level, UINT8_MAX, (g->number_vertices + 1) * sizeof(uint8_t));
    uint8_t current_level = 0;
    uint64_t neighbor_count = 0;
    bool should_advance = true;

    // And set self as parent
    has_parent[start] = start;
    level[start] = 0;
    while (should_advance)
    {
        should_advance = false;
#pragma omp parallel for
        for (uint32_t current_vertex = 1; current_vertex < g->number_vertices; current_vertex++)
        {
            if (level[current_vertex] == current_level)
            {
                neighbor_count = getDegree(g, current_vertex);
                // TODO: I inexplicably need to pad this number... Why?
                uint32_t *neighbors = malloc((getDegree(g, current_vertex) + 200) * sizeof(uint32_t));
                getNeighbors(g, current_vertex, neighbors);

                for (uint32_t i = 0; i < neighbor_count; i++)
                {
                    if (level[neighbors[i]] == UINT8_MAX)
                    {
                        should_advance = true;
                        level[neighbors[i]] = current_level + 1;
                        has_parent[neighbors[i]] = current_vertex;
                    }
                }
                free(neighbors);
            }
        }
        current_level++;
    }
    free(level);
}

void print_parents(graph *g, uint32_t has_parent[])
{
    for (uint32_t i = 1; i <= g->number_vertices; i++)
    {
        printf("%u has parent %u\n", i, has_parent[i]);
    }
}
