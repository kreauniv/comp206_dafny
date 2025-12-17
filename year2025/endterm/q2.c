#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>
#include <math.h>

/*
 * Q2: 5 points
 *
 * For this question, your function will be provided with an array of
 * characters - i.e. a string - terminated by a null character of ASCII value
 * 0. This string is "normalized" from ordinary prose by removing all digits
 * and punctuations and converting all upper case letters to lower case.
 * Therefore the only characters which can feature in this string are the lower
 * case letters and the space character (a set of 27 characters).
 *
 * Write a function to sort such an array of characters and place the sorted
 * result in the given output array of the same length as the input.
 *
 * NOTE: Points will be given for writing appropriate assert() expressions.
 * Consider expressing loop invariants using such assertions too.
 *
 * You can write any helper function you want to and call them from these
 * functions given below.
 *
 * Hint(s):
 *
 * 1. In case you need temporary storage, you can allocate using malloc.
 *    Ensure that you free whatever you allocate so that there are no
 *    "memory leaks" in your program.
 *
 * 2. No specific sorting algorithm is mandated. You can write any
 *    algorithm you want to.
 *
 * 3. size_t is just a short hand for "enough sized integer to hold
 *    the length of an array." Often it is a 64-bit unsigned integer.
 *
 * 4. Think through the problem first ... allowing for the possibility that
 *    it may be simpler than you might imagine it to be.
 */

bool sort_normalized_text_precond(const char *input, char *output, size_t output_len) {
    // TODO: Implement this. You'll need to validate the input here.
    return true;
}

bool sort_normalized_text_postcond(const char *input, const char *output, size_t output_len) {
    // TODO: Implement this.
    return true;
}

// You have to sort the characters you find in the input string which will be
// given with a terminating null character ... and place the result into the
// given output array which has been preallocated with capacity output_len bytes.
// You'll need output_len to be large enough to hold all the characters of the
// input string in order to proceed with your algorithm.
void sort_normalized_text(const char *input, char *output, size_t output_len) {
    assert(sort_normalized_text_precond(input, output, output_len));

    // TODO: Implement this.

    assert(sort_normalized_text_postcond(input, output, output_len));
}


float sort_normalized_text_complexity(int input_len) {
    // TODO: Implement this function to return the complexity of your
    // sorting algorithm as an expression. (Do not call
    // sort_normalized_text and count steps).
    //
    // You can use any of the math.h functions like log, sqrt
    // and so on.
    return 1.0;
}
