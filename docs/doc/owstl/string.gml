:H1.Introduction
:P.
The class template :CLASS.std::~string
provides dynamic strings of objects with a type given by the type
parameter :CLASS.CharT.
The behavior of :CLASS.CharT
objects is described by a suitable traits class. By default a
specialization of :CLASS.std::~char_traits< CharT >
is used. Specializations of :CLASS.std::~char_traits
for both character and wide character types are part of the library and
are used without any further intervention by the programmer.

:P.
Most of the methods in class template :CLASS.std::~string
are located in :PATH.hdr/watcom/string.mh.
This file is also used to generate the C library header :PATH.string.h
and the corresponding "new style" C++ library header :PATH.cstring.
This is accomplished by executing :TOOL.wsplice
over the file multiple times using different options. The material
that goes into the C++ library header :PATH.string
appears in :PATH.string.mh
below the C library material.

:P.
The class template :CLASS.std::~char_traits
along with its specializations for character and wide character, the
definition of :CLASS.std::~string,
and certain methods of :CLASS.std::~string
are located in :PATH.hdr/watcom/_strdef.mh.
This file generates the header :PATH._strdef.h
which is not to be directly included in user programs. It is, however,
included in :PATH.string
thus completing the contents of :PATH.string.
The reason for this separation of :PATH.string
is because of the exception classes. The standard exception classes use
strings and yet some of the methods of string throw standard exceptions. This
leads to circular inclusions which are clearly unacceptable. To resolve
this problem, the parts of :PATH.string
that are needed by the standard exception classes are split off into
:PATH._strdef.h.
These parts do not themselves need the standard exceptions and so the
circular reference is broken.

:H2. Status
:P.
:AUTHOR date='4 Dec 2005'.Peter C. Chapin
:P.
Reviewer: Not reviewed
:P.
Most of the required functionality has been implemented together with
moderately complete regression tests. There has so far been very little
user feedback, however.

:P.
The main component that is missing is the I/O support for
:CLASS.std::~string.
Implementing this component has been put on hold until the iostreams
part of the library is reworked. In the meantime users will have to do
string I/O using C-style strings and convert them between
:CLASS.std::~string.
This is a significant issue; it is assumed that most standard programs
will do I/O on strings directly and the library doesn't currently support
such programs no matter how complete the :CLASS.std::~string
implementation itself might be.

:P.
In addition to the problem above, the template methods of
:CLASS.std::~string
have not been implemented because the compiler does not yet support
template methods sufficiently well.

.H1. Design Details
:P.

.H2. Copy-On-Write?
:P.
This implementation of
:CLASS.std::~string
does not use a copy-on-write or a reference counted approach. Instead
every string object maintains its own independent representation. This
was done, in large measure, to simplify the implementation so that a
reasonable
:CLASS.std::~string
could be offered quickly. However there are a number of difficulties
with making
:CLASS.std::~string
reference counted and it is worth reviewing those issues here.

:P.
The fundamental problem is that the
:CLASS.std::~string
interface leaks references to a string's internal representation. It
could be argued that this is a design problem with
:CLASS.std::~string.
Consider the following program.

:P.
:XMP
#include <iostream>
#include <string>

using namespace std;

int main()
{
  string s1( "Hello" ), s2;
  char &c( s1[0] );

  s2 = s1;  // s1 and s2 perhaps share representations
  c = 'x';  // Do both s1 and s2 change?
  if( s2[0] == 'x' )
    cout << "Wrong!\n";
  else
    cout << "Right!\n";
  return 0;
}
:eXMP

:P.
The value semantics of
:CLASS.std::~string
require that modifying one string should not influence the value seen in
another logically distinct string. Thus all correct implementations of
:CLASS.std::~string
should produce "Right!" for the program above.

:P.
To deal with this case properly while using reference counted strings,
the implementation must "unshare" the representation whenever a method
is called that leaks a reference to that representation. The method
:FUNC.operator[]
is one example of such a method. In fact, section 21.3, paragraph 5 of
the C++ standard contains explicit language regarding this issue. The
standard allows implementations to invalidate references, pointers, and
iterators to the elements of a
:CLASS.basic_string
sequence whenever, for example, the non-const
:FUNC.operator[]
is called. However, this leads to rather unexpected behavior in at least
two respects. In particular:

:OL
:LI.
Accessing a string might be an O(n) operation.
:LI.
Accessing a string might cause a
:CLASS.std::bad_alloc
exception to be thrown.
:eOL

:P.
The first issue is a concern to those doing time sensitive operations,
such as those writing embedded systems (Open Watcom's support for 16 bit
8086 targets might be attractive to such programmers). In fact,
:CLASS.std::~string
provides a
:FUNC.reserve
method specifically to give the programmer some degree of control over
when allocations are done. Copying a string's representation
unexpectedly when a string is accessed frustrates this intention.

:P.
The second issue is a concern to those writing robust, exception
safe code. To build code that is exception safe it is important to know
when exceptions might be thrown. A savy programmer might know that
calling the
:CLASS.std::~string
:FUNC.operator[]
might throw an exception. However because that is an unnatural
side-effect many programmers won't be expecting it and thus using such
an implementation will be error prone. Note that on some systems,
notably Linux, the operating system will usually terminate the program
when it runs out of memory before
:CLASS.std::bad_alloc
can be thrown. However, that is not the case on smaller, real-mode
systems like DOS. Thus for Open Watcom this issue is a concern.

