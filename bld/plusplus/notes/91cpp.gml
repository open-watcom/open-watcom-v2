:H3.Project Name: CPP - WATCOM C++ Compiler
:H4.Common Challenges: 2.1, 2.2, 2.3, 2.5, 2.6
:P.
Work was begun to create a C++ compiler.
The objective of this project was to produce a highly optimizing
C++ compiler compatible with our existing C compiler.
The major areas of development included:
:UL.
:LI.
The C++ language was thoroughly investigated to determine an
efficient parsing strategy.

:LI.
A prototype implementation of the language was created using a
recursive-descent (LL(1)) parser.
It soon became clear that this method could not handle the
complexity of the C++ language.
Representations
for C++ classes and inheritance were developed in the prototype.
These
representations are still present in the current WATCOM C++ compiler.
The prototype experiment revealed that new techniques would be required for
the WATCOM C++ compiler.
The problems of parsing ambiguities inherent in the
C++ language was becoming evident at this time.

:LI.
A new implementation was started with a LALR(1) grammar being used for
parsing.
An attempt to resolve the parser ambiguities via two parsers
being run simultaneously on the same input was developed.
This approach
was maintained for 3-4 months until cases were found that rendered it
a failed approach.
The complexity of the approach (each parser had to
be insulated from affecting the other) was also another factor against it.
The parallel parser approach (with insulation between parsers) was abandoned.
We switched over to a traditional LALR(1) implementation that diagnosed
ambiguity situations as errors.
At the time of writing (March 1991), we
still have this operational but with new knowledge from the London, England
ANSI C++ meeting we are developing an approach which should solve all the
known problems.
We do not know of any other implementation of C++ which
correctly parses all know C++ fragments.
We hope to be the first with a fully functional parser.
This approach will probably be publishable
if it becomes clear that no other company markets a correct C++ parser.
:eUL.
