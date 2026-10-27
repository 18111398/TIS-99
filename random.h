#ifndef RANDOM_H
#define RANDOM_H
#include <stdint.h>
#include <stdlib.h>

typedef struct unity_rng {
    uint32_t x, y, z, w;
} unity_rng;

static const uint32_t MT19937 = 1812433253;

unity_rng create_unity_rng(uint32_t seed) {
    unity_rng rng;
    rng.x = seed;
    rng.y = rng.x * MT19937 + 1;
    rng.z = rng.y * MT19937 + 1;
    rng.w = rng.z * MT19937 + 1;
    return rng;
}

uint32_t unity_rng_next(unity_rng *rng) {
    uint32_t t = rng->x ^ (rng->x << 11);
    rng->x = rng->y;
    rng->y = rng->z;
    rng->z = rng->w;
    rng->w = rng->w ^ (rng->w >> 19) ^ t ^ (t >> 8);
    return rng->w;
}

static inline uint32_t unity_rng_next_between(unity_rng *rng, uint32_t min, uint32_t max) {
    return min + unity_rng_next(rng) % (max - min);
}

// Implementation of Mono RNG. The original implementation has SeedArray[0] unused.
// I am removing that slot, which causes all indices to be reduced by 1.

typedef struct mono_rng {
    uint32_t inext;
    uint32_t inextp;
    int32_t seed_array[55];
} mono_rng;

mono_rng create_mono_rng(int32_t seed) {
    mono_rng rng;
    int ii = 20;
    int mj, mk;
    mj = 161803398 - abs(seed);
    rng.seed_array[54] = mj;
    mk = 1;
    for (int i = 1; i < 55; i++) {
        rng.seed_array[ii] = mk;
        mk = mj - mk;
        if (mk < 0) {
            mk += 2147483647;
        }
        mj = rng.seed_array[ii];
        ii += 21;
        if (ii >= 55) ii -= 55;
    }

    for (int k = 1; k < 5; k++) {
        for (int i = 0; i < 55; i++) {
            int i2 = i + 31;
            if (i2 >= 55) i2 -= 55;
            rng.seed_array[i] -= rng.seed_array[i2];
            if (rng.seed_array[i] < 0) {
                rng.seed_array[i] += 2147483647;
            }
        }
    }

    rng.inext = 0;
    rng.inextp = 31;
    return rng;
}

double mono_rng_next(mono_rng *rng) {
    int retVal = rng->seed_array[rng->inext] - rng->seed_array[rng->inextp];
    if (retVal == 2147483647) {
        retVal--;
    }
    if (retVal < 0) {
        retVal += 2147483647;
    }
    rng->seed_array[rng->inext] = retVal;

    if (++rng->inext >= 55) rng->inext = 0;
    if (++rng->inextp >= 55) rng->inextp = 0;
    return retVal * (1. / 2147483647.);
}

static inline int32_t mono_rng_next_between(mono_rng *rng, int32_t min, int32_t max) {
    return (int32_t)(mono_rng_next(rng) * (max - min)) + min;
}

#endif // RANDOM_H
