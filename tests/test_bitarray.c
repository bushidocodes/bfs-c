/*
 * Unit tests for bitarray set_bit / get_bit / clear_bit (issue #15),
 * using the Unity framework. Build with -fsanitize=address to catch OOB writes.
 */
#include <string.h>
#include "unity.h"
#include "bitarray.h"
#include "constants.h"

/* Array sized for MAXV+1 bits, matching globals.c */
static word_t bits[BITS_TO_WORDS(MAXV + 1)];

void setUp(void)
{
    memset(bits, 0, sizeof bits);
}

void tearDown(void)
{
}

static void test_fresh_array_has_no_bits_set(void)
{
    TEST_ASSERT_EQUAL_INT(0, get_bit(bits, 0));
    TEST_ASSERT_EQUAL_INT(0, get_bit(bits, 31));
    TEST_ASSERT_EQUAL_INT(0, get_bit(bits, 32));
    TEST_ASSERT_EQUAL_INT(0, get_bit(bits, MAXV));
}

static void test_set_get_roundtrip(void)
{
    set_bit(bits, 0);
    TEST_ASSERT_EQUAL_INT(1, get_bit(bits, 0));

    set_bit(bits, 31);
    TEST_ASSERT_EQUAL_INT(1, get_bit(bits, 31));

    set_bit(bits, 32); /* first bit of next word */
    TEST_ASSERT_EQUAL_INT(1, get_bit(bits, 32));

    set_bit(bits, MAXV);
    TEST_ASSERT_EQUAL_INT(1, get_bit(bits, MAXV));
}

static void test_set_does_not_affect_adjacent_bits(void)
{
    set_bit(bits, 5);
    TEST_ASSERT_EQUAL_INT(0, get_bit(bits, 4));
    TEST_ASSERT_EQUAL_INT(0, get_bit(bits, 6));
    TEST_ASSERT_EQUAL_INT(0, get_bit(bits, 37));
}

static void test_set_is_idempotent(void)
{
    set_bit(bits, 5);
    set_bit(bits, 5);
    TEST_ASSERT_EQUAL_INT(1, get_bit(bits, 5));
}

static void test_clear_bit(void)
{
    set_bit(bits, 10);
    set_bit(bits, 11);
    clear_bit(bits, 10);
    TEST_ASSERT_EQUAL_INT(0, get_bit(bits, 10));
    TEST_ASSERT_EQUAL_INT(1, get_bit(bits, 11));
}

/* Word boundary — bit 31 (last in word 0) and bit 32 (first in word 1) */
static void test_word_boundary_independence(void)
{
    set_bit(bits, 31);
    set_bit(bits, 32);
    TEST_ASSERT_EQUAL_INT(1, get_bit(bits, 31));
    TEST_ASSERT_EQUAL_INT(1, get_bit(bits, 32));

    clear_bit(bits, 31);
    TEST_ASSERT_EQUAL_INT(1, get_bit(bits, 32));
    TEST_ASSERT_EQUAL_INT(0, get_bit(bits, 31));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_fresh_array_has_no_bits_set);
    RUN_TEST(test_set_get_roundtrip);
    RUN_TEST(test_set_does_not_affect_adjacent_bits);
    RUN_TEST(test_set_is_idempotent);
    RUN_TEST(test_clear_bit);
    RUN_TEST(test_word_boundary_independence);
    return UNITY_END();
}