:P.
In a multithreaded program reference counted strings encounter other
problems. Since Open Watcom supports a number of multithreaded targets
this is also a concern.

:P.
The C++ standard does not address the semantics of programs in the face
of multithreading. However, most programmers implicitly assume the
following behavior (described by SGI in the documentation for their STL
implementation). [Should this discussion be moved to a more generic part
of this document? Some of this would be applicable to the whole OWSTL
library.]

:OL
:LI
Two threads can read the same object without locking. This means that if
reading an object changes its internal state, the implementation must
provide appropriate locking.
:LI
Two threads can operate on logically distinct objects without locking.
This means that if objects share information internally, the
implementation must provide appropriate locking.
:LI
If two threads operate on the same object and at least one of the
threads is modifying that object, the programmer must provide locking.
This means that the implementation does not need to protect itself from
this case.
:eOL

:P.
Reference counted strings must deal with both situations 1 and 2 above.
This means they must provide locks on the representations and use them
when appropriate. The problem with this is that strings are rather low
level objects and locking them is generally inappropriate. Most strings
are used entirely by one thread; locks are usually only needed on larger
structures. For example consider the following function:

:P.
:XMP
typedef std::map< std::string, std::string > string_map_t;

string_map_t global_map;

void f( )
{
  // Modify the global_map.
}
:eXMP

:P.
If more than one thread is modifying the global map it would be
appropriate to include a lock for the entire map. Locking the individual
strings in the map would most likely be too fine-grained since a single
transaction might involve updating several strings. It would be
important to serialize the entire transaction. Locking the components of
the transaction separately would be incorrect.

:P.
Yet a reference counted implementation of
:CLASS.std::~string
must add locking to the strings themselves to ensure correct behavior
when apparently unrelated strings are simultaneously modified. This
would be adding a large amount of logically unnecessary locking overhead
in cases such as the one above. This overhead can cause reference
counted strings to have very poor performance when used in a
multithreaded environment [reference?]. This is particularly ironic
considering that reference counting is intended to improve string
performance.

:CMT Are there also deadlock problems in an MT program?

:P.
Concerns about the day-to-day performance of Open Watcom's non-reference
counted implementation have been partially addressed by the results of
some (minimal) benchmark tests. See
:PATH.bld/bench/owstl.
These tests show that the current performance of
:CLASS.std::~string
is at least competitive with that offered by other implementations. More
complete benchmark testing is needed to verify this result.

:P.
It is interesting to note that
:TOOL.gcc,
which at the time of this writing (2005) uses a reference counted
approach, has extraordinarly poor performance on these benchmark tests.
If this result stands up to further investigation it would be dramatic
evidence that a reference counted approach does not automatically ensure
good performance. In fact I am lead to wonder if the
:TOOL.gcc
maintainers did any benchmark studies of their implementation or if they
just assumed that it would be fast because it is reference counted.
Either way this highlights the importance, in my mind, of following up
performance assumptions by making real measurements on the final
implementation. One should always verify that any change designed to
improve performance actually does improve performance before committing
to it.

:H2. Design Overview
:P.
This implementation of
:CLASS.std::~string
uses a single pointer and two counters to define the buffer space
allocated for a string. One counter measures the length of the allocated
space while the other measures the number of character units in that
space that are actually used. In order to meet the complexity
requirements of the standard,
:CLASS.string
allocates more space than it needs, increasing that amount of space by a
constant multiplicative factor whenever more is needed. This
implementation uses a multiplicative factor of two [note: other factors,
such as 1.5, might be more desirable; a factor of two causes somewhat
inefficient memory reuse characteristics [reference?]]. The capacity of
a string is always an exact power of two. When a string is first created
it is given a particular minimum size for its capacity (currently 8) or
a capacity that is the smallest power of two larger than the new
string's length, whichever is larger.

:P.
A string's capacity is never reduced in this implementation. Once a
string's capacity is increased, the memory is not reclaimed until the
string is destroyed. This was done on the assumption that if a string
was once large it will probably be large again. Not returning memory
when a string's length shrinks reduces the total number of memory
allocation operations and reduces the chances of an out of memory
exception being thrown during a string operation. However, this design
choice is not particularly friendly to low memory systems. Considering that
Open Watcom targets some small machines, an alternative memory
management strategy might be worth offering as an option. In the
meantime programmers on such systems should be careful to destroy large
strings when they are no longer needed rather than, for example, just
calling 
:FUNC.erase.

:H2. Relationship to vector
:P.
The
:CLASS.std::~string
template is very similar in many ways to the
:CLASS.std::~vector
template. In fact, in OWSTL both implementations use a similar
representation technique and a similar memory management approach.
However, the implementation of
:CLASS.std::~vector
is more complicated because the objects in a vector need not be of a POD
type (as is the case for string) so they need to be carefully copied and
initialized using appropriate methods. In contrast the
:CLASS.CharT
type used by
:CLASS.std::~string
can be copied and moved with low level memory copying functions (see
:CLASS.std::char_traits
).

:H2. Open Watcom Extensions
:P.
Because of the widespread demand for case insensitive string
manipulation, OWSTL provides a traits class that includes case
insensitive character comparisons. An instantiation of
:CLASS.std::~string,
called
:CLASS._watcom::~istring
is provided that uses this traits class.
