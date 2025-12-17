
Assignment 1: GCD
=================

This section is about the first assignment regarding the GCD function.
Based on the submissions I went through, there seems to be considerable
confusion regarding the concepts involved. So in this section we'll
go through those in somewhat excruciating detail to understand the
basics well.

.. WARNING:: Spoilers ahead!

We started with the python function that supposedly computes the GCD
of two numbers ::

  def gcd(m, n):
      while m > 0 and n > 0:
          m, n = m % n, n % m
      return m + n

When given any function like this, the first step is to understand
what this function is supposed to do. The next step is to understand
what it does. The final step is to codify this understanding in
contracts.

When writing functions on the other hand from scratch, start with a
notion of what the function is expected to do, codify it in contracts
and **then** implement the function.

The first task has been made somewhat simple for us - we've been told
that this function is supposed to compute the GCD of two numbers.

Now, mathematically, the GCD of two positive integers is defined as
the largest integer that divides (i.e. that is a factor of) both the
numbers. We should capture this idea in a spec ::

   def gcd_spec(m, n):
       candidates = range(max(m,n)+1)
       return max(f for f in candidates if f > 0 and m % f == n % f == 0)

Here, we're taking the description literally. We first identify candidate
values for the GCD, then filter out values that aren't divisors of both
given numbers, and then pick the largest of them.

Python is expressive enough to let you make such specifications in high
level code. Why is making such a specification useful? We can now test
the given function against it. But before that, let's see all the other
ways in which we could've written this specification without loss of
generality ::

   # Start with the largest possible value and
   # keep trying lower values until you hit a common
   # factor. Then that will be the largest.
   def gcd_spec(m, n):
       c = max(m,n)
       while c > 0:
           if m % c == n % c == 0:
	       return c
	   c = c - 1
       return c

   # Make a range of values from the largest candidate to the
   # smallest (which is 1) and pick the first one of this range
   # that is a factor of both numbers.
   def gcd_spec(m, n):
       return next(f for f in range(max(m,n),0,-1) if m % f == n % f == 0)


   # Make a list of all common factors in descending order and pick the first one.
   def gcd_spec(m, n):
       return [f for f in range(max(m,n)+1) if f > 0 and m % f == n % f == 0][0]
 

   # Make a list of all common factors and pick the largest.
   def gcd_spec(m, n):
       return max([f for f in range(max(m,n)+1) if f > 0 and m % f == n % f == 0])


All the above versions appear obvious enough provided you know what ``range``,
``max`` and such things do.

Testing the function
--------------------

Now let's test the given ``gcd`` function against this spec and see
whether it meets the spec. Paraphrasing a popular saying, "In God we
trust. The rest better show tests."

Let's make pairs of numbers such that at least one of them is > 0
(since mathematically gcd(0,0) is not defined) and then check what
``gcd`` gives us and the ``gcd_spec`` (any of the above versions)
gives us::

   pairs = [(m,n) 
            for m in range(50)
            for n in range(50)
            if m + n > 0 and gcd(m,n) != gcd_spec(m,n)]

In the above code, we're collecting all the pairs for which the
``gcd`` function does not produce the same result as our spec
function. We use ``m+n>0`` to indicate that at least one of the two
numbers must be ``> 0``.

If the ``gcd`` function does what it was advertised to do, we should
get an empty array in the above step since we're looking for ``(m,n)``
combinations for which the ``gcd`` function does not match the
``gcd_spec`` function.

If you calculate the above list, you'll see that the ``gcd`` function
does not return the same value as the spec whenever ``m == n``.
So let's see what happens for ``m == n``. To do that, we can rewrite
the given gcd function using m alone instead of n ::

    while m > 0:
        m = m % m
    return m + m

Since ``m % m`` is zero for ``m > 0``, we get a result of ``0`` whenever
``m == n``. Whereas we should get the answer as ``m``. There are a couple
of ways we can address this::

    def gcd(m, n):
        if m == n: return m
        while m > 0 and n > 0:
            m, n = m % n, n % m
        return m + n

    def gcd(m, n):
        while m > 0 and n > 0:
            if m > n: 
                m = m % n
            else: 
                n = n % m
        return m + n

