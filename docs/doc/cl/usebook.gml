.pp
This book is a description of the C programming language as implemented
by the
.*
.************************************************************************
.*
..if '&target' eq 'PC' ..th ..do begin
&wcboth.
compilers for the 80x86 family of processors.
..do end
..el ..if '&target' eq 'PC 370' ..th ..do begin
&wcall. compilers.
..do end
.*
.************************************************************************
.*
It is intended to be an easy-to-read description of the
C language.
The ISO C standard is the last word on details about the language,
but it describes the language in terms that must be interpreted for
each implementation of a C compiler.
.pp
This book attempts to describe the C language in terms of
general behavior, and the specific behavior of the
C compiler
when the standard describes the behavior as
.ix 'implementation-defined behavior'
.us implementation-defined.
.*
.************************************************************************
.*
..if '&target' eq 'PC' ..th ..do begin
.shade begin
Areas that are shaded describe the interpretation of the behavior
that the &wcboth. compilers follow.
.shade end
..do end
..el ..if '&target' eq 'PC 370' ..th ..do begin
.* .shade begin
.* Areas that are shaded blue describe the interpretation of the behavior
.* that the &company. C compiler follows.
.* .shade end
.* .shade begin
.* Areas that are shaded green describe the interpretation of the behavior
.* that the &wlooc. compiler follows.
.* .shade end
.* .shade begin
.* Areas that are shaded grey describe the interpretation of the behavior
.* that both compilers follow.
.* .shade end
.shade begin
Areas that are shaded grey describe the interpretation of the behavior
that &wc286., &wc386. and/or &wlooc. compilers follow.
.shade end
..do end
.*
.************************************************************************
.*
.pp
Programmers who are writing a
program that will be ported to other systems should pay particular
attention when using these features, since other compilers may
behave in other ways.
As much as possible, an attempt is made to describe
other likely behaviors.
.pp
This book does not describe any of the library functions
.ix 'library function'
that a C program might use to interact with its environment.
In particular,
.ix 'input/output'
.ix 'output'
input and output is not described in this manual.
The C language does not contain any I/O capabilities.
The &libref describes all of the library functions, including those
used for input and output.
.pp
.ix 'glossary'
A glossary is included in the appendix,
and describes all terms used in the book.
