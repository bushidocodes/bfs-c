#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "constants.h"
#include "rand_uint64.c"

typedef struct header
{
    uint64_t numVertices;
    uint64_t numEdges;
} header;

typedef struct edgerecord
{
    uint64_t source, destination;
} edgerecord;

int main(int argc, char *argv[])
{
    unsigned int edgeFactor, scale;
    char filePath[100] = FILE_PATH;

    header *newHeader;
    edgerecord *newEdgerecord;
    FILE *fp;

    printf("You entered %d arguments\n", argc - 1);
    if (argc == 1)
    {
        printf("You are missing the required scale integer\n");
        printf("This command takes the arguments generategraph SCALE EDGEFACTOR\n");
        exit(-1);
    }
    scale = atoi(argv[1]);
    edgeFactor = (argc == 3) ? atoi(argv[2]) : 16;
    if (argc == 4)
    {
        strncpy(&filePath, argv[3], 100);
        printf("Writing Edgelist to %s\n", filePath);
    }

    newHeader = malloc(sizeof(header));
    newEdgerecord = malloc(sizeof(edgerecord));
    // Open a File
    fp = fopen(filePath, "w");

    // Calculate number of vertices: 2^(scale int)
    newHeader->numVertices = pow(2.0, scale);
    // Calculate number of edges: #nodes * edgefactor
    newHeader->numEdges = newHeader->numVertices * edgeFactor;

    printf("Scale of %d yields %lu vertices\n", scale, newHeader->numVertices);
    printf("Edge Factor of %d yields %lu edges\n", edgeFactor, newHeader->numEdges);
    // Writing Edge List Header to Text file (#nodes #edges)
    fprintf(fp, "%lu %lu\n", newHeader->numVertices, newHeader->numEdges);

    srand(time(0));
    for (uint64_t i = 1; i <= newHeader->numEdges; i++)
    {
        // For Loop to generate a random connection between two nodes until complete
        newEdgerecord->source = (rand_uint64() % newHeader->numVertices) + 1;
        newEdgerecord->destination = (rand_uint64() % newHeader->numVertices) + 1;
        // Write the edge record to a text file
        fprintf(fp, "%lu %lu\n", newEdgerecord->source, newEdgerecord->destination);
    }

    fclose(fp);
    free(newHeader);
    free(newEdgerecord);
}
