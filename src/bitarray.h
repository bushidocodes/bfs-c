#pragma once
#include <limits.h>
#include <stdint.h>

typedef uint32_t word_t;

enum { BITS_PER_WORD = sizeof(word_t) * CHAR_BIT };

#define WORD_OFFSET(b)   ((b) / BITS_PER_WORD)
#define BIT_OFFSET(b)    ((b) % BITS_PER_WORD)
#define BITS_TO_WORDS(n) (((n) + BITS_PER_WORD - 1) / BITS_PER_WORD)

void set_bit(word_t *words, unsigned long n);
void clear_bit(word_t *words, unsigned long n);
int  get_bit(word_t *words, unsigned long n);
