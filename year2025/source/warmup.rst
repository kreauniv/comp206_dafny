
Warming up with the power function
==================================

We'll consider the function shown below and take it as a case
to work with the ``cc0`` compiler and the ``coin`` interpreter
and learn about contracts::

    int f(int x, int y)
    {
        int r = 1;
        while (y > 1)
        {
            if (y % 2 == 1)
                r = r * x;
            x = x * x;
            y = y / 2;
        }
        return r * x;
    }

While it may not be clear when you look at the function, it (for
the most part) computes x to the power of y for integers x and y.
However, it has a bug and we'll work through systematically to
find and fix that bug.

1. Type the above function into a separate text file (say ``pow.c0``) and save
   it.
   
2. Load it up in ``coin`` using ``coin pow.c0<enter>``. (We'll omit mentioning
   ``<enter>`` key at the end from now on.)

3. At the prompt, type ``#functions`` to see that the ``f`` function is indeed
   defined and available. 
   
4. Try various values for inputs and see what you get::

        > f(2,3);
        8
        > f(3,5);
        243
        > f(3,0);
        3
        > f(2,-3);
        2
        > f(-2,3);
        -8
        > f(-2,4);
        16

As you see, some of those values are correct and some are not as you expect.

Specifying the power function
-----------------------------

Mathematically, you can define the power function for integers
as follows -

.. math::
    x^y = \begin{cases}
            1 & \text{for } y = 0\\
            x \times x^{y-1} & \text{for } y > 0
           \end{cases}

The above definition (or "specification") happens to be recursive,
which is permitted in mathematical specifications since it calls
on a proof by induction to establish the definition.

The important point to note here is that this specification does
not consider values of :math:`y` that are less than :math:`0`.
Therefore it is considered to be meaningless (in the context of
this specification) to ask what is the value of :math:`2^{-3}`.

The specification as a program
------------------------------

In this case, it happens that the recursive definition is also an
"algorithm" to compute the power function, which lends itself
to a recursive implementation in C0 as shown below::

    int power_spec(int x, int y) {
        if (y == 0) { return 1; }
        return x * power_spec(x, y-1);
    }

In this case, it almost looks reasonable to use this as an
implementation as well, but that won't be true in general.
For instance, consider the following specification of
addition of two positive integers -

.. math::

    m + n = \begin{cases}
        m & \text{for } n = 0\\
        \text{succ}(m) + \text{pred}(n) & \text{for } n > 0
    \end{cases}

... where the definition of :math:`\text{succ}` is from the `Peano axioms`_
and :math:`\text{pred}` can be defined accordingly as well.

.. _Peano axioms: https://en.wikipedia.org/wiki/Peano_axioms

While that serves as a sufficient specification, it is not suitable
as an algorithm, let alone being considered for implementation,
since the number of steps it would require to add two integers
is proportional to the values of those integers. You wouldn't want
to use the following function to add two integers, given you know
how to increment and decrement integers::

    int add_spec(int m, int n) {
        if (n == 0) { return m; }
        m++;
        n--;
        return add_spec(m, n);
    }

Preconditions
-------------

.. index:: @requires

Since we have a specification of :math:`x^y` that is valid only for :math:`y \geq 0`, 
We need to document in our function that the input must meet that safety constraint,
because we cannot guarantee a correct result if that constraint is not met. You do
that in C0 using the ``@requires`` contract as shown below ::

    int f(int x, int y)
    //@requires y >= 0;
    {
        int r = 1;
        while (y > 1)
        {
            if (y % 2 == 1)
                r = r * x;
            x = x * x;
            y = y / 2;
        }
        return r * x;
    }

