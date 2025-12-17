Practice problems
=================

Here are some simple and likely familiar problems on which to practice
identification of pre and post conditions and loop invariants.  In all these
cases, translate the python program to C0 and add **contracts** that take care
of **safety** and **correctness**.  Write the contracts **first** before you
write the code. It might also be instructive for you to write the functions
that have loops using recursion. If you want to, you may want to write the
function just from the description, but you've dealt with problems of this type
in ICT, so that's up to you.

.. WARNING:: Do not take any given function to be correct!

Max and min
-----------

Functions to calculate the maximum and minimum of two given integers.

.. code::

   def max(m, n):
        if m > n:
            return m
        else:
            return n

    def min1(m, n):
        return m + n - max(m,n)


    def min2(m, n):
        if m < n:
            return m
        else:
            return n


Implementing multiplication using addition and bitshift
-------------------------------------------------------

If you only have addition, bitshift and comparison operators, can you implement
multiplication? Restricting to non-negative numbers. This should look
**really** familiar!

.. code::

    def mul(m, n):
        result = 1;
        while n > 0:
            if n % 2 == 1:
                result = result + m
            m = m << 1
            n = n >> 1
        return result



Test of primes
--------------

A function of an integer that returns ``True`` if it is prime
and ``False`` if it isn't.

.. code::

   def is_prime(m):
        candidate = 2
        while candidate < m:
            if m % candidate == 0:
                return False
        return True


Silly summing
-------------

Here, we do a weighted count of certain non-negative numbers < m.  If a number
is a multiple of 3, we give a weight of 1.  If a number is a multiple of 5, we
give a weight of 2.  If a number is a multiple of both, we give a weight of 4.

.. code::

    def silly_sum(m):
        result = 0
        for i in range(1,m):
            if i % 3 == 0 and i % 5 == 0:
                result += 4
            elif i % 3 == 0:
                result += 1
            elif i % 5 == 0:
                result += 2
        return result


Bit counting
------------

The task here is to count the number of bits in the binary representation of a
given non-negative number that are set to 1.

.. code::

   def numbits(m):
        n = 0
        while m > 0:
            n += m % 2
            m = m // 2
        return n


Change base
-----------

Given a number m and two bases b1 and b2, the task is to find the digits of m in base
b1 and then construct a new number in base b2 using the same digits. For
example, changebase(73,10,12) should give 87.

.. code::

   def changebase(m, b1, b2):
        n = 0
        rb = 1
        while m > 0:
            d = m % b1
            n += d * rb
            rb = rb * b2
            m = m // b1
        return n


Reverse digits
--------------

Along the lines of ``changebase`` above, take a number and construct a new
number that has reversed digits in the given base ``b``. So
``revdigits(73,10)`` should give ``37``.

.. code::

   def revdigits(m, b):
        result = 0
        while m > 0:
            result = result * b + m % b
            m = m // b
        return result


