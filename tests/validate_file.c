/*
 * Standalone graph-file validator (no MPI).
 * Exit 0: file is valid (all vertices within bounds).
 * Exit 1: bounds violation detected.
 * Exit 2: I/O or usage error.
 *
 * Replicates the validation logic added to globals.c for issue #4.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAXV 1000000

typedef struct { unsigned long numVertices; unsigned long long numEdges; } header;
typedef struct { unsigned long source, destination; } edgerecord;

typedef struct edge { unsigned long destination; struct edge *next; } edge;
typedef struct {
    edge *edges[MAXV + 1];
    unsigned long number_vertices;
    unsigned long long number_edges;
    bool is_directed;
} graph;

static graph g;

static void insert_edge(unsigned long source, unsigned long destination, bool is_directed)
{
    if (source > MAXV || destination > MAXV) {
        fprintf(stderr, "insert_edge: vertex out of bounds (source=%lu, destination=%lu, MAXV=%d)\n",
                source, destination, MAXV);
        exit(1);
    }
    edge *e = malloc(sizeof(edge));
    if (!e) { perror("malloc"); exit(2); }
    e->destination = destination;
    e->next = g.edges[source];
    g.edges[source] = e;
    if (!is_directed)
        insert_edge(destination, source, true);
}

int main(int argc, char *argv[])
{
    if (argc != 2) { fprintf(stderr, "usage: validate_file <graph-file>\n"); return 2; }

    FILE *fp = fopen(argv[1], "rb");
    if (!fp) { perror("fopen"); return 2; }

    header h;
    if (fread(&h, sizeof h, 1, fp) != 1) { fclose(fp); return 2; }

    if (h.numVertices > MAXV) {
        fprintf(stderr, "graph file numVertices %lu exceeds MAXV %d\n",
                h.numVertices, MAXV);
        fclose(fp);
        return 1;
    }

    g.number_vertices = h.numVertices;
    g.number_edges    = h.numEdges;

    for (unsigned long long i = 0; i < h.numEdges; i++) {
        edgerecord r;
        if (fread(&r, sizeof r, 1, fp) != 1) { fclose(fp); return 2; }
        insert_edge(r.source, r.destination, false);
    }
    fclose(fp);
    return 0;
}