The latter version might be familiar to you from the first lecture.

Now, you were not expected to bug fix this function right at the first step,
but the hope was that by the time you got around to Q7, you would've poked
around enough with the logic of this function to see its flaws.

Translating to C0
-----------------

Let's do a straight forward translation to C0.

.. code-block:: C

   int gcd(int m, int n) {
        while (m > 0 && n > 0) {
            m = m % n;
            n = n % m;
        }
        return m + n;
    }

This translation indicates the failings of not having an accurate mental model
of the language we're translating to and relying only on what one might call
"surface structure" or "syntax". If you look carefully, you'll notice that
we've not introduced a dependency between the two update calculations, which
did not exist in the python code. The python code ``m,n = m%n,n%m`` calculates
the RHS first before reassigning ``m`` and ``n`` to the new values, whereas
the C0/C version above updates ``m`` first and tries to calculate an updated
value for ``n`` using the updated ``m``. The way to address this is to 
store away the calculations in temporary variables and then perform the
assignment.

.. code-block:: C

   int gcd(int m, int n) {
        while (m > 0 && n > 0) {
            int tm = m % n;
            int tn = n % m;
            m = tm;
            n = tn;
        }
        return m + n;
    }

Yes we haven't fixed the logical bug, but at least our translation is now
on par with the python version.

Writing the specification in C0
-------------------------------

The first while-loop based formulation of ``gcd_spec`` is more amenable
to translation to C0 than the others, since the other versions rely on
python facilities that don't exist in C0.

.. code-block:: C

   int gcd_spec(int m, int n) {
        if (m < n) { return gcd_spec(n, m); }

        int candidate = m;
        while (candidate > 0) {
            if (m % candidate == 0 && n % candidate == 0) {
                return candidate;
            }
            candidate = candidate - 1;
        }

        return candidate;
    }

Note that the goal of writing the spec function is that its correctness is as
obvious as possible - i.e. makes the fewest assumptions about the properties of
``gcd`` as possible.

One thing to note here is that we expect the ``gcd_spec`` to be valid only for
the condition ``m >= 0 && n >= 0 && m+n > 0`` since ``gcd_spec(0,0)`` is not
defined and for this purpose we aren't concerned about GCD of negative numbers.


Pre-conditions for ``gcd``
--------------------------

As we saw, our ``gcd`` function can only promise to produce a correct result
when ``m >= 0 && n >= 0 && m+n > 0``. In particular, it cannot guarantee a
correct result for ``m < 0 || n < 0 || m + n == 0``. Therefore our
precondition for ``gcd`` should read ::

    //@requires m >= 0 && n >= 0 && m + n > 0;

... or equivalently::

    //@requires m >= 0 && n >= 0 && (m > 0 || n > 0);
    
However, since the ``gcd`` function produces a value for ``m+n==0`` case too,
we may choose to accept that value even though it doesn't make sense
mathematically. The reason this is not a bad thing to do is that it is
reasonable to expect that someone calling ``gcd`` will be aware of the fact
that GCD is not defined when both numbers are 0, but might not want to add
special cases to check that. In a real program, such a condition might be
signalled by ``gcd`` never returning properly and instead "raising an
exception".

That said, since we're using contracts here to do the checking for us, it
would be great if the checker can point out when the caller has forgotten
that GCD is undefined for ``gcd(0,0)``. Therefore the full ``@requires``
expression above is valuable.

Post-conditions for ``gcd``
---------------------------

Having written ``gcd_spec``, this should be easy to see ::

    //@ensures \result == gcd_spec(m,n);


Examining the iteration
-----------------------

Let's look at the body of the loop we wrote for ``gcd``::

    int tm = m % n;
    int tn = n % m;
    m = tm;
    n = tn;

