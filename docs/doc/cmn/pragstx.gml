.if &e'&dohelp eq 0 .do begin
.section Introduction
.do end
.*
.np
.ix 'pragma'
.if '&cmpclass' ne 'load-n-go' .do begin
A pragma is  a compiler directive that provides the following
capabilities.
.begbull
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.bull
Pragmas allow you to specify certain compiler options.
.do end
.bull
Pragmas can be used to direct the &cmpname code generator to emit
specialized sequences of code for calling functions which use argument
passing and value return techniques that differ from the default used
by &cmpname..
.bull
Pragmas can be used to describe attributes of functions (such as side
effects) that are not possible at the &lang language level.
The code generator can use this information to generate more efficient
code.
.bull
Any sequence of in-line machine language instructions,
.if '&target' eq 'QNX' .do begin
including QNX function calls,
.do end
.el .do begin
including DOS and BIOS function calls,
.do end
can be generated in the object code.
.endbull
.np
.do end
Pragmas are specified in the source file using the
.kw pragma
directive.
.ix 'pragmas' 'notation used to describe'
The following notation is used to describe the syntax of pragmas.
.begnote
.note keywords
A
.mono keyword
is shown in a mono-spaced courier font.
.note program-item
A
.bi program-item
is shown in a roman bold-italics font.
A
.bi program-item
is a symbol name or numeric value supplied by the programmer.
.note punctuation
A
.mono punctuation character
shown in a mono-spaced courier font must be entered as is.
.np
A
.bi punctuation character
shown in a roman bold-italics font is used to describe syntax.
The following syntactical notation is used.
.begpoint
.point [abc]
The item
.sy abc
is optional.
.point {abc}
The item
.sy abc
may be repeated zero or more times.
.point a|b|c
One of
.sy a,
.sy b
or
.sy c
may be specified.
.point a ::= b
The item
.sy a
is defined in terms of
.sy b.
.point (a)
Item
.sy a
is evaluated first.
.endpoint
.endnote
