#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>

/*
 * Q1: 10 points
 *
 * You're given a 26-way tree - a.k.a. a "word trie" - which
 * stores a collection of words from a dictionary.
 * Your task is to write a function that will print
 * all words in the dictionary that have a given prefix.
 *
 * For example, if the tree consisted of -
 *
 *             ROOT
 *     a         m          o
 *   h   n     e   y      n   u           
 *        t     w        e   n  t
 *                           c
 *                           e
 *
 * and the given prefix was "ou", you're expected to 
 * print out the following words, one word per line -
 *
 * ounce
 * out
 *
 * Given:
 *
 * 1. Only lower case letters are expected to be found in the given trie.
 *
 * 2. The depth of the given trie will not be more than 80.
 *
 * 3. If the given prefix is not in the trie, then print the word
 *    "NONE" on a line by itself instead.
 *
 * Hints:
 *
 * 1. Strings in C are just arrays of characters with an extra
 *    "null character" at the end - i.e. a character with ASCII
 *    value 0 is added to the end of the character sequence to
 *    indicate "end of string". 
 *
 * 2. Note that "char" in C is just an alias for "signed 8-bit integer".
 *    So you can convert a char to an int (and vice versa) using a "cast".
 *
 * 3. Any pointer argument may be NULL. You need to account for that.
 *
 * 4. You can write your own helper functions if you want to, and call
 *    them from the given functions.
 *
 * 5. If w is a string (i.e. a "char*" type) and you want to print it,
 *    you can use - printf("%s", w);
 *
 */

typedef struct TrieNode *trie_t;
struct TrieNode {
    char ch;
    trie_t children[26];
};

bool is_valid_word_trie(trie_t t) {
    // TODO: Implement to return true if t is a valid "word trie"
    // and false otherwise.
    return true;
}

bool print_words_precond(trie_t t, const char *prefix) {
    // TODO: Modify this function to check preconditions
    // and return true if met and false if not.
    //
    // Hint: This function depends on what your implementation
    // of print_words permits the arguments to be.
    return true;
}

void print_words(trie_t t, const char *prefix) {
    assert(print_words_precond(t, prefix));

    // TODO: Your code.
}

bool depth_trie_precond(trie_t t) {
    // TODO: Modify to check t.
    return true;
}

int depth_trie(trie_t t) {
    assert(depth_trie_precond(t));

    // TODO: Your code
    // You will also need to modify/edit the return
    // statement below or write your own.
    
    return 0;
}

