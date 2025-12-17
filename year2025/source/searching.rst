Searching
=========

Having introduced arrays as a representation for storing and manipulating
sequences of values, we now venture into some common algorithms using arrays -
searching and sorting.

Preliminaries
-------------

When working with collection types like arrays and structures, it is useful to
create a number of utility functions for producing some instances of these
structures so that we can test our algorithms on them. We'll start with
making a random array of length ``n``. This is itself instructive in that
we'll be making a small "library" that can be reused in our programs.

The ``rand`` library provided by C0 (see `c0-libraries.pdf`_)  gives us
the facility we need to make random numbers that we can use to make
random arrays. This library provides two functions ::

    rand_t init_rand(int seed);
    int rand(rand_t);

The first function creates a structure that will hold the state of our
random number generator. This is a "pseudo" random number generator, meaning
it produces a sequence based on a purely determinstic process, just that the
sequence "looks random" to someone who doesn't know the innards of the
algorithm. This is often sufficient for most purposes. The ``seed`` parameter
is any number of your choice and is used to determine which of the possible
sequences will be produced subsequently. You can choose a seed based on the
current time, for example, to make the sequence quite unpredictable, or you
can fix the seed because you're debugging your program and you want total
predictability of the sequence. Completely your choice.

Every time we call the ``rand`` function, passing it the ``rand_t`` structure
created by ``init_rand``, ``rand`` will produce the next number in the sequence
and store an updated state inside ``rand_t``. The details of ``rand_t`` and how
it is implemented are not relevant for our purpose currently, but this library
is indicative of how to structure such a utility and expose only "how to use it"
without exposing "how it is made". This abstraction principle underlies pretty
much all of programming activity.

The ``rand`` function produces a random integer, spanning the full range of
integers. If you look at the contracts for ``rand``, you see that there is no
``@ensures`` contract on the result, meaning the result can be any valid C0
integer. This indicates one logistical problem with contracts - you cannot
distinguish between "there are no constraints on the result" and "the library
author forgot to specify constraints on the result". We'll live with this
and supplement it with tests for now.

Place the following code in a separate file named ``myutils.c0``.

.. code:: C

    #use <rand>

    // This function makes an array of ``n`` random numbers in the range
    // [range_start, range_end] which includes both ends of the range.
    // The given random number sequence generator is used.
    int[] make_random_array(rand_t gen, int n, int range_start, int range_end)
    //@requires gen != NULL && n > 0 && range_end >= range_start;
    //@ensures \length(\result) == n;
    {
        int[] a = alloc_array(int, n);
        // Since n is used here, and alloc_array constrains n to be > 0, 
        // it also becomes part of the outer function's preconditions.

        int mod = range_end - range_start + 1;

        for (int i = 0; i < n; i++)
        //@loop_invariant i >= 0;
        {
            a[i] = range_start + ((rand(gen) % mod) + mod);
        }

        return a;
    }

Now, you can use this "library function" in another file "test.c0" as follows ::

    #use "myutils.c0"
    #use <rand>
    #use <conio>

    bool test_array() {
        rand_t gen = init_rand(123);
        int [] a = make_random_array(gen, 10, 1, 100);
        for (int i = 0; i < 10; i++) {
            printint(a[i]);
            print("\n");
            flush();
        }
        return true;
    } 
            
Now, you can do ``coin -d test.c0`` and try out your new random array making
function by typing ``test_array();`` on ``coin``'s prompt. Note that every time
you run ``test_array();``, you get the same set of numbers printed out. This
is because we use the same seed inside ``test_array()``. Modify ``test_array``
to take the seed as an argument. Now try different seed values.

.. admonition:: **Exercise**

    Go forth and make your own library functions! You can keep building your
    own library for use in your assignments and submit them alongside too.
    

Linear search over an unsorted array
------------------------------------

Functions that work with arrays can make assumptions about them in order to
gain efficiencies where possible, but usually also need to make some provision
for when those assumptions are broken. So the first order of business is to 
be able to do things with unsorted arrays. We already took the first step 
by making an unsorted array of random numbers. So let's find whether a given
integer is present in an array and then use our random array maker function
to test it.

