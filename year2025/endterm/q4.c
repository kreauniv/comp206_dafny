#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

/*
 * Q4: 10 points
 * Computing the Walsh-Hadamard transform.
 * You can choose to tackle this or Q3.
 *
 * The Walsh-Hadamard transform, often just called the Hadamard transform, is a
 * common operation used in quantum computing.  Don't worry you don't need
 * to know quantum computing or learn it for this question though.
 *
 * The transform takes a vector of length 2^n and converts it into
 * another vector of the same length. Using the following logic -
 *
 * For a 2-element vector [a, b], the Hadamard transform ignoring
 * normalization factors, is defined as [a + b, a - b]. You can look
 * at this as a matrix multiplication as shown below -
 *
 *   
 * [1  1      [a       [a+b
 *  1 -1]  x   b]   =   a-b]
 *
 * Let's call that 2x2 matrix as H1. For a 4-element vector, you can 
 * construct the Hadamard matrix recursively as follows -
 *
 * [H1  H1
 *  H1 -H1] ... which gives H2 =
 *
 * [1  1   1  1
 *  1 -1   1 -1
 *  1  1  -1 -1
 *  1 -1  -1  1]
 *
 * Similarly, you can construct H3 = [H2  H2
 *                                    H2 -H2]
 *
 * and so on. Note that H3 is an 8x8 matrix. Continuing the process,
 * you see that Hn is a 2^n x 2^n matrix. A Hadamard mixing is a common
 * first step in several quantum algorithms where the states of n-quantum
 * bits (which has a state space of 2^n dimensions) are mixed. 
 *
 * TASK: Write the function below for calculating the hadamard-n
 * of a given floating point vector of numbers and store the result
 * into another floating point vector of numbers.
 *
 * If you're doing this question instead of Q3, you will need to
 * argue that your implementation is correct supported by appropriate contracts
 * in the code.
 */

// Note: You're guaranteed that the in and out arrays will have 2^n
// elements.
void hadamard(int n, const float *in, float *out) {

};

int hadamard_complexity(int n) {
    // Write an expression here that captures (as Big-O)
    // the order or the number of additions/subtractions that
    // your algorithm will perform for a given n (which operates
    // on a vector of length 2^n).
    return 1;
}