The scope of the variables ``tm`` and ``tn`` (chosen to be short for
"temporary m" and "temporary n") is limited to the inner-most pair
of braces, which is the ``while`` loop. So the state when entering
the loop is only ``(m,n)`` and the state when exiting the loop
is also ``(m,n)`` since ``tm`` and ``tn`` go out of scope. 

Therefore the relevant state of this loop is only the two variables
``m`` and ``n``. 

From a scoring perspective, I have not penalized if you included the
temporary variables also, since I do think it is a good practice to 
first account for all variables before deciding which ones are actually
relevant.

The python equivalent of this iteration is therefore just::

    def gcd_iter(m,n):
        return m % n, n % m

since the loop can be written as::

    while m > 0 and n > 0:
        m, n = gcd_iter(m, n)

Supposing we rewrote the python version of the iteration using temporary
variables like this::

    def gcd_iter(m,n):
        tm = m % n
        tn = n % m
        return tm, tn

you can see that the outer loop remains the same. This is the reason
we do not consider local variables like ``tm`` and ``tn`` as part of the
state that is transformed by the loop.

That said, it isn't a fatal error to have done so. That's because pretty
soon in the analysis, you'll start to see the irrelevance of the temporaries
to the correctness and safety of the function. So starting with a diligent
collection of relevant variables as part of the "state" is ok and you'll soon
be able to identify irrelevant variables quickly.

Pre-conditions of ``gcd_iter``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Now, what preconditions must hold for ``gcd_iter`` to be able to perform
its task? Looking at the operations it uses, we see that both ``m`` and ``n``
feature in the denominator of a modulus operation. Therefore neither
variable can be 0. Since we also know they cannot be negative in our
case, we can express the preconditions for ``gcd_iter`` as::

    m > 0 and n > 0

Post-conditions of ``gcd_iter``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If we look at ``tm = m % n`` and ``tn = n % m``, we see that one of them does
not change value and the other one reduces. If say ``m > n``, then ``n % m ==
n``. Likewise if ``m < n``, then ``m % n == m``. In both those cases, the
following mathematical property holds ::

    gcd_spec(tm, tn) == gcd_spec(m, n)

In other words, if the GCD can be computed for ``m`` and ``n``, it is the 
same value as you'd get if you calculated the GCD of ``tm`` and ``tn``.
This is therefore our loop invariant.

.. IMPORTANT:: The value of this loop invariant lies in the fact that
   it can help us prove that we will get the result eventually,
   since we know that at least one of the two values reduces in each
   iteration. This is, however, operational reasoning. 

Also important to see is that the loop invariant must hold **irrespective**
of how the loop body does its work!! This is the key to understanding
loop invariants and is also why you're asked to write the invariants first
before writing the loop body. Having identified the invariant, we can write
it down so that ``coin -d`` can check it for us and point to any errors
in the loop body.

To see why this has to be the loop invariant for this algorithm, look
at the last case (for ``m != n``) when we exit the while loop. At that
point, one of the two values is 0 and we expect the other non-zero value
to be the GCD of the original ``m`` and ``n``. So if this invariant did
not hold all through the loop, this condition will not be possible.

We also have another invariant here::

    m + n > 0

This is because we cannot compute ``gcd(0,0)`` anywhere as it does not have
mathematical validity.

Now how about ``m == n`` which is the only remaining condition to check? In
this case, we see that both ``tm`` and ``tn`` become 0 and our loop
invariant fails because we then try to compute ``gcd_spec(0,0)``. Since we
know that ``m + n > 0`` must hold, and this loop is not giving us that,
**we conclude that there is a bug in the loop**.

A fixed version of ``gcd_iter`` is therefore::

    def gcd_iter(m, n):
        tm = m % n if m > n else m
        tn = n % m if n > m else n
        return tm, tn

Now, why should ``gcd_spec(tm,tn) == gcd_spec(m,n)``?


This is because for given ``m > n``, we have ``gcd_spec(m,n) == gcd_spec(n, m%n)``.

