/*
 * Pseudorandom number generator built with a balanced Feistel network.
 * The round function was developed by a genetic algorithm, and tested extensively.
 * This prng is not only very strong, but also very quick, and dead simple as well.
 * Initially written in C# by EnderPi, ported to C by WCBROW01.
 */

#include <stdbool.h>
#include <stdint.h>

#include "prand.h"

// This macro optimizes down to the rotate left instruction on most compilers.
#define ROL(x, n) (((x) << (n)) | ((x) >> (sizeof(x) * 8 - (n))))

/* If a C11 compiler is present, make the global prng thread-local.
 * I am not going to try to target every compiler's thread-local implementation,
 * so if your's doesn't support it, too bad. */
#if __STDC_VERSION >= 201112L
#include <threads.h>

static thread_local bool prand_ready = false;
static thread_local prand_t prand_state;
#else
static bool prand_ready = false;
static prand_t prand_state;
#endif

// Functions for getting and setting the state of the global rng.
prand_t prand_getstate(void) {
	return prand_state;
}

void prand_setstate(prand_t p) {
	prand_state = p;
}

// Pseudorandom number generation functions. Seeded with 1 by default.
void prand_seed(uint64_t seed) {
	prand_ready = true;
	prand_seed_r(&prand_state, seed);
}

uint64_t prand(void) {
	if (!prand_ready) prand_seed(1);
	return prand_r(&prand_state);
}

// Used to generate the keys array.
static inline uint64_t enderlcg(uint64_t *seed) {
	*seed = (*seed * 6364136223846793005) + 1442695040888963407;
	uint64_t output = ROL(*seed, 9) * 1498817317654829;
	output ^= output >> 32;
	return output;
}

/* Reentrant variants that allow you to provide your own generator instance.
 * Make sure you seed it before using these, otherwise your results are undefined. */
void prand_seed_r(prand_t *p, uint64_t seed) {
	p->perm = 0;

	for (int i = 0; i < PRAND_NUM_ROUNDS; ++i)
		p->keys[i] = enderlcg(&seed) >> 32;
}

// This dead simple function was generated with a genetic algorithm.
static uint32_t prand_round(uint32_t x, uint32_t key) {
	return key + x + ROL(x, x & 31);
}

uint64_t prand_r(prand_t *p) {
	uint32_t right = p->perm;
	uint32_t left = p->perm++ >> 32;
	for (int i = 0; i < PRAND_NUM_ROUNDS; ++i) {
		uint32_t temp = right;
		/* This previously used an entry from a random array of keys,
		 * but it turns out that the keys don't need to random, just unique. */
		right = left ^ prand_round(right, p->keys[i]);
		left = temp;
	}

	return (uint64_t) left << 32 | right;
}
