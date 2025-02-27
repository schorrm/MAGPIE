#ifndef XOSHIRO_H
#define XOSHIRO_H

#include <stdint.h>

#define XOSHIRO_MAX UINT64_C(18446744073709551615)

typedef struct XoshiroPRNG {
  uint64_t s[4];
} XoshiroPRNG;

XoshiroPRNG *create_prng(uint64_t seed);
void copy_prng_into(XoshiroPRNG *dst, XoshiroPRNG *src);
void seed_prng(XoshiroPRNG *x, uint64_t seed);
void xoshiro_jump(XoshiroPRNG *x);
uint64_t xoshiro_next(XoshiroPRNG *x);
void destroy_prng(XoshiroPRNG *x);

#endif