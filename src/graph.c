#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "constants.h"

// This edge struct acts as a linked list node
typedef struct edge
{
    uint64_t destination; /* destination vertex of edge a.k.a an adjascent vertex */
    uint64_t weight;      /* edge weight */
    struct edge *next;    /* next edge in list... I have to use the struct keyboard because this is recursive */
} edge;

typedef struct graph
{
    edge *edges[MAXV + 1];     /* Intermediate Linked List Representation before CSR Conversion */
    uint64_t degree[MAXV + 1]; /* outdegree of each vertex */
    uint64_t number_vertices;  /* number of vertices in graph */
    uint64_t number_edges;     /* number of edges in graph */
    bool is_directed;          /* Is the graph directed */
} graph;

void initialize_graph(graph *g, bool directed);
void read_graph(graph *g, bool directed);
void insert_edge(graph *g, uint64_t x, uint64_t y, bool directed);
void print_graph(graph *g);

uint64_t getDegree(graph *g, uint64_t vertex);
uint64_t getNeighbors(graph *g, uint64_t source, uint64_t results[]);

// TODO: Figure out why the result of this function differs from the count returned by getNeighbors
uint64_t getDegree(graph *g, uint64_t vertex)
{
    return g->degree[vertex];
}

void initialize_graph(graph *g, bool directed)
{
    for (uint64_t i = 1; i < MAXV; i++)
    {
        g->edges[i] = NULL;
    }
    memset(g->degree, 0, (MAXV + 1) * sizeof(uint64_t));
    g->number_vertices = 0;
    g->number_edges = 0;
    g->is_directed = directed;
    printf("Graph Initialization complete\n");
}

void read_graph(graph *g, bool is_directed)
{
    uint64_t edge_count;          /* number of edges */
    uint64_t source, destination; /* vertices in edge (source, destination) */

    scanf("%lu %lu", &(g->number_vertices), &edge_count);

    for (uint64_t i = 1; i <= edge_count; i++)
    {
        scanf("%lu %lu", &source, &destination);
        insert_edge(g, source, destination, is_directed);
    }
}

// Inserts an edge from source to destination in the adjascency list of graph g. If the edge is not directed, it adds source -> destination and destination -> but only increments th edge count once.
void insert_edge(graph *g, uint64_t source, uint64_t destination, bool is_directed)
{
    edge *new_edge = malloc(sizeof(edge)); /* temporary pointer */
    new_edge->destination = destination;
    new_edge->next = g->edges[source];
    g->edges[source] = new_edge;
    g->degree[source]++;

    if (is_directed == false)
    {
        // We set direted to true in this call so we don't infinitely loop
        insert_edge(g, destination, source, true);
    }
    else
    {
        // By incrementing in this else block, we only increment one for undirected graphs
        g->number_edges++;
    }
}

// Takes a pointer to an array, returns the length of the array
uint64_t getNeighbors(graph *g, uint64_t source, uint64_t results[])
{
    uint64_t results_length = 0;
    for (edge *currentEdge = g->edges[source]; currentEdge->next != NULL; currentEdge = currentEdge->next)
    {
        results[results_length] = currentEdge->destination;
        results_length++;
    }
    return results_length;
}

void print_graph(graph *g)
{
    edge *p; /* temporary pointer */

    for (uint64_t i = 1; i <= g->number_vertices; i++)
    {
        printf("%lu: ", i);
        p = g->edges[i];
        while (p != NULL)
        {
            printf(" %lu", p->destination);
            p = p->next;
        }
        printf("\n");
    }
}