#include <limits.h>    /* for CHAR_BIT */
#include <stdint.h>   /* for uint32_t */

// This is based on the Stack Overflow example by Dale Hagglund and Dan Grahn located at
// https://stackoverflow.com/questions/1225998/what-is-a-bitmap-in-c

typedef uint32_t word_t;

enum { BITS_PER_WORD = sizeof(word_t) * CHAR_BIT };

#define WORD_OFFSET(b) ((b) / BITS_PER_WORD)
#define BIT_OFFSET(b)  ((b) % BITS_PER_WORD)

void set_bit(word_t *words, int n) { 
    words[WORD_OFFSET(n)] |= (1 << BIT_OFFSET(n));
}

void clear_bit(word_t *words, int n) {
    words[WORD_OFFSET(n)] &= ~(1 << BIT_OFFSET(n)); 
}

int get_bit(word_t *words, int n) {
    word_t bit = words[WORD_OFFSET(n)] & (1 << BIT_OFFSET(n));
    return bit != 0; 
}