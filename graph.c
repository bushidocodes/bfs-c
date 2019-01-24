#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#define MAXV 1000 /* maximum number of vertices */

/**
 * Basic Graph Data Structure that uses Adjacency lists
 * Main parses text from STDIN. 
 * The two digits on the first line are number of nodes and number of edges
 * Each following line include two vertices forming an edge
 * The resulting graph is printed to console 
 **/

typedef struct
{
    int y;                 /* adjacency info */
    int weight;            /* edge weight */
    struct edgenode *next; /* next edge in list... I have to use the struct keyboard because this is recursive */
} edgenode;

typedef struct
{
    edgenode *edges[MAXV + 1]; /* adjascency info */
    int degree[MAXV + 1];      /* outdegree of each vertex */
    int nvertices;             /* number of vertices in graph */
    int nedges;                /* number of edges in graph */
    bool directed;             /* Is the graph directed */
} graph;

void initialize_graph(graph *g, bool directed);
void read_graph(graph *g, bool directed);
void insert_edge(graph *g, int x, int y, bool directed);
void print_graph(graph *g);

void initialize_graph(graph *g, bool directed)
{
    int i; /* counter */
    g->nvertices = 0;
    g->nedges = 0;
    g->directed = directed;

    for (i = 1; i < MAXV; i++)
    {
        g->degree[i] = 0;
        g->edges[i] = NULL;
    }
}

void read_graph(graph *g, bool directed)
{
    int i;    /* counter */
    int m;    /* number of edges */
    int x, y; /* vertices in edge (x, y) */

    initialize_graph(g, directed);

    scanf("%d %d", &(g->nvertices), &m);

    for (i = 1; i <= m; i++)
    {
        scanf("%d %d", &x, &y);
        insert_edge(g, x, y, directed);
    }
}

void insert_edge(graph *g, int x, int y, bool directed)
{
    edgenode *p;                  /* temporary pointer */
    p = malloc(sizeof(edgenode)); /* allocate edgenode storage */

    p->weight = NULL;
    p->y = y;
    p->next = g->edges[x];

    g->edges[x] = p;
    g->degree[x]++;

    if (directed == false)
    {
        insert_edge(g, y, x, true);
    }
    else
    {
        g->nedges++;
    }
}

void print_graph(graph *g)
{
    int i;       /* counter */
    edgenode *p; /* temporary pointer */

    for (i = 1; i <= g->nvertices; i++)
    {
        printf("%d: ", i);
        p = g->edges[i];
        while (p != NULL)
        {
            printf(" %d", p->y);
            p = p->next;
        }
        printf("\n");
    }
}

int main()
{
    graph *g;
    g = malloc(sizeof(graph));
    read_graph(g, false);
    print_graph(g);
    return 0;
}