.preface Preface
.pp
This book describes the C programming language as implemented by the
&wcboth. compilers for 80x86-based
.*
.************************************************************************
.*
..if '&target' = 'PC' ..th ..do begin
processors.
&wcboth. are implementations
..do end
..el ..if '&target' = 'PC 370' ..th ..do begin
processors, and by the
&wlooc. compiler for IBM 370
systems.
&wcall. are implementations
..do end
.*
.************************************************************************
of ANSI/ISO 9899:1990 Programming Language C.
The standard was developed by the ANSI X3J11 Technical Committee on
the C Programming Language.
In addition to the full C language standard,
.*
.************************************************************************
.*
..if '&target' = 'PC' ..th ..do begin
the compiler supports numerous extensions for the 80x86 environment.
..do end
..el ..if '&target' = 'PC 370' ..th ..do begin
each compiler supports extensions for their respective environments.
..do end
.*
.************************************************************************
.pp
This book is intended to be a reference manual and hence a precise
description of the C language.
It also attempts to remain readable by ordinary humans.
When new concepts are introduced, examples are given to provide
clarity.
.pp
Since C is a programming language that is supposed to aid programmers
trying to write portable programs, this book points out those areas
of the language that may vary from one system to another.
Where possible, the probable behavior of other C compilers is
mentioned.
..if &targetQNX = 0 ..th ..do begin
:cmt. .pp
:cmt. Additional copies of this manual may be ordered from:
:cmt. .illust begin
:cmt. &company. International Corp.
:cmt. 415 Phillip St.
:cmt. Waterloo, Ontario
:cmt. CANADA, N2L 3X2
:cmt. .illust end
..do end
..el ..do begin
.pp
Additional copies of this manual may be ordered from:
.illust begin
QNX Software Systems Ltd.
175 Terence Matthews Crescent
Kanata, Ontario
CANADA  K2M 1W8
Phone: 613-591-0931
Fax:   613-591-3579
.illust end
..do end
