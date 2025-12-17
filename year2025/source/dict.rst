
Dictionaries and associated data structures
===========================================

Many programming languages today provide a utility data structure
usually called a "dictionary" (python, julia, ruby) or a "table"
(lua) that is used to associate orderable "keys" with "values".
In fact, "key value data stores" are a common base layer in many
production grade data systems such as `Redis`_, `AWS S3`_,
`FoundationDB`_, `RocksDB`_ and many others.


.. _Redis: https://redis.io
.. _AWS S3: https://aws.amazon.com/s3/
.. _FoundationDB: https://www.foundationdb.org/
.. _RocksDB: https://rocksdb.org/

Given the utility of the key-value association structures, it is
not surprising that a number of data structures fall in that
category. All of them may be summarized by the following
interface (in C) -

.. code-block:: C

    dict_t make_dict();
    bool dict_haskey(dict_t d, key_t k);
    value_t dict_get(dict_t d, key_t k);
    void dict_set(dict_t d, key_t k, value_t v);

where ``dict_t``, ``key_t`` and ``value_t`` are appropriately
defined types. The main constraint on these types is that
``dict_t`` should be of pointer type and ``key_t`` should be
orderable using a comparison function. For example, if ``key_t``
is just ``int`` you can use the ``<`` operator to determine ordering.

An implementation of the above interface will assume the availability
of such a comparison operation. However, you can also modify the
interface to take in a "comparison function pointer" to customize the
comparison as shown below -

.. code-block:: C

    // Returns -1 if k1 < k2, 0, if k1 == k2 and 1 if k1 > k2.
    // The comparator must also be transitive - i.e. k1 < k2 and k2 < k3
    // must imply k1 < k3 as well as k1 == k2 and k2 == k3 should imply
    // k1 == k3. Also, k1 < k2 must imply k2 > k1 and vice versa.
    typedef int (*key_comparator_t)(key_t k1, key_t k2);

    dict_t make_dict(key_comparator_t comp);

The dictionary implementation can then use the given comparison function
to order the structure. For example -

.. code-block:: C

    typedef int key_t;
    int greater_than(key_t k1, key_t k2) {
        if (k1 > k2) { return -1; }
        if (k1 == k2) { return 0; }
        return 1;
    }

    // ....
    dict_t d = make_dict(&greater_than); // Notice the '&' in front of the function name.


The dictionary interface specification above make no guarantees about
how efficiently the mapping is performed, and we have scope for various kinds
of trade offs which we'll look at now.

Arrays
------

An array can also be seen as a key-value mapping dictionary whose key type
is an integer index and whose value can be anything. The main characteristic
of an array is that you will need to allocate space to store values corresponding
to all keys. This is called a "dense" data structure for that reason, as opposed
to a "sparse" data structure where the actual keys used will be a small subset of
the possible set of keys.

Suppose we have a 256 element array (of anything).

.. code-block:: C

   value_t array[256];

