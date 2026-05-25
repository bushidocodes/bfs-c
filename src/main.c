#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <mpi.h>
#include "aml.h"
#include "globals.h"

int main(int argc, char *argv[])
{
    unsigned long start;
    unsigned long vertex;

    aml_init(&argc, &argv);
    createGlobals();
    aml_barrier();
    read_graph();

    srand(time(0));
    printf("# Vertices: %lu\n", g->number_vertices);
    start = rand() % (g->number_vertices - 1);
    printf("%d generated random start of %lu\n", processId, start);

    edgerecord *record = malloc(sizeof(edgerecord));
    if (record == NULL) { fprintf(stderr, "malloc failed\n"); MPI_Abort(MPI_COMM_WORLD, 1); }
    record->destination = start;
    record->source = start;
    if (processId == 0)
    {
        aml_send(record, HANDLER_PROCESS_NEIGHBOR, sizeof(edgerecord),
                 record->destination % noProcesses);
    }
    free(record);

    aml_barrier();
    usleep(1000000); /* 1 second — give messages time to propagate */
    aml_barrier();

    bool should_loop = true;
    int retries = 3;
    while (should_loop)
    {
        while (len(current_queue) > 0)
        {
            vertex = dequeue(current_queue);
            printf("%d dequeued %lu\n", processId, vertex);
            edgerecord *msg = malloc(sizeof(edgerecord));
            if (msg == NULL) { fprintf(stderr, "malloc failed\n"); MPI_Abort(MPI_COMM_WORLD, 1); }
            msg->source = vertex;
            aml_send(msg, HANDLER_FIND_NEIGHBORS, sizeof(edgerecord), vertex % noProcesses);
            free(msg);
        }

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
                printf("Empty frontier... Sleeping for 100ms just in case...\n");
                usleep(100000); /* 100 ms */
                retries--;
            }
        }
    }

    printf("%d END\n", processId);
    cleanGlobals();
    /* aml_finalize() hangs — MPI_Finalize is called implicitly on exit */
    return 0;
}
