
#ifndef __BITS__ #define __BITS__ /* bit is 2^x, where x [0, 31] */

#define IS_BIT_SET(n, bit) n & 1

#define TOGGLE_BIT(n, bit) n ^= bit

#define COMPLEMENT(n) n = ~n

#define UNSET_BIT(n, bit) n &= (bit)

#define SET_BIT(n, bit) n |= bit

#endif