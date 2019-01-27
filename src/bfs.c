#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "constants.h"

void process_vertex_early(int vertex);
void process_vertex_late(int vertex);
void process_edge(int source, int destination);
void initialize_search(graph *g);

/* Use recursion to reverse the ancestry chain defined by the has_parent data structures to find the shortest path */

void find_path(int start, int end, int parents[])
{
    /* Base Case: Roots are either expressed by being their own parent or being -1 depending on the graph implementation */
    if ((start == end) || (end == -1))
        printf("\n%d", start);
    else
    {
        find_path(start, has_parent[end], has_parent);
        printf(" %d", end);
    }
}

void print_parents(graph *g)
{
    for (int i = 0; i < g->number_vertices; i++)
    {
        printf("%d has parent %d\n", i, has_parent[i]);
    }
}
