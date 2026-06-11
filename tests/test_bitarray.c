/*
 * Unit tests for bitarray set_bit / get_bit / clear_bit (issue #15).
 * Build with -fsanitize=address to catch OOB writes.
 */
#include <stdio.h>
#include <string.h>
#include "bitarray.h"
#include "constants.h"

static int pass_count = 0;
static int fail_count = 0;

static void check(const char *name, int ok)
{
    if (ok) { printf("[PASS] %s\n", name); pass_count++; }
    else     { printf("[FAIL] %s\n", name); fail_count++; }
}

/* Array sized for MAXV+1 bits, matching globals.c */
static word_t bits[BITS_TO_WORDS(MAXV + 1)];

int main(void)
{
    memset(bits, 0, sizeof bits);

    /* Test 1: freshly zeroed array has no bits set */
    check("fresh array: bit 0 is clear",        get_bit(bits, 0) == 0);
    check("fresh array: bit 31 is clear",       get_bit(bits, 31) == 0);
    check("fresh array: bit 32 is clear",       get_bit(bits, 32) == 0);
    check("fresh array: bit MAXV is clear",     get_bit(bits, MAXV) == 0);

    /* Test 2: set_bit / get_bit round-trip */
    set_bit(bits, 0);
    check("set bit 0: get returns 1",           get_bit(bits, 0) == 1);

    set_bit(bits, 31);
    check("set bit 31: get returns 1",          get_bit(bits, 31) == 1);

    set_bit(bits, 32);
    check("set bit 32 (next word): get returns 1", get_bit(bits, 32) == 1);

    set_bit(bits, MAXV);
    check("set bit MAXV: get returns 1",        get_bit(bits, MAXV) == 1);

    /* Test 3: adjacent bits are not affected */
    memset(bits, 0, sizeof bits);
    set_bit(bits, 5);
    check("set bit 5: bit 4 unaffected",        get_bit(bits, 4) == 0);
    check("set bit 5: bit 6 unaffected",        get_bit(bits, 6) == 0);
    check("set bit 5: bit 37 unaffected",       get_bit(bits, 37) == 0);

    /* Test 4: set_bit is idempotent */
    set_bit(bits, 5);
    check("set bit 5 twice: still 1",           get_bit(bits, 5) == 1);

    /* Test 5: clear_bit */
    memset(bits, 0, sizeof bits);
    set_bit(bits, 10);
    set_bit(bits, 11);
    clear_bit(bits, 10);
    check("clear bit 10: get returns 0",        get_bit(bits, 10) == 0);
    check("clear bit 10: bit 11 unaffected",    get_bit(bits, 11) == 1);

    /* Test 6: word boundary — bit 31 (last in word 0) and bit 32 (first in word 1) */
    memset(bits, 0, sizeof bits);
    set_bit(bits, 31);
    set_bit(bits, 32);
    check("bits 31 and 32 both set independently",
          get_bit(bits, 31) == 1 && get_bit(bits, 32) == 1);
    clear_bit(bits, 31);
    check("clear bit 31: bit 32 unaffected",    get_bit(bits, 32) == 1);
    check("clear bit 31: bit 31 is now 0",      get_bit(bits, 31) == 0);

    printf("\nResults: %d passed, %d failed out of %d tests\n",
           pass_count, fail_count, pass_count + fail_count);
    return fail_count ? 1 : 0;
}
