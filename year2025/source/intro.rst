Introduction to imperative computation
======================================

Imperative computation is about seeing a program as a process that achieves a
desired end state from a given starting state through a sequence of
intermediate states that can be shown to progress towards the goal. Hence
imperative computation is about working with state. In this course, we'll be
looking at implementations of algorithms and how to reason about them when
expressed as such "state manipulation machines".

Functions as abstractions
-------------------------

Most all programming languages, except some lowest level ones, provide the
ability to encapsulate a calculation as a "function" or "procedure", as a means
of "absraction". A abstraction helps you reason about a complex activity as through
it were one of the primtive steps available to you, ignoring the details of
how it accomplishes what it promises to do.

In mathematics, a theorem, once proven from more basic premises (either axioms
or other theorems), has the same footing as every other theorem and can be
assumed to be just as true to prove any other theorem. Therefore a theorem can
be seen as an abstraction that helps build the edifice of mathematics.

Similarly, complex programs are built up from progressively smaller sets of
functions (or equivalently "procedures") where a function (the "caller") that
invokes another function (a "callee") relies on the **correctness** of the
callee for its own correctness. Of course, the correctness of the callee is not
*sufficient* to establish the correctness of the caller, but it is a
*necessary* condition. A complex system such as the linux kernel or the
software that runs your mobile phone is built up from such smaller pieces of
functionality, with each level relying on the correctness of the level below
for its own correctness, and each level ensuring that it will make use of the
lower level in a **safe** manner.

Therefore, understanding how to establish that a function call is both **safe**
and **correct** is crucial in the activity of building complex systems through
programs.

Safety
------

A function or procedure requires that the caller provide valid inputs that it
can work on.  If its inputs are not valid, a function implementation can make
no guarantees about the value that it will produce as the result.

Therefore the responsibility to ensure that a function call is **safe** lies with
the caller.

Correctness
-----------

Given valid input parameters, a function then promises that it will produce an
output that meets a requirement contract. A function implementation is said to be
**correct** if it can be shown to meet this contract. The correctness of a function
does not depend on what it does when the safety conditions of calling it are not
met, since it is not part of what it is agreeing on. It only depends on what it
produces when it gets valid inputs.

Therefore the **correctness** of a function is the responsibility of the function
(or rather the author of the function or procedure) and not the caller.

When a function is shown to produce incorrect results for safe inputs, it is
informally said to `have a bug`_.

.. _have a bug: https://en.wikipedia.org/wiki/Software_bug#History

Safety + Correctness = Abstraction
----------------------------------

When the conditions of calling a function safely are met and the function is
known to be correct, it can serve as an **abstraction** - i.e. you can forget
about how the function does what it promised to do and simply rely on the fact 
that it will definitely do it when you're building other procedures using it.

If one of the two criteria isn't met -- i.e. if the caller does not pass safe
arguments to a function or the function does not provide valid result values
when passed safe arguments -- the function can no longer serve as an
abstraction to build other procedures that uses it.


Specifications, algorithms and implementations
----------------------------------------------

A program captures "how to" knowledge about a particular problem or task, much
as a recipe captures such knowledge about making a particular dish.  In trying
to understand how to work with such recipes, it is valuable to understand the
difference between "specifications", "algorithms" and "implementations", even
if at times the boundary between them can get blurred as languages and systems
become more expressive and sophisticated.

Specifications
^^^^^^^^^^^^^^

A specification of a procedure states **what** the procedure should accomplish,
leaving out the **how**.

Let's take the example of a sorting function that sorts an array of numbers.
What would be a specification of such a sorting function? What we do here is to
describe what we mean by "this array of numbers is sorted in ascending order".
Here is an candidate -

    An array of numbers :math:`A` is said to be sorted in ascending order if
    for all pairs of valid indices :math:`(i,j)` such that :math:`i < j`,
    :math:`A[i] \leq A[j]` is true.

Notice how it only makes a statement about **what** the function is expected to
produce and not **how** it should do it. It describes a property of the function's
result that a caller would expect, without dictating how the function ought to
achieve that result.

Algorithms
^^^^^^^^^^

