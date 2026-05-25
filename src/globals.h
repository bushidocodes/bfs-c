#pragma once
#include <stdbool.h>
#include "constants.h"
#include "queue.h"

typedef enum {
    HANDLER_HEADER           = 1,
    HANDLER_EDGE_RECORD      = 2,
    HANDLER_FIND_NEIGHBORS   = 3,
    HANDLER_PROCESS_NEIGHBOR = 4,
} HandlerID;

typedef struct edge {
    unsigned long destination;
    struct edge *next;
} edge;

typedef struct graph {
    edge *edges[MAXV + 1];
    unsigned long number_vertices;
    unsigned long long number_edges;
    bool is_directed;
} graph;

typedef struct header {
    unsigned long numVertices;
    unsigned long long numEdges;
} header;

typedef struct edgerecord {
    unsigned long source, destination;
} edgerecord;

/* Global state — defined in globals.c */
extern int noProcesses, processId;
extern graph *g;
extern queue *current_queue, *temp, *next_queue;
extern bool is_discovered[MAXV + 1];
extern int has_parent[MAXV + 1];

void createGlobals(void);
void cleanGlobals(void);
void edgerecordHandler(int from, void *data, int sz);
void findneighborsHandler(int from, void *data, int sz);
void headerHandler(int from, void *data, int sz);
void initialize_graph(graph *g, bool directed);
void insert_edge(graph *g, unsigned long source, unsigned long destination, bool is_directed);
void print_graph(graph *g);
void processneighborHandler(int from, void *data, int sz);
void read_graph(void);
