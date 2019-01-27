#include <mpi.h>
#include "aml.h"
#include "queue.c"

typedef struct edge
{
    unsigned long destination; /* destination vertex of edge a.k.a an adjascent vertex */
    struct edge *next;         /* next edge in list... I have to use the struct keyboard because this is recursive */
} edge;

// Our adjacency list is an array of linked lists called edges. The index of the edges array represents the id of the

typedef struct graph
{
    edge *edges[MAXV + 1];           /* adjacency info */
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

// Globals
int noProcesses, processId;
graph *g;
queue *next_queue;
bool is_discovered[MAXV + 1];
int has_parent[MAXV + 1];

void bfsHandler(int from, void *data, int sz);
void createGlobals();
void cleanGlobals();
void edgerecordHandler(int from, void *data, int sz);
void findneighborsHandler(int from, void *data, int sz);
void headerHandler(int from, void *data, int sz);
void initialize_graph(graph *g, bool directed);
void insert_edge(graph *g, unsigned long source, unsigned long destination, bool is_directed);
void print_graph(graph *g);
void processneighborHandler(int from, void *data, int sz);
void read_graph();
void read_graph_stdin(graph *g, bool is_directed);

void bfsHandler(int from, void *data, int sz)
{
    unsigned long *start = data;
    printf("%d sees start of %lu\n", processId, *start);
    int vertex;

    // Initialize queue
    queue *current_queue, *temp;
    current_queue = malloc(sizeof(queue));
    reset(current_queue, false);

    // And set to discovered
    printf("Length of queue is %lu\n", len(current_queue));
    enqueue(*start, current_queue);
    printf("Length of queue is %lu\n", len(current_queue));
    is_discovered[*start] = true;
    // And set self as parent
    has_parent[*start] = *start;

    while (len(current_queue) > 0)
    {
        vertex = dequeue(current_queue);
        printf("%d dequeued %lu\n", processId, vertex);
        // send AML
        edgerecord *record = malloc(sizeof(edgerecord));
        record->source = vertex;
        aml_send(record, 3, sizeof(edgerecord), vertex % noProcesses); /* findNeighborsHandler */

        if (len(current_queue) == 0)
        {
            aml_barrier();
            // Once the current queue is empty, join together next_queue and then rotate out levels
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
};

void createGlobals()
{
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);
    MPI_Comm_size(MPI_COMM_WORLD, &noProcesses);

    /* Initialize Graph */
    g = malloc(sizeof(graph));
    initialize_graph(g, false);

    /* Create Next Queue */

    next_queue = malloc(sizeof(queue));
    reset(next_queue, false);

    /* Register my AML Handlers */
    aml_register_handler(headerHandler, 1);
    aml_register_handler(edgerecordHandler, 2);
    aml_register_handler(findneighborsHandler, 3);
    aml_register_handler(processneighborHandler, 4);
    aml_register_handler(bfsHandler, 5);
};

void cleanGlobals()
{
    //TODO
}

void edgerecordHandler(int from, void *data, int sz)
{
    edgerecord *record = data;
    printf("%d inserting edge %lu %lu\n", processId, record->source, record->destination);
    insert_edge(g, record->source, record->destination, false);
}

void findneighborsHandler(int from, void *data, int sz)
{
    edgerecord *record = data;
    printf("%d %lu\n", processId, record->source);
    unsigned long adjacentVertex;
    edge *edgeLinkedList;
    edgeLinkedList = g->edges[record->source];
    while (edgeLinkedList != NULL)
    {
        record->destination = edgeLinkedList->destination;
        printf("Are these equal? %lu %lu\n", record->destination, edgeLinkedList->destination);
        aml_send(record, 4, sizeof(edgerecord), record->destination % noProcesses); /* processneighborHandler */
        edgeLinkedList = edgeLinkedList->next;
    }
};

void headerHandler(int from, void *data, int sz)
{
    header *newHeader = data;
    printf("%d setting num vertices to %lu\n", processId, newHeader->numVertices);
    g->number_vertices = newHeader->numVertices;
    g->number_edges = newHeader->numEdges;
}

void initialize_graph(graph *g, bool directed)
{
    for (unsigned long i = 0; i < MAXV; i++)
    {
        g->edges[i] = NULL;
    }
    g->number_vertices = 0;
    g->number_edges = 0;
    g->is_directed = directed;
}

// Inserts an edge from source to destination in the adjascency list of graph g. If the edge is not directed, it adds source -> destination and destination -> but only increments th edge count once.
void insert_edge(graph *g, unsigned long source, unsigned long destination, bool is_directed)
{
    edge *new_edge;                  /* temporary pointer */
    new_edge = malloc(sizeof(edge)); /* allocate edge storage */

    new_edge->destination = destination;

    new_edge->next = g->edges[source];

    g->edges[source] = new_edge;

    if (is_directed == false)
    {
        // We set directed to true in this call so we don't infinitely loop
        insert_edge(g, destination, source, true);
    }
    else
    {
        // By incrementing in this else block, we only increment one for undirected graphs
        // g->number_edges++;
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

void processneighborHandler(int from, void *data, int sz)
{
    edgerecord *record = data;
    printf("%d is checking %lu %lu\n", processId, record->source, record->destination);

    if (is_discovered[record->destination] == false)
    {
        enqueue(record->destination, next_queue);
        is_discovered[record->destination] = true;
        has_parent[record->destination] = record->source;
    }
};

void read_graph()
{
    printf("Read_graph %d %d\n", processId, noProcesses);

    edgerecord *newEdgerecord;
    FILE *fp;

    // Read the Header
    if (processId == 0)
    {
        header *newHeader = malloc(sizeof(header));
        newEdgerecord = malloc(sizeof(edgerecord));
        fp = fopen("/d/scratch/millionAsBinary", "r");
        fread(newHeader, sizeof(struct header), 1, fp);
        for (int i = 0; i < noProcesses; i++)
        {
            printf("Sending Stuff to %d\n", i);
            aml_send(newHeader, 1, sizeof(header), i); /* Send the header to each of the other processes */
        }
        free(newHeader);
    }
    aml_barrier();
    printf("%d has %lu\n", processId, g->number_edges);
    if (processId == 0)
    {
        printf("Reading from Disk\n");
        printf("%lu %llu\n", g->number_vertices, g->number_edges);
        for (unsigned long long i = 0; i < g->number_edges; i++)
        {
            printf("Reading %llu / %llu\n", i + 1, g->number_edges);
            printf("Dispatching to %llu\n", i % noProcesses);
            fread(newEdgerecord, sizeof(struct edgerecord), 1, fp);
            printf("New Edge Record %lu %lu\n", newEdgerecord->source, newEdgerecord->destination);
            aml_send(newEdgerecord, 2, sizeof(edgerecord), i % noProcesses); /* edgerecordHandler */
        }
        free(newEdgerecord);
    }
    aml_barrier();
}