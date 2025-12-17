Arrays
======

We saw the basics of arrays in the class on C0. To recap,
arrays are declared using a syntax that is read right to left::

    var a: array<int> = new int[10];

The ``int []`` type expression is to be read right-to-left
as "array of ints". So if you wanted an array of pointers
to a struct named ``XYZ``, you'd write that type as 
``struct XYZ * []``. Again, you can read that right to
left as "array of pointers to struct XYZ".

Aliasing
--------

You're already aware of how two variables can refer to the
same data structure (ex: array, dictionary, set) in python::

    a = [1,2,3]
    b = a
    a.append(4)
    print(a) # shows [1,2,3,4]
    print(b) # shows [1,2,3,4]

This is because the value associated with ``a`` and ``b``
is simply the starting address at which the sequence of items
occurs in memory. The same applies to arrays in C0 (and C) too::

    var a: array<int> = new int[3];
    a[0] := 13;
    a[1] := 17;
    a[2] := 19;
    var b: array<int> = a;
    print a[1], "\n"; // Shows 17
    print b[1], "\n"; // Shows 17
    a[1] := 23;
    print a[1], "\n"; // Shows 23
    print b[1], "\n"; // Shows 23

When you type ``int [] a = alloc_array(int, 5);`` in ``coin -d``, you'll see a
hexadecimal number printed out next to ``a`` after executing that statement.
This number is the "memory address" at which the array elements begin. In our
spreadsheet metaphor, this address is the number of the column which contains
the value of the first element of the array.

.. admonition:: **Exercise**

    Find out what happens if you try to access elements using an index value
    that's beyond what is appropriate for the allocated length of an array. i.e.
    if you did ``alloc_array(int, 5)`` for an array ``a``, then you're
    guaranteed the availability of ``a[0]``, ``a[1]``, ... ``a[4]``.  What
    happens if you try to access ``a[5]``?


Looping through the elements of an array
----------------------------------------

We've used the ``while`` loop almost exclusively so far.
There is another looping construct in C0 (and C)
called ``for``, which is more convenient for looping
through indexed structures like arrays::

    for <variable>: <type> := <startval> to <endval>
        invariant <condition>
    {
        <body>
    }

    for <variable>: <type> := <startval> downto <endval>
        invariant <condition>
    {
        <body>
    }

The above is exactly equivalent to::

    var <variable>: <type> = <startval>;
    while (<variable> < <endval>)
    {
        <loop-body>;
        <variable> := <variable> +/- 1;
    }

So the ``for`` loop form is a syntactic convenience that
helps us read loops more easily because the initialization,
loop condition and update are available on the same line.
It also helps us avoid common coding errors when writing
``while`` loops, where we may forget to initialize, or
update a loop variable.

The form you're most likely to encounter in array-based
code is::

    for i: int = 0 to n
    {
        // do something with array a[i]
    }

... where ``n`` is taken to be the number of elements in the
array (a.k.a. its "length").

Copying an array
----------------

Let's write a simple utility function in C0, complete with contracts --
a function to take in an integer array and return a copy of it. We know
that aliasing is as easy as assignment, but copying takes more work.
To copy an array, we need to --

1. Allocate a new array of the same length as the original array.

2. Copy each element of the original array to the corresponding location
   of the result array.

3. Return the result array.

.. code :: dafny

    method copy_array(a: array<int>, n: int) returns (b: array<int>)
        requires n > 0
        requires a != null
        ensures b.Length == n
    {
        b := new int[n];
        for i := 0 to n
            invariant i >= 0
        {
            b[i] = a[i];
        }
    }

We can add some redundant but instructive checks to this as well --
where we declare that ``copy_array`` will indeed produce a copy of
the input array --

.. code :: dafny

    function have_same_values(a: array<int>, b: arrya<int>, n: int) : bool
        requires n > 0
        requires a.Length > 0
        requires b.Length > 0
        requires n <= a.Length
        requires n <= b.Length
    {
        forall i: int | 0 <= i < n :: a[i] == b[i]
    }

    method copy_array(a: array<int>, n: int) returns (b: array<int>)
        requires n > 0
        requires a != null
        ensures b.Length == n
        ensures have_same_values(a, b, n)
    {
        b := new int[n];
        for i := 0 to n
            invariant i >= 0
            invariant have_same_values(a, b, i)
        {
            b[i] = a[i];
        }
    }

Both the above ``have_same_values`` checks are quite redundant, since it is
plenty obvious, looking at ``b[i] = a[i]`` that we should expect that condition
to hold. So the above version primarily serves an instructional purpose.
A general heuristic to use is to ask whether a statement being made by 
a contract expression is evident from any other single line in the code.
If it is, then the contract clause may not be needed. For example --

.. code :: C

    var x: int = 1;
    assert x == 1;

is a case where the ``@assert`` would be redundant because we know that
the variable ``x`` was just initialized to ``1``. However, consider the
following --

.. code :: C

    var y: int = 15;
    var x: array<int> = new int[1];
    x[0] := 1;
    var z: int = some_function(x, y);
    assert x[1] == 1;

In the above case, ``some_function(x)`` may have modified the ``int`` contents
that ``x`` is pointing to, so the ``@assert`` is informative. However, if we
should always expect ``*x`` to not be changed, then we may want to make that a
post-condition of ``some_function`` instead, so that we don't have to repeat
such an assertion every time we call ``some_function``.

.. admonition:: **Exercise**

    Why does the loop invariant fail if we changed ``@loop_invariant i >= 0``
    to ``@loop_invariant i >= 0 && i < n``?

.. admonition:: **Exercise**

    Write out the expanded form of the above ``copy_array`` function body
    without using ``while`` or ``for`` but using only ``goto`` s and labels. Make
    sure that the contract expressions are placed at the correct points in the
    expanded form.