1. Let :math:`g` be the GCD of :math:`m` and :math:`n`. This means :math:`m =
   ga` and :math:`n = gb` for some integers :math:`a` and :math:`b`.

2. If we now assume :math:`m > n`, then we have :math:`a > b` and therefore
   :math:`g` is also the GCD of :math:`m-n` and :math:`n` since :math:`m-n = g(a-b)`.

3. As long as :math:`m-n` continues to be :math:`> n`, we can keep subtracting :math:`n`
   from it. The result at the end of this process will be :math:`m \text{mod} n`. Therefore
   :math:`\text{gcd}(m \text{mod} n,n) = \text{gcd}(m,n)` for :math:`m > n`.

The algorithm is derived from this property of modulo division.

Another useful way to look at this is to rewrite the loop using recursion::

    def gcd(m, n):
        # The terminating condition for the recursion.
        if m == 0 or n == 0:
            return m + n

        if m > n: return gcd(m % n, n)
        else: return gcd(m, n % m)

We can rewrite the above recursive form using some additional local variables
like this::

    def gcd(m, n):
        if m == 0 or n == 0:
            return m + n

        tm = m
        tn = n
        if tm > tn: tm = m % n
        else: tn = n % m
        return gcd(tm, tn)

The above form should make clear the existence nad nature of the invariant because
it is essentially saying ``gcd(m,n) == gcd(tn,tn)``.

Now, why can't we write everything using recursion? Nothing really stops us from
doing it except that many languages do not guarantee that they will support recursion
to arbitrary depths. Depending on the algorithm it might actually be quite feasible
to use recursion even in such languages like Python, Java and C. Other languages
like Common Lisp and Scheme support a mechanism called "tail call elimination"
which enables us to use recursion to express loops without a performance penalty.
If time permits, we'll visit that topic in this course.

For another example, below is our ``pow`` function expressed recursively, so you
can see the invariant clearly::

    int pow(int m, int n, int r) {
        if (n == 0) { return r; }
        if (n % 2 == 0) {
            return pow(m * m, n / 2, r);
        } else {
            return pow(m * m, (n - 1) / 2, r * m);
        }
    }
     
Where we're expected to call ``pow`` as ``pow(m,n,1)`` to compute ``m^n``.

.. NOTE:: The extra argument ``r`` in such recursive forms is called an
   "accumulator" and is used to contain the result when expressing recursive
   calculations using "tail recursive functions". In the above ``pow``
   implementation, you may be able to see that the top level ``pow`` function
   does not have anything else to do after it finishes computing the inner
   ``pow`` call. Therefore the inner call is called the "tail call". Some
   compilers can notice such tail calls and turn them into loops automatically,
   giving us eht benefit of logical clarity without compromising efficiency.
   This is just some extra info at this point for those interested.

The complete implementation
---------------------------

Putting all of the above together,

.. code-block: C

   int gcd_spec(int m, int n) {
        if (m < n) { return gcd_spec(n,m); }
        int candidate = m;
        while (candidate > 0) {
            if (m % candidate == 0 && n % candidate == 0) {
                return candidate;
            }
            candidate = candidate - 1;
        }
        return candidate;
    }

    int gcd(int m, int n)
    //@requires m >= 0 && n >= 0 && m+n > 0;
    //@ensures \result == gcd_spec(m,n);
    {
        int tm = m;
        int tn = n;

        while (tm > 0 && tn > 0)
        //@loop_invariant tm + tn > 0;
        //@loop_invariant gcd_spec(m,n) == gcd_spec(tm, tn);
        {
            if (tm > tn) { tm = tm % tn; }
            else { tn = tn % tm; }
        }

        return tm + tn;
    }

When written out in this form, it is easy to see that the ``tm + tn > 0`` loop
invariant is superfluous since we already have ``m+n > 0`` prior to the loop's
start and both can become 0 only if both ``m`` and ``n`` are 0 in the first
place. Anyway, we'll leave it in there as an illustration.

The LCM
-------

