#include <stdlib.h>
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
    unsigned long destination; /* destination vertex of edge a.k.a an adjascent vertex */
    struct edge *next;         /* next edge in list... I have to use the struct keyboard because this is recursive */
} edge;

// Our adjacency list is an array of linked lists called edges. The index of the edges array represents the id of the
typedef struct graph
{
    edge *edges[MAXV + 1]; /* adjacency info */
    // int degree[MAXV + 1];  /* outdegree of each vertex */
    unsigned long number_vertices;   /* number of vertices in graph */
    unsigned long long number_edges; /* number of edges in graph */
    bool is_directed;                /* Is the graph directed */
} graph;

typedef struct header
{
    unsigned long numVertices;
    unsigned long long numEdges;
} header;

typedef struct edgerecord
{
    unsigned long source, destination;
} edgerecord;

void initialize_graph(graph *g, bool directed);
void read_graph(graph *g, bool directed);
void insert_edge(graph *g, unsigned long source, unsigned long destination, bool directed);
void print_graph(graph *g);

void initialize_graph(graph *g, bool directed)
{
    for (int i = 1; i < MAXV; i++)
    {
        g->edges[i] = NULL;
        // g->degree[i] = 0;
    }
    g->number_vertices = 0;
    g->number_edges = 0;
    g->is_directed = directed;
}

void read_graph_stdin(graph *g, bool is_directed)
{
    unsigned long long m; /* number of edges */
    unsigned long x, y;   /* vertices in edge (x, y) */

    initialize_graph(g, is_directed);

    scanf("%lu %llu", &(g->number_vertices), &m);

    for (unsigned long i = 1; i <= m; i++)
    {
        scanf("%lu %lu", &x, &y);
        insert_edge(g, x, y, is_directed);
    }
}

void read_graph(graph *g, bool is_directed)
{
    header *newHeader;
    edgerecord *newEdgerecord;
    FILE *fp;

    newHeader = malloc(sizeof(header));
    newEdgerecord = malloc(sizeof(edgerecord));

    initialize_graph(g, is_directed);

    // Now Re-open to Read
    fp = fopen("/d/scratch/millionAsBinary", "r");
    fread(newHeader, sizeof(struct header), 1, fp);
    printf("Reading from Disk\n");
    printf("%lu %llu\n", newHeader->numVertices, newHeader->numEdges);
    g->number_vertices = newHeader->numVertices;
    printf("%lu\n", g->number_vertices);
    for (unsigned long long i = 1; i <= newHeader->numEdges; i++)
    {
        printf("Reading %llu / %llu\n", i, newHeader->numEdges);
        fread(newEdgerecord, sizeof(struct edgerecord), 1, fp);
        insert_edge(g, newEdgerecord->source, newEdgerecord->destination, is_directed);
    }

    free(newHeader);
    free(newEdgerecord);
}

// Inserts an edge from source to destination in the adjascency list of graph g. If the edge is not directed, it adds source -> destination and destination -> but only increments th edge count once.
void insert_edge(graph *g, unsigned long source, unsigned long destination, bool is_directed)
{
    edge *new_edge;                  /* temporary pointer */
    new_edge = malloc(sizeof(edge)); /* allocate edge storage */

    new_edge->destination = destination;

    new_edge->next = g->edges[source];

    g->edges[source] = new_edge;
    // // g->degree[source]++;

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

    for (unsigned long i = 1; i <= g->number_vertices; i++)
    {
        printf("%lu: ", i);
        p = g->edges[i];
        while (p != NULL)
        {
            printf("%lu", p->destination);
            p = p->next;
        }
        printf("\n");
    }
}