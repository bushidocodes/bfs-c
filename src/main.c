#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <mpi.h>
#include "aml.h"
#include "aml.c"
#include "constants.h"
#include "globals.c"
#include "bfs.c"

int main(int argc, char *argv[])
{
    /* Problem Vars */
    unsigned long start;
    // Initialize queue
    // queue *current_queue, *temp;
    // current_queue = malloc(sizeof(queue));
    unsigned long vertex;
    // reset(current_queue, false);

    /* MPI Setup */
    aml_init(&argc, &argv); //includes MPI_Init inside
    createGlobals();
    aml_barrier();
    read_graph();

    // Generate a Random Start Vertex and begin BFS
    srand(time(0));
    printf("# Vertices: %lu\n", g->number_vertices);
    start = rand() % (g->number_vertices - 1);
    printf("%d generated random start of %lu\n", processId, start);
    edgerecord *record = malloc(sizeof(edgerecord));
    record->destination = start;
    record->source = start;
    if (processId == 0)
    {
        aml_send(record, 4, sizeof(edgerecord), record->destination % noProcesses); /* processneighborHandler */
    }

    aml_barrier();
    sleep(1);
    aml_barrier();
    bool should_loop = true;
    int retries = 3;
    while (should_loop)
    {
        while (len(current_queue) > 0)
        {
            vertex = dequeue(current_queue);
            printf("%d dequeued %lu\n", processId, vertex);
            // send AML
            edgerecord *record = malloc(sizeof(edgerecord));
            record->source = vertex;
            aml_send(record, 3, sizeof(edgerecord), vertex % noProcesses); /* findNeighborsHandler */
        }
        // aml_barrier();
        if (len(current_queue) == 0 && len(next_queue) > 0)
        {
            printf("%d Flipping queues\n", processId);
            temp = current_queue;
            current_queue = next_queue;
            reset(temp, false);
            next_queue = temp;
            retries = 3;
        }
        else
        {
            aml_barrier();
            if (retries == 0)
                should_loop = false;
            else
            {
                printf("Empty frontier... Sleeping for 1s just in case...\n");
                sleep(0.1);
                retries--;
            }
            // I suspect I might might need to add some retries and sleeps here because aml_barrier does not seem to be working above as I otherwise would expect
        }
    }
    // If this were working properly, I would need to reduce the has_parents data structure
    printf("%d END\n", processId);
    cleanGlobals();
    // aml_finalize(); //includes MPI_Finalize()... This hangs inexplicably...
    return 0;
}