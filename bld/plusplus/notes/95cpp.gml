:H3.Project Name: CPP - Watcom C++ Compiler
:cmt. Feb. 11, 1994 - Dec. 31, 1994
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
The technology developed is a significant asset for Watcom.
:P.
This technology is still leading edge years after its introduction.
The correctness of the C++ compiler parsing has enabled Watcom to
contribute key insights to the C++ standardization effort.
In fact, section 8.2 paragraphs 2 to 6 were copied verbatim from
a paper presented by Watcom.

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
Watcom has devised a method whereby all unreferenced items can be discarded.
This technology requires partial snapshots of the program structure
to be combined by the Watcom linker.  The technical difficulties
included how to represent the information, how to minimize
the information, and how the linker should use the information.
This technology will enable Watcom to produce smaller programs than
competing companies.
:P.
This year, work continued in the area of improved linking support.
Refinements in the compiler-linker relationship have allowed Watcom C++
to improve the correctness of our C++ implementation along with
improving the linker response time.
:P.
The increased use of Dynamic Load Libraries (DLL) has impacted our
linker level optimization technology.  The use of a DLL forces the
linker to assume that it cannot see the entire program thus reducing
the possibilities for optimization.  Watcom is researching new techniques
that are compatible with the new view of programs using DLLs.

:LI.
The C++ language has an elaborate methodology for handling exceptional
conditions.
:P
Watcom started the implementation effort with the goal of producing
a fully functional C++ compiler with all of the current features
described in the ISO/ANSI C++ draft.
Watcom has implemented the complete definition of C++ exceptions.
:P.
This year we introduced a higher speed and more correct implementation
of C++ exception handling based on our early experience.  We are currently
maintaining our lead in the marketplace with our exception handling
technology.
:P.
Based on our experience with the new method of exception handling, we
developed techniques within the compiler so that the compiler is simpler
to maintain and enhance.  The end result is object code that is similar
to our faster implementation but much easier to maintain and verify correctness.

:LI.
A larger view of optimization is required for high performance C++ compilers.
The research into optimizing C++ code is still in its infancy so a lot
of the optimizations included in the Watcom C++ compiler are completely new.
:P.
This year saw some advanced analysis of simple constructors and destructors
applied to optimization.  The Watcom C++ compiler analyzes complete
structures in order to optimize the generation of compiler generated
routines.  The introduction of some new code level optimizations designed
for C++ in the code generator has also solidified our position as the
best optimizing compiler.
:P.
New optimizations with regard to identifying functions that do not
throw exceptions were developed this year.  These techniques reduce
or eliminate the need for compiler generated "book keeping" code that
contributes to the bloat of current C++ generated code.  
The C++ compiler that contains these optimizations is still in
development and has not been released yet.

:LI.
The C++ language is complicated enough that we had to develop and refine
our testing procedures for the Watcom C++ compiler.  The compiler is
tested during every step of its development to ensure that it performs
correctly in all cases.  This is an iterative process because we continue
to learn and discover new facets of the C++ language.  The testing
has helped us to produce a stable and correct compiler early in the
development cycle.  This has been an on going process throughout the
development of the compiler.
:P.
This year has seen a large growth in our regression test suite.  A
regression test suite forms a repository for reported problems with
the compiler along with a self checking test case (if possible).
This technology helps to prevent the reintroduction of problems
into the compiler that can happen easily due to the complexity
of the compiler.
:P.
The regression test suite has been expanded and is now being used
everyday to verify the correctness of the current shipping version
of the C++ compiler.  This was done before major releases in previous
years but we have found that running the regression test stream
everyday helps to keep problems in the compiler to a minimum.
The expanded use of C++ at Watcom is also an important component
of our testing of the C++ compiler.


:LI.
Compiling the C++ language can be time consuming for a developer since
the compilers are often processing the same code over and over again
even though it is known that there are no errors.  A method for not
duplicating the analysis of already working code moved up in priority
with the quantum leap in operating system complexity this year.
The header files for the new operating systems are richer and more
complicated than ever before so speeding up the processing of these
files of source code became an important task.
:P
The method we used to solve this problem is called a pre-compiled header file.
The problems encountered in implementing this feature of the compiler include
saving and restoring the complete internal state of the compiler.  This 
involved research into the latest papers on persistent objects combined
with some Watcom innovations.  The completed implementation satisfied our
goal of increased throughput during compilation of C++ source code.
:P.
The advent of new RISC architectures has reuired some changes to our
pre-compiled header file implementation.  These changes have been made
to the C++ compiler but have not been tested on true RISC architectures
yet.

:eUL.

:H5.Run-time libraries

:P.
The Watcom C++ run-time libraries were all developed by Watcom.
Watcom has implemented compiler support routines,
as well as String, Complex and iostreams class libraries.
The industry has only settled on de facto standards for behaviour
in the area of the iostreams class library.
The current ISO C++ standard does not fully define the behaviour
required of the runtime library.
:P.
The tug between existing code and the new ISO C++ Working Paper
continues.  This often requires delicate changes to the various
C++ run-time libraries to maintain an acceptable balance between
these opposing forces.


:H6.The compiler support routines

:P.
The ISO C++ standard was changed to specify that the life-time of 
temporary objects is the duration of a statement. The WATCOM compiler
was implemented with life-times being the duration of a scope.
:P
To support this new definition,
the Watcom C++ compiler exception handling mechanism was restructured.
This required a complete rewrite of most the compiler support routines
because the destruction of objects and exception-handling are an integrated
feature of the language.
:P
The new implementation allows both inline destruction and table-driven (using
run-time routines) destruction of objects.  This increased flexibility permits
a user to generate code optimized for space or time in various ways.
:P.
The use of Operating System specific exception handling mechanisms has
been used in the C++ compiler run-time.  This enables high-speed registration
of exception handling "book keeping" data structures for multi-threaded
applications.  Watcom is exploring the use of more OS-specific exception
handling mechanisms.


:H6.The iostreams class library

:P.
To ensure compatibility with the largest
number of users of the iostreams class library, competitive products,
the draft specification and published works were consulted.
The Watcom iostreams class library was evaluated in terms of conformance
to the de facto industry standard.
:P.
In the process of researching and evaluating the Watcom iostreams
class library conformance an extensive set of test programs were created.
These programs were then bundled into an automated test stream
to ensure continued conformance in the face of future demands on the library.
The construction of the automated test stream environment was complicated
by the multi-platform nature of the Watcom tools.
Each test program is run under all platforms that are supported and must
always produce identical results.  Building the infrastructure required
to support this form of automated test execution and output collection
and verification required further development.
:P.
Additional work was done to ensure that the iostreams class library
performance was in keeping with the Watcom standard for excellence.
This involved creating benchmark programs and then running these
with competitive products.  Deficiencies were found in the Watcom support.
The class library was then modified to address these deficiencies.
