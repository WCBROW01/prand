/*
 * Pseudorandom number generator built with a balanced Feistel network.
 * The round function was developed by a genetic algorithm, and tested extensively.
 * This prng is not only very strong, but also very quick, and dead simple as well.
 * Initially written in C# by EnderPi, ported to C by WCBROW01.
 */

#ifndef PRAND_H
#define PRAND_H

#ifdef __cplusplus
extern "C" {
#endif

/* This was experimentally tested to be safe with as few as 18 rounds,
 * but increased to 24 for safety. */
#define PRAND_NUM_ROUNDS 24

typedef struct prand_t {
	uint32_t keys[PRAND_NUM_ROUNDS];
	uint64_t perm;
} prand_t;

// Functions for getting and setting the state of the global rng.
prand_t prand_getstate(void);
void prand_setstate(prand_t p);

// Pseudorandom number generation functions. Seeded with 1 by default.
void prand_seed(uint64_t seed);
uint64_t prand(void);

/* Reentrant variants that allow you to provide your own generator instance.
 * Make sure you seed it before using these, otherwise your results are undefined. */
void prand_seed_r(prand_t *p, uint64_t seed);
uint64_t prand_r(prand_t *p);

#ifdef __cplusplus
}
#endif

#endif
