// Based on https://stackoverflow.com/questions/1225998/what-is-a-bitmap-in-c
// by Dale Hagglund and Dan Grahn.
#include "bitarray.h"

void set_bit(word_t *words, unsigned long n)
{
    words[WORD_OFFSET(n)] |= (word_t)1 << BIT_OFFSET(n);
}

void clear_bit(word_t *words, unsigned long n)
{
    words[WORD_OFFSET(n)] &= ~((word_t)1 << BIT_OFFSET(n));
}

int get_bit(word_t *words, unsigned long n)
{
    return (words[WORD_OFFSET(n)] >> BIT_OFFSET(n)) & 1;
}
