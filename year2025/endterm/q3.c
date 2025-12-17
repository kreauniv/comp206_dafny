#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>
#include "utils.h"

/*
 * Q3: 10 points
 * You can choose to tackle this or Q4.
 * 
 * You're given several hash functions, many based on the
 * "linear congruential generator" idea. You're expected
 * to write the "eval_hash" function implementation such
 * that the value returned by eval_hash indicates the
 * "quality" of the hash function.
 *
 * You can use either of a couple of possible approaches -
 * 1. You can estimate the average number of bits that will
 *    change in the hash when you increment the hash argument
 *    by 1.
 * 2. You can calculate the variance in the number of
 *    hashes for, say, 10000 random keys selected from a larger range 
 *    like [100,1000000] and mapped to 10000 buckets.
 *    Variance = mean(n^2) - mean(n)^2 where n is the number 
 *    of keys mapped to a particular bucket and the mean
 *    runs over all the buckets.
 *    For an even distribution, we'd expect the variance
 *    to be near zero.
 *
 * We will also consider only the lower 24 bits of the hash
 * for the evaluation since we want to use it to implement
 * a hash table of a few million buckets.
 *
 * NOTE: Read the description above eval_hash carefully.
 * You will also have to provide a comment explaining
 * why you think the value you return characterizes the
 * quality of the hash function.
 *
 * Points will also be awarded for appropriate use of
 *         assert(<boolean-expr>);
 * in your code to call out what you know to be true
 * at various points in your code.
 *
 * Hints:
 *
 * 1. You can convert an integer x into float value using
 *           (float)x
 *
 * 2. Be aware of whether you're doing integer division or
 *    floating point division when computing the average.
 *
 * 3. You'll have to use appropriate bit-wise operators and
 *    write the necessary helper functions.
 */

// This is the type of any "hash function" that takes
// a uint32_t (unsigned 32 bit integer) argument and computes
// a "hash" that is also of the same type. The hash functions
// given below are of this form.
typedef uint32_t (*hashfn)(uint32_t x);

uint32_t hash0(uint32_t x) {
    // Base line that we have to do better than.
    return x;
}

uint32_t hash1(uint32_t x) {
    uint32_t a = 1664525;
    uint32_t b = 1013904223;
    return a * x + b;
}

uint32_t hash2(uint32_t x) {
    uint32_t a = 1664525;
    uint32_t b = 1013904223;
    uint32_t x0 = x & 65535;
    uint32_t x1 = (x >> 16) & 65535;

    uint32_t hash = 0;
    hash = a * (hash ^ x0) + b;
    hash = a * (hash ^ x1) + b;
    return hash;
}


uint32_t hash3(uint32_t x) {
    uint32_t a = 1664525;
    uint32_t b = 1013904223;
    uint32_t x0 = x & 255;
    uint32_t x1 = (x >> 8) & 255;
    uint32_t x2 = (x >> 16) & 255;
    uint32_t x3 = (x >> 24) & 255;

    uint32_t hash = 0;
    hash = a * (hash ^ x0) + b;
    hash = a * (hash ^ x1) + b;
    hash = a * (hash ^ x2) + b;
    hash = a * (hash ^ x3) + b;
    return hash;
}

uint32_t hash4(uint32_t x) {
    return x * (x + 1013904223);
}

uint32_t hash5(uint32_t x) {
    uint32_t a = 16777619;
    uint32_t b = 2166136261;
    uint32_t x0 = x & 255;
    uint32_t x1 = (x >> 8) & 255;
    uint32_t x2 = (x >> 16) & 255;
    uint32_t x3 = (x >> 24) & 255;

    uint32_t hash = b;
    hash = a * (hash ^ x0);
    hash = a * (hash ^ x1);
    hash = a * (hash ^ x2);
    hash = a * (hash ^ x3);
    return hash;
}


int num_bits_different(uint32_t a, uint32_t b, int nbits) {
    assert(nbits > 0 && nbits <= 24);

    // TODO: Implement this to compute the number of
    // bits that differ between the two given numbers,
    // considering only the lower "nbits" bits of both
    // numbers.
    return 0;
}

// This function will be called, for example, using hash1 as follows -
//
//    float score = eval_hash(&hash1, 100, 1000000, 16);
//
// The score is expected to be indicative of how "bad" the hash
// function is. The lower the score, the better the hash function
// likely is. 
float eval_hash(hashfn hash, uint32_t xfrom, uint32_t xto, int nbits) {
    assert(xfrom < xto);
    assert(nbits > 0 && nbits <= 24);

    // TODO: Implement the evaluation.
    // You can call the given hash function like this -
    //    uint32_t h = (*hash)(x);

    return 0.0f; // A decimal number with an 'f' suffix indicates
                 // a floating point constant.
}

// We often want to store values associated with string keys and so
// need good hash functions on strings. Use what you figured out with
// eval_hash to write a string hash below. 
//
// Hint: You may be able to take ideas on how to implement such a hash
// by looking through the above given hash functions. After all, a
// string in C is just a sequence of bytes terminated by a '\0' character
// with ASCII value 0.
uint32_t hash_string(const char *str) {
    // TODO:  Your implementation.
    return 0;
}

