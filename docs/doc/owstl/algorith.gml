:H1.Introduction
:P.
The algorithm header (truncated to :Q.algorith:eQ. for 8.3 file name 
compatibility) contains definitions of the algorthms from chaper 25 of 
the standard.


:H2.Status
:P.
:AUTHOR date='20 Jun 2006'.P Chapin, D Cletheroe
:P.
Reviewer: Not reviewed
:P.
About two thirds of the required algorithms have been implemented. For a
list of those remaining, see the Wiki web site.

:H1.Design Details
:P.
Most of the standard algorithms are template functions that operate on
iterators to perform some common task. Each function template is quickly
addressed in the sections that follow. They are generally quite simple
and looking directly at the source may be the simplest form of
information.

:P.
A number of the algorithms come in both a form that uses
:CODE.operator<
or
:CODE.operator==
(as appropriate) and in a form that uses a predicate. The predicate form is
more general. The non-predicate form can be implemented in terms of the
predicate form by using the function objects in
:PATH.functional.
In theory implementing the non-predicate forms in terms of the predicate
forms should not entail any abstraction penalty because the compiler should
be able to optimize away any overhead due to the function objects. Some
investigation was done using Open Watcom v1.5 to find out if that was true.
In fact, the compiler was able to produce essentially identical code for
the non-predicate functions that were implemented directly as for
non-predicate functions that were implemented in terms of the predicate
functions. However, at the call site, there was some abstraction penalty:
the compiler issued a few extra instructions to manipulate the (zero sized)
function objects.

:P.
These experiments led us to conclude that the non-predicate functions
should be implemented directly for short, simple algorithms where the extra
overhead might be an issue. For the more complex algorithms, the
non-predicate forms should be implemented in terms of the corresponding
predicate forms. The extra overhead of doing so should be insignificant in
such cases and the savings in source code (as well as the improved ease of
maintanence) would make such an approach desirable.

:P.
If the compiler's ability to optimize away the function objects improves,
this matter should be revisited.

:H2.*_heap
:P.
The functions
:CODE.push_heap, pop_heap, make_heap,
and
:CODE.sort_heap
support the manipulation of a heap data structure. Currently only
versions using an explicit
:CODE.operator<
have been implemented. The versions taking a comparison object
have yet to be created. Several heap related helper functions have been
implemented in namespace
:CODE.std::_ow.
These functions are not intended for general use.

:P.
There is a compiler bug that prevents the signature of the internal
:CODE.heapify
function from compiling. This has been worked around by providing the
necessary type as an additional template parameter. See the comments in
:PATH.algorith.mh
for more information.

:H2.remove remove_if
:P.
These functions :Q.remove:eQ. the value that compares equal or the
element at which the predicate evaluates != false. Because iterators
can't be used to access the underlying container the element can't
really be removed. It may well be the container is just a bit of stack
and the iterator a pointer. These functions instead copy elements from
the right (an incremented iterator) over the top of the element that is
:Q.removed:eQ. and then return an iterator identifying the new end of
the sequence. The initial implementation just called the
:CODE.remove_copy
and
:CODE.remove_copy_if
functions described below. This would perform unnecessary copies on top
of the same object if there any values at the begining of the container
that aren't to be removed. This could cause a bit of performance hit if
the object is large and there are lots of objects that don't need to be
removed, therefore these functions were re-written to be independent of
the the
:CODE.*_copy
versions and perform a check for this condition.

:H2.remove_copy remove_copy_if
:P.
This makes a copy of the elements that don't compare equal, or when the
predictate is false, starting at the location given by Output.
It is a simple while loop over the input iterator first to last, either
just skipping the element or copying it to the output.

:H2.unique
:P.

For C++98 and C++2003 there is an open library issue regarding the behavior
of
:CODE.unique
when non-equivalence relations are used. The standard says that the
predicate should be applied in the opposite order of one's intuition. In
particular:
:CODE.pred(*i, *(i-1)).
This means the predicate compares an item with its previous item.

:P.
The resolution of the open issue suggests that non-equivalence relations
should not be premitted. In any case, the standard should apply the
predicate between an item and the next item:
:CODE.pred(*(i-1), *i).

:P.
The Open Watcom implementation follows the proposed resolution and thus
deliberately violates the standard. Most (all?) other implementations do
the same.

:H2.find_first_of
:P.
There are two versions of this, one that uses
:CODE.operator==
and one that uses a binary predicate. There is a simple nested loop to
compare each element with each element indexed by the 2nd iterator
range.

:H2.find_end
:P.
There are two versions of this, one that uses
:CODE.operator==
and one that uses a binary predicate. The main loop executes two other
loops. The first loop finds an input1 element that matches the first
input2 element. When a match is found the second loop then checks to see
if it is complete match for the subsequence. If it is, the position the
subsequence started is noted and the main loop is iterated as there may
be another match later on. Note this can't search for the substring
backwards as the iterators are ForwardIterators.

:H2.random_shuffle
:P.
The
:CODE.random_shuffle
template with two arguments has been implemented using the C library
function
:CODE.rand.
However, the 1998/2003 standard is unclear about the source of random
numbers that
:CODE.random_shuffle
should use. There is an open library issue about this with the C++
standardization group. See
http://anubis.dkuug.dk/JTC1/SC22/WG21/docs/lwg-active.html, item #395.
The proposed resolution is to allow the implementation to use
:CODE.rand
without requiring it to do so (the source of random numbers is proposed
to be implementation defined).

:P.
The problem with
:CODE.rand
in this case is that Open Watcom's implementation of
:CODE.rand
is limited to 16 bits of output even on 32 bit platforms. This means
that
:CODE.random_shuffle
will malfunction on sequences larger than 32K objects. This is a problem
that needs to be resolved. The solution, probably, will be to provide 32
bit random number generators as an option (check this: has it already
been done?)

:H2.sort
:P.
The
:CODE.sort
template is implemented using a QuickSort algorithm. This was shown to be
significantly faster (over twice as fast) as using a HeapSort based on the
heap functions in this library. This implementation of QuickSort is
recursive. Since each recursive call has private state, it is unclear if a
non-recursive version would be any faster (at the time of this writing, no
performance comparisons between recursive and non-recursive versions have
been made). Stack consumption of the recursive implementation should be
O(log(n)) on the average, which is not excessive. However, the stack
consumption would be O(n) in the worst case, which would be undesirable for
large n.

:H2.add quick descriptions of other algorithms here...
:P.