.. code:: C

    // We first write a function that can test whether a given
    // element is in a range, without telling us where it was found.
    // This is slightly simpler than the ``linear_search`` function below.
    bool is_in_range(int []a, int x, int start_index, int end_index)
    //@requires a != NULL;
    //@requires start_index >= 0 && end_index >= start_index && end_index <= \length(a);
    {
        if (start_index < end_index) {
            if (a[start_index] == x) {
                return start_index;
            } else {
                return is_in_range(a, x, start_index+1, end_index);
            }
        } else {
            return -1;
        }
    }

    // Our unsorted linear search function is expected to return the index
    // at which the given value is found in the array, or -1 if the given
    // value is not present in the array. We search the range of indices
    // from start_index up to but not including end_index.
    int linear_search(int []a, int x, int start_index, int end_index)
    //@requires a != NULL;  
    //@requires start_index >= 0 && end_index >= start_index && end_index <= \length(a);
    //@ensures \result == -1 || (\result >= 0 && \result < \length(a) && a[\result] == x);
    {
        for (int i = start_index; i < end_index; i++)
        //@loop_invariant i >= start_index;
        //@loop_invariant i == start_index || !is_in_range(a, x, start_index, i)
        {
            if (a[i] == x) {
                return i;
            }
        }
        return -1;
    }


.. admonition:: **Exercise**

    Rewrite the functions in this section using recursion instead of a loop.
    What can you say about the correctness of the resulting function?

.. admonition:: **Exercise**

    Modify the above ``linear_search`` function so that it returns a structure
    containing two values - one being the result index and the other being the
    number of times the function went through the loop to find the value.. or not.



Linear search over a sorted array
---------------------------------

What can we do differently if we can assume that we have a sorted array (say in 
ascending order)? First we need to develop the notion of what a "sorted array"
is, so we can test for it in the preconditions.

Mathematically speaking, an array :math:`a` of :math:`n` elements is said to be
sorted if we have a binary relation :math:`<` using which we can compare the
elements of the array, and for every pair of integers :math:`(i,j)` such that
:math:`0 <= i < n \text{ and } 0 <= j < n`, :math:`i \le j \Leftrightarrow a[i] \le a[j]`.

To make this notion easy to compute, we can infer that that description is
equivalent to requiring that :math:`a[i] \le a[i+1]` for all :math:`1 \le i+1 < n`.
We can express that in a test function as follows ::

    bool is_sorted(int []a, int start_index, int end_index)
    //@requires a != NULL;
    //@requires start_index >= 0 && start_index <= end_index && end_index <= \length(a);
    {
        if (start_index + 1 < end_index) {
            if (a[i] > a[i+1]) {
                return false;
            }
            return is_sorted(a, start_index+1, end_index);
        }
        return true;
    }


Given the test function above, we can write our linear search over a sorted
array as ::

    int linear_search(int []a, int x, int start_index, int end_index)
    //@requires is_sorted(a, start_index, end_index);
    //@requires start_index >= 0 && start_index <= end_index && end_index < \length(a);
    //@ensures \result == -1 || (\result >= 0 && \result < \length(a) && a[\result] == x);
    {
        for (int i = start_index; i < end_index; ++i)
        //@loop_invariant i == start_index || a[i-1] < x;
        {
            if (a[i] == x) {
                return i;
            } else if (a[i] > x) {
                return -1;
            }
        }
        return -1;
    }

Notice how we can slightly improve the situation by testing whether ``a[i] >
x``. If that happens to be true, then because the array is sorted, we know that
every other element of ``a`` that we'll examine after index ``i`` will also
satisfy ``a[i] > x`` and therefore we can abort the search immediately.

.. admonition:: **Exercise**
   
    Explain the ``@ensures`` expression and show how it expresses correctly
    the requirements of ``linear_search``. In particular, explain how all parts
    of the expression are evaluated such that no safety conditions are violated.

.. admonition:: **Exercise**
   
    Prove the safety and correctness properties of the above ``linear_search``
    function. Partial hint: Show that the ``@loop_invariant`` helps establish
    the correctness.

.. admonition:: **Exercise**
   
    Rewrite the ``is_sorted`` function to use an approach where we declare
    the given range to be sorted if the first half and the second half of the
    range are both sorted and the last item of the first half is less than
    or equal to the first item of the second half. Is this approach "better"
    in any way? 

.. admonition:: **Exercise**
   
    Rewrite the ``linear_search`` to return, as in an earlier exercise, a structure
    containing two values - the found index (which can be -1 if not found), and 
    the number of times the loop was entered.

.. admonition:: **Exercise**

    What can you learn about the structure of the ``linear_search`` function if
    you rewrite it using recursion instead of a loop?

Binary searching a sorted array
-------------------------------

The latest ``linear_search`` function reduces the search range by one element
each time the loop is entered. We can exploit the sortedness of the array
quite a bit more than that - the observation is that if the given ``x`` is
less than the value in the middle of the array, we don't need to bother searching
the second half. Similarly if it is greater than the middle value of the array,
we don't need to bother searching the first half.

.. admonition:: **Exercise**

    Write a ``binary_search`` function using recursion that works using the
    observation described, along with its contracts and prove its safety and
    correctness. What can you say about this function relative to linear
    search over a sorted array?



.. _c0-libraries.pdf: http://w.twelf.org/doc/c0-libraries.pdf
