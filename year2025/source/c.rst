Transitioning to C
==================

Henceforth, we'll be using the C programming language instead of C0.  C is one
of the most widely used languages for writing system software for large and
small (i.e. "embedded") systems. Examples include the Linux kernel itself which
has over 25million lines of C code today. The language specification that we'll
be using for this course is called `C99 <https://en.wikipedia.org/wiki/C99>`_.
The reason for this popularity is several fold -

1. C gives you very low level control of system resources like memory, which is
   important in constrained systems. The early systems for which the C language
   itself was developed would look extremely constrained from today's perspective.

2. With the long history that C has (it was born in 1972), C compilers have gotten
   extremely good at producing highly optimized code, which again is beneficial
   to all systems large and small.

The downside of the language is that its strength of giving the programmer a
lot of control over system resources can itself be a problem when developing
application software, since the author is forced to deal with these resources
in every circumstance. Derived languages like "C++" and "Objective-C" add
features that make routine record keeping of resources much more manageable and
less error prone.

That said, it is a great language to get to grips with the machine model that
we all use today - the model where a processor runs a loop of the form -

1. (implicit) Fetch instruction - which can tell the processor to do one of ...
2. Fetch data from memory into registers
3. Compute result
4. Store result from registers into memory

The above is a simplified picture and modern "super scalar" processors are far
more complicated, but that deserves a whole course on its own and we'll live
with the above sketch for the purpose of this course.


The C memory model
------------------

When we began this course, we looked at the "spreadsheet" model of memory and
how we can trace and understand the state of a program. That's not far from the
truth. The computer's addressable memory is like an array of single bytes whose
indices serve as their "address". The "RAM" -- random access memory -- when
given an address, can give us the byte stored at that address or store a given
byte at an address. On systems today, our programs get such a virtual array
that's about 4GB in size on 32-bit machines and 16billion GB in size on 64-bit
machines.  Of course we don't have that much physical memory, so the kernel
uses storage like hard disks or SSDs to extend the memory available to our
programs without us having to think about it. It is **as though** the RAM is
modeled by the following functions ::

    uint8_t ram_load(uint64_t address);
    void ram_store(uint64_t address, uint8_t byte);

Since the round trip to the RAM and back taken time relative to how fast our
processors can compute today, this interface is extended to fetch blocks of
memory at a time, so in reality it is more like ::

    block_t ram_load_block(uint64_t address);
    void ram_store(uint64_t address, block_t b);

where we've used the name ``block_t`` to stand for the block. (i.e. it is not a
real data type in C.)

The C stack
-----------

In addition to the main memory bulk, the C language also provides a "stack"
that is tied to scoped variables in the language.

.. NOTE:: The scope of a variable in C is determined by the inner most pair
   of braces ``{}`` that enclose the definition of the variable.

Consider the following example code -

.. code-block:: C

    int accumulate(int N, int *in, int *out) {
        int val = 0;
        int i = 0;
        for (i = 0; i < N; i++) {
            int tmp = val + in[i];
            out[i] = tmp;
            val = tmp;
        }
        return val;
    }

Your program will need some memory to store the variables ``val`` (4-bytes),
``i`` (4-bytes) and ``tmp`` (4-bytes) and also for the arguments to your
function ``N`` (4-bytes), ``in`` (8-bytes) and ``out`` (8-bytes). Since ``in``
and ``out`` are pointer types, they usually are stored as 64-bit address values.

The storage for these variables is allocated and managed by the compiler
automatically using a "stack" structure. A variable is given some memory
on the stack only while the code within its immediate scope is executing.
So a variable is said to be "visible" only within its scope. Once a scope
is exited, the compiler may release the storage needed for the variable.

In the above example, the ``tmp`` variable is local to the ``for`` loop
and is not available at the point where we're doing ``return val;``.
However, the variables ``val``, ``i`` and so on are available within
the ``for`` loop as its scope is contained in the scope of those variables.