An algorithm describes **how** to obtain the result given by a specification. To
continue the above sorting example, the "quick sort" algorithm provides a way to
derive a result array that meets the specification. The algorithm can be described
as follows -

    1. The sorted version of an empty array is the array itself.  
           
    2. If the given array is non-empty, let :math:`x_0` be the first element of
       the array. Then the sorted version can be obtained by first
       splitting the remainder of the array :math:`A` into two parts
       :math:`A_l` and :math:`A_g` such that all elements of :math:`A_l`
       are less than :math:`x_0` and all elements of :math:`A_g` are
       greater than or equal to :math:`x_0`. If :math:`A_l` and :math:`A_g`
       are then sorted and combined with :math:`x_0` in between, you obtain
       a sorted array that meets the specification.

Note that the algorithm above describes a way to break down the problem into
simpler parts that can be solved - either using the same algorithm recursively
or using another algorithm to solve those sub problems. An algorithm that claims
to meet a particular specification makes no claims about the correctness of that
specification .. which would be of concern only to the ones who specify it.

Also note that this description says nothing about how to implement it as a
program that runs on a specific machine.

Implementations
^^^^^^^^^^^^^^^

Programming environments (languages + runtimes) provide facilities using
which the steps of algorithms can be expressed. Different environments therefore
call for different expressions of an algorithm. Even within a single environment,
multiple implementations of the "same" algorithm are possible. Therefore an 
**implementation** of an algorithm ties the algorithm to a particular model of
computation.

For example, here is an implementation of the quicksort algorithm expressed
in basic python::

    def quicksort(arr):
        if len(arr) == 0:
            return arr

        x0 = arr[0]
        arr_l = [x for x in arr[1:] if x < x0]
        arr_g = [x for x in arr[1:] if x >= x0]
        return quicksort(arr_l) + [x0] + quicksort(arr_g)

The above implementation creates intermediate arrays to store the smaller
"lesser" and "greater" splits. Below is another implementation of the
same algorithm which does not do that and sorts the array in-place. It 
is therefore considered to be better from a memory allocation perspective.
It also makes use of the fact that the "pivot" can be any element in the array::

    # Sorts arr indices range(i,j)
    def quicksort(arr, i, j):
        if i >= j-1:
            return arr
        
        x0 = arr[(i+j-1)//2]
        m = i-1
        n = j

        while True:
            while True:
                m = m + 1
                if arr[m] >= x0: break
            while True:
                n = n - 1
                if arr[n] <= x0: break
            if m >= n: break;
            tmp = arr[m]
            arr[m] = arr[n]
            arr[n] = tmp
        
        quicksort(arr, i, n+1)
        quicksort(arr, n+1, j)
        return arr

The second implementation is also suitable in environments where memory may be
scarce, such as embedded devices whereas the first implementation will require
an environment where memory can be dynamically allocated when required. These
details, however, are not of concern to the "quicksort algorithm", which is
itself not of concern to the specification of "sortedness". So when we use the
python (or Java or C or Haskell ...) builtin sorting function, what we're
relying on is that the function will meet our specification of sortedness if we
give it the right kind of input.

The correctness of an implementation can be narrower than the scope of the
algorithm that it "implements". For example, an implementation of quicksort in
C may be done only for arrays of 32-bit integers, whereas the algorithm is
general enough to work on any sequence of objects that can be compared in some
manner.

Our approach
------------

We'll learn how to design a few data structures, implement some algorithms that
operate on them, and learn how to prove the correctness of those algorithms
given safety conditions. We'll learn about "checked contracts" and how to
express them using the C0 dialect of C and to use these contracts to identify
bugs in implementations.

Our goal here is to help develop the right kind of mental habits early on so
that when you set out to build complex programs, you know how to ensure your
confidence in them (i.e. ensure safety + correctness), and also how to
troubleshoot problems when you find your program not producing the result you
intend it to for some particular inputs, using contracts.

As languages get more powerful and expressive, you may find that your
implementations starting to look like the algorithm they implement in its most
general form, or even simply the specification of the result, but the notions
of safety and correctness are still required to build up your edifice in a way
that ensures that it doesn't turn into a house of cards.