#include <CrackSeed.h>

void mt19937_reload(mt19937_state *state, uint32_t flavour)
{
    uint32_t *p = state->state;
    uint32_t i;

    if (flavour) {
        for (i = N - M; i--; ++p) {
            *p = twist(p[M], p[0], p[1]);
        }
        for (i = M; --i; ++p) {
            *p = twist(p[M-N], p[0], p[1]);
        }
        *p = twist(p[M-N], p[0], state->state[0]);
    } else {
        for (i = N - M; i--; ++p) {
            *p = twist_php(p[M], p[0], p[1]);
        }
        for (i = M; --i; ++p) {
            *p = twist_php(p[M-N], p[0], p[1]);
        }
        *p = twist_php(p[M-N], p[0], state->state[0]);
    }

    state->count = 0;
}

void mt19937_seed(mt19937_state *state, uint32_t seed)
{
    uint32_t i, prev_state;
    state->state[0] = seed;
    for (i = 1; i < N; i++) {
        prev_state = state->state[i - 1];
        state->state[i] = (1812433253U * (prev_state  ^ (prev_state  >> 30)) + i) & 0xffffffffU;
    }
    state->count = i;

    mt19937_reload(state, 1);
}


uint32_t php_mt_rand(mt19937_state * state, uint32_t flavour){
  mt19937_state *s = state;
  if (s->count >= N) {
        mt19937_reload(s, flavour);
    }
  uint32_t s1 =  s->state[s->count++];
  s1 ^= (s1 >> 11);
  s1 ^= (s1 <<  7) & 0x9d2c5680U;
  s1 ^= (s1 << 15) & 0xefc60000U;
  return s1 ^ (s1 >> 18);
}

uint32_t php_mt_rand_range(mt19937_state * state, uint32_t flavour, uint32_t min, uint32_t max){
  uint32_t result=php_mt_rand(state,1); 
  uint32_t umax=max+1;
  uint32_t limit = 0xffffffffU - (0xffffffffU % umax)-1 ;
  while (result>limit)
    {
        result = result | php_mt_rand(state, flavour);
        
    }
    return (result%umax);
}



