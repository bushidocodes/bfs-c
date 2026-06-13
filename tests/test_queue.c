/*
 * Unit tests for queue reset() with should_zero_data = true (issue #12).
 * Verifies written elements are zeroed and no out-of-bounds write occurs.
 * Build with -fsanitize=address to catch OOB writes from the original bug.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "queue.h"

static int pass_count = 0;
static int fail_count = 0;

static void check(const char *name, int ok)
{
    if (ok) { printf("[PASS] %s\n", name); pass_count++; }
    else     { printf("[FAIL] %s\n", name); fail_count++; }
}

/* Use static allocation — queue is 8 MB, too large for the stack */
static queue q;

int main(void)
{
    /* Test 1: reset() with should_zero_data=true clears all written elements */
    reset(&q, false);
    enqueue(10, &q);
    enqueue(20, &q);
    enqueue(30, &q);

    /* data[0]=10, data[1]=20, data[2]=30, end=4 */
    reset(&q, true);

    check("reset clears data[0]", q.data[0] == 0);
    check("reset clears data[1]", q.data[1] == 0);
    check("reset clears data[2]", q.data[2] == 0);
    check("reset restores start=0", q.start == 0);
    check("reset restores end=1",   q.end   == 1);
    check("queue is empty after reset", len(&q) == 0);

    /* Test 2: reset() on a freshly initialised (empty) queue is safe */
    reset(&q, false);
    reset(&q, true);  /* end=1 here; original code would write data[0] and data[1] */
    check("reset on empty queue: start=0", q.start == 0);
    check("reset on empty queue: end=1",   q.end   == 1);

    /* Test 3: enqueue/dequeue/reset round-trip */
    reset(&q, false);
    enqueue(42, &q);
    unsigned long v = dequeue(&q);
    check("dequeue returns correct value", v == 42);
    check("queue empty after dequeue", len(&q) == 0);
    reset(&q, true);
    check("reset after dequeue: end=1", q.end == 1);

    /* Test 4: full capacity — all MAXV slots must be usable (issue #27).
     * The off-by-one bug let only MAXV-1 items be stored before a spurious
     * "Queue is full". Enqueue exactly MAXV items and verify all are kept. */
    reset(&q, false);
    for (unsigned long i = 0; i < (unsigned long)MAXV; i++)
        enqueue(i, &q);
    check("queue holds full MAXV items", len(&q) == MAXV);
    check("last slot data[MAXV-1] was written", q.data[MAXV - 1] == (unsigned long)(MAXV - 1));
    check("dequeue returns first item after full fill", dequeue(&q) == 0);
    check("dequeue returns second item after full fill", dequeue(&q) == 1);

    printf("\nResults: %d passed, %d failed out of %d tests\n",
           pass_count, fail_count, pass_count + fail_count);
    return fail_count ? 1 : 0;
}
