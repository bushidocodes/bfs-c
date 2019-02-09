#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/time.h>
#include <time.h>

#include "bfs.c"

int main()
{
    struct timeval start, end;
    double execution_times[TEST_RUNS];
    bool is_discovered[MAXV + 1];
    uint64_t has_parent[MAXV + 1];
    float avg = 0;
    float sum = 0;

    srand(time(NULL));

    /* Create a Graph */
    graph *g = malloc(sizeof(graph));
    initialize_graph(g, false);

    /* Feed in graph data from STDIN */
    read_graph(g, false);
    printf("Reading Graph from STDIN complete\n");

    /* Print the contents of the graph if desired */
    // print_graph(g);

    printf("Starting Tests\n");
    for (int i = 0; i < TEST_RUNS; i++)
    {
        printf("Running test %d out of %d\n", i + 1, TEST_RUNS);
        printf("Cleaning BFS Data Structures\n");

        for (uint64_t i = 1; i <= g->number_vertices; i++)
        {
            is_discovered[i] = false;
            has_parent[i] = 0;
        }
        // Select a random vertex as the root
        uint64_t root = rand() % (g->number_vertices - 1);
        printf("Starting BFS\n");

        // Execute and profile BFS
        gettimeofday(&start, NULL);
        bfs(g, root, is_discovered, has_parent);
        gettimeofday(&end, NULL);

        // Add execution_times of this iteration to array
        execution_times[i] = ((end.tv_sec - start.tv_sec) * 1e6 + (end.tv_usec - start.tv_usec)) * 1e-6;
    }

    /* Print the parents array to prove correctectness if desired */
    // printf("Printing last execution_times to prove correctness:\n");
    // print_parents(g, has_parent);

    for (int i = 0; i < TEST_RUNS; i++)
    {
        printf("Test %d: %fs\n", i + 1, execution_times[i]);
        sum += execution_times[i];
    }
    avg = sum / TEST_RUNS;
    printf("Averaged over %d iterations, Time: %fs\n", TEST_RUNS, avg);

    return 0;
}