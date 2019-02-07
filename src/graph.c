#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

/**
 * Basic Graph Data Structure that uses Adjacency lists
 * Main parses text from STDIN. 
 * The two digits on the first line are number of nodes and number of edges
 * Each following line include two vertices forming an edge
 * The resulting graph is printed to console 
 **/

// This edge struct acts as a linked list node
typedef struct edge
{
    uint64_t destination; /* destination vertex of edge a.k.a an adjascent vertex */
    uint64_t weight;      /* edge weight */
    struct edge *next;    /* next edge in list... I have to use the struct keyboard because this is recursive */
} edge;

// Our adjacency list is an array of linked lists called edges. The index of the edges array represents the id of the
typedef struct graph
{
    edge *edges[MAXV + 1];     /* adjacency info */
    uint64_t degree[MAXV + 1]; /* outdegree of each vertex */
    uint64_t number_vertices;  /* number of vertices in graph */
    uint64_t number_edges;     /* number of edges in graph */
    bool is_directed;          /* Is the graph directed */
} graph;

void initialize_graph(graph *g, bool directed);
void read_graph(graph *g, bool directed);
void insert_edge(graph *g, uint64_t x, uint64_t y, bool directed);
void print_graph(graph *g);

void initialize_graph(graph *g, bool directed)
{
    for (uint64_t i = 1; i < MAXV; i++)
    {
        g->edges[i] = NULL;
        g->degree[i] = 0;
    }
    g->number_vertices = 0;
    g->number_edges = 0;
    g->is_directed = directed;
}

void read_graph(graph *g, bool is_directed)
{
    uint64_t m;    /* number of edges */
    uint64_t x, y; /* vertices in edge (x, y) */

    initialize_graph(g, is_directed);

    scanf("%lu %lu", &(g->number_vertices), &m);

    for (uint64_t i = 1; i <= m; i++)
    {
        scanf("%lu %lu", &x, &y);
        insert_edge(g, x, y, is_directed);
    }
}

// Inserts an edge from source to destination in the adjascency list of graph g. If the edge is not directed, it adds source -> destination and destination -> but only increments th edge count once. Interestingly, weight is not yet implemented. This function would likely need to be modified to support weights on our edges
void insert_edge(graph *g, uint64_t source, uint64_t destination, bool is_directed)
{
    edge *new_edge;                  /* temporary pointer */
    new_edge = malloc(sizeof(edge)); /* allocate edge storage */

    new_edge->weight = 1;
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