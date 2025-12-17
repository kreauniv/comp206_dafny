Binary search
=============

.. NOTE:: Between this section and the preceding one, we've already looked at
   stacks, queues, (some) sorting and the notion of computational complexity. 
   I discuss binary search here as a leap off point for tree algorithms and
   linked lists. We'll return to some simple sorts along the way.

We saw earlier that sorting is an operation that can help speed up searching
through an array very fast using the binary search algorithm.

While we'll be illustrating sorting and searching using integer arrays,
in practice we're not so interested in integer data for these purposes.
However, we very often want to use some kind of an ordered key like an
integer that we associate with a value and we want to find the value given
the key.

Let's look at a structure that reflects this reality better. Having done
stacks already, the following approach should look familiar. I'm omitting
contracts here in the interest of brevity and since you're already familiar
with these patterns by now. [#conts]_

.. [#conts] In the code in this section, you'll find that contract lines are
   not terminated by semicolons. Please do add them yourselves. I excluded
   them since a bug in the documentation generator tool falsely declares that
   it is not valid C code with the semicolon in there. I'll revert this code
   to correctly include the semicolons once I've fixed that bug.

.. code-block:: C

    struct Assoc {
        int key;
        void *value;
    };

    typedef struct KeyValAssoc *dictionary_t;

    struct KeyValAssoc {
        struct Assoc [] table;
        int length;
        int capacity;
    };

    dictionary_t make_dict() {
        dictionary_t d = alloc(KeyValAssoc);
        d->capacity = 1;
        d->table = alloc_array(Assoc, d->capacity);
        d->length = 0;
        return d;
    }

    int dict_size(dictionary_t d) {
        return d->length;
    }

    void *dict_get(dictionary_t d, int key) {
        // This is a linear scan through the array and
        // therefore of complexity O(N) where N is the
        // number of key-value pairs in the dictionary.
        for (int i = 0; i < d->length; i++) {
            if (d->table[i].key == key) {
                return d->table[i].value;
            }
        }
        return NULL; 
        // Question: How will we distinguish between "No such key
        // in the table" and "This key is associated with a NULL
        // value"? What assumptions are we making by returning 
        // NULL here?
    }

    void dict_ensure_capacity(dictionary_t d)
        //@requires dict_is_valid(d)
        //@ensures d->length < d->capacity
    {
        // If we don't have enough space, make more.
        if (d->length == d->capacity) {
            int newcapacity = d->capacity * 2;
            struct Assoc [] newtable = alloc_array(struct Assoc, newcapacity);
            for (int i = 0; i < d->length; i++) {
                newtable[i] = d->table[i];
            }
            d->table = newtable;
            d->capacity = newcapacity;
        }
    }
        
    void dict_set(dictionary_t d, int key, void *value) {
        // Check if key is present. If present, replace the value there.
        for (int i = 0; i < d->length; i++) {
            if (d->table[i].key == key) {
                d->table[i].value = value;
                return;
            }
        }

        dict_ensure_capacity(d);

        //@assert d->length < d->capacity
        d->table[d->length].key = key;
        d->table[d->length].value = value;
        d->length = d->length + 1;
    }


Observe that for this "interface" to a key-value storage structure, there is
no information about the ordering of the internal structures that is exposed via
the available functions. The entire set of functions are listed below.

.. code-block:: C
    
   dictionary_t make_dict();
   int dict_size(dictionary_t d);
   void* dict_get(dictionary_t d, int key);
   void dict_set(dictionary_t d, int key, void* value);

That's it. If we just wanted a small key-value store for 10s of items, this
might even be viable without imposing too much cost. However, the cost of
the ``dict_get`` operation, which is :math:`O(N)` as noted, makes some kinds
of usage patterns very expensive, making a linear search unviable in many
contexts.

However, if we place the constraint that we always keep the table sorted by
the key, then we can speed up ``dict_get`` to be :math:`O(\text{log} N)` which is
a fantastic speed up. 

.. code-block:: C

    void *binary_search(dictionary_t d, int from, int to, int key);

    void *dict_get(dictionary_t d, int key) {
        return binary_search(d, 0, d->length, key);
    }

    void *binary_search(dictionary_t d, int from, int to, int key) {
        if (from >= to) { return NULL; }

        int mid = (from + to) / 2;
        if (d->table[mid].key == key) { return d->table[mid].value; }
        if (d->table[mid].key > key) { return binary_search(d, from, mid, key); }

        //@assert d->table[mid].key < key
        return binary_search(d, mid+1, to, key);
    }

Going from :math:`O(N)` to :math:`O(\text{log} N)` is huge improvement.
However, we've now complicated the task of maintaining the table sorted.
So when we now insert a new element into the table, we have to move things
around so it remains sorted. So ``dict_set`` unfortunately remains :math:`O(N)`
in the worst case. In the previous implementation, ``dict_set`` had to do
a linear search to first figure out if the key was present, which is an
:math:`O(N)` operation. But if it determined it to be absent, the insertion
step was :math:`O(1)`. Now, that insertion step itself becomes :math:`O(N)`
since we need to move things around.

.. admonition:: Exercise

   Implement a ``dict_set`` that maintains the table in a sorted form when
   a new key-value pair is inserted.


Can we get around this and make ``dict_set`` also faster? To understand how,
we need to look deeper into the relationship between binary search as an 
algorithm and the data pattern that it is relying on to do its job.

Binary search trees
-------------------

When doing binary search on our sorted table, for each range, we focus
on one entry - the midpoint. Based on the key at the midpoint, we branch
either to the left or to the right if the key we're looking for 
does not happen to match the one at the mid point. The "left" and "right"
ranges can themselves now be represented with their respsective midpoints
as the focus of search. We therefore get two sequences as shown in the 
table below. We place "-1" to indicate there are no more left or right
branch intervals to explore.

In the below table for a 10-sized dictionary, the ``mid`` is calculated as
``(from + to)/2``, the ``left-mid`` is calculated as ``(from+mid)/2`` and the
``right-mid`` is calculated as ``(mid+1+to)/2`` .. according to the left/right
range calculations in the binary search algorithm above.

.. csv-table:: Index sequence in binary search
   :header: range, left-range, right-range, mid, left-mid, right-mid
   
   [0-10), [0-5), [6-10), 5, 2, 8
   [0-5), [0-2), [3-5), 2, 1, 4
   [6-10), [6-8), [9-10), 8, 7, 9
   [0-2), [0-1), [], 1, 0, -1
   [3-5), [3-4), [], 4, 3, -1
   [6-8), [6-7), [], 7, 6, -1
   [9-10), [], [], 9, -1, -1
   [0-1), [], [], 0, -1, -1
   [3-4), [], [], 3, -1, -1
   [6-7), [], [], 6, -1, -1

In the above table, there is one entry for every element in the dictionary
which assures us of the completeness of the search procedure. The interesting
point here is that we don't need to perform the mid point calculations if
we already know the values of ``left-mid`` and ``right-mid`` columns when we're
examining a given ``mid`` value. We can turn those columns into simple ``int``
arrays like this -

.. code-block:: C

    struct KeyValAssoc {
        int capacity;
        int length;
        struct Assoc [] table;
        int [] left;
        int [] right;
    };

.. admonition:: Exercise

   Implement a function ``make_index(dictionary_t d, int from, int to)`` that fills
   the ``left`` and ``right`` index arrays of the dictionary, assuming that the table
   is sorted in ascending order by the integer ``key`` field of ``struct Assoc``.

If the ``left`` and ``right`` arrays were correctly initialized, then our ``binary_search``
becomes --

.. code-block:: C

    void *binary_search(dictionary_t d, int start, int key)
        //@requires dict_is_valid(d)
        //@requires start == -1 || (start >= 0 && start < d->length)
    {
        if (start == -1) { return NULL; } // Not found.

        if (d->table[start] == key) { return start; } // Found.

        // Left branch.
        if (d->table[start] > key) { return binary_search(d, d->left[start], key); }
        
        // Right branch.
        //@assert d->table[start] > key
        return binary_search(d, d->right[start], key);
    }

We gain something interesting with this structure. If we want to add one more key-value
association, we no longer need to shift the array items around in an :math:`O(N)` manner.
We can simply use the ``left`` and ``right`` arrays and adjust the indices to include
the new entry. 

.. code-block:: C

    // We're assuming there's enough capacity because you know how to deal
    // with that. You can write a separate dict_ensure_capacity(d) function
    // that will make sure length < capacity at the end by reallocating
    // the array with a larger capacity if necessary.
    bool binary_search_set(dictionary_t *d, int start, int key, void *value) 
        //@requires dict_is_valid(d)
        //@requires start >= 0 && start < d->length && d->length < d->capacity
    {
        if (d->table[start].key == key) {
            // Found. Set the value and wrap up.
            d->table[start].value = value;
            return true;
        }

        if (d->table[start].key > key) {
            // Left branch.
            int l = d->left[start];
            if (l > -1) {
                return binary_search_set(d, l, key, value);
            }
            dict_ensure_capacity(d);
            return insert_key_value_left(d, start, key, value);
        } else {
            int r = d->right[start];
            if (r > -1) {
                return binary_search_set(d, l, key, value);
            }
            dict_ensure_capacity(d);
            return insert_key_value_right(d, start, key, value);
        }
    }

    bool insert_key_value_left(dictionary_t *d, int parent, int key, void *value)
        //@requires dict_is_valid(d)
        //@requires d->length < d->capacity
        //@requires parent >= 0 && parent < d->length
        //@requires d->left[parent] == -1 && d->table[parent].key > key
        //@ensures d->table[d->length-1].key == key
        //@ensures d->table[d->length-1].value == value
        //@ensures d->left[parent] == d->length-1
        //@ensures d->left[d->length-1] == -1 && d->right[d->length-1] == -1
    {
        int n = d->length;
        d->length = d->length + 1;
        d->table[n].key = key; // YAY! No need to shift blocks of array entries!
        d->table[n].value = value; 
        d->left[n] = -1;
        d->right[n] = -1;
        d->left[parent] = n;
        return true;
    }
        
    bool insert_key_value_right(dictionary_t *d, int parent, int key, void *value) 
        //@requires dict_is_valid(d)
        //@requires d->length < d->capacity
        //@requires parent >= 0 && parent < d->length
        //@requires d->right[parent] == -1 && d->table[parent].key < key
        //@ensures d->table[d->length-1].key == key
        //@ensures d->table[d->length-1].value == value
        //@ensures d->right[parent] == d->length-1
        //@ensures d->left[d->length-1] == -1 && d->right[d->length-1] == -1
    {   
        int n = d->length;
        d->length = d->length + 1;
        d->table[n].key = key; // YAY! No need to shift blocks of array entries!
        d->table[n].value = value; 
        d->left[n] = -1;
        d->right[n] = -1;
        d->right[parent] = n;
        return true;
    }    
        
                
    void dict_set(dictionary_t d, int key, void *value) {
        binary_search_set(d, d->length/2, key, value);
    }

Problems, problems, problems!
-----------------------------

That all sounded nice on paper, but we have a problem! 

.. admonition:: The problem!

   The structure of the elements in our dictionary is now dependent on the order in
   which we insert key-value pairs!! The reason we should be alarmed by this is
   that if we somehow intend the collection to remain organized, this cannot be true,
   at least it must "mostly" remain independent of the insertion order.

To understand this, consider how we'd build such a binary tree for 
a sequence of elements ``[10,20,30,40,50,60,70,80,90,100]``. 
If we start with ``10``, then the next value goes to the right
of 10. The value after goes to the right of 20 and so on, so each
node in our "binary tree" now only has a right branch and the left
branch is left unused. This is, in essence, a "linked list" which
has :math:`O(N)` lookup complexity, though addition is (amortized)
:math:`O(1)` at a known point in the list ::

    10
     \_ 20
         \_ 30
             \_ 40
                 \_ 50
                     \_ 60
                         \_ 70
                             \_ 80
                                 \_ 90
                                     \_ 100

The trick is to keep the tree "balanced" as we add new elements, so that
the depth of the tree remains :math:`O(\text{log} N)` as we add more and
more elements. We'll look at this balancing process as part of the "AVL tree"
structure and procedures in the next part.

Where did the pointers go?
--------------------------

We wrote the tree using two integer arrays now, but in an earlier instance,
we'd defined an expression tree where the expression parts were pointers.
These two ways of representing a tree are equivalent - i.e. you understand
one, you understand the other.

In other words, the pointer representation is as though the pointer is an
integer index into a global large array of bytes that is all of the memory
available to your program.

Just as you can include the ``left`` and ``right`` indices into your
``struct Assoc`` like below ::

    struct Assoc {
        int key;
        void *value;
        int left;
        int right;
    };

which will permit you to get rid of the explicit ``int [] left`` and ``int [] right``
in the dictionary structure, you can turn these into pointers and get rid of the
array itself as shown below ::

    struct Assoc *tree_t;

    struct Assoc {
        int key;
        void *value;
        tree_t left;
        tree_t right;
    };

Now you can treat your whole dictionary as just a pointer to the root node.
There's one subtlety there -- we may need to create a "dummy" root node
in order to represent the possibility of an empty dictionary. Otherwise
we will not have anything to add new elements to.

.. NOTE:: Throughout this, notice that we can continue to maintain the same
   set of functions to operate the dictionary while we play around with ways
   to implement it "under the hood". This set of functions constitutes the
   dictionary's "interface" and this "interface-implementation" separation is
   one of the corner stone techniques to manage complexity in large programs.
   A piece of code that uses a structure like a dictionary must not make
   any assumptions about how it is implemented and only rely on the contracts
   provided by its interface functions. This is pretty much the definition of
   "modularity".

Next up - balanced binary trees and balancing techniques.


Exercises
---------

In all these, write the contracts too.

1. Implement a function that computes the ``left`` and ``right`` arrays 
   given a binary search interval of length ``N`` (given as a half-open 
   interval :math:`[\text{from},\text{to})`. It should place ``-1`` as
   the value for those indices that don't have left/right children. For
   what the left/right arrays should contain, refer to the table in
   the previous section::

       void build_left_right_indices(int from, int to, int [] left, int [] right);

2. Implement a function that uses the ``left`` and ``right`` arrays a computes
   a "tree depth" given a starting point. The tree depth is defined as the maximum
   number of times you can step into the left or right branches until you hit a
   ``-1`` value::

        int tree_depth(int [] left, int [] right, int start);

3. Implement a function which when given the left/right arrays and a start position,
   checks that all the reachable nodes meet the ordering property - i.e.
   that all nodes to the left of a node have values less than the node value,
   and all nodes to the right of a node have values greater than the node value::

        bool check_ordering(int [] left, int [] right, int start);

4. Compute some simple "tree traversals" to get a feel of the common recursive
   traversal pattern - 

    a. Find the maximum value of all reachable nodes.
    b. Find the sum of all reachable nodes.
    c. Find the count of all reachable nodes.

5. Can you generalize (using the C1 language extension) the above traversals so
   we can use the same generic "traverse" function that takes a function argument
   that tells what to compute while traversing?

6. Given a sorted list of numbers ``[1,2,3,4,5,6,7,8,9,10]``, in what order
   should we insert these elements using the naive tree insertion function we
   wrote earlier? Can you generalize it to an array of ``N`` elements?



