.ix 'identifier'
.pp
Identifiers are used as:
.begbull $compact
.bull
.ix 'object'
.ix 'variable name'
.ix 'name' 'object'
.ix 'name' 'variable'
object or variable names,
.bull
.ix 'function' 'name'
.ix 'name' 'function'
function names,
.bull
.ix 'label' 'name'
.ix 'name' 'label'
labels,
.bull
.ix 'structure' 'name'
.ix 'name' 'structure'
.ix 'union' 'name'
.ix 'name' 'union'
.ix 'enumeration name'
.ix 'name' 'enumeration'
structure, union or enumeration tags,
.bull
.ix 'name' 'structure member'
.ix 'name' 'union member'
.ix 'structure' 'member' 'name'
.ix 'union' 'member' 'name'
the name of a member of a structure or union,
.bull
enumeration constants,
.ix 'enumeration constant'
.bull
.ix 'macro name'
.ix 'name' 'macro'
macro names,
.bull
.ix 'typedef'
.ix 'type definition'
typedef names.
.endbull
.pp
An identifier is formed by a sequence of the following characters:
.begbull $compact
.bull
upper-case letters "A" through "Z",
.bull
lower-case letters "a" through "z",
.bull
the digits "0" through "9",
.bull
the underscore "_".
.endbull
.pp
The first character may not be a digit.
.pp
An identifier cannot be a
member of the list of keywords.
.pp
Identifiers
can consist of any number of characters, but
the compiler is not required to consider more than 31 characters
as being significant, provided the identifier does not have
.ix 'external linkage'
.ix 'linkage' 'external'
.us external linkage
(shared by more than one compiled
module of the program).
If the identifier is
external,
the compiler is not required to consider more than 6 characters as
being significant.
External identifiers may be
.ix 'case sensitive'
case-sensitive.
.pp
Of course, any particular compiler may choose to consider more
characters as being significant, but a portable C program
will strictly adhere to the above rules.
(This restriction is likely to be relaxed in future versions of the
ISO C standard and corresponding C compilers.)
.*
.************************************************************************
.*
..if '&target' eq 'PC' or '&target' eq 'PC 370' ..th ..do begin
.shade begin
The &wcboth. compilers do not restrict the number of significant
characters for functions or objects with external or internal linkage.
.pp
The linker provided with &wcboth.
restricts the number of significant characters in external
.ix 'identifier' 'external' 'significant characters'
.ix 'linker' 'external identifer' 'significant characters'
identifiers to 40 characters, and
by default, distinguishes between identifiers that differ only in the
.ix 'case sensitive'
.ix 'linker' 'case sensitive'
case of the letters. An option may be used to
force the linker to ignore case differences.
.shade end
..do end
..if '&target' eq 'PC 370' ..th ..do begin
.shade begin
The &wlooc. compiler does not restrict the number of significant
characters for functions or objects with internal linkage.
.pp
Objects with external linkage are restricted to 8 characters due
to limitations of the linking/loading process provided by the
operating system.
Any underscores (_) are mapped to dollar signs ($).
The case of letters is ignored.
.shade end
..do end
.pp
Any external identifier that starts with the
underscore
.ix 'underscore' 'leading'
.ix 'leading underscore'
character ("_") may be
reserved
.ix 'reserved identifier'
.ix 'identifier' 'reserved'
by the compiler. Any other identifier that starts with two
underscores, or an underscore and an upper-case letter may be
reserved.
Generally, a program should avoid creating
identifiers that start with an underscore.
