.preface Preface
.np
.if '&lang' eq 'C' .do begin
&company C is an implementation of
ISO 9899:1990 Programming Language C.
The standard was developed by the ANSI X3J11 Technical Committee
on the C Programming Language.
In addition to the full C language standard, the compiler supports
numerous extensions for the IBM PC environment.
.do end
.el .do begin
The &company &lang Optimizing Compiler is an implementation of
the American National Standard programming language FORTRAN, ANSI
X3.9-1978, commonly referred to as FORTRAN 77.
The language level supported by this compiler includes the full
language definition as well as significant extensions to the language.
.do end
.np
This manual describes the &company &lang Graphics Library.
This library of &routines is used to create graphical images such as
lines and circles on the screen.
&rroutines are also provided for displaying text.
.*
.im gmlack
.*
.sect Acknowledgements
.*
Many members of Watcom International Corp. have made a significant
contribution to the design and implementation of the &company &lang
Graphics Library. The design of this software is based upon ideas
evolved and proven over the past decade in other software projects
in which these people have been involved.
:CMT. The major project members for the implementation
:CMT. of the &company &lang Graphics Library were Bill Hillis, Claude Lepage,
:CMT. Tom Vandeloo and Robin Walsh.
:CMT. .np
:CMT. H.W.Hillis
:CMT. .br
:CMT. J.W.Welch
.np
&pubdate..
.*
.sect Trademarks Used in this Manual
.np
IBM is a registered trademark of International Business Machines Corp.
.np
Hercules is a registered trademark of Hercules Computer Technology.
.np
Microsoft and Windows are registered trademarks of Microsoft Corp.
.if '&machsys' eq 'QNX' .do begin
.np
QNX is a registered trademark of QNX Software Systems Ltd.
.do end
.np
.im wtrdmrk
