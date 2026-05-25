#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "rand_uint64.h"

#define FILE_PATH "./res/graph"

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
    if (argc == 1)
    {
        printf("You are missing the required scale integer\n");
        printf("This command takes the arguments generategraph SCALE EDGEFACTOR\n");
        exit(-1);
    }

    header *newHeader = malloc(sizeof(header));
    if (newHeader == NULL) { fprintf(stderr, "malloc failed\n"); return 1; }
    edgerecord *newEdgerecord = malloc(sizeof(edgerecord));
    if (newEdgerecord == NULL) { fprintf(stderr, "malloc failed\n"); return 1; }

    unsigned int edgeFactor = (argc == 3) ? (unsigned int)atoi(argv[2]) : 16;
    unsigned int scale = (unsigned int)atoi(argv[1]);

    newHeader->numVertices = (unsigned long)pow(2.0, scale);
    newHeader->numEdges = newHeader->numVertices * edgeFactor;

    printf("Scale of %u yields %lu vertices\n", scale, newHeader->numVertices);
    printf("Edge Factor of %u yields %llu edges\n", edgeFactor, newHeader->numEdges);

    FILE *fp = fopen(FILE_PATH, "w");
    if (fp == NULL) { fprintf(stderr, "fopen failed: %s\n", FILE_PATH); return 1; }

    fwrite(newHeader, sizeof(header), 1, fp);
    srand(time(0));
    for (unsigned long long i = 1; i <= newHeader->numEdges; i++)
    {
        printf("Writing %llu / %llu\n", i, newHeader->numEdges);
        newEdgerecord->source = rand_uint64() % newHeader->numVertices;
        newEdgerecord->destination = rand_uint64() % newHeader->numVertices;
        fwrite(newEdgerecord, sizeof(edgerecord), 1, fp);
    }
    fclose(fp);

    /* Verify by re-reading */
    newHeader->numEdges = 0;
    newHeader->numVertices = 0;
    fp = fopen(FILE_PATH, "r");
    if (fp == NULL) { fprintf(stderr, "fopen failed on verify\n"); return 1; }
    fread(newHeader, sizeof(header), 1, fp);
    printf("Verify — read back: %lu vertices, %llu edges\n",
           newHeader->numVertices, newHeader->numEdges);
    for (unsigned long long i = 1; i <= newHeader->numEdges; i++)
    {
        fread(newEdgerecord, sizeof(edgerecord), 1, fp);
        printf("%lu %lu\n", newEdgerecord->source, newEdgerecord->destination);
    }
    fclose(fp);

    free(newHeader);
    free(newEdgerecord);
    return 0;
}
