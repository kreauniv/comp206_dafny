Linked lists
============
       
.. |O(1)| replace:: :math:`\mathcal{O}(1)`
.. |O(N)| replace:: :math:`\mathcal{O}(N)`
.. |O(logN)| replace:: :math:`\mathcal{O}(\text{log}(N))`

We saw linked lists briefly when looking at binary search trees and what
happens if we use a naive insertion algorithm. Usually, linked lists are dealt
with before trees, but we dealt with trees first since we'd already dealt with
searching and sorting.

That said, linked lists are a strange beast in the computing world.  On the one
hand, they are a generic data structure that some programming languages use to
underpin much of the structures of the language (ex: LisP -- short for List
Processor -- which is actualy a family of languages including Common Lisp,
Scheme and Clojure .. or functional languages like Haskell). The reason for
this is the simplicity of the structure is also its power. That said, in real
world algorithms, linked lists (or lists for short) rarely ever feature due to
the |O(N)| complexity for basic operations like lookup, insert/delete at a
position, and so on. 

And yet, they have a conceptual elegance that you can use them to understand
much about data structures. For example, lists can also be used as trees. We'll
see a taste of the possibilities here.

The LisP family of languages represents a list in text form as a sequence
of items within parentheses. Below is a program represented as a (nested) list.

.. code-block:: scheme

   (define (max x y) (if (> x y) x y)))

In the LisP family of languages, a list expression of the form ``(a b c d)`` is
interpreted as though the first item is an "operator" and the rest are its
"operands". So :math:`1+2` is notated as ``(+ 1 2)``.  The above shown
expression is to be interpreted as the following list structure ::

    [define]|--->()|---->()|----X
                  \       \
                   \       -->[if]|---->()|----->[x]|---->[y]|--->X
                    \                    \
                     \                    -->[>]|--->[x]|---->[y]|----X
                      \
                       -->[max]|---->[x]|---->[y]|---X


A "node" in such a list consists of two "cells" - the first one holds the
"content" of the node and the second one (usually) holds a pointer or
reference to the next node. [#car]_ If you have a mechanism to indicate what type
the "content" is, then you can generalize the notion of content to include
other lists. After all, why shouldn't we be able to make lists of lists?

.. [#car] For historical reasons, the "content" cell is referred to as ``car``
   and the cell holding the pointer to the next cell is referred to as ``cdr``.
   They are respectively the abbreviations of "Contents of Address Register"
   and "Contents of Data Register". An unusual feature of Common Lisp is that
   it lets you talk about the "contents of the address register of the contents
   of the data register" (i.e. the second item in the list) using a short form
   like ``cadr``.

That is the facility used to represent the above program for computing the
maximum of two numbers. You can see that the list picture looks more like a
tree and indeed, it is called the "abstract syntax tree" of the program,
commonly abbreviated as AST. The lisp "language" is a direct representation of
the program's AST. Since the language also has functions using which you can
make and manipulate lists, you can write lisp programs that produce programs
and output which are then passed to the cmopiler. It is even possible in some
lisps to write programs that can modify themselves. It is for that reason that
lisps enjoyed support during the early days of artificial intelligence when
"symbolic AI" was the dominant paradigm.

In code, a list's form (i.e. interface) should be quite familiar to you by now --

.. code-block:: C

    typedef struct Node *list_t;

    struct Node {
        void *content;
        list_t next;
    };

    // Constructs a new node with the given content
    // and with the next pointer pointing to ... well,
    // the rest of the list. This function is also known
    // as "cons" -- short for "construct a node".
    list_t mk_node(void *content, list_t next);

    // These get the parts of a list's node.
    // It is conventional to talk about the first
    // node of a list as its "head" and the rest of the
    // list as its "tail".
    void *get_content(list_t node);
    list_t get_tail(list_t node);

    // These modify the respective parts of a node.
    void set_content(list_t node, void *content);
    void set_tail(list_t node, list_t tail);

As with a tree, a list is also a recursively defined
data structure, where the "rest of the list" is of the
same type as the list.

All algorithms working on singly linked lists can be written
using the above set of functions. To represent the end of a list,
we use the ``NULL`` pointer. So when a node has a ``next`` field
which is ``NULL``, it means there is no following node in the list.
    
.. admonition:: Exercise

   Write a function to compute the length of a given list only using the above
   functions. The list is passed as a pointer to the first node. What
   considerations will apply?

For example, here is a function to insert/delete a given node after
a particular identified node in the list --

.. code-block:: C

    void insert_after(list_t node, list_t insertee)
        //@requires node != NULL
        //@requires insertee != NULL
        //@ensures node->next == insertee
    {
        list_t tail = get_tail(node);
        set_tail(insertee, tail);
        set_tail(node, insertee);

        // Note that if the insertee had a tail, that 
        // will now be forgotten. So the following function
        // might be safer.
    }

    void insert_content_after(list_t node, void *content)
        //@requires node != NULL
    {
        insert_after(node, mk_node(content, node->next));
    }

    void delete_after(list_t node)
        //@requires node != NULL
    {
        list_t tail = get_tail(node);
        if (tail == NULL) {
            return;
        }

        set_tail(node, get_tail(tail));
        // Note that the ``tail`` pointer will now
        // be forgotten since, at least within this function,
        // there are no more references to the tail once
        // the variable goes out of scope.
    }


Circular lists
--------------

Given that the next pointer of a node of a list can refer to any node,
what if we set up a list such that the next pointer of the 10th node
points to the 5th node?

Such a list does not have a defined "length" property. So when trying to
compute the length of a list, we will need to ensure that the list is
not circular.

There is an elegant algorithm called the "hare and tortoise algorithm" that can
be used to determine whether a list is circular or not.  Note that if we
linearly scan a list, we will reach the end if and only if the list is not
circular. So a simple linear scan will not suffice.

Take a circular list where we number the nodes in the order we can visit them,
with the first node being 0 as usual. Furthermore, let's say the list has (an
unknown) :math:`N` nodes that we can visit before it loops back to the node
numbered :math:`l`. We can then see how if we try to reach the :math:`k`-th
node after :math:`k` steps, we will land on the node numbered :math:`k` if
:math:`k \leq l`, and on the node numbered :math:`n(k) = l + (k - l) \text{mod}
(N-l)` if :math:`k > l`.

The "hare and tortoise algorithm" is based on the observation that :math:`n(k)
= n(2k)` if :math:`k \text{mod} (N-l) = 0`. So we start with two pointers to
the beginning of the list. In each iteration we step one of them -- the
"tortoise" -- by one node down the list, and the other -- the "hare" -- by two
nodes. If the pointers ever become equal again, then the list is circular.
Otherwise the hare will reach the end of the list first and we can terminate
the iteration.

.. admonition:: Exercise

   Write a function ``bool is_circular_list(node_t n)`` which takes a pointer
   to a node and uses the hare-tortoise algorithm to find out whether the list
   is circular.

For the purpose of this course, we'll stop there with lists and revisit them
when dealing with hash tables.



