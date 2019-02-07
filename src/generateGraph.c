#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "constants.h"
#include "rand_uint64.c"

typedef struct header
{
    unsigned long numVertices;
    unsigned long long numEdges;
} header;

typedef struct edgerecord
{
    unsigned long source, destination;
} edgerecord;

int main(int argc, char *argv[])
{
    unsigned int edgeFactor, scale;
    header *newHeader;
    edgerecord *newEdgerecord;
    FILE *fp;

    if (argc == 1)
    {
        printf("You are missing the required scale integer\n");
        printf("This command takes the arguments generategraph SCALE EDGEFACTOR\n");
        exit(-1);
    }

    newHeader = malloc(sizeof(header));
    newEdgerecord = malloc(sizeof(edgerecord));
    fp = fopen(FILE_PATH, "w");

    edgeFactor = (argc == 3) ? atoi(argv[2]) : 16;

    printf("You entered %d arguments\n", argc - 1);

    scale = atoi(argv[1]);

    newHeader->numVertices = pow(2.0, scale);
    newHeader->numEdges = newHeader->numVertices * edgeFactor;

    printf("Scale of %d yields %lu vertices\n", scale, newHeader->numVertices);
    printf("Edge Factor of %d yields %llu edges\n", edgeFactor, newHeader->numEdges);
    printf("%lu %llu\n", newHeader->numVertices, newHeader->numEdges);
    // Write the header record to a binary file
    fwrite(newHeader, sizeof(struct header), 1, fp);
    srand(time(0));
    for (long long int i = 1; i <= newHeader->numEdges; i++)
    {
        printf("Writing %llu / %llu\n", i, newHeader->numEdges);
        newEdgerecord->source = rand_uint64() % newHeader->numVertices;
        newEdgerecord->destination = rand_uint64() % newHeader->numVertices;
        printf("%lu %lu\n", newEdgerecord->source, newEdgerecord->destination);
        // Write the edge record to a binary file
        fwrite(newEdgerecord, sizeof(struct edgerecord), 1, fp);
    }

    // Calculate number of nodes: 2^(scale int)
    // Calculate number of edges: #nodes * edgefactor
    // Open a File
    // Write the first line (#nodes #edges)
    // For Loop to generate a random connection between two nodes until complete
    // Write the lines out at a time to STDOUT
    // This allows us to pipe either to a file or another process
    fclose(fp);

    newHeader->numEdges = 0;
    newHeader->numVertices = 0;
    printf("Proving that state is wiped\n");
    printf("%lu %llu\n", newHeader->numVertices, newHeader->numEdges);

    // Now Re-open to Read
    fp = fopen(FILE_PATH, "r");
    fread(newHeader, sizeof(struct header), 1, fp);
    printf("Reading from Disk\n");
    printf("%lu %llu\n", newHeader->numVertices, newHeader->numEdges);
    for (long long int i = 1; i <= newHeader->numEdges; i++)
    {
        printf("Reading %llu / %llu\n", i, newHeader->numEdges);
        fread(newEdgerecord, sizeof(struct edgerecord), 1, fp);
        printf("%lu %lu\n", newEdgerecord->source, newEdgerecord->destination);
    }

    free(newHeader);
    free(newEdgerecord);
}

void write_graph(graph *g, bool is_directed)
{
    int m;    /* number of edges */
    int x, y; /* vertices in edge (x, y) */

    initialize_graph(g, is_directed);

    scanf("%d %d", &(g->number_vertices), &m);

    for (int i = 1; i <= m; i++)
    {
        scanf("%d %d", &x, &y);
        insert_edge(g, x, y, is_directed);
    }
}