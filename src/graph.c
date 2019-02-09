#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "constants.h"
#include "bitarray.c"

/**
 * Basic Graph Data Structure that uses Adjacency lists
 * Main parses text from STDIN. 
 * The two digits on the first line are number of nodes and number of edges
 * Each following line include two vertices forming an edge
 * The resulting graph is printed to console 
 **/

typedef struct graph
{
    word_t edges[MAXV + 1][(MAXV + 1) / sizeof(word_t)]; /* adjacency matrix represented by array of bit arrays */
    uint64_t degree[MAXV + 1];                           /* outdegree of each vertex */
    uint64_t number_vertices;                            /* number of vertices in graph */
    uint64_t number_edges;                               /* number of edges in graph */
    bool is_directed;                                    /* Is the graph directed */
} graph;

void initialize_graph(graph *g, bool directed);
void read_graph(graph *g, bool directed);
void insert_edge(graph *g, uint64_t x, uint64_t y, bool directed);
void print_graph(graph *g);

uint64_t getDegree(graph *g, uint64_t vertex);
uint64_t getNeighbors(graph *g, uint64_t source, uint64_t results[]);

uint64_t getDegree(graph *g, uint64_t vertex)
{
    return g->degree[vertex];
}

// This initialization takes a while because the matrix is huge
void initialize_graph(graph *g, bool directed)
{
    // Using memset to zero out an array
    memset(g->edges, 0, (MAXV + 1) * ((MAXV + 1) / 8));
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
    set_bit(g->edges[source], destination);

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
    for (uint64_t destination = 1; destination <= g->number_vertices; destination++)
    {
        if (get_bit(g->edges[source], destination) == true)
        {
            results[results_length] = destination;
            results_length++;
        }
    }
    return results_length;
}

void print_graph(graph *g)
{
    for (uint64_t i = 1; i <= g->number_vertices; i++) // TODO: Why is this numbered from 1???
    {
        printf("%lu: ", i);
        for (uint64_t j = 1; j <= g->number_vertices; j++)
        {
            if (get_bit(g->edges[i], j))
            {
                printf("%lu, ", j);
            }
        }
        printf("\n");
    }
}