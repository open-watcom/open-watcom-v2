:H3.Project Name: CPP - WATCOM C++ Compiler
:cmt. June 1992 to May 1993
:P.
Work was continued to create a C++ compiler.
The objective of this project was to produce a highly optimizing
C++ compiler compatible with our existing C compiler.
The major areas of development included:
:UL.

:LI.
The C++ language syntax is ambiguous.  A statement, of unbounded length, can
be syntactically either a declaration or an executable call to a function.
In such situations, a declaration must be chosen.
This presents a significant implementation problem which competing
implementations have attempted to solve heuristically: all known
implementations fail after a small number of input tokens.
:P
A new parsing implementation was completed using a LALR(1) grammar.
Parsing ambiguities are resolved by executing simultaneous parsers
on the same input.
We previously abandoned this approach as unworkable because we thought
that an exponential number of parallel parsers might be required
for pathological C++ statements.  An exponential number of parsers
would have meant that a large amount of computer resources would be
required for statements with a small number of ambiguities.
Further study revealed that the C++ language was defined in such
a way that we could resolve ambiguities in the language using
a linear number of parsers.  This was a major breakthrough because
a statement with 10 ambiguities that would have required 1024 parsers
can be disambiguated with 20 parsers with our current implementation.
:P
We do not know of any other commercial implementation of C++ which
correctly parses the complete C++ language.
The technology developed is a significant asset for WATCOM.

:LI.
The C++ language definition may cause a function and its data to be
generated from many separately-compiled source modules.
Duplicates need to be discarded when the object files are linked together
to form a program.
This requires the linker to recognize common elements and to appropriately
combine them.
:P
The language specifies all the functions which operate in conjunction
with a C++ class.
A specific program may require only a subset of all the possibilities.
The linker can discard those functions and data elements that are not
required by a specific program.
:P
Existing implementations have been observed to discard simple functions.
No implementation successfully discards virtual functions.
:P
WATCOM has devised a method whereby all unreferenced items can be discarded.
This technology requires partial snapshots of the program structure
to be combined by the WATCOM linker.  The technical difficulties
included how to represent the information, how to minimize
the information, and how the linker should use the information.
This technology will enable WATCOM to produce smaller programs than
competing companies.

:LI.
The C++ language defines a new class of pointers for referencing members
within objects.  The definition and required behaviour of member pointers
has been fully investigated by WATCOM.
:P.
WATCOM has completely implemented C++ member pointers.
This implementation allows fully general conversion of member pointers in
arbitrarily complex inheritance hierarchies.
No other implementation supports fully general member pointer conversion.

:LI.
The C++ language has an elaborate methodology for handling exceptional
conditions.
Only experimental implementations have been developed.
:P
WATCOM started the implementation effort with the goal of producing
a fully functional C++ compiler with all of the current features
described in the ISO/ANSI C++ draft.
WATCOM has implemented the complete definition of C++ exceptions.
The design will allow asynchronous exceptions to be processed in the future.
No other implementation can handle asynchronous exceptions.

:LI.
A larger view of optimization is required for high performance C++ compilers.
The research into optimizing C++ code is still in its infancy so a lot
of the optimizations included in the WATCOM C++ compiler are completely new.
High performance exception handling has not been achieved yet but it
remains a goal of the WATCOM C++ compiler development.

:LI.
The C++ language is complicated enough that we had to develop and refine
our testing procedures for the WATCOM C++ compiler.  The compiler is
tested during every step of its development to ensure that it performs
correctly in all cases.  This is an iterative process because we continue
to learn and discover new facets of the C++ language.  The testing
has helped us to produce a stable and correct compiler early in the
development cycle.  This has been an on going process throughout the
development of the compiler.
:P.
This year we developed tools to extract out source code examples
from our error message documentation.  These source code examples
were then fed back into the compiler to ensure that the documentation
and compiler were always in synch.

:eUL.
