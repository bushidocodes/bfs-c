#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <mpi.h>
#include "aml.h"
#include "globals.h"
#include "rand_uint64.h"
#include "bfs.h"

int main(int argc, char *argv[])
{
    unsigned long start;
    unsigned long vertex;

    aml_init(&argc, &argv);
    createGlobals();
    aml_barrier();
    read_graph();

    if (g->number_vertices < 1) {
        fprintf(stderr, "error: graph has %lu vertices; need at least 1\n", g->number_vertices);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    if (processId == 0) {
        srand(time(0));
        start = rand_uint64() % g->number_vertices;
    }
    MPI_Bcast(&start, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
    printf("# Vertices: %lu\n", g->number_vertices);
    printf("%d using start vertex %lu\n", processId, start);

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

    aml_barrier(); /* ensure root vertex is in next_queue before BFS loop */

    while (true)
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

        aml_barrier(); /* flush all in-flight messages for this BFS level */

        int local_len = len(next_queue);
        int global_len = 0;
        MPI_Allreduce(&local_len, &global_len, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
        if (global_len == 0)
            break;

        printf("%d Flipping queues\n", processId);
        temp = current_queue;
        current_queue = next_queue;
        reset(temp, false);
        next_queue = temp;
    }

    printf("%d END\n", processId);

    /* Merge distributed has_parent[] views, then report the BFS tree on rank 0 */
    MPI_Allreduce(MPI_IN_PLACE, has_parent, (int)g->number_vertices,
                  MPI_UNSIGNED_LONG, MPI_MAX, MPI_COMM_WORLD);
    if (processId == 0)
    {
        printf("BFS tree from start vertex %lu:\n", start);
        print_parents(g);
    }

    cleanGlobals();
    aml_finalize();
    return 0;
}
