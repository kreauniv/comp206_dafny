#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

// Q1
typedef struct TrieNode *trie_t;
struct TrieNode {
    char ch;
    trie_t children[26];
};
bool is_valid_word_trie(trie_t t);
bool print_words_precond(trie_t t, const char *prefix);
void print_words(trie_t t, const char *prefix);
void depth_trie_precond(trie_t t);
int depth_trie(trie_t t);


// Q2
typedef uint32_t (*hashfn)(uint32_t x);
uint32_t hash0(uint32_t x);
uint32_t hash1(uint32_t x);
uint32_t hash2(uint32_t x);
uint32_t hash3(uint32_t x);
uint32_t hash4(uint32_t x);
uint32_t hash5(uint32_t x);
int num_bits_different(uint32_t a, uint32_t b, int nbits);
float eval_hash(hashfn hash, uint32_t xfrom, uint32_t xto, int nbits);
uint32_t hash_string(const char *str);


// Q3
bool sort_normalized_text_precond(const char *input, char *output, size_t output_len);
bool sort_normalized_text_postcond(const char *input, const char *output, size_t output_len);
void sort_normalized_text(const char *input, char *output, size_t output_len);
float sort_normalized_text_complexity(int input_len);

// Q4
void hadamard(int n, const float *in, float *out);
int hadamard_complexity(int n);

