/*
 * Unit tests for the queue (issues #12 and #27), using the Unity framework.
 * Build with -fsanitize=address to catch OOB writes from the reset() bug.
 */
#include "unity.h"
#include "queue.h"
#include "constants.h"

/* Use static allocation — queue is 8 MB, too large for the stack */
static queue q;

void setUp(void)
{
    reset(&q, false);
}

void tearDown(void)
{
}

/* Issue #12: reset() with should_zero_data=true clears all written elements */
static void test_reset_clears_written_elements(void)
{
    enqueue(10, &q);
    enqueue(20, &q);
    enqueue(30, &q);

    /* data[0]=10, data[1]=20, data[2]=30, end=4 */
    reset(&q, true);

    TEST_ASSERT_EQUAL_UINT(0, q.data[0]);
    TEST_ASSERT_EQUAL_UINT(0, q.data[1]);
    TEST_ASSERT_EQUAL_UINT(0, q.data[2]);
    TEST_ASSERT_EQUAL_INT(0, q.start);
    TEST_ASSERT_EQUAL_INT(1, q.end);
    TEST_ASSERT_EQUAL_INT(0, len(&q));
}

/* Issue #12: reset() on a freshly initialised (empty) queue must not write OOB */
static void test_reset_on_empty_queue_is_safe(void)
{
    /* end=1 here; the original code would write data[0] and data[1] */
    reset(&q, true);
    TEST_ASSERT_EQUAL_INT(0, q.start);
    TEST_ASSERT_EQUAL_INT(1, q.end);
}

static void test_enqueue_dequeue_reset_roundtrip(void)
{
    enqueue(42, &q);
    TEST_ASSERT_EQUAL_UINT(42, dequeue(&q));
    TEST_ASSERT_EQUAL_INT(0, len(&q));
    reset(&q, true);
    TEST_ASSERT_EQUAL_INT(1, q.end);
}

/* Issue #27: all MAXV slots must be usable. The off-by-one bug let only
 * MAXV-1 items be stored before a spurious "Queue is full". */
static void test_full_capacity_holds_maxv_items(void)
{
    for (unsigned long i = 0; i < (unsigned long)MAXV; i++)
        enqueue(i, &q);

    TEST_ASSERT_EQUAL_INT(MAXV, len(&q));
    TEST_ASSERT_EQUAL_UINT((unsigned long)(MAXV - 1), q.data[MAXV - 1]);
    TEST_ASSERT_EQUAL_UINT(0, dequeue(&q));
    TEST_ASSERT_EQUAL_UINT(1, dequeue(&q));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_reset_clears_written_elements);
    RUN_TEST(test_reset_on_empty_queue_is_safe);
    RUN_TEST(test_enqueue_dequeue_reset_roundtrip);
    RUN_TEST(test_full_capacity_holds_maxv_items);
    return UNITY_END();
}