In C-like languages, the part of a line of code from ``//`` to the end of that
line is considered a "comment". C0 however makes use of this to let us specify
formally what we'd otherwise use normal language to "comment" on our function
(maybe like ``// Do not pass negative values for y``).

Load up the new function in ``coin`` using ``coin -d pow.c0`` and try calling
``f(2,-3);`` to see what happens.

Postconditions
--------------

.. index:: @ensures

Now that we've noted the precondition contract, we need to indicate to the world
what the ``f`` function is intended to compute. For that, we'll use the ``power_spec``
we wrote above. In your file, you'll need to paste the definition of that function
above the definition of ``f`` and save your file. Then modify ``f`` to the following::

    int f(int x, int y)
    //@requires y >= 0;
    //@ensures \result == power_spec(x,y)
    {
        ...
    }

Again, try the above definition using ``coin -d pow.c0``. The "-d" flag causes ``coin``
to check the conditions as contracts and not as comments. Does it go through fine?
Can you make sense of what you see?

Expanded form
-------------

.. index:: @requires, @ensures

It is useful to understand where the pre and post condition checks go when coin
(and cc0) translate the contract lines. In effect, the function ``f`` becomes
the form shown below::

    int f(int x, int y)
    {
        //@requires y >= 0;
        int r = 1;

        LOOP:
        if (!(y > 0)) goto EXIT_LOOP;
        if (!(y % 2 == 1)) goto SKIP;
        r = r * x;
        
        SKIP:
        x = x * x;
        y = y / 2;
        goto LOOP;

        EXIT_LOOP: 
        {
            int result = r * x;
            //@ensures \result == power_spec(x,y)
            return result;
        }
    }

Given the expanded view above, the error message given by ``coin`` ought to become
clearer. We're modifying the values of ``x`` and ``y`` as part of the while loop,
but in the end we want to use the original values (which have since been erased)
to check against the ``power_spec`` function. Therefore, we need to ensure that
the input variables are not modified within the function body so we can do a
valid ``@ensures`` check::

    int f(int x, int y)
    //@requires y >= 0;
    //@ensures \result == power_spec(x,y)
    {
        int _x = x;
        int _y = y;
        int r = 1;

        while (_y > 1) {
            if (_y % 2 == 1) {
                r = r * _x;
            }
            _x = _x * _x;
            _y = _y / 2;
        }

        return r * _x;
    }

Try this version in ``coin`` with various input values to see what you get.

1. If the **safety** condition specified by ``@requires`` line fails, it means
   you're calling the function wrongly and so the responsibility for the error
   is with the caller.

2. If the **correctness** condition specified by ``@ensures`` line fails, it
   means there is a problem with the implementation that needs to be fixed for
   the function to become correct.


Running costs of checks
-----------------------

It costs additional compute time to perform these checks for every function.
In a large system with thousands of functions calling each other, these costs
can add up significantly, resulting in a slow program. Therefore it is desirable
to be able to turn on these checks only when you want to troubleshoot a problem,
and have them be turned off when you have already established that the functions
are correct.

The ``-d`` flag you passed to ``coin`` turns on these checks. If you do not pass
that flag, then ``coin`` will not do these checks for you. Similar flags are often
provided by real-world compilers.

Once you know that all calls to your function are safe and you've proved your
function to be correct, you can turn off these checks. However, proving correctness
for every function can be very hard in real programs. Some reasonable alternatives
to that include creating "test suites" (which can show the presence of bugs but not 
their absence) and maybe 


Tracing the states of ``power_spec`` and the implementation ``f``
-----------------------------------------------------------------

We earlier said that imperative programs are characterized by "state changes".
In this section, we'll look at how ``power_spec`` and ``f`` compute their
results to understand why it is worth separating the two, and also to introduce
a model of tracing state changes in an implementation.

To simplify the discussion, we'll rewrite ``power_spec`` first to also 
use a loop, so that the comparison becomes clearer and is not muddled by
the recursive step. It is, for the moment, sufficient to remember that loops
can be implemented using recursion and vice versa.

.. code :: C

    int power_spec(int x, int y) {
        int r = 1;
        while (y > 0) {
            r = r * x;
            y = y - 1;
        }
        return r;
    }

We see that within the above ``power_spec`` function, there are three "variables"
that may change value as the function performs its steps - ``x``, ``y`` and ``r``.
We consider these to be columns in a spreadsheet and each row to be a "step"
taken by the function, and trace out the state changes as follows. We take 
``power_spec(3,8)`` as an example. We also track the boolean "y > 0" as a derived
state for convenience.


===================  ======== ======== ======== ==============
**Step**             **x**    **y**    **r**    **y>0**
===================  ======== ======== ======== ==============
``power_spec(3,5)``  3        5        UNDEF    true
``int r = 1;``       3        5        1        **true**
``1) r = r * x;``    3        5        3        true
``y = y - 1;``       3        4        3        **true**
``2) r = r * x;``    3        4        9        true
``y = y - 1;``       3        3        9        **true**
``3) r = r * x;``    3        3        9        true
``y = y - 1;``       3        2        27       **true**
``4) r = r * x;``    3        2        27       true
``y = y - 1;``       3        1        81       **true**
``5) r = r * x;``    3        1        81       true
``y = y - 1;``       3        0        243      **false**
``return r;``        3        0        **243**  false
===================  ======== ======== ======== ==============


