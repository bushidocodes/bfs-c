#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include "aml.h"
#include "globals.h"

#define FILE_PATH "./res/graph"

/* Global definitions (declared extern in globals.h) */
int noProcesses, processId;
graph *g;
queue *current_queue, *temp, *next_queue;
bool is_discovered[MAXV + 1];
unsigned long has_parent[MAXV + 1];

void createGlobals(void)
{
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);
    MPI_Comm_size(MPI_COMM_WORLD, &noProcesses);

    g = malloc(sizeof(graph));
    if (g == NULL) { fprintf(stderr, "malloc failed for graph\n"); MPI_Abort(MPI_COMM_WORLD, 1); }
    initialize_graph(g, false);

    current_queue = malloc(sizeof(queue));
    if (current_queue == NULL) { fprintf(stderr, "malloc failed for current_queue\n"); MPI_Abort(MPI_COMM_WORLD, 1); }
    reset(current_queue, false);

    next_queue = malloc(sizeof(queue));
    if (next_queue == NULL) { fprintf(stderr, "malloc failed for next_queue\n"); MPI_Abort(MPI_COMM_WORLD, 1); }
    reset(next_queue, false);

    aml_register_handler(headerHandler,          HANDLER_HEADER);
    aml_register_handler(edgerecordHandler,       HANDLER_EDGE_RECORD);
    aml_register_handler(findneighborsHandler,    HANDLER_FIND_NEIGHBORS);
    aml_register_handler(processneighborHandler,  HANDLER_PROCESS_NEIGHBOR);
}

void cleanGlobals(void)
{
    printf("Clean globals...");
    for (unsigned long i = 0; i <= MAXV; i++)
    {
        edge *e = g->edges[i];
        while (e != NULL)
        {
            edge *next = e->next;
            free(e);
            e = next;
        }
    }
    free(g);
    free(current_queue);
    free(next_queue);
}

void edgerecordHandler(int from, void *data, int sz)
{
    (void)from; (void)sz;
    edgerecord record;
    memcpy(&record, data, sizeof record);
    printf("%d inserting edge %lu %lu\n", processId, record.source, record.destination);
    insert_edge(g, record.source, record.destination, false);
}

void findneighborsHandler(int from, void *data, int sz)
{
    (void)from; (void)sz;
    edgerecord record;
    memcpy(&record, data, sizeof record);
    printf("%d %lu\n", processId, record.source);
    if (record.source > MAXV) {
        fprintf(stderr, "findneighborsHandler: source %lu exceeds MAXV %d\n",
                record.source, MAXV);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    edge *edgeLinkedList = g->edges[record.source];
    while (edgeLinkedList != NULL)
    {
        record.destination = edgeLinkedList->destination;
        aml_send(&record, HANDLER_PROCESS_NEIGHBOR, sizeof(edgerecord),
                 record.destination % noProcesses);
        edgeLinkedList = edgeLinkedList->next;
    }
}

void headerHandler(int from, void *data, int sz)
{
    (void)from; (void)sz;
    header h;
    memcpy(&h, data, sizeof h);
    printf("%d setting num vertices to %lu\n", processId, h.numVertices);
    g->number_vertices = h.numVertices;
    g->number_edges = h.numEdges;
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

void insert_edge(graph *g, unsigned long source, unsigned long destination, bool is_directed)
{
    if (source > MAXV || destination > MAXV) {
        fprintf(stderr, "insert_edge: vertex out of bounds (source=%lu, destination=%lu, MAXV=%d)\n",
                source, destination, MAXV);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    edge *new_edge = malloc(sizeof(edge));
    if (new_edge == NULL) { fprintf(stderr, "malloc failed for edge\n"); MPI_Abort(MPI_COMM_WORLD, 1); }

    new_edge->destination = destination;
    new_edge->next = g->edges[source];
    g->edges[source] = new_edge;

    if (!is_directed)
    {
        /* Pass true so the reverse direction doesn't recurse again */
        insert_edge(g, destination, source, true);
    }
}

void print_graph(graph *g)
{
    for (unsigned long i = 1; i <= g->number_vertices; i++)
    {
        printf("%lu: ", i);
        edge *p = g->edges[i];
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
    (void)from; (void)sz;
    edgerecord record;
    memcpy(&record, data, sizeof record);
    printf("%d is checking %lu %lu\n", processId, record.source, record.destination);

    if (record.destination > MAXV) {
        fprintf(stderr, "processneighborHandler: destination %lu exceeds MAXV %d\n",
                record.destination, MAXV);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    if (!is_discovered[record.destination])
    {
        enqueue(record.destination, next_queue);
        is_discovered[record.destination] = true;
        has_parent[record.destination] = record.source;
    }
}

void read_graph(void)
{
    printf("Read_graph %d %d\n", processId, noProcesses);

    edgerecord *newEdgerecord;
    FILE *fp;

    if (processId == 0)
    {
        header *newHeader = malloc(sizeof(header));
        if (newHeader == NULL) { fprintf(stderr, "malloc failed\n"); MPI_Abort(MPI_COMM_WORLD, 1); }
        newEdgerecord = malloc(sizeof(edgerecord));
        if (newEdgerecord == NULL) { fprintf(stderr, "malloc failed\n"); MPI_Abort(MPI_COMM_WORLD, 1); }

        fp = fopen(FILE_PATH, "r");
        if (fp == NULL) { fprintf(stderr, "fopen failed: %s\n", FILE_PATH); MPI_Abort(MPI_COMM_WORLD, 1); }

        fread(newHeader, sizeof(struct header), 1, fp);
        if (newHeader->numVertices > MAXV) {
            fprintf(stderr, "graph file numVertices %lu exceeds MAXV %d\n",
                    newHeader->numVertices, MAXV);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        for (int i = 0; i < noProcesses; i++)
        {
            printf("Sending Stuff to %d\n", i);
            aml_send(newHeader, HANDLER_HEADER, sizeof(header), i);
        }
        free(newHeader);
    }

    aml_barrier();
    printf("%d has %llu\n", processId, g->number_edges);

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
            aml_send(newEdgerecord, HANDLER_EDGE_RECORD, sizeof(edgerecord), i % noProcesses);
        }
        free(newEdgerecord);
        fclose(fp);
    }

    aml_barrier();
}
