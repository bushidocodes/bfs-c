#include <stdio.h>
#include "bfs.h"

/* Recursively traces parent chain to print the shortest path from start to end */
void find_path(unsigned long start, unsigned long end, unsigned long parents[])
{
    if ((start == end) || (end == (unsigned long)-1))
        printf("\n%lu", start);
    else
    {
        find_path(start, parents[end], parents);
        printf(" %lu", end);
    }
}

void print_parents(graph *g)
{
    for (unsigned long i = 0; i < g->number_vertices; i++)
    {
        printf("%lu has parent %lu\n", i, has_parent[i]);
    }
}
