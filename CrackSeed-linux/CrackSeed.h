#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

/* from php source */
#define hiBit(u)      ((u) & 0x80000000U)  /* mask all but highest   bit of u */
#define loBit(u)      ((u) & 0x00000001U)  /* mask all but lowest    bit of u */
#define loBits(u)     ((u) & 0x7FFFFFFFU)  /* mask     the highest   bit of u */
#define mixBits(u, v) (hiBit(u)|loBits(v)) /* move hi bit of u to hi bit of v */
#define twist(m,u,v)  (m ^ (mixBits(u,v)>>1) ^ ((uint32_t)(-(int32_t)(loBit(v))) & 0x9908b0dfU)) // (PHP7+)
#define twist_php(m,u,v)  (m ^ (mixBits(u,v)>>1) ^ ((uint32_t)(-(int32_t)(loBit(u))) & 0x9908b0dfU))  //(PHP5) 
/* end from php source */

#define N 624
#define M 397


typedef struct _mt19937_state {
	uint32_t count;
	uint32_t state[624];
} mt19937_state;


void mt19937_reload();

void mt19937_seed(mt19937_state *state, uint32_t seed);


uint32_t php_mt_rand(mt19937_state * state, uint32_t flavour);

uint32_t php_mt_rand_range(mt19937_state * state, uint32_t flavour, uint32_t min, uint32_t max);