Given an index ``i`` in binary form such as ``01001010``, we can squint and
look at the array as a tree traversal to locate this index as follows [#bindex]_ -

0. Start with the full span of the array - :math:`[0,256)`.
1. Bit7 is 0, so reduce the span to the left half - :math:`[0,128)`.
2. Bit6 is 1, so reduce the span to the right half - :math:`[64,128)`.
3. Bit5 is 0, so reduce the span to the left half - :math:`[64,96)`.
4. Bit4 is 0, so reduce the span to the left half - :math:`[64,80)`.
5. Bit3 is 1, so reduce the span to the right half - :math:`[72,80)`.
6. Bit2 is 0, so reduce the span to the left half - :math:`[72,76)`.
7. Bit1 is 1, so reduce the span to the right half - :math:`[74,76)`.
8. Bit0 is 0, so reduce the span to the left half - :math:`[74,75)`.

.. [#bindex] We number the bits from right (least significant) to left (most significant).

So we get the same answer as when we convert ``01001010`` from binary into
decimal - i.e. ``64 + 8 + 2 = 74``. So we can look at the binary digits as a
sequence of instructions to traverse a binary tree to the leaf nodes -
``LRLLRLRL``. 

So what? 

If you notice, we started with a "dense" data storage like an array that maps
integer keys to values and came up with a scheme that permits "sparse" data
storage. Our key space can now be large -- like 4 billion -- and still we can
map a small set of these keys to values in under 32 steps.  If you want to make
integer keys to values, you can use an incomplete binary tree where you locate
a leaf by using the bit sequence as traversal instruction. For example, the
branch corresponding to ``Bit6 == 0`` can be entirely absent from the tree
and that would be ok, unlike with an array.

The "trie"
----------

The above way of looking at arrays also permits us to ask the next question -
why should we only look at this as binary choice at each level? After all, we
can club the bits together like this - ``01 00 10 10`` and consider it as a
base-4 number ``1022`` and use each digit to step down one of 4 branches at
each level.

Indeed, nothing really stops us from doing that if for some reason our problem
naturally falls into the base-4 category. But are there problems that naturally
fall into other bases?

Yup - your regular language dictionary is one such data structure. A word like
"hello" consists of a stream of characters each of which can take on one of 26
possible values. So you can look at a word as a number expressed in base 26
and therefore use a 26-way branching tree to arrive at a descriptive meaning
for a given word, in only as many steps as the number of characters in the word.

Such a data structure is called a "trie" and is commonly used for
literally-dictionary like problems such as looking up word meaning or
enumerating words that have a given prefix as with autocomplete features in
devices.

How would you code up a node of such a 26-way trie? It is pretty much the
same as for a binary search tree where we do 26 instead of 2!

.. code-block:: C

    typedef struct TrieNode *trie_t;
    struct TrieNode {
        char ch; // The character of this node.
        void *value;
        trie_t children[26];
    };

If a node is a "leaf node", then none of its children will be non-NULL pointers
and the ``value`` field will give the associated value.

C strings
^^^^^^^^^

Strings in C are not special objects. They are simply an array of ASCII
characters whose last character is the "null character" - i.e. one which has
ASCII value 0 and is notated ``'\0'``. So the string "hello" in C is actually a
sequence of 6 bytes - ``[104, 101, 108, 108, 111, 0]``. Since the null
character is a terminating character, it is not permitted in the middle of
strings ... which, though not ideal, is usually not a problem in much practice.

Furthermore, the type of the string ``"hello"`` is actually ``char *`` - i.e.
a pointer to ``char`` type values which are signed 8-bit numbers. So, we we're
to write a "find value for given key in given trie", we can do it recursively
like this -

.. code-block:: C

    void *trie_get(trie_t t, const char *word) {
        assert(t != NULL);
        // "word[0]" can also be written as "*word"
        // since word[0] == *(word+0) = *word
        if (word[0] != '\0') {
            // The next character is not the null character
            trie_t next = t->children[word[0]];
            if (next == NULL) { return NULL; } // Not found.
            assert(next->ch == word[0]);
            return trie_get(next, word+1); // Step to the next character.
        } else {
            return t->value;
        }
    }

The ``const`` keyword indicates a promise that the ``trie_get`` function
will not modify the characters of the given ``word`` string.

If you notice, the ``trie_get`` call is in the "tail position" and so we
can turn it into a loop easily like this -

.. code-block:: C

    void *trie_get(trie_t t, const char *word) { 
        START:
        assert(t != NULL);
        // "word[0]" can also be written as "*word" 
        // since word[0] == *(word+0) = *word 
        if (word[0] != '\0') {
            // The next character is not the null character
            trie_t next = t->children[word[0]];
            if (next == NULL) { return NULL; } // Not found.
            assert(next->ch == word[0]);
            t = next;
            word = word + 1; // Step to the next character.
            goto START;
        } else {
            return t->value;
        }
    }

... which then becomes -

.. code-block:: C

    void *trie_get(trie_t t, const char *word) { 
        while (1) { // A non-zero integer is treated as "true" in C. 
            assert(t != NULL);
            // "word[0]" can also be written as "*word" 
            // since word[0] == *(word+0) = *word 
            if (word[0] != '\0') {
                // The next character is not the null character
                trie_t next = t->children[word[0]];
                if (next == NULL) { return NULL; } // Not found.
                assert(next->ch == word[0]);
                t = next;
                word = word + 1; // Step to the next character.
            } else {
                return t->value;
            }
        }
        return NULL;
    }

Recall that if ``p`` is a pointer valued variable, then ``p[i]`` is 
equivalent to ``*(p+i)``. This means if we want to get the pointer
to the next element treating ``p`` as an array, we want ``&p[1]``,
which is therefore equivalent to ``&*(p+1)`` and since ``&*`` can
be deleted anywhere, that just become ``p+1``. Therefore we step to
the next character of the string using ``word = word + 1;``.

The hash table
--------------

We saw how we can use 26-way branching "trie" to look up some value associated
with ASCII "words". Our original intention is to map a subset of keys from
a large key space to corresponding values. So if we're dealing with ``N`` keys,
why don't we just use N-way branching so that we'll be done in a single step?

That may appear to be a silly question to ask, but it really isn't. [#sillyq]_
If we can somehow map a key in the larger key space to the set of integers
:math:`[0,N)`, in a sufficiently "spread out" way -- i.e. each of our keys is
equally likely to be mapped to one of the ``N`` indices -- then we may actually
be able to use such a branching strategy!! Such a mapping function is called a
"hash function".

.. [#sillyq] Therefore never stop yourself from asking what you've judged to be
   a "silly question"!

.. index:: buckets

We do have to deal with one problem though ... one known as "collision". 
Where two keys from our original key space get mapped to the same index.
Then we have to decide how to store the two keys. Several strategies are
possible, including using a simple linked list, or even a small tree at
each of these ``N`` branches. A common strategy used is to store a linked list
of key-value associations at each of the ``N`` positions. Since we're storing
many keys in the same index, these are also known as "buckets".

So this is where we are at now -

.. code-block:: C

    struct LLNode {
        int key;
        void *value;
        struct LLNode *next;
    };

    struct HashTable {
        int N;
        struct LLNode **buckets;
    };

The ``buckets`` field is now an array of ``N`` pointers to ``struct LLNode``.
So if our hash function is called ``hash(key, N)``, then we can do a lookup like this -

.. code-block:: C

    void *hashtable_get(struct HashTable *ht, int key) {
        assert(ht != NULL);

        int bucket = hash(key, N); // We let the hash function sort out
                                   // how to limit to the range [0,N).

        struct LLNode *items = ht->buckets[bucket];
        while (items != NULL) {
            if (items->key == key) {
                return items->value;
            }
            items = items->next;
        }

        return NULL;
    }
            

We see how to get items out of such a hashtable with a magic hash function.
How do you add items to it though?

.. index:: load factor

For that, we use the same strategy we used to deal with unbounded arrays.
We keep adding items to these linked lists until we find that
the average length of these linked lists grows beyond a certain threshold.
If we have a great hash function, then this average will be around
``num_elements_in_hash_table / num_buckets`` ... which is called the
"load factor" of the hash table. Most hash table implementations try to
keep the load factor under ``1.0`` and it is a data structure parameter
that can be used to trade off storage space against speed.

So when the load factor goes above the threshold, we can double the number of
buckets. As with unbounded arrays, the cost of doubling the buckets is incurred
once in a while compared to many operations where we use existing buckets, and
therefore we get an amortized cost of ``O(1)`` for hashtable get and set
operations.

Hash functions
^^^^^^^^^^^^^^

So how do we actually construct these magical hash functions?

Consider this one -

.. code-block:: C

    int simple_hash(int key, int N) {
        return key % N;
    }

Is it good enough? It might be, depending on the problem, but one issue
it does have is that nearby keys will be mapped to nearby buckets and
all keys of the form ``k + m*N`` for various ``m`` will all map to the
same bucket. This is a bit too regular and can result in pathological
situations where we end up with ``O(N)`` linear scanning.

.. index:: linear congruential generator

To get a more random looking -- i.e. pattern-less -- spread, we can use
a pseudo random sequence function such as the family of "linear congruential
generators" - which are integer functions of the form -

.. code-block:: C

    int lcgen(int key, int a, int b, int N) {
        return (a + b * key) % N;
    }

Let's look at what ``lcgen`` produces for some values of a, b and N.
Consider :math:`f(x) = 2*x+3 \mod 9`, :math:`g(x) = 3*x+2 \mod 9` and
:math:`h(x) = 5*x+3 \mod 9`.

.. csv-table:: Linear congruential function examples
   :header: "x", "f(x)", "g(x)", "h(x)"
   :widths: auto
   :align: center

    0,3,2,3
    1,5,5,8
    2,7,8,4
    3,0,2,0
    4,2,5,5
    5,4,8,1
    6,6,2,6
    7,8,5,2
    8,1,8,7


All three functions have a period of 9. Recall that the "period of f" is a
value p such that :math:`f(x+p) = f(x)`. But we see that :math:`g(x)` has a
period of 3 as well. In fact, we're asking "when does a*x become divisible by
9" because from when it does, the pattern is bound to repeat. Since the longest
period we can get is 9, we see that we can get that only if the :math:`gcd(a,9)
= 1` - i.e. if and only if a and 9 are "relatively prime" (which is the same as
saying :math:`gcd(a,n) = 1`). Furthermore, we see that choosing small values
for "a" (relative to 9) gives us some obvious increasing patterns whereas
choosing larger values in the middle of the range :math:`[2,9)` gives us a
better -- i.e. more random looking -- sequence. This is because incrementing x
will then give us an even chance of overflowing the modulus and wrapping back.
For example, for :math:`h(x)`, consider the two "low and high" groups
[0,1,2,3,4] and [5,6,7,8]. If :math:`h(x)` is in one of the groups, it will
often get pushed into the other group if you merely increment x. So if the
modulus is large enough, we will likely not be able to figure out the pattern
by looking at the result.

So if we have an integer key k, and we want to assign it to one of n buckets,
we can use an appropriate linear congruential function like :math:`h(x)` with
:math:`gcd(a,n) = 1` to assign a bucket for it. If we choose "a" wisely, and
"b" is not too much of a concern except that it must be :math:`> 0`,  we can
get a pretty good behaviour where :math:`h` can scatter its domain values over
the co-domain in a random-looking manner.

In random number generators, you typically use such a :math:`h(x)` iteratively
to make the sequence :math:`h(x)`, :math:`h(h(x))`, :math:`h(h(h(x)))` and so
on. When doing this, you'll want "b" to be relatively prime to "n" as well. In
our case with :math:`h(x)`, we see that :math:`h(0) = 3`, :math:`h((h(0)) = 0`,
:math:`h(h(h(0))) = 3`, ....  and it keeps oscillating between 0 and 3.  To
prevent that, we can choose :math:`h(x) = 5*x+7 \mod 9`, in which case we get
the series :math:`[0,7,6,1,3,4,0,7,6,...]` which has a period of 6. We can see
that :math:`h^k(x) = 5^k x + (5^{k-1} + 5^{k-2} + ... + 1) \times 7 \mod 9`.
When taking remainder on dividing by 9, the powers of 5 make the series
:math:`[1,5, 7,8,4,2,1,5,7,8,4,2,...]` which has a period of 6, which shows up
when we iterate :math:`h(x)` starting from :math:`x = 0`. Those mathematically
inclined can read up on "linear congruential generators" here -
https://en.wikipedia.org/wiki/Linear_congruential_generator (Links to an
external site.) . You don't need to know their mathematics for this course ...
just that with appropriate choice of a and b and n, we can get a fairly random
looking bucket choice for our hash function.

Hash functions are central in cryptography, where both the domain and co-domain
are large for practical purposes. The domain can be a whole document (which in
essence is a number if you concatenate all of its bits) and the co-domain is a
number in the range :math:`[0,2^k)` where k is chosen to be something like 256
or 512. The additional constraint for cryptographic hash functions is that
collisions must be so extremely unlikely that it should be impractical to
determine another input which produces the same hash by mere enumeration within
a reasonable amount of time - i.e. if :math:`h(x_1) == h(x_2)`, for a
cryptographic hash :math:`h(x)`, then we can be nearly certain than :math:`x_1
== x_2`, given that :math:`x_1`, :math:`x_2` and so on are values produced by
humans.

A common choice of :math:`h(x)` for 32-bit integers is

.. math::

    h(x) = 1664525 * x + 1013904223 \mod 2^{32}

since the :math:`\mod 2^{32}` part can be done by just retaining the lowest
32-bits of the integer and no division is required.

.. Note:: If you want a random number in a smaller range like :math:`[0,32)`,
   it is tempting to do ``h(x) % 32`` or ``h(x) & 31`` which masks off the
   lower 5 bits.  While this is simple, there is "less randomness" in the lower
   bits of such a :math:`h(x)`. So it is better to scale :math:`h(x)` using
   :math:`32 * h(x) / hmax` (either doing it using a wider integer or using
   floating point). It can be somewhat counter-intuitive that there is a notion
   of "quality" of random number generators ... but there is, and the whole
   area is fascinating for study and underpins much of statistical calculations
   on computers such as Monte Carlo simulations, and we already mentioned
   cryptography.


