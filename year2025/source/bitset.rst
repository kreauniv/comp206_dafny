Bit sets
========

When we discussed the "trie" structure for organizing a literal dictionary that
maps words to their meaning strings, we treated it as a 26-way branching tree.
The reality is that for quite some nodes, many of those 26 entries will be
NULL.  For example, a vowel is more likely to follow a consonant than a
consonant and that too the preceding character may determine what consonant may
actually follow. Therefore, a strict 26-way branching node structure ends up
wasting a lot of space.

Can we do better?

What we want to do here is to have a variable length array where at each index
we store the branch corresponding to some character. One way to do that is -

.. code-block:: C

    typedef struct TrieNode *trie_t;

    struct CharSet {
        char ch;
        trie_t branch;
    };

    struct TrieNode {
        char ch;
        int N;
        struct CharSet *branches;
    };

This is a better strategy relative to what we had before and we can even get rid
of the ``ch`` field within the ``TrieNode`` structure since it is already a part of
the ``CharSet`` structure. 

The problem we're now faced with is that even testing whether a character is part of
the ``branches`` array involves a linear scan .. or we need to invest in keeping the
array sorted by character.

The broader problem of representing a small set of values is quite common though
and has a rather elegant way of treating it where set membership tests are constant
time and so are addition and deletion of members.

What we want to represent at each branch point is a selection of characters
from the alphabet and use an array of appropriate length. We can model the
"selection of characters" by dedicating one bit to each character that says
whether it is included in the set or not. This requires only 26 bits and CPUs
usually provide cheap bit-wise operations on 32-bit integers. So a single
32-bit integer suffices to answer the question of "which characters are part of
the next trie set?". We can map Bit0 to ``'a'``, Bit1 to ``'b'`` and so on to
mapping Bit25 to ``'z'``. 

So we have our "bitset" type of size 32 which is as simple as -

.. code-block:: C

   typedef unsigned int bitset32;

The "unsigned" indicates the positive numeric range :math:`[0,2^{32})` of 32 bits.

So how do we test whether a character belongs to a given bitset?

.. code-block:: C
    
    #include <stdbool.h>

    bool bitset_haschar(bitset32 bs, char c) {
        assert(c >= 'a' && c <= 'z');
        int i = c - 'a';
        return (bs & (1 << i)) > 0;
    }

The expression ``1 << i`` calculates :math:`2^i` and the bit-wise and operation ``&``
picks out the bit corresponding to :math:`2^i` from the binary representation.
If the result happens to be 0, then that bit is not set and the character is not
a member. If it happens to be non-zero, then that bit is set and the character is
a member.

We can use similarly use bit-wise "or" to add a character to a bitset.

.. code-block:: C

    bitset32 bitset_add(bitset32 bs, char c) {
        assert(c >= 'a' && c <= 'z');
        return bs | (1 << i);
    }

Things start getting interesting when we want to find out more about the bitset,
like count the number of characters that belong to it. The straight forward way to
do that is -

.. code-block:: C

    int bitset_size_spec(bitset32 bs) {
        int count = 0;
        while (bs > 0) {
            count += bs & 1;
            bs = bs >> 1;
        }
        return count;
    }

However, we can be smart about it and write a loop that only has to run as many
times as the number of bits that are set in the bitset. To do this, we need to use
the "bitwise XOR" operator, which is the "exclusive or" - i.e. the result is 1 only
if exactly one of the two inputs are 1 and is 0 otherwise. This is in contrast with
the previously used "inclusive or" which will produce a 1 if at least one of the
two input bits is 1.

Given a number whose bits are, say, ``101001000``, subtracting 1 from this
number gives us the bit pattern ``101000111`` -- i.e. all the trailing zeroes
turn to 1 and the least significant bit that is 1 turns to 0. If we now
bit-wise XOR this with the original number, we will get ``000001111``. Notice
that the number of 1s in this number corresponds to the bit position of the
least significant bit that is set.  If we then add 1 to this value, we will get
``000010000``. The value of that result is that if we then divide (integer division)
the original number by this, we will get ``10100`` .. and we have counted one
set bit. We can now repeat the process until we get 0 to count all the set bits
in the number.

