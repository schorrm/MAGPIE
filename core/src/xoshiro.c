/*  Written in 2019 by David Blackman and Sebastiano Vigna (vigna@acm.org)

To the extent possible under law, the author has dedicated all copyright
and related and neighboring rights to this software to the public domain
worldwide. This software is distributed without any warranty.

See <http://creativecommons.org/publicdomain/zero/1.0/>. */

/* Modified by MAGPIE team to add a struct and parallelizability. */

#include <stdint.h>
#include <stdlib.h>

#include "xoshiro.h"

/* This is xoshiro256++ 1.0, one of our all-purpose, rock-solid generators.
   It has excellent (sub-ns) speed, a state (256 bits) that is large
   enough for any parallel application, and it passes all tests we are
   aware of.

   For generating just floating-point numbers, xoshiro256+ is even faster.

   The state must be seeded so that it is not everywhere zero. If you have
   a 64-bit seed, we suggest to seed a splitmix64 generator and use its
   output to fill s. */

static inline uint64_t rotl(const uint64_t x, int k) {
  return (x << k) | (x >> (64 - k));
}

uint64_t xoshiro_next(XoshiroPRNG *prng) {
  const uint64_t result = rotl(prng->s[0] + prng->s[3], 23) + prng->s[0];

  const uint64_t t = prng->s[1] << 17;

  prng->s[2] ^= prng->s[0];
  prng->s[3] ^= prng->s[1];
  prng->s[1] ^= prng->s[2];
  prng->s[0] ^= prng->s[3];

  prng->s[2] ^= t;

  prng->s[3] = rotl(prng->s[3], 45);

  return result;
}

/* This is the jump function for the generator. It is equivalent
   to 2^128 calls to next(); it can be used to generate 2^128
   non-overlapping subsequences for parallel computations. */

void xoshiro_jump(XoshiroPRNG *prng) {
  static const uint64_t JUMP[] = {0x180ec6d33cfd0aba, 0xd5a61266f0c9392c,
                                  0xa9582618e03fc9aa, 0x39abdc4529b1661c};

  uint64_t s0 = 0;
  uint64_t s1 = 0;
  uint64_t s2 = 0;
  uint64_t s3 = 0;
  for (unsigned long i = 0; i < sizeof JUMP / sizeof *JUMP; i++)
    for (int b = 0; b < 64; b++) {
      if (JUMP[i] & UINT64_C(1) << b) {
        s0 ^= prng->s[0];
        s1 ^= prng->s[1];
        s2 ^= prng->s[2];
        s3 ^= prng->s[3];
      }
      xoshiro_next(prng);
    }

  prng->s[0] = s0;
  prng->s[1] = s1;
  prng->s[2] = s2;
  prng->s[3] = s3;
}

/* This is the long-jump function for the generator. It is equivalent to
   2^192 calls to next(); it can be used to generate 2^64 starting points,
   from each of which jump() will generate 2^64 non-overlapping
   subsequences for parallel distributed computations. */

void xoshiro_long_jump(XoshiroPRNG *prng) {
  static const uint64_t LONG_JUMP[] = {0x76e15d3efefdcbbf, 0xc5004e441c522fb3,
                                       0x77710069854ee241, 0x39109bb02acbe635};

  uint64_t s0 = 0;
  uint64_t s1 = 0;
  uint64_t s2 = 0;
  uint64_t s3 = 0;
  for (unsigned long i = 0; i < sizeof LONG_JUMP / sizeof *LONG_JUMP; i++)
    for (int b = 0; b < 64; b++) {
      if (LONG_JUMP[i] & UINT64_C(1) << b) {
        s0 ^= prng->s[0];
        s1 ^= prng->s[1];
        s2 ^= prng->s[2];
        s3 ^= prng->s[3];
      }
      xoshiro_next(prng);
    }

  prng->s[0] = s0;
  prng->s[1] = s1;
  prng->s[2] = s2;
  prng->s[3] = s3;
}

void copy_prng_into(XoshiroPRNG *dst, XoshiroPRNG *src) {
  for (int i = 0; i < 4; i++) {
    dst->s[i] = src->s[i];
  }
}

/*  Written in 2015 by Sebastiano Vigna (vigna@acm.org)

To the extent possible under law, the author has dedicated all copyright
and related and neighboring rights to this software to the public domain
worldwide. This software is distributed without any warranty.

See <http://creativecommons.org/publicdomain/zero/1.0/>. */

/* This is a fixed-increment version of Java 8's SplittableRandom generator
   See http://dx.doi.org/10.1145/2714064.2660195 and
   http://docs.oracle.com/javase/8/docs/api/java/util/SplittableRandom.html

   It is a very fast generator passing BigCrush, and it can be useful if
   for some reason you absolutely want 64 bits of state. */

static uint64_t xxsplit; /* The state can be seeded with any value. */

uint64_t splitmix_next() {
  uint64_t z = (xxsplit += 0x9e3779b97f4a7c15);
  z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
  z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
  return z ^ (z >> 31);
}

void seed_prng(XoshiroPRNG *prng, uint64_t seed) {
  xxsplit = seed;
  for (int i = 0; i < 4; i++) {
    prng->s[i] = splitmix_next();
  }
}

// initializer
// create and seed PRNG.
XoshiroPRNG *create_prng(uint64_t seed) {
  XoshiroPRNG *prng = malloc(sizeof(XoshiroPRNG));
  seed_prng(prng, seed);
  return prng;
}

void destroy_prng(XoshiroPRNG *prng) { free(prng); }