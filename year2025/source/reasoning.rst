Reasoning with imperative programs
===================================

So we finished off with the version of our power function inappropriately
named ``f`` in the form shown below --

.. code-block :: c

    int power_spec(int x, int y) {
        if (y == 0) { return 1; }
        return power_spec{x, y - 1) * x;
    }

    int f(int x, int y)
    //@requires y >= 0;
    //@ensures \result == power_spec{x,y);
    {
        int x1 = x;
        int y1 = y;
        int r = 1;
        while (y1 > 1)
        {
            if (y1 % 2 == 1) { 
                r = x1 * r;
            }
            x1 = x1 * x1; 
            y1 = y1 / 2;
        }
        return r * x1;
    }

The above code has a bug in it and in this part, we'll find out what the
bug is and look at ways to fix it. How do we know there's a bug in there?
We know that the ``@ensures`` contract fails when you call ``f`` with
``y == 0``. It seems to work for every value ``y > 0`` (provided the result
is small enough to hold in 31 bits).

The primary object we need to look at here is the while loop. As a method,
we'll be rewriting/recasting the program in different forms so we can understand
it. This kind of **rewriting to understand*** is very valuable, while a little 
risky as well - since you can introduce errors when you rewrite a program. With
practice, and with enough guards such as the contracts we're placing here,
such "rewriting for understanding" becomes viable.

What is a while loop?
---------------------

All imperative programs that feature a while loop, have it structured
in the following manner ::

    while (<<condition derived from state>>) {
        <<steps that modify the state>>
    }

In our case, the ``<<condition derived from state>>`` is ``y > 1``
and the ``<<steps that modify the state>>`` are ::

    if (y1 % 2 == 1) {
        r = r * x1;
    }
    x1 = x1 * x1;
    y1 = y1 / 2;

This block of code can be uderstood as an "iteration function", which
was presented when discussing recursion in ICT. We'll recap that here.

A function :math:`f : \mathcal{A} \longrightarrow \mathcal{A}` is said to be "iterable" because
it can be used on its own result because its domain and codomain are the
same. Put explicitly, it means you can compute :math:`f(x)`, :math:`f(f(x))`,
:math:`f(f(f(x)))` and so on ... which we can express as :math:`f^n(x)`.

How is this relevant in our situation? The body of code inside a loop can be
seen as an iterable function that maps the outer function's state space to the
same state space. In our case, our state space consists of three variables -
``x1``, ``y1`` and ``r``. To see the iteration function for what it is,
we'll temporarily switch to python as it affords better notation suited
for the purpose. ::

    def pow_iter(x1, y1, r1):
        if y1 % 2 == 1:
            r1 = r1 * x1
        x1 = x1 * x1
        y1 = y1 // 2
        return x1, y1, r1

If we label the arguments to reflect that this can be the n-th state
and the output variables to reflect that the result will be the n+1-th state,
we get::

    def pow_iter(xn, yn, rn):
        if yn % 2 == 1: rnp1 = rn * xn
        xnp1 = xn * xn
        ynp1 = yn // 2
        return xnp1, ynp1, rnp1

This is essentially the same function, but is clearer because we don't have it
clobbering the values of the variables used in the input .. which is a useful
property to have to reason about the code. We've also renamed the argument
variables and the result-holding variables as ``(xn,yn,rn)`` and
``(xnp1,ynp1,rnp1)`` respectively to reflect their mathematical notation and
the purpose of ``pow_iter`` -- :math:`(x_n,y_n,r_n)` and
:math:`(x_{n+1},y_{n+1},r_{n+1})`. You can also think of the :math:`n`
subscript as the row number when we work out the state using a spreadsheet. 

In the above form, you can see that the three lines before ``return`` can be
**reordered in any way** without changing the behaviour of the function. 

The full python version of our buggy ``f`` function can be written as ::

    def f(x, y):
        x1, y1, r = x, y, 1
        while y1 > 1:
            x1, y1, r = pow_iter(x1, y1, r)
        return r * x1;

It isn't clear at this point how this is useful, but what we've done here is to
convince ourselves that we need to understand ``pow_iter`` to understand ``f``.

Properties of the iteration function
------------------------------------

We've now transferred part of the responsibility of correctness of ``f`` to
the correctness of ``pow_iter``! So what safety and correctness conditions
would we want of ``pow_iter``? Before we get there though, we need to understand
what it is doing and codify it first.

We know that because the ``pow_iter`` function is used within ``while (y1 >
1)``, we can be sure that ``yn > 1`` internally as well. However, there is
nothing within the function that breaks (i.e. violates some safety condition)
if we supply a ``yn > 0`` instead. Therefore the weaker ``yn > 0`` condition
suffices for its safety. We don't need to broaden it beyond that and accept
``yn < 0`` as well because we make no guarantees about ``f``'s behaviour for
negative ``y``.

