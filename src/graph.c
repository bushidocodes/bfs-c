#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "constants.h"

// This edge struct acts as a linked list node
typedef struct edge
{
    uint32_t destination; /* destination vertex of edge a.k.a an adjascent vertex */
    struct edge *next;    /* next edge in list... I have to use the struct keyboard because this is recursive */
} edge;

typedef struct graph
{
    uint32_t number_vertices; /* number of vertices in graph */
    uint64_t number_edges;    /* number of edges in graph */
    bool is_directed;         /* Is the graph directed */
    uint32_t *IA;             /* Cumulative count of non zero values excluding the row at this index */
    uint32_t *JA;             /* Actual Column Indices of non zero values. Use IA first... */

} graph;

void initialize_graph(graph *g, bool directed);
void read_graph(graph *g, bool directed);
void insert_edge(graph *g, edge *edges[], uint32_t x, uint32_t y, bool directed);
void print_graph(graph *g);
void build_csr(graph *g, edge *edges[]);

uint64_t getDegree(graph *g, uint32_t vertex);
void getNeighbors(graph *g, uint32_t source, uint32_t results[]);

uint64_t getDegree(graph *g, uint32_t vertex)
{
    return g->IA[vertex + 1] - g->IA[vertex];
}

// Takes a pointer to an array and writes out all neighbors from CSR
void getNeighbors(graph *g, uint32_t source, uint32_t results[])
{
    for (uint32_t idx = 0; idx + g->IA[source] < g->IA[source + 1]; idx++)
    {
        results[idx] = g->JA[idx + g->IA[source]];
    }
}

void initialize_graph(graph *g, bool directed)
{
    g->number_vertices = 0;
    g->number_edges = 0;
    g->is_directed = directed;
    printf("Graph Initialization complete\n");
}

void read_graph(graph *g, bool is_directed)
{
    edge **edges = malloc((MAXV + 1) * sizeof(edge)); /* Array of Pointers used to buffer adjasency list as itermediate representation before building CSR*/
    uint64_t edge_count;                              /* number of edges */
    uint32_t source, destination;                     /* vertices in edge (source, destination) */

    scanf("%u %lu", &(g->number_vertices), &edge_count);
    printf("Reading Edge List\n");
    for (uint64_t i = 1; i <= edge_count; i++)
    {
        if (i % 10000000 == 0)
        {
            printf("Read %lu / %lu Edges\n", i, edge_count);
        }
        scanf("%u %u", &source, &destination);
        insert_edge(g, edges, source, destination, is_directed);
    }
    build_csr(g, edges);
    // Now I can just free all the memory in the adjacency list
    for (uint64_t i = 1; i < (sizeof(edges) / sizeof(&edges[0])); i++)
    {
        if (edges[i] != NULL)
        {

            edge *current = edges[i];
            while (current->next != NULL)
            {
                edge *deleteMe = current;
                current = current->next;
                free(deleteMe);
            }
            free(current);
            edges[i] = NULL;
        }
    }
    free(edges);
}

void build_csr(graph *g, edge *edges[])
{
    printf("Starting build_csr\n");
    g->IA = malloc((g->number_edges + 2) * sizeof(uint32_t)); /* I might be off by one here */

    // Mark the 0 positions as Evil to force a crash to ensure I'm indexing starting at 1 to be consistent
    g->IA[0] = 666;

    g->IA[1] = 0;
    // I'm just multiplying by 2 here because the number_edges is only incremented one for the opposing edges on a non-DiGraph. I should really only store non-directed edges once.
    g->JA = malloc((g->number_edges * 2) * sizeof(uint32_t)); // "Column Indices"
    printf("Starting CSR conversion\n");
#pragma omp parallel for
    for (uint32_t current_vertex = 1; current_vertex < g->number_vertices; current_vertex++)
    {
        uint64_t edge_idx = 0;
        if (current_vertex % 10000 == 0)
        {
            printf("Read %u / %u Vertices\n", current_vertex, g->number_vertices);
        }
        for (edge *currentEdge = edges[current_vertex]; currentEdge->next != NULL; currentEdge = currentEdge->next)
        {
            g->JA[edge_idx] = currentEdge->destination;
            edge_idx++;
        }
        g->IA[current_vertex + 1] = edge_idx;
    }

    for (uint32_t current_vertex = 1; current_vertex < g->number_vertices; current_vertex++)
    {
        g->IA[current_vertex + 1] += g->IA[current_vertex];
    }
}

// Inserts an edge from source to destination in the adjacency list of graph g. If the edge is not directed, it adds source -> destination and destination -> but only increments th edge count once.
void insert_edge(graph *g, edge *edges[], uint32_t source, uint32_t destination, bool is_directed)
{
    edge *new_edge = malloc(sizeof(edge)); /* temporary pointer */
    new_edge->destination = destination;
    new_edge->next = edges[source];
    edges[source] = new_edge;

    if (is_directed == false)
    {
        // We set direted to true in this call so we don't infinitely loop
        insert_edge(g, edges, destination, source, true);
    }
    else
    {
        // By incrementing in this else block, we only increment one for undirected graphs
        g->number_edges++;
    }
}

void print_graph(graph *g)
{
    for (uint32_t current_vertex = 1; current_vertex <= g->number_vertices; current_vertex++)
    {
        printf("%u: ", current_vertex);
        for (uint64_t idx = 0; idx + g->IA[current_vertex] < g->IA[current_vertex + 1]; current_vertex++)
        {
            printf(" %u", g->JA[idx + g->IA[current_vertex]]);
        }
        printf("\n");
        //     edge *p; /* temporary pointer */

        //     for (uint64_t i = 1; i <= g->number_vertices; i++)
        //     {
        //         printf("%lu: ", i);
        //         p = g->edges[i];
        //         while (p != NULL)
        //         {
        //             printf(" %lu", p->destination);
        //             p = p->next;
        //         }
        //         printf("\n");
    }
}