Why is this called a "stack"? You can think of an open brace ``{`` as an
instruction to "push new variables on to the stack" and the matching ``}`` as
an instruction to pop all the variables up to the matching ``{`` from the
stack. So the stack gets deeper the more nested scopes there are (including
function calls which add their own scope) and when these scopes are exited, the
stack becomes shallower again.  This is why if your function is recursive and
makes too many recursive calls, you may run out of stack space and your program
may crash with a "stack overflow" error.

The stack is not very visible in C0 since you can only work with it via the
scopes. In C though, you can take the address of variables allocated on the
stack and pass them around .. while making sure you don't use them after the
variable goes out of scope .. which means the stack is very much visible
to the programmer and the program.


Differences from C0
-------------------

For the most part, C0 is a subset of C, so if you write C0 code for your
functions, you'll be writing valid C code. The difference will be in the
functions available to you in the two languages. C offers more to the
adventurous though. Below we consider the language differences from several
perspectives.

.. list-table:: C0 versus C - pragmatics
   :widths: 20,40,40
   :header-rows: 1

   * - **Task**
     - **For C0**
     - **For C**
   * - Compile a program
     - ``cc0 program.c0``
     - ``gcc program.c`` or ``tcc program.c`` or ``clang program.c``
   * - Run compiled program
     - ``./a.out``
     - ``./a.out``
   * - Compile with contract checks/debugging enabled.
     - ``cc0 -d program.c0``
     - ``gcc -d program.c`` (same for others)
   * - Compile with optimizations
     - Nothing particular
     - ``gcc -O3 program.c`` - turns on maximum "level 3" optimizations.
   * - Use standard libraries
     - ``#use <conio.h>`` etc. Must occur first in source file.
     - Include the library's headers. Common ones given below.

       .. code-block:: C

           #include <stdio.h>
           #include <stdlib.h>
           #include <math.h>
           #include <assert.h>

       They can technically occur anywhere, since ``#include <stdio.h>`` just
       means "copy paste contents of stdio.h into my code at this point",
       however they're usually placed at top of source file.
   * - Contracts
     - ``@requires``, ``@ensures`` and ``@assert``
     - Not supported in general, but if you ``#include <assert.h>``,
       an ``assert`` "macro" becomes available which can be used to
       declare known properties at certain points in your code ::

            assert(some_variable == some_value);

       Any boolean expression may be used as the argument to ``assert``.
       When compiling using ``gcc -O3``, all ``assert`` expressions are
       slided from the code.

.. list-table:: C0 versus C - basic language features
   :widths: 20,40,40
   :header-rows: 1

   * - **Feature**
     - **In C0**
     - **In C**
   * - Statement structure
     - ``<statement>;``
     - Same - i.e. terminated by a semi-colon.
   * - Scope delimiters
     - A pair of braces -- i.e. ``{<code>}``
     - Same
   * - Number types
     - Only 32-bit signed integers - ``int``
     - Many sized integers both signed and unsigned.
       Ex: ``int`` (signed "word"-sized integer) ::

           int8_t / uint8_t 
           int16_t / uint16_t
           int32_t / uint32_t
           int64_t / uint64_t
   * - Floating point numbers
     - Not supported
     - ``float`` (32-bit) and ``double`` (64-bit)
       types are available.
   * - Character type
     - ``char``
     - ``char``
   * - Boolean type
     - ``bool``
     - You need to ``#include <stdbool.h>`` and then
       you can use ``bool`` along with ``true`` and ``false``.
   * - Get address of a variable
     - Not allowed. You can only get pointers using ``alloc``.
     - If ``x`` is a variable of type ``T``, then ``&x``
       gives its address and is an expression of type ``T*`` --
       i.e. a pointer to ``T``. ``&`` can also be used on
       members of ``struct`` ::

            struct Pair {
                int x;
                int y;
            };

            struct Pair p1;
            p1.x = 42;
            p1.y = 23;

            int *yptr = &p1.y;
            printf("%d\n", *yptr); // Prints 23

       In general, ``&`` can be used on any expression that is valid on
       the LHS of an assignment statement and will yield a pointer to
       the memory location that is the target of that assignment. The
       technical term for such an expression that can be used on the LHS
       is, unsurprisingly, "l-value".
   * - Get contents of a pointer
     - If ``p`` is of type ``T*``, then its contents are obtained
       using ``*p``.
     - Same as C0.
   * - Treat boolean as integer
     - Not allowed
     - ``(int)b`` where ``b`` is declared as ``bool b;``
       A "boolean" value in C is equivalent to an integer
       that is 0 to represent falsehood and 1 to represent
       truth. So ``printf("%d\n", b)`` will print either
       0 or 1 where ``b`` is a ``bool`` value.
   * - String type
     - Native type ``string``
     - Modeled as "pointer to character" (``char*``) with
       the end of string signified by a character with numeric code ``0``
       called the "null character", notated as ``'\0'``. When the C
       compiler sees a string constant notated as ``"hello"``,
       it automatically inserts the null character at the end,
       so the storage used for ``"hello"`` is actually 6 bytes
       and not 5 bytes.
   * - Array type
     - Native type ``T []`` allocated using ``alloc_array(T, n)``.
     - An array of ``T`` is essentially a pointer at which ``n``
       consecutive values of type ``T`` are stored. If ``T *p``,
       then you can access the ``T`` at index 4 using ``p[4]``
       or equivalently ``*(p+4)``. C also has an array syntax
       ``int arr[10];`` that can be used for temporary "stack" 
       allocation and for sequences stored in ``struct``s. Such
       a variable ``arr`` is effectively a pointer though.
       This adds more confusion at this stage, so we'll not be 
       using C style arrays and will stick to pointer-based approach.
   * - Print things
     - Use ``printint`` or ``print`` as appropriate.
     - ``#include <stdio.h>`` provides "formatted print"
       ``printf(<format-string>, <additional-values>)``.
       So for example ``printf("%d+%d=%d\n", 2, 3, 5);``
       will print "2+3=5". See documentation of ``printf``
       for more details about format codes. ``printf`` will
       automatically flush the output when it prints a new-line
       character ``'\n'``.
   * - Define type names
     - Ex: ``typedef struct Node *node_t;``
     - Same

.. list-table:: C0 versus C - memory management
   :widths: 20,40,40
   :header-rows: 1

   * - **Task**
     - **In C0**
     - **In C**
   * - Allocate memory for a type ``T``
     - ``T *x = alloc(T);``
     - ``T *x = (T*)malloc(sizeof(T));``
   * - Allocate memory for an array of ``N`` elements of type ``T``.
     - ``T []x = alloc_array(T, N);``
     - ``T *x = (T*)malloc(N * sizeof(T));``
   * - Free allocated memory.
     - Not needed since garbage collector takes care of it.
     - ``free(x)`` where x stores a pointer returned by ``malloc``.
       Not freeing allocated memory in C results in a "memory leak".
   * - Casting
     - Restricted scenarios in C1 language extension
     - 1. Free casting between pointer types using ``(T2*)v`` where
          ``v`` is of type ``T1*``.
       2. Free casting between structure types is an error.
       3. Free casting between ``int`` and ``bool`` or ``char``
          since all three are integers under the hood. The compiler
          may warn when you cast a "larger type" to a "smaller type" --
          ex: from ``int`` to ``bool``.
   * - Treat character as integer
     - Use ``int char_ord(char ch)`` function in ``string`` library.
     - ``(int)ch`` where ``ch`` is a ``char``.
   * - Treat integer as character
     - Use ``char char_chr(int i)`` function in ``string`` library.
     - ``(char)i`` where ``i`` is an integer type.

.. list-table:: C0 versus C - iteration
   :widths: 20,40,40
   :header-rows: 1

   * - **Feature**
     - **In C0**
     - **In C**
   * - For loops
     - ``for (<init>;<cond>;<step>) { <body> }``
     - Same
   * - While loops
     - ``while (<cond>) { <body> }``
     - Same
   * - Break out of ``for`` and ``while`` loops
     - Not supported
     - ``break;`` statement
   * - Jump back to loop condition ignoring rest of loop body.
     - Not supported
     - ``continue;`` statement
   * - Returning from a function
     - ``return <expr>;``
     - Same
   * - Functions
     - ``<return-type> <fn-name>(<arg1-type> <var1>, ...) { <body> }``
     - Same

Casting
-------

Casting is about treating a value of one type as a value of another type.
The syntax for that in C is -

.. code-block:: C

    one_type val1;
    another_type val2 = (another_type)val1;

i.e. You place the type you want to treat ``val1`` as within parentheses preceding the
value of interest. A common need is to cast between various numeric types, ``int``
to ``float``, ``uint8_t`` to ``uint32_t`` and so on.

"Downcasting" is the term used to refer to a casting operation from a "larger" type
to a "smaller" type. For example, since ``uint32_t`` is 4-bytes in size, casting a
``uint32_t`` value to ``uint8_t`` which is only one byte would be a "downcasting".
Downcasting usually results in loss of information unless you know by some other
means that there won't be information loss. For example, you may have a ``uint32_t``
value that you know is in the range :math:`[0,255]` and therefore that it is safe
to cast it down to ``uint8_t``. The compiler won't be able to validate that for you
in general and will at most warn you about potential information loss.

Another commonly used casting operation in C is between pointer types, known as
a "reinterpret cast". The syntax is the same, except that two types are both
pointer types. While C may not permit you to cast from a ``float`` to a particular
``struct Something``, C permits you to cast between **any** two pointer types.

For example -

.. code-block:: C

    #include <stdio.h>
    #include <stdint.h> // For uint8_t and such

    int main() {
        int v = 878265;
        printf("v = %d\n", v);
        int *vptr = *v; // vptr points to an int, which is stored ass 4 consecutive bytes,
                        // (depending on CPU type) in least-significant-byte-first order.

        // We're now looking at the bytes that make up the integer.
        uint8_t *vbytes = (uint8_t*)vptr;
        printf("vbytes = ");
        for (int i = 0; i < sizeof(int); i++) {
            printf("%d ", vbytes[i]);
        }
        printf("\n");

        // We now compute the integer back from the individual bytes and check it.
        // Note that the order of the bytes is machine dependent. So this program
        // will print out whether it uses "little endian" or "big endian" ordering.
        int computed1_v = vbytes[0] + vbytes[1] * 256 + vbytes[2] * 256 * 256 + vbytes[3] * 256 * 256 * 256;
        int computed2_v = vbytes[3] + vbytes[2] * 256 + vbytes[1] * 256 * 256 + vbytes[0] * 256 * 256 * 256;
        if (computed1_v == v) {
            printf("computed_v = %d and your computer is 'little endian'\n", computed1_v);
        } else if (computed2_v == v) {
            printf("computed_v = %d and your computer is 'big endian'\n", computed2_v);
        } else {
            printf("little_endian_v = %d, big_endian_v = %d and I don't know what went wrong.\n", 
                   computed1_v, computed2_v);
        }
        return 0;
    }


.. Warning:: C's permission to cast between **any** two pointer types is a
   double edged sword.  It can be used very effectively to implement various
   abstractions (example ``qsort`` discussed in the next section) including
   what was the origins of "object oriented programming". It also comes with
   the danger of crashing your system if you don't really know what you're
   doing. For example, casting a ``float`` to an ``int`` will get you a
   truncating behaviour. However, if you cast a ``float*`` to an ``int*`` since
   both are 4-bytes in size, there is no information loss, but the integer
   value won't make any sense unless your intention is to work with the
   bit-representation of floating point numbers for some reason.

The cast operation can also be "chained" like this - ``(float)(int)(uint8_t)val``.

Function pointers
-----------------

You're used to passing around functions in python as "first class values".
They were called by various names, "lambda", "closure" or just plain "function".

Lower level languages like C eschew the complexity that comes with implementing
closures in favour of simpler primitives that you can build up from. While C
does not have the concept of "lambda" or "closure" [#cblocks]_, it does permit
you to write code that can store and pass around stateless functions as values
as "function pointers".

The underlying principle is that a function is just a block of code or machine
instructions itself residing in memory at a certain location. So if we have the
address of this location and know something about what the code residing there
will do if we jump to that location, then we can use that to add customizations
to behaviour of our code. This is, once more, the notion of "interface" coming
up of use.

A common kind of function pointer usage is to write custom comparison functions
for sorting algorithms. For example, the standard C ``qsort`` function has the
following signature -

.. code-block:: C

    void qsort(void *base, size_t nitems, size_t size, int (*compare)(const void *, const void*));

Let's break that down. 

1. We will want to be able to sort arrays of arbitrary items. The ``base`` argument
   is used to provide a pointer to the first element of the array of items.
2. The sorting algorithm needs to know how many items and how big each item is
   in order to be able to compare them, swap items at indices and such. The ``nitems``
   argument gives the length of the array and ``size`` gives how many bytes to
   skip from one item to the next.
3. It also needs to know how to declare one item to be "less than" another
   so that it can decide whether to swap them or not. This is the ``compare``
   argument.

It does not need to know anything else about the structure of the array and its items,
and therefore the ``qsort`` function's signature precisely declares only what it
needs.

What's of interest here is the last ``compare`` argument. 

It is common to provide comparison functions with the following contract -

.. code-block:: C

    int my_comparison_fn(thing_t t1, thing_t t2);
    // Returns -1 if t1 is "less than" t2,
    // Returns 1 if t1 is "greater than" t2,
    // Returns 0 if t1 is "equal to" t2.

We could have three functions to do that, but that will usually result in a lot
of duplicate code needing to be written between the three functions and this way
is more compact and sufficiently general. The reason for that redundancy is that
the contract it needs to follow is not merely about how it treats its input
and what set of values it returns. Such a comparison function must also meet some
additional criteria -

1. If ``compare(v1,v2) == compare(v2,v3)``, then ``compare(v1,v2) == compare(v1,v3)``
   as well. This captures the transitive nature of comparisons -

   * If a < b and b < c, then a < c.
   * If a == b and b == c, then a == c.
   * If a > b and b > c, then a > c.

2. ``compare(v1,v2) + compare(v2,v1) == 0``. This captures the reflexivity of
   equality - i.e. a == b implies b == a. It also captures the relationship between
   "less than" and "greater than" - i.e. a < b implies b > a.   

The ``compare`` argument to ``qsort`` also has the same structure as the
comparison function given above, except that the ``thing_t`` is a ``const void
*`` since ``qsort`` doesn't know anything about the type of values in the
array. The ``const`` here is used to signify that the comparison function will
not modify the contents of what it is comparing. That would be a disaster if
we were to permit it.

You can turn an ordinary C function signature into a type that represents
"functions like this" by simply wrapping the function name as ``(*comparison_fn)``
and using it in a ``typedef`` -

.. code-block:: C

    typedef int (*comparison_fn)(thing_t t1, thing_t t2);

Once defined like that, the name ``comparison_fn`` will be a type that represents
"function that takes two ``thing_t`` values as arguments and returns an ``int``".
So for ``qsort``, we could've split the declaration like this as well -

.. code-block:: C

    typedef int (*qsort_comparator)(const void *v1, const void *v2);
    void qsort(void *base, size_t nitems, size_t size, qsort_comparator compare);

... which is equivalent to the previous declaration but perhaps a little
more readable. If we have an array of ``float`` values and want to sort
based on, say, the ``sin`` of these values for some reason, we could do it
like this -

.. code-block:: C

    int sin_compare(const void *v1, const void *v2) {
        const float *f1 = (const float *)v1;
        const float *f2 = (const float *)v2;
        float sf1 = sin(f1[0]);
        float sf2 = sin(f2[0]);
        if (sf1 < sf2) { return -1; }
        if (sf1 > sf2) { return 1; }
        return 0; // Although equality of floats is not good to rely on.
    }

    float values[100];
    // Fill up values array with some values.
    
    // Sort them according to our ``sin_compare`` function.
    qsort(values, 100, sizeof(float), &sin_compare);

The expression ``&sin_compare`` gives us the "function pointer" for the
``sin_compare`` function. Since ``sin_compare`` has the exact same signature
as the ``compare`` argument of ``qsort``, we can simply pass this pointer to
``qsort`` and it will use it to determine the order within the array.

It is arguable that a more flexible way to provide such a sorting facility is
to include a "context" pointer that ``qsort`` can make available to the comparison
function. For example, we may have an array of indices into another array and we
may want to sort the array of indices based on some property of the corresponding
item in the other array. With the current implementation of qsort, you will be forced
to model your problem as an array of pointers first and then convert back to indices.
However, if we include a context pointer that can be passed around, that lets us
model the problem whatever way we want to and write an appropriate comparison
function. Indeed, the ``qsort_s`` function provides exactly that.

.. code-block:: C

    void qsort_s(
        void *base, 
        size_t items, 
        size_t size, 
        int (*compare)(const void *v1, const void *v2, void *context),
        void *context
        );

The expectation is that the context pointer you pass to ``qsort`` will be
passed to every call to the compare function so that additional information it
needs to decide the comparison can be made available through it.

.. [#cblocks] Although Apple added a notion of "blocks" to C which are closures
   https://en.wikipedia.org/wiki/Blocks_(C_language_extension).