Exploiting the property that ``gcd(m,n) * lcm(m,n) == m*n``, we can write
an initial ``lcm_spec`` like this ::

    int lcm_spec(int m, int n) {
        return m * n / gcd_spec(m, n);
    }

Now, pay careful attention to the exact calculation specified by that expression.
These expressions we write here, though they look like they're the same as their
mathematical counterparts, they actually aren't since the machine has to make
particular choices about the order in which to evaluate the expression.

In this case, the ``lcm_spec`` function is equivalent to ::

    int lcm_spec(int m, int n) {
        int prod = m * n;
        int g = gcd_spec(m, n);
        return prod / g;
    }

Now, it should be clear that this approach can work only if the ``m*n`` operation
does not overflow. To avoid this, we can exploit the fact that the GCD is a common
factor for both ``m`` and ``n`` and write ::

    int lcm_spec(int m, int n) {
        return (m / gcd_spec(m, n)) * n;
    }

This works since the ``m / gcd_spec(m, n)`` expression is guaranteed to be an
integer without a remainder, by property of ``gcd_spec``. Of course, now we are
also faced with the constraint that the LCM itself should not be so large as to
not fit in a 32-bit signed integer. However, that is a lost cause for us,
because if it was indeed so large that the result wouldn't fit in 32 bits, we
can't represent it in our system using an ``int`` anyway and therefore have to
resort to other approaches.

Therefore our ``lcm`` implementation can closely follow the spec::

    int lcm_spec(int m, int n) {
        return (m / gcd_spec(m, n)) * n;
    }

    int lcm(int m, int n) 
    //@requires m >= 0 && n >= 0 && m+n > 0
    //@ensures \result == lcm_spec(m, n)
    {
        return (m / gcd(m, n)) * n;
    }


Alternatively, we can resort to an iterative form for ``lcm_spec`` where
we assume that ``m*n`` is within ``int`` limits::

    int lcm_spec(int m, int n) {
        if (m > n) { return lcm_spec(n, m); }
        int candidate = m;
        int limit = m * n;
        while (candidate < limit) {
            if (candidate % m == 0 && candidate % n == 0) {
                return candidate;
            }
            candidate = candidate + 1;
        }
        return candidate;
    }

In this case, you don't want the ``lcm`` implementation to closely follow
the spec. Note here that since our loop condition is ``candidate < limit``,
we're guaranteed that ``m > 0 && n > 0`` holds within the loop and therefore
the modulo operations' preconditions are met.

Things to be wary of
--------------------

Logical expressions in C and python (and in most languages) are evaluated
using an approach called "short circuit evaluation" that can causes different
behaviour than what one would expect mathematically. This is done for efficiency
by avoiding unnecessary computation.

If you have a logical condition expressed as a conjunction of boolean
expressions using "logical and" like -- ``A && B && C && .. && Z``, 
then if you consider it mathematically, it is easy to see that if any one of
``A``, ``B``, ... , ``Z`` happen to be ``false``, then the whole expression
becomes ``false`` too. Therefore, when checking each component of this
compound expression in sequence, the moment you encounter a ``false`` result,
you can stop checking the other parts and mark the expression's result as ``false``.

From a mathematical perspective, it does not matter what order we present these
logically anded parts, the result is the same, but from a computational perspective
it can matter depending on the situation.

Consider the expression ``a[i] % 2 == 0 && i < 10``. Let's say that the array
``a`` is of length ``10``. Mathematically, the ``i < 10`` can be expected to
restrict the other condition. However, in a program, the first expression will
be evaluated without checking whether ``i`` is a valid index into ``a`` and can
therefore cause an "array index out of bounds" error. On the other hand, if the
expression were written as ``i < 10 && a[i] % 2 == 0``, the second expression
will be evaluated only if the first succeeds. Thus, the first expression serves
as a "guard" for the second expression.

A similar "short circuiting" behaviour also happens for "logical or", except that
in that case, if any component expression becomes ``true``, the whole expression
can be considered ``true`` without examining any of the other sub-expressions.