There are two cases inside the ``pow_iter`` function -- ``yn`` is even
and ``yn`` is odd, and we can split the two cases explicitly to see
what's going on::

    def pow_iter(xn, yn, rn):
        if yn % 2 == 1:
            xnp1, ynp1, rnp1 = xn * xn, yn // 2, rn * xn
        else:
            xnp1, ynp1, rnp1 = xn * xn, yn // 2, rn
        return xnp1, ynp1, rnp1

The "odd ``yn``" case can be made a little mathematically precise without
invoking truncated division like this::

    def pow_iter(xn, yn, rn):
        if yn % 2 == 1:
            xnp1, ynp1, rnp1 = xn * xn, (yn - 1) // 2, rn * xn
        else:
            xnp1, ynp1, rnp1 = xn * xn, yn // 2, rn
        return xnp1, ynp1, rnp1


Let's now look at what we're calculating for even values of ``yn``. We
find that we're (in mathematical terms) calculating :math:`x_n^2`,
:math:`y_n/2` and :math:`r_n`. If you consider :math:`{x_n}^{y_n}`
and :math:`{x_{n+1}}^{y_{n+1}}`, we see that the latter is :math:`(x_n^2)^{y_n/2}`,
which is the same as :math:`{x_n}^{y_n}`.

Let's look at the case of odd values of ``yn`` now. If we try to calculate
:math:`{x_{n+1}}^{y_{n+1}}`, we get :math:`(x_n^2)^{(y_n-1)/2}` which gives
:math:`(1/x_n){x_n}^{y_n}` .. but in this case, there is an extra factor of
``xn`` that ``rn`` gains when we calculate ``rnp1``. Therefore if you consider
the combined expression :math:`{r_n}{x_n}^{y_n}` and
:math:`{r_{n+1}}{x_{n+1}}^{y_{n+1}}`, we find them to be equal in both cases -
irrespective of whether ``yn`` is odd or even. 

.. ATTENTION:: We call values derived from the state that are unchanged by a
   change of state "invariants". Slightly more formally, for an iterable
   function :math:`f : \mathcal{A} \longrightarrow \mathcal{A}`, a function
   :math:`g : \mathcal{A} \longrightarrow \mathcal{B}` is said to be an
   "invariant of :math:`f`" if :math:`g(f(x)) = g(x)` for any valid :math:`x
   \in \mathcal{A}`. Without loss of generality, we can also take the :math:`g`
   to be a function that produces a boolean result.

So the quantity :math:`{r}{x}^{y}` is an "invariant" of the ``pow_iter``
function.

If we then label all the sequentially generated values of
:math:`\text{pow{\_}iter}^n(x,y,r)` as :math:`(x_n,y_n,r_n)`, then we see that,
:math:`{r_{n+1}}{x_{n+1}}^{y_{n+1}} = {r_n}{x_n}^{y_n}`. 

**But then, what exactly is this value equal to?**

To answer that, all we need to do is to examine the values coming in to our
``pow_iter`` the very **first** time we enter the loop -- i.e.
:math:`(x_0,y_0,r_0)`. In that case, we know that ``r == 1`` because that's
what we initialized it to. Therefore the value is simply :math:`x^y` -- i.e.
the very power function that we wish to compute.  That means that our
:math:`rx^y` quantity always has a value equal to the result we want to
compute. So if we run ``pow_iter`` enough times so that :math:`y_n = 0`, then
we know that :math:`{r_n}{x_n}^{y_n} = r_n = x^y` -- i.e. our answer will
simply be in the final value of ``r``.

The very last time ``pow_iter`` gets to run, we have ``yn == 1`` and therefore
the final values of ``(xnp1,ynp1,rnp1)`` are ``(xnp1,1,rnp1)``. Therefore, by
our invariant, the answer is simply the expression ``power_spec(xnp1,1)*rnp1`` ..
which is just ``rnp1 * xnp1``.  So for values of ``y > 1``, we know that our
``return r1 * x1`` statement is correct.

For ``y == 1`` too, we see that ``f`` reduces to just ``return r * x1`` which is
just ``return x1`` because we know ``r == 1`` by initialization. The result is
correct for this case too.

For ``y == 0`` though, the expression ``return r * x1`` gives us the wrong answer.
The correct answer being ``return r`` since in this case by initialization ``r == 1``.
We also saw above that if we let ``pow_iter`` run enough times so that :math:`y_n = 0`,
we get our answer directly in :math:`r_n` which we can return. Therefore to address the
"bug", all we need to do are the following -

1. Change ``return r * x1;`` to ``return r;``, and
2. Change ``while (y1 > 1)`` to ``while (y1 > 0)``

The corrected program therefore is ::

    int f(int x, int y)
    //@requires y >= 0;
    //@ensures \result == power_spec(x, y)
    {
        int x1 = x;
        int y1 = y;
        int r = 1;
        while (y1 > 0) {
            if (y1 % 2 == 1) {
                r = r * x1;
            }
            x1 = x1 * x1;
            y1 = y1 / 2;
        }
        return r;
    }

.. index:: @loop_invariant, @assert

However, in writing in the above form, we've lost some important guarantees that we
worked out for ``pow_iter`` - its "invariant" :math:`{r_n}{x_n}^{y_n}` and how
the loop is supposed to end - i.e. with :math:`y_n = 0`. The C0 language lets
us declare these using the ``//@loop_invariant`` contract and a somewhat general
``//@assert`` contract. We can write::

    int f(int x, int y)
    //@requires y >= 0;
    //@ensures \result == power_spec(x, y)
    {
        int x1 = x;
        int y1 = y;
        int r = 1;
        while (y1 > 0) 
        //@loop_invariant y1 >= 0;
        //@loop_invariant r * power_spec(x1, y1) == power_spec(x, y);
        {
            if (y1 % 2 == 1) {
                r = r * x1;
            }
            x1 = x1 * x1;
            y1 = y1 / 2;
        }
        //@assert y1 == 0;
        return r;
    }

The ``y1 >= 0`` invariant is a requirement for us because otherwise we are not assured
of the safety of the ``power_spec(x1, y1)`` call.

There is one more easy to see fact about ``pow_iter``: :math:`y_{n+1} < y_n`.
That's true because in the even case, we have :math:`y_{n+1} = y_{n} / 2` and for the
odd case, we have :math:`y_{n+1} = (y_n - 1) / 2`. So in both cases, the inequality
will hold .. except when :math:`y_n = 0`, but in that case we're already done
and the loop wouldn't have been entered in the first place.

There are other ways to fix the function.

1. Since we know that ``f(x,y)`` is misbehaving only for ``y==0``, we can special case
   case it by adding an ``if (y1 == 0) { return 1; }`` before the ``while (y1 > 0)``.

2. Since we now know that when ``y == 0``, the return value should be ``r`` and not 
   ``r * x1`` and that the latter is correct of higher values of ``y``, we can special
   case the return statement as ``if (y == 0) { return r; } else { return r * x1; }``.

The fix where we modify the while loop and the return statement is considered a
better fix due to its greater generality. We're showing that we trust the while
loop fully. Both the above fixes, on the other hand, add a special case without
considering the full validity of the while loop. This kind of a fix, in
general, indicates a reduced trust in the main part of the function -- the
while loop -- because by doing this, we show that we're not confident that the
while loop is the correct thing to do.  Special cases also add complications to
analysis by adding new conditions (the "special cases") to check for under
various circumstances. 

.. index:: reasoning
   
"Point-to" reasoning
--------------------

What is the value to making those additional ``//@...`` contract annotations
apart from the fact that C0 can check them for us? It is that every fact that
we want to know about in order to determine the correctness and safety of our
function ``f`` is now made explicit in the code itself. You do not need to
infer anything else given these annotations ... apart maybe from some
mathematical facts.

1. The ``//@requires y >= 0;`` and ``//@ensures \result == power_spec(x,y);``,
   if they are never violated, gives us the necessary assurance to rely on ``f``
   -- they assert both the **safety** and **correctness** of ``f``.

2. The ``//@loop_invariant y1 >= 0;`` tells us that it is safe to make checking
   calls to ``power_spec(x1,y1)``.

3. The ``//@loop_invariant r1*power_spec(x1,y1) == power_spec(x,y);`` tells us that
   we don't need to worry about the contents of the loop and can focus on how
   the last value or ``r`` on exit (or even if the loop is never entered) gives
   us the answer.

4. The ``//@assert y1 == 0;`` tells us that the ``return r;`` is indeed the
   correct result since ``power_spec(x1,y1)`` will then be 1.

5. Based on (4), we can reason that the ``//@ensures..`` condition is indeed
   true and therefore our function ``f`` is **correct**.

Therefore any fact we need to use in our reasoning process is available in
the source directly and we can "point to" it.

Now, if every function written were to have such (verifiable) annotations,
won't it be a pleasure to develop complex programs confidently?

A fine point about ``@loop_invariant``
--------------------------------------

Earlier, we considered loop invariants as a property of the loop iteration
function (such as ``pow_iter``). While it is correct that it is a property that
the iteration function must meet, that is not the complete picture.

The loop invariant must also hold in the case when the loo body is never
entered.  This is ensured by C0's implementation of ``@loop_invariant``. For our
power function, the ``@loop_invariant`` check is as though the expanded form
were as shown below::

    int f(int x, int y) 
    {
        //@requires y >= 0;
        int r = 1;
        int x1 = x;
        int y1 = y;

    LOOP:
        //@loop_invariant r * power_spec(x1, y1) == power_spec(x,y)
        if (!(y1 > 0)) goto EXIT_LOOP;
        if (!(y1 % 2 == 1)) goto SKIP_R_UPDATE;
        r = r * x1;
    SKIP_R_UPDATE:
        x1 = x1 * x1;
        y1 = y1 / 2;
        goto LOOP:

    EXIT_LOOP:
        //@ensures \result == power_spec(x,y)
        return r;
    }

So, in the above expanded form, we've made it explicit that the loop invariant
must hold even for the case ``y1 == 0`` when the while loop is not entered.
Therefore, the expression you place there must account for this too.

Another thing you might've noticed is that we declare our loop invariants
not as "functions whose values are preserved by the state changing actions
in the loop", but as a boolean expression that must hold prior to loop
entry. This takes advantage of the fact that we without loss of generality,
a loop invariant can be restricted to produce a boolean result that must hold
true at all times.

To understand why, consider the mathematical formulation again --
:math:`g(f^n(x)) = g(x_0)` being the property that the function ``g`` must
satisfy in order to be called an invariant of the iterable function ``f``. If
that condition is to hold, we can express that as a boolean condition
equivalently -- :math:`g(\text{state}) == \text{constant}`. 

Another way to look at it is that if ``g`` is an invariant of ``f``, 
then so is :math:`h(x) = (g(x) == g(x_0))` which is a boolean function.

Operationally, using the "boolean invariant" approach eases the implementation
burden of C0 by merely requiring the compiler to insert a check at a single
point in the code, instead of having to track the value of a function
application within the loop.


The two important questions
---------------------------

If you're finding it hard grappling with proofs done in the "point to"
reasoning style, thinking about loop invariants, pre and post conditions
and such, here is something that might help you. You only need to remember
two important questions -

1. What do I know?
2. How do I know it?

These two are surprisingly effective in a wide variety of areas spanning
personal thinking to work in deep science. We'll leverage these here to
explain what we're trying to do.

"What do I know?" - An answer to this question comes as some kind of factual
statement like "I know that the sun will rise tomorrow at 6am." Of course, there
are fact-like statements that fit as answers to this question, not all of which
are true. For example, "Our ancestors are currently living on Neptune." That
brings us to the next question -

"How do I know it?" - Gaps in the believability of a fact-sounding answer given
to the first question are expected to be filled by answers to this question.
"The sun has always risen at 6am on the 28th of August every year." would be
a reasonable answer as it draws on inductive reasoning as an explanation.
Another candidate is to say "I know that the earth is a rotating sphere
and by the law of conservation of angular momentum, I expect it to continue
to spin until I see the sun rise tomorrow at 6am." 

As you see, The answer to the second question may itself invoke facts that fit
the "what do I know?" question. If these happened to be shared knowledge, then
the enquiry will usually stop there. If not, then you need to recursively ask
"how do I know **that**?" until you reach a point where the explanations are
satisfactory to the inquirer. In scientific research papers, many "how do you
know that?" questions that can come up along the way are answered by "pointing
to" another research paper that presented that fact and explained it.

How is this useful to us here?

When I give you a plain program like the one below and ask you "for what values
of m and n does this function produce the correct result?", you might answer
"for m > 0 and n > 0". Then I'll ask you "how do you know that?" and you'll have
to examine the code, perform some logical reasoning in your head and then
articulate that as an explanation.::

    int gcd(int m, int n) {
        while (m > 0 && n > 0) {
            int m1 = m % n;
            int n1 = n % m;
            m = m1;
            n = n1;
        }
        return m + n;
    }

On the other hand, if you instead give me the code below, and I ask you the same
question, you can just point me to the ``@requires`` line  and say "because this
line says so". Of course, the understanding here is that you've completed your
analysis of the function and what you can point to is factually accurate::

    int gcd(int m, int n)
    //@requires m > 0 && n > 0;
    {
        while (m > 0 && n > 0) {
            int m1 = m % n;
            int n1 = n % m;
            m = m1;
            n = n1;
        }
        return m + n;
    }

    So your task in declaring assertions about what your function does (i.e.
    ``@requires``, ``@ensures``, ``@loop_invariant`` and ``@assert``), both
    internally and externally, is so that you can answer any such "how do you
    know that?" question by pointing to some one line and saying "because of
    what that line says". It doesn't literally have to be one line always though
    that's obviously preferrable. It can be two truths you know that are both
    explicit in the code. However, if the question asked can reasonably be
    expected to be asked again, it will be worth putting in an assertion that
    can be pointed to as an explanation. If then someone asked "how do you know
    that the line 83 assertion is true?", then you can point to the other lines
    and explain "because lines 72 and 78 imply that", but by then, hopefully the
    lines 72 and 78 evidently entail the assertion on line 83 without much
    additional reasoning support.  

