.np
This chapter contains, in alphabetical order, descriptions of the
&routines which comprise the graphics library.
Each description consists of a number of subsections:
.begnote
.setptnt 0 2
.note Synopsis:
.sk 0
.if '&lang' eq 'C' .do begin
This subsection gives the header files that should be included within
a source file that references the &routine..
It also shows an appropriate declaration, or prototype, for the &routine..
This declaration is not included in your program; only the header
file should be included.
.np
The &routine descriptions make use of the symbols
.id _FAR
and
.id _HUGE.
For the 16-bit &company C compiler, these are replaced by the keywords
.id __far
and
.id __huge.
For the 32-bit &company C compiler, they are replaced by null strings.
.do end
.el .do begin
This subsection gives an example of a declaration for the &routine,
showing the types of the &routine and its arguments.
.do end
.note Description:
.sk 0
This subsection is a description of the &routine..
.note Returns:
.sk 0
This
.if '&lang' eq 'FORTRAN' .do begin
optional
.do end
subsection describes the return value (if any)
for the &routine..
.note See Also:
.sk 0
This subsection provides a list of related &routines..
.note Example:
.sk 0
This subsection consists of an example program demonstrating
the use of the &routine..
In some cases the output from the program is also displayed.
.note Classification:
.sk 0
This subsection provides an indication of where the &routine is
commonly found.
The following notation is used:
.begpoint $break $setptnt 12
.point PC Graphics
.ix 'PC Graphics classification'
These &routiness are part of the PC graphics library.
.endpoint
.note Systems:
This subsection provides an indication of where the &routine is
supported.
The following notation is used:
.begpoint $break $setptnt 12
.point DOS
This &routine is available on both 16-bit DOS and 32-bit extended DOS.
.point QNX
This &routine is available on QNX Software Systems' 16 or 32-bit
operating systems.
.endpoint
.endnote