We've numbered the steps where we enter the loop each time, so we can see how
many times we run through the loop. We've also marked in bold the values of the
``y > 0`` column that are actually relevant for the next step. Now let's do a
similar thing for our ``f`` function. At this point, we know that it does
produce the correct value for ``f(3,5)``. For ``f``, the variables that
constitute its state are still ``x``, ``y`` and ``r``.


===================  ======== ======== ======== ======== ============== ==============
**Step**             **x**    **y**    **r**    **r*x**  **y>1**        **y%2==1**
===================  ======== ======== ======== ======== ============== ==============
``power_spec(3,5)``  3        5        UNDEF    UNDEF    true           true
``int r = 1;``       3        5        1        3        **true**       **true**
``(1) r = r * x;``   3        5        3        9        true           true   
``x = x * x;``       9        5        3        27       true           true
``y = y / 2;``       9        2        3        27       **true**       **false**
``(2) x = x * x;``   81       2        3        243      true           false
``y = y / 2;``       81       1        3        243      **false**      true
``return r * x;``    81       1        3        **243**  false          true
===================  ======== ======== ======== ======== ============== ==============

So we see that what took 5 loops doing 10 computations in all with ``power_spec``
above (and it is easy to see that it takes as many loops as the value of
``y``), the ``f`` implementation only took 2 loops and 6 computations in all.
This discrepancy grows quickly if you double the value of ``y``. When you try
that, you'll see that ``power_spec`` will take 10 loop iterations with 20 computations
in all, but ``f`` will take only 3 loop iterations doing 5 computations in all.
So clearly, ``f`` is more efficient than ``power_spec``. [#eff]_

.. [#eff] We'll be formalizing this notion of efficiency later in the course. For
          now, it is sufficient to understand this notion informally.

Some notes about the C0 language
--------------------------------

1. "Statements" in the language are terminated by a semicolon ``;``. 

2. Variables are declared using statements of the form ``<type> <variable-name>
   = <initial-value>;``. For example ``int r = 1;``. This means that the variable
   ``r`` can only take on integer values.
   
3. The scope of a variable is limited to the immediately enclosing pair of
   braces ``{}``.
   
4. The conditions for ``if`` and ``while`` must be surrounded by parentheses --
   like ``if (y % 2 == 0)``. Unparenthesized expressions (like what python allows)
   are not permitted in C0 and C.

5. If a particular ``if`` or ``while`` block consists of only one statement
   terminated by a semicolon, the braces become optional. So you can write ``if
   (y % 2 == 1) r = r * x;`` on a single line. However, we recommend that you
   consider it as though the braces are mandatory in all situations as it helps
   reduce certain kinds of programming errors.