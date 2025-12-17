Sorting - an information theoretic introduction
===============================================

We've seen how having an array of keys (integers in our case) in sorted order
helps make *information retrieval* fast. So it is no surprise that algorithms
that take an unsorted array and turn it into sorted form -- ie. sorting
algorihtms -- are a focus of study in computer science.

Before we look at the algorithms themselves, let's consider the nature of
the problem. We first look at the notion of "information" introduced by
Claude Shannon.

Shannon's information theory
----------------------------

We won't be dealing with this theory in any measure of completeness, but
a gist of it that is sufficient for us to consider the sorting problem.

We talk about "information" as though it were a quantity much like energy, or
pressure, but we lacked a quantitative way to look at information until Shannon
came along with his proposal. The gist of his proposal is to consider the state
of our knowledge about a system and quantify what we need to know about the
system in order to eliminate all lack of information about it.

Consider an unbiased coin - i.e. one that flips to head or tail about evenly --
i.e. without bias. If you flip the coin :math:`N` times, where :math:`N` is a
suitably large number, about half of the flips will turn up heads and the other
half tails.

.. admonition:: Exercise

   Write a function using your favourite pseudo random number generator which
   when given an ``N`` returns the number of times heads turned up when you
   used it to decide the outcome of flipping a coin ``N`` times. Do you find it
   unbiased? Return the result from the function as a percentage value and see
   if it gets closer to 50.

Supposing I have a coin in my hand, I flip it and clap it shut in my hand.
I take a peek at it and so I know what turned up. But you're watching me
and didn't get to peek at the coin. We can say that there is now some
"information discrepancy" between us. I know something you don't. For
such a two-faced coin flip, we say that this information discrepancy is
"1 bit" worth of information. We can fix this discrepancy if I utter
a single syllable "head" or "tails".

Supposing I flip :math:`N` such two-faced unbiased coins and hide the result
from you while I peek at them, then we say that the information discrepancy
between us is :math:`N` bits, because we can fix the discrepancy by uttering a
sequence of N syllables consisting of "head" or "tails". This way we quantify
information with the most common unit being "bits". We just declared this
"information quantity" to be additive in nature. Let's explore this a bit more
(pun not intended).

With :math:`N` coin flips, we see that there are :math:`2^N` possible outcomes.
So you could say that the information discrepancy between people who know
the outcome and those who don't is :math:`\text{log}_2(\text{possible outcomes})`.
This is because we can play a game of "which half?" to communicate which
of the :math:`2^N` outcomes I encountered to communicate it, and it'll take
:math:`\text{log}_2(\text{possible outcomes})` syllables to communicate it.
So if we toss a 6-faced unbiased cube instead, the information discrepancy would
turn out to be :math:`\text{log}_2(6)` bits.

Supposing we toss such a 6-faced die and we classify the faces 1, 2 and 3 as
"a" and 4, 5 and 6 as "b" and we concern ourselves with only whether the
outcome was in category "a" or category "b", we see that this ought to be
equivalent to a coin flip and therefore the information discrepancy about which
category turned up ought to be 1 bit.

Now suppose instead we classify faces 1 and 2 as "a" and 3, 4, 5 and 6 as "b",
what should we take the information discrepancy to be? We know overall that we
have :math:`\text{log}_2(6)` bits of information. If we know that category "a"
turned up, then we know there is 1 more bit of information to be gained (1 of 2
possibilities, so :math:`\text{log}_2(2)`).  If we know that "b" turned up, we
know that there are 2 more bits of information to be gained (1 of 4
possibilities, so :math:`\text{log}_2(4)`). So the average information left to
be gained is :math:`(2/6)\text{log}_2(2) + (4/6)\text{log}_2(4)`.
Since the total information is :math:`\text{log}_2{6}`, we may say that the
information gained in knowing about whether the category "a" turned up or
"b" is given by :math:`\text{log}_2{6} - (2/6)\text{log}_2(2) + (4/6)\text{log}_2(4)`,
which simplifies to :math:`-\{(2/6)\text{log}_2(2/6) + (4/6)\text{log}_2(4/6)\}`.

This gives us the full picture when we're considering :math:`N` possible
outcomes where each outcome :math:`i` has probability :math:`p_i` (such that
:math:`\sum_i{p_i}=1`), we may state the average information gained by knowing
which one of these turned up is given by --

.. math::

   \text{Information} = -\sum_i{p_i\text{log}_2(p_i)} = 
   -(1/\text{ln}(2))\sum_i{p_i\text{ln}(p_i)}

The above is "Shannon's information formula". One thing to note is that
if the probability of an outcome is :math:`0`, then it looks like we might
have an infinity lurking in there, since :math:`\text{ln}(0)` is infinite.
However, we need to consider not :math:`\text{ln(p)}` near :math:`0^+`, but
:math:`\lim_{p \to 0^+} {p\text{ln}(p)}`. It is easy to see that this
limit is :math:`0` though, using `L'Hospital's rule`_ on the limit --

.. math::

   & \lim_{p \to 0^+} {p\text{ln}(p)} \\
   & = \lim_{p \to 0^+} {\frac{\text{ln}(p)}{1/p}} \\
   & = \lim_{p \to 0^+} {\frac{(1/p)}{-(1/p^2)}} \\
   & = \lim_{p \to 0^+} {-p} \\
   & = 0

.. _L'Hospital's rule: https://en.wikipedia.org/wiki/L%27H%C3%B4pital%27s_rule

.. NOTE:: If the logarithm is to the base :math:`e` in Shannon's formula,
   the unit of information is called a "nit" since log to the base :math:`e`,
   written as :math:`ln(x)` is called the "natural logarithm".

Shannon's notion of information is deeply connected to the concept of "entropy"
in physics. You could say they are both the same thing .. which can be
surprising since entropy emerged from thermodynamic considerations at first in
physics .. and is generally considered to be a measure of "disorder" in a
system. Squint a little bit and you may see how the "disorder in a system" can
be viewed as "lack of information about a system".

What has all that got to do with sorting?
-----------------------------------------

When we have an array of unsorted items, we consider that we wish to reduce
any possible ordering of these items into exactly one ordering as determined
by the comparison function of choice - i.e. "greater than" or "lesser than".

The number of possible ways to order a sequence of :math:`N` items is
:math:`N!` (i.e. factorial of :math:`N`), assuming all of them are different
(which we'll assume for now to simplify our analysis).

So if we consider that the given array of items can be in any one of these :math:`N!`
orders with equal probability before we set out to sort it, we might say that
the information discrepancy between the unsorted array and the sorted array
(of which there is exactly one configuration), is :math:`\text{log}_2(N!)`.

From `Stirling's approximation`_, we know that that is approximately
:math:`N\text{log}_2(N) - N/\text{ln}(2)`.

We'll ignore the second part and consider that the first part dominates for
large :math:`N`.  Note that in practice this is not that accurate an assumption
since :math:`N` will have to be really large for this to happen, considering
that if :math:`N` is about 1 billion, then :math:`\text{log}_2(N)` is only
about :math:`30`. However, if we take our job of reducing the possibilities to
be a little harder than it actually is, then our analysis would be ok. In that
spirit, we'll just consider the :math:`N\text{log}_2(N)` part as the (slightly
pessimistic measure) of information content in the ordering of an unsorted
array.

So where are we now? We now know that we need to siphon out about
:math:`N\text{log}_2(N)` bits of information from the ordering of an unsorted
array in order to get it into sorted form. This means that if we have an
algorithm that can reduce the information discrepancy by 1 bit in each step,
we'll need :math:`N\text{log}_2(N)` steps to get to our end point.

Siphoning off 1 bit of information
----------------------------------

So what does reducing the uncertainty in ordering by 1 bit mean? You could, for
example, take one of the :math:`N` elements and determine whether it will
feature in the left-half of the sorted array or in the right-half.  It is easy
to see that this would improve ordering by 1 bit, since if we didn't manage to
do it, the array is about as unsorted as it was before, and we only have
information about one of two possibilities.

So if we take the :math:`N` elements and determine whether each one of them
is going to occur in the left-half or the right-half, then we would have
eliminated the information discrepancy by :math:`N` bits.

Can we verify that in some other way? Supposing we start with the sorted
array and scramble the left-half and right-half independently, how many
combinations can we end up with? That would be --

.. math::

   (N/2)! \times (N/2)!

So the remaining information discrepancy after determining the left-right
affinity for each element would be --

.. math::

   & \text{log}_2((N/2)! \times (N/2)!) \\
   & \approx 2 (N/2) \text{log}_2(N/2) \\
   & = N \text{log}_2(N/2) \\
   & = N\text{log}_2(N) - N

i.e. We started off with an information discrepancy of :math:`N\text{log}_2(N)`
and we've reduced it indeed by :math:`N` bits. 

What might be a procedure to determine which half each element should wind up in?

A candidate procedure for this is to pick an element at random (call it the
"pivot") and to put everything that is less than this into the left bucket and
everything greater than this in the right bucket. Once that is done, we now
know for sure what the position of this pivot in the final sorted array is,
except that the left part and the right part remain unsorted.

We can now apply the same procedure to the left-half (half in the average case)
and right-half separately and recursively until we end up with all elements
in their rightful places.

This is the "quicksort" procedure. 

Sorting considerations
----------------------

Before we dive deeper, it is good to understand some common considerations
for sorting algorithms. 

1. We're not usually interested in sorting integer arrays. We usually have
   an array of records and we want to sort according to some field of these
   records - which we can call the "key". 

2. Given that we're usually sorting records based on a key, it is quite
   possible that two unequal records may have the same key. In such cases, we
   usually want to preserve the order in which these records occurred in the
   unsorted list after the list gets sorted. This is because we way want to
   perform some other computation that might rely on this ordering that we'd
   deliberately prepared in the first place. A sorting algorithm that preserves
   this record ordering is said to be "stable", and one that doesn't preserve
   this ordering is said to be "unstable".

3. The main cost factor of a sorting algorithm is therefore the step of
   comparing two records to determine their relative order in the final sorted
   array. We wish to minimize the number of comparisons we need to make.

4. If we tweak our "quicksort" description to collect all the records
   with equal keys into a "middle array" in the same order they were found in 
   the original array, we can get a "stable quicksort". Otherwise, the stability
   will depend on a number of implementation factors. In all those cases though,
   the compleixty analysis will remain the same.

5. A sorting algorithm may behave well in the average case, but may show high
   complexity with specific input patterns - especially when the sequence is
   partially sorted. So it is important to address these "pathological cases".

6. While we discussed the complexity of computation, there is also the issue of
   memory usage. Sorting algorithms that can sort an array of records in-place
   instead of requiring additional memory are desirable since they can be used
   in resource constrained environments.

Pathological cases in quicksort
-------------------------------

The quicksort algorithm described above is alright in the average case, where
we get a sorted result in :math:`N\text{log}_2(N)` steps. However, depending on
how we choose the "pivot", the complexity can vary.  Supposing we always pick
the first element of the range to be sorted as the pivot (or equivalently the
last element). In this case, It is easy to see that we need to do
:math:`\mathcal{O}(N^2)` comparisons since in each step where we partition the
sequence into left-right portions, we first need to make :math:`N-1`
comparisons, followed by :math:`N-2`, then :math:`N-3` and so on till we have a
singleton range. This means we need to do a total of :math:`N(N-1)/2`
comparisons .. which is :math:`\mathcal{O}(N^2)`.

For this reason, how to choose a pivot to approximate the median better is an
important consideration. The following strategies are known to work better than
the strategy above -

1. Pick a random element as the pivot. At least this break us away from the
   pathological cases in the average.

2. Pick the middle element as the pivot. This works better when the array is
   partially sorted .. where we can expect the middle value to be the actual
   median.

3. Pick the median of the first, middle and last element in the unsorted
   sequence. This is arguably a better approximation to the median than
   picking just the middle.

Quicksort partitioning scheme
-----------------------------

The step of bucketing the elements of the array to "left" or "right" buckets
based on a pivot is referred to as the "partitioning" step of the quicksort
algorithm. It is clear that if the partitioning can be done in-place,
the whole algorithm can be done in-place as well. The Hoare partitioning
scheme lets us partition an array in-place. It works as follows.

1. Choose the middle element of the array as the pivot.

2. Maintain two indices - one initially pointing to the start of the array,
   and the other to the end of the array. Call these ``i`` and ``j`` 
   respectively.

3. If ``a[i]`` is less than the pivot and ``a[j]`` is greater than the pivot,
   they are already in the correct order. In such a case, we can step ``i``
   forward and ``j`` backward. We can keep doing this until the element
   at ``i`` or ``j`` is not in the correct place.

4. Once we locate an i/j pair that is in the wrong order, we swap them
   and continue as with step 3 again.

5. We repeat this procedure until i and j cross at which point we're done.

.. code-block:: C

   // Note that here the [i,j] forms an inclusive interval.
   int partition(int [] a, int pivot, int i, int j)
        //@requires i >= 0 && i < \length(a)
        //@requires j+1 >= i && j < \length(a)
        //@ensures leq(a, from, \result, pivot)
        //@ensures geq(a, \result, to, pivot) 
    {
        if (a[i] < pivot) { return partition(a, pivot, i+1, j); }

        if (a[j] > pivot) { return partition(a, pivot, i, j-1); }

        if (i >= j) { return j; }

        //@assert i < j && a[i] >= pivot && a[j] <= pivot
        swap(a, i, j);
        //@assert i < j && a[i] <= pivot && a[j] >= pivot
        return partition(a, pivot, i+1, j-1);
    }

    // Since all the recursive calls to partition there are in
    // the "tail position" - i.e. are the final calls, we can
    // translate that into a loop as ..
    int partition(int [] a, int pivot, int i, int j)
    {
        while (true) {
            while (a[i] < pivot) { i = i + 1; }

            while (a[j] > pivot) { j = j - 1; }

            if (i >= j) { return j; }

            swap(a, i, j);
            i = i + 1;
            j = j + 1;
        }
    }

    // The above is often expressed in C as shown below
    // where the initial i,j to be passed in are not
    // the indices of the first and last elements,  but
    // one before and one after.
    int partition(int [] a, int pivot, int from, int to) {
        int i = from-1;
        int j = to+1;
        while (true) {
            do {
                i++;
            } while (a[i] < pivot);

            do {
                j--;
            } while (a[j] > pivot);

            if (i >= j) { return j; }

            swap(a, i, j);
        }
    }

