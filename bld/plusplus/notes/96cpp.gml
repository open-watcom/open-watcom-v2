:H3.Project Name: CPP - Watcom C++ Compiler
:cmt. Jan. 1, 1995 - Dec. 31, 1995
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
:P.
The linker-level optimizations are not user-friendly when it
comes to using object files that do not have the correct
information in them.  To address this problem, the first steps
of adding a signature to the object file that indicates that
the contents cannot be optimized has been initiated.
This will go a long way to improving the development
process using the Watcom C++ compiler.


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
:P.
The regression test suite is continuing to be a strategic
tool for the Watcom C++ compiler.  It's influence is spreading
to many of the other tools within the Watcom C++ product.
This year saw the adoption of the C++ test harness by
one to two projects within Watcom.


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
The advent of new RISC architectures has required some changes to our
pre-compiled header file implementation.  These changes have been made
to the C++ compiler but have not been tested on true RISC architectures
yet.
:P.
The RISC targeted C++ compiler is now in final stages of testing
and the pre-compiled header technology has in fact changed to adapt
to the new environment.


:LI.
The adoption of the C++ compiler as an interactive tool has been
the primary development effort for this year.
The first problem that had to be solved was to develop
techniques to "restart" the compiler so that the compiler
executable did not have to be reloaded before compiling
another source file.  This is accomplished in modern operating
systems by creating a C++ compiler Dynamic Loadable Library (DLL).
:P.
The C++ compiler uses a large shared code base so the "restart"
problem has to pushed into all of the code that the C++ compiler
uses in a client manner.  Some of the code is obsolete and so
it could not be changed easily, for these cases, the C++ compiler
had to adapt to the obsolete code's requirements.
:P.
Interactive environments demand high speed compilation.
There are two possibilities for increasing the speed
of a compilation, namely, make the compiler do less
work and feed less data into the compiler.
:P.
Extensive overhaul of the compiler internals produced
a speed gain of roughly 15-20%.  Improvement of the
pre-compiled header file load posted dramatic gains
in compile speed.  The reduction of data was aided
by instrumenting the compiler internally to produce
reports on what was happening in the compiler.
This allowed us to modify our input into the compiler
so that repeated compiles were faster than our fastest
competitor.


:LI.
The compilation of class and function templates is still
an ad hoc process.  The internals of the compiler seem
to be robust enough to handle most templates but because
the ISO C++ WP is still not very precise, it is difficult
to decide when to overhaul the compiler handling of templates.
:P.
This year saw the incremental improvement of the template
handling of the compiler.  We hope to add some of the new
features in the ISO C++ WP in the near future.


:LI.
A common theme in the development of the C++ compiler has been
the balance between conforming to the ISO C++ WP and maintaining
compatibility with legacy source code.  The competitive pressure
to adopt subtle changes that allow the Watcom C++ compiler to
compile our competitor's source code is also present.
We have taken the path of issuing warnings in preference to
hard errors for violations that can still generate acceptable
code.  This allows us to diagnose problems in source code
for users that are concerned about minimizing problems but
still support compilation of legacy source code without any
hard errors.
:P.
Another problem with conformance to the ISO C++ WP is that
the standard is still highly volatile and subject to change.
We have taken the stand that we won't implement anything
unless it has survived for roughly a year without change.
Even this conservative manner has resulted in our having
to change the compiler because of a substantial change in
the ISO C++ WP.  We continue to attend the standards
committee meeting in the hope that we can minimize these
kinds of surprises.
:P.
As the WP chapter on overloading has remained mostly unchanged for at 
least a year, our support for overloading has undergone major revisions 
in order to conform more closely to the WP.  To thoroughly 
understand the WP chapter on overloading, several other areas of the WP 
needed to be revisited and re-interpreted in light of their use in the 
context of overloading.  The result was not only the change of overloading 
code, but also the rewrite of type conversion code in order to avoid 
duplication of logic and inconsistent interpretation of the standard 
within the compiler.
:P.
In several cases, it was impossible to conform to both the WP and the
existing practices of our major competitors.  This created the need for the 
compiler to work either way depending on how it is invoked.  In such 
situations, reverse engineering is required in order to understand how the 
competitor must have interpreted the WP.  Once this is established,
a consistent implementation can be created.  
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
