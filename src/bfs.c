#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "constants.h"
#include "graph.c"
#include "queue.c"

void process_vertex_early(int vertex);
void process_vertex_late(int vertex);
void process_edge(int source, int destination);
void initialize_search(graph *g);
void bfs(graph *g, int start);

/* BFS Stuff */

bool is_processed[MAXV + 1];
bool is_discovered[MAXV + 1];
int has_parent[MAXV + 1];

/* Initialize our data structures */
void initialize_search(graph *g)
{
    for (int i = 1; i <= g->number_vertices; i++)
    {
        is_processed[i] = is_discovered[i] = false;
        has_parent[-i] = -1;
    }
}

void bfs(graph *g, int start)
{
    queue *q;
    int vertex;
    int adjacentVertex;
    edge *edgeLinkedList;

    // Initialize queue
    q = malloc(sizeof(queue));
    reset(q, false);

    // Add start to queue
    printf("Generated Root Vertex %d\n", start);
    enqueue(start, q);

    // And set to discovered
    is_discovered[start] = true;
    // And set self as parent
    has_parent[start] = start;

    while (len(q) > 0)
    {
        vertex = dequeue(q);
        printf("Dequeued %d\n", vertex);
        process_vertex_early(vertex);
        is_processed[vertex] = true;
        edgeLinkedList = g->edges[vertex];
        while (edgeLinkedList != NULL)
        {
            adjacentVertex = edgeLinkedList->destination;
            if ((is_processed[adjacentVertex] == false) || g->is_directed)
            {
                process_edge(vertex, adjacentVertex);
            }
            if (is_discovered[adjacentVertex] == false)
            {
                enqueue(adjacentVertex, q);
                is_discovered[adjacentVertex] = true;
                has_parent[adjacentVertex] = vertex;
            }
            edgeLinkedList = edgeLinkedList->next;
        }
        process_vertex_late(vertex);
    }
}

void process_vertex_early(int vertex)
{
    printf("Processed vertex %d\n", vertex);
}
void process_vertex_late(int vertex) {}

void process_edge(int source, int destination)
{
    printf("processed edge (%d, %d)\n", source, destination);
}

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