The return value ``k`` of the above partition function is used to
recursively step into quicksort-ing the left and right parts,
where the left consists of ``[from,k)`` and the right consists of
``[k,to]``. 

.. NOTE:: We're not guaranteed that ``k`` will point to a pivot-valued entry,
   so we need to include it in the recursive step.

.. admonition:: Exercise

   Complete the in-place quicksort using the above discussed Hoare
   partitioning scheme.

Merge sort
----------

Consider the problem of combining two sorted arrays of (roughly) equal length
into one sorted array that contains all the elements. It is easy to see how
this operation can be done in :math:`\mathcal{O}(N)` where :math:`N` is the
total number of elements. We can keep one index for each of the sub arrays, and
step through one at a time to determine each element of the result :math:`N`
element array.

Given some two such sorted arrays, the number of possible ways in which 
the process of combining them might have worked can be roughly thought of
as :math:`2^N` .. since for each element in the output, we might have to pick
from one of two possible input arrays. This tells us that this procedure
also reduces the information discrepancy by at most :math:`N` bits.

Let's make that argument more precise like we did for the quicksort
partitioning by starting with the actual count of possibilities. If we have two
sorted arrays of length :math:`N/2` (say :math:`A` and :math:`B`) and we want
to count the number of ways in which we can construct a merged array where the
ordering of the individual subsequences is preserved while otherwise choosing
in arbitrary ways between the two arrays, what we need to do is to create an
intermediate :math:`N` element binary array where half the elements are
:math:`0` and the other half are :math:`1` and count the number of ways we can
permute this. Once we have such an array, we can walk through it and pick the
next element of the merged array from :math:`A` if we see a :math:`0` and from
:math:`B` if we see a :math:`1`, so counting the permutations of the binary
array would yield the same number as the number of ways we can merge the two
sorted subarrays. The number of ways to permute such a binary array is
:math:`\frac{N!}{(N/2)! \times (N/2)!}`.  So the information discrepancy resolved
in such a merge is the logarithm of that --

.. math::

   & = \text{log}_2\left(\frac{N!}{(N/2)! \times (N/2)!}\right) \text{ bits }\\
   & = \text{log}_2(N!) - 2 \times \text{log}_2((N/2)!)  \text{ bits }\\
   & \approx N\text{log}_2(N) - \frac{N}{\text{ln}(2)} - 2 \times ((N/2)\text{log}_2(N/2) - \frac{N/2}{\text{ln}(2)}) \\
   & = N\text{log}_2(N) - \frac{N}{\text{ln}(2)} - (N\text{log}_2(N) - N - \frac{N}{\text{ln}(2)}) \\
   & = N \text{ bits }

Once we implement such a merge step in linear time, we can come up
with a sorting algorithm based on recursively applying this step
as follows --

1. Divide the given array into two roughly equal halves.

2. Sort the left and right halves recursively.

3. Combine the sorted left/right halves using the linear merge step.

Note that in this case, we're doing things the other way around to quicksort,
where we split first and then recursively sorted the split parts. Here we
recurse into the sorting first and finally merge the sorted arrays. the
terminating condition of the sort of course is that once we hit a 1-element
array, there is no sorting left to be done and we can just return.

.. admonition:: Exercise

   Write a function that performs the linear merge step for two given
   sorted arrays and writes the merged result into a third given array::

        void merge(int [] left, int nleft, int [] right, int nright, int [] result);

This mergesort is again :math:`\mathcal{O}(N\text{log}(N))` in complexity since
we can at most continue the "divide into two halves" procedure
:math:`\text{log}_2(N)` times, but that is its **worst case** complexity,
**unlike** quicksort. This is because we're always guaranteed to have a roughly
equal division step unlike quicksort where the balance of the division is
dependent on the choice of pivot. 

Merge sort is also useful in other ways --

1. If we have multiple processors available, the tasks of sorting the 
   left and right halves can be independently done by the processors,
   unlike the pivot split step in quicksort. i.e. mergesort can exploit
   parallelism to gain additional (constant factor) speed up.

2. If the data we need to sort cannot fit into main memory, then
   mergesort can be useful to split the data into chunks that do fit into
   RAM, sort them in RAM and finally combine them into the result
   on disk storage.

.. admonition:: Question

   Can you think of how the merge step can be implemented without requiring
   additional memory?

.. _Stirling's approximation: https://en.wikipedia.org/wiki/Stirling%27s_approximation




