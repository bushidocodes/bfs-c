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

bool is_discovered[MAXV + 1];
int has_parent[MAXV + 1];

/* Initialize our data structures */
void initialize_search(graph *g)
{
    for (int i = 1; i <= g->number_vertices; i++)
    {
        is_discovered[i] = false;
        has_parent[-i] = -1;
    }
}

void bfs(graph *g, int start)
{
    int vertex;
    int adjacentVertex;
    edge *edgeLinkedList;

    // Initialize queue
    queue *current_queue, *next_queue, *temp;
    current_queue = malloc(sizeof(queue));
    reset(current_queue, false);
    next_queue = malloc(sizeof(queue));
    reset(next_queue, false);

    // Add start to queue
    printf("Generated Root Vertex %d\n", start);

    // And set to discovered
    enqueue(start, current_queue);
    is_discovered[start] = true;
    // And set self as parent
    has_parent[start] = start;

    while (len(current_queue) > 0)
    {
        vertex = dequeue(current_queue);
        edgeLinkedList = g->edges[vertex];
        while (edgeLinkedList != NULL)
        {
            adjacentVertex = edgeLinkedList->destination;
            // Round Robin this code between MPI nodes
            if (is_discovered[adjacentVertex] == false)
            {
                enqueue(adjacentVertex, next_queue);
                is_discovered[adjacentVertex] = true;
                has_parent[adjacentVertex] = vertex;
            }
            edgeLinkedList = edgeLinkedList->next;
        }
        if (len(current_queue) == 0)
        {
            // Once the current queue is empty, join together next_queue and is_discovered
            // And then rotate out levels
            if (len(next_queue) > 0)
            {
                temp = current_queue;
                current_queue = next_queue;
                reset(temp, false);
                next_queue = temp;
            }
        }
    }
    // Combine has_parent
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
