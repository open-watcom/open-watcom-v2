.ix 'translation limits'
.pp
All standard-conforming C compilers must be able to translate and
execute a program that contains one instance of every one of the
following limits. Each limit is the minimum limit (the smallest
maximum) that the compiler may impose.
.*
.************************************************************************
.*
.shade begin
.* Except where noted,
..if '&target' eq 'PC' ..th ..do begin
The &wcboth. compilers do
..do end
..el ..if '&target' eq 'PC 370' ..th ..do begin
The &wcall. compilers do
..do end
not impose any arbitrary restrictions in any of these areas.
Restrictions arise solely because of memory limitations.
.shade end
.*
.************************************************************************
.*
.begbull
.bull
15 nesting levels of compound statements,
iteration control structures (
..ct .kw for
..ct ,
.kw do
..ct /
..ct .kw while
..ct ,
.kw while
..ct ),
and selection control structures (
..ct .kw if
..ct ,
.kw switch
..ct ),
.bull
8 nesting levels of conditional inclusion (
..ct .kwpp #if
..ct ),
.bull
12 pointer, array and function declarators (in any order)
modifying an arithmetic,
structure, union or incomplete type in a declaration,
.bull
31 nesting levels of parenthesized declarators within a full declarator,
.bull
32 nesting levels of parenthesized
expressions within a full expression,
.bull
31 significant initial characters in an internal identifier
or a macro name,
.keep begin
.bull
6 significant initial characters in an external identifier,
.*
.************************************************************************
.*
..if '&target' eq 'PC 370' ..th ..do begin
.shade begin
The &wlooc. compiler limits external identifiers to 8 significant
initial characters due to limitations in the linking/loading process.
.shade end
..do end
.*
.************************************************************************
.*
.keep end
.bull
511 external identifiers in one translation unit (module),
.bull
127 identifiers with block scope declared in one block,
.bull
1024 macro identifiers simultaneously defined in one translation unit (module),
.bull
31 parameters in one function definition,
.bull
31 arguments in one function call,
.bull
31 parameters in one macro definition,
.bull
31 parameters in one macro invocation,
.bull
509 characters in a logical
.ix 'continuation lines'
.ix 'line' 'continuation'
.ix 'line' 'logical'
(continued) source line,
.bull
509 characters in a character
.ix 'string literal'
string literal
or
.ix 'wide string literal'
.ix 'string literal' 'wide'
wide string literal
(after concatenation),
.bull
32767 bytes in an object,
.bull
8 nesting levels for
.kwpp #include
..ct d
files,
.bull
257
.ix 'case label'
.kw case
labels for a
.ix 'switch statement'
.ix 'statement' 'switch'
.kw switch
statement
(excluding those for any nested
.kw switch
statements),
.bull
127 members in a single structure or union,
.bull
127 enumeration constants in a single enumeration,
.bull
15 levels of nested structure or union definitions in a single
struct-declaration-list
(structure or union definition).
.endbull
