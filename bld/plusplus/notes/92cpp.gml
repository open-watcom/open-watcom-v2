:H3.Project Name: CPP - WATCOM C++ Compiler
:cmt. June 1991 to May 1992
:P.
Work was continued to create a C++ compiler.
The objective of this project was to produce a highly optimizing
C++ compiler compatible with our existing C compiler.
The major areas of development included:
:UL.
:LI.
The C++ language, as it evolves, was thoroughly monitored to determine an
efficient implementation strategy.  This involves active participation in
the ISO/ANSI C++ committee where this evolution is both devised
and mandated.  As well, discussions (via mediums such as electronic
mail) with peers in industry and in academia are continuing regarding
unresolved and new language issues.

:LI.
The representation of complicated concepts within the compiler have to be
developed early in the compiler development phases.  Changes must
be made when it becomes clear that early decisions did not cover
all aspects of the C++ language.  The technical difficulties involved
here include analysis of the problems with the old representation
and the development of a new internal representation.  Since the
C++ language is still evolving, we must also consider potential
future changes so that we design structures that will be general
enough for future enhancments.

:LI.
The development of the virtual function and virtual base features
of the language were quite complicated.
The introduction of virtual bases into the C++ language initially
appeared to be free of surprises.  Unfortunately, a new problem
was found when virtual bases were combined with older features
of the language and new interactions were discovered.
:P.
The problems occurred if a constructor or destructor body of code
called a virtual function in a virtual base.  The adjustor thunk
for the virtual function was static in nature and it could not
compensate for the dynamic rearrangement of class offsets produced
by the new virtual base feature.
This produced incorrect results in C++ programs that made use of
virtual bases under these circumstances.
:P.
Fortunately for WATCOM, we knew about these problems early in the
development cycle.  The solution for the problem was circulating
within the C++ community of experts and soon C++ implementations
started to address the problem.  At WATCOM, we studied the problem
to ensure that we understood the complete picture so that we could
correctly implement the solution.  We found that the solution was
sufficient to solve the general problem provided that the thunk code
was generated properly by the C++ compiler.
Upon completion of this phase of development, we found that we
had discovered some new knowledge in this area.
Currently, we have the only C++ compiler with a complete and
correct implementation of virtual functions and virtual bases.

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
WATCOM C++ matches
existing implementations by having the ability to discard simple functions.
The technical difficulties were primarily in the WATCOM linker.
The WATCOM C++ compiler made use of existing technology that had to
be generalized for the unique aspects of C++ compilation.

:LI.
The C++ language definition contains a mechanism for overloading of functions
based on function argument type.  This mechanism is loosely described in the
literature.
WATCOM has taken this loose description and created a full-fledged
implementation of C++ function overloading.
:P.
The technical difficulties involved in the implementation of an ad hoc
description are considerable.  Our presence on the ISO/ANSI C++
committee was important because it gave us access to the top C++
experts in the world.  Combined with research into past C++ conferences
we were able to develop the necessary rules for function overloading.

:LI.
User-defined conversions are perhaps the least understood
and most complex feature of the C++ language.  Existing
implementations have used largely ad hoc methods to
generate code for C++ expressions.
:P.
The WATCOM C++ compiler is built on a formalized description
of conversions developed at WATCOM.  The combination of
a wide variety of ideas into a unified whole has allowed
WATCOM to develop a robust and accurate implementation of
user-defined conversions.  The formalized description
of user-defined conversions (which includes operator overloading)
represents an important contribution of WATCOM to the
ISO/ANSI C++ standardization effort.  With other members
of the ISO/ANSI C++ committee, we hope to improve the elegance
and simplicity of the description of this facet of the C++ language.

:LI.
The C++ language has an elaborate methodology for handling exceptional
conditions.
Only experimental implementations have been developed.
:P
WATCOM started the implementation effort with the goal of producing a
fully functional C++ compiler with all of the current features described
in the ISO/ANSI C++ draft.  An exception handling mechanism was designed
and the subset that allowed scoped named and unnamed classes to be
destructed was developed.

:LI.
Code sequences for C++ were extensively analysed.
The code generator was extended to handle new sequences and to optimize
those sequences which occur more frequently in C++ than C.
The code generated by WATCOM C++ (and C) continues to be the best of
commercially available compilers.

:LI.
A C++ compiler must be accompanied with support libraries for features such
as input/output, string handling and complex numbers.
The ANSI and ISO Committees continue to refine the definitions for these
libraries.
WATCOM invested considerable effort to implement efficient libraries
to operate in conjunction with the C++ compiler.
This involved applied research into possible algorithms as well as
monitoring and adjusting the code in these libraries.

:LI.
The C++ language is complicated enough that we had to develop and refine
our testing procedures for the WATCOM C++ compiler.  The compiler is
tested during every step of its development to ensure that it performs
correctly in all cases.  This is an iterative process because we continue
to learn and discover new facets of the C++ language.  The testing
has helped us to produce a stable and correct compiler early in the
development cycle.

:eUL.