.. code-block:: C

    int bitset_size(bitset32 bs) {
        int count = 0;
        while (bs > 0) {
            count++;
            bs = bs / ((bs ^ (bs - 1)) + 1);
        }
        return count;
    }

    struct BSIterState {
        bitset32 bs;
        int count;
    };

    BSIterState bitset_size_iter(BSIterState s) {
        // Note that s is passed by value and we're also returning
        // a structure by value. This is not permitted in C0 but
        // is valid (and very useful + efficient for small structures)
        // in C.
        BSIterState s2;
        s2.bs = s.bs / ((s.bs ^ (s.bs - 1)) + 1);
        s2.count = s.count + 1;

        // Here is our iteration invariant declared as a post condition
        // on the calculation we do here.
        assert(bitset_size_spec(s.bs) + s.count == bitset_size_spec(s2.bs) + s2.count);
        return s2;
    }
        
This loop will run only as many times as the number of "characters" we've added
to the bitset.

We can use the same logic to also get the index position of a character in
our array of trie branches.

.. code-block:: C

    int bitset_index(bitset32 bs, char c) {
        assert(c >= 'a' && c <= 'z');
        assert(bitset_haschar(bs, c));

        int i = 1 << (c-'a');
        return bitset_size(bs & (i-1));
    }

Here, ``bs & (i-1)`` will result in only the bits less significant than the bit
corresponding to ``c`` being included in the count. Therefore that is a subset
of the original bitset. For example, if ``c`` is ``'d'`` and ``bs`` is ``10010101001``,
then ``i`` is ``1000`` and ``i-1`` is ``0111`` and therefore ``bs & (i-1)`` is
``10010101001 & 0111`` which gives us ``001``. Counting the number of bits of that
gets us 1, which is the index of the character ``'d'`` in the set. We can now
use this index to lookup the corresponding trie branch in the array.

Why can it be worth bothering with this if we can use a sorted array instead?

You will have to make a choice about that based on measurements on the system
your program will be running on .. or something equivalent. For small enough
collections, you saw how a 32-bit integer suffices to represent the set. 
We did all our counting and indexing operations by directly manipulating
this 32-bit integer. An integer fits within a CPU "register" which is the
fastest kind of storage available to you and all our processing never needed
to go back to memory for any data .. except the final stage. Not even to the
closest and fastest memory - the L1 cache. The large constant costs of going
back to memory may be more than enough to offset the cost of "linear" search
through the bitset. So we have to make the choice based on measurement.

C tricks
^^^^^^^^

C is in some sense an "I guess you know what you're doing" language and will
let you do things like allocate an array of N elements and index beyond N
without complaining about it at compilation, though the operation may bomb at
runtime. We can at times exploit this behaviour to make compact data
structures like our bitset-trie branches array.

.. code-block:: C

    typedef struct TrieNode *trie_t;
    struct TrieNode {
        char ch;
        bitset32 bs;
        trie_t branches[0];
    };

Note how we declared a zero-length array in there. The size of this structure
will be 8 bytes, with 4-bytes being allocated for the ``ch`` field and 4 being
allocated for the ``bs`` field. If we tell the compiler to compact this structure,
then it will be only 5 bytes (which will be slightly slower to operate due
to misaligned data).

However, if we want to have, say, 6 branches, we can allocate a node like this -

.. code-block:: C

    trie_t n = (trie_t)malloc(sizeof(struct TrieNode) + sizeof(trie_t) * 6);

See how we're allocating not just enough memory for the ``ch`` and ``bs`` fields,
but we're including space for 6 more pointers to ``struct TrieNode`` which will
be 8 bytes each. It is as though we've allocated memory for the following
structure -

.. code-block:: C

    struct TrieNode6 {
        char ch;
        bitset32 bs;
        trie_t branches[6];
    };


We can then proceed to populate the ``bs`` field with an appropriate set of 6
characters. If we want to store more, we can always reallocate the structure. 

.. code-block:: C

    // We can access the index corresponding to the 'd' branch
    // like this -
    n->branches[bitset_index(n->bs, 'd')]

The cost of calculating ``bitset_index`` is the number of elements in the set
since we have to count each one at worst. Can we do better in cases where
we're likely to have, say, > 20 characters in the set?

Because of the way we've defined the ``bitset_index``, our array is actually
ordered by character value -- i.e. characters corresponding to higher bits
will be present after characters corresponding to lower bits. So, we can 
indeed use binary search here. Though binary search is not really needed for
a small set like 26 characters, it might be valuable for larger bitsets
like 128 bits ... if we have a use case for tries of that sort. Such tries
are not entirely unthinkable since we can use it for a set of known character
combinations or "tokens" instead of single characters.

This should give you a flavour of a case in which we're making certain kinds
of "processing versus memory" trade offs.

