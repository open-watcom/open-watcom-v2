.preface Preface
.*
.np
The &product Optimizing Compiler (&cmpname) is an implementation of
the American National Standard programming language FORTRAN, ANSI
X3.9-1978, commonly referred to as FORTRAN 77.
The language level supported by this compiler includes the full
language definition as well as significant extensions to the language.
&cmpname evolved out of the demands of our users for a companion
optimizing compiler to &company's WATFOR-77 "load-and-go" compiler.
.np
The "load-and-go" approach to processing FORTRAN programs emphasizes
fast compilation rates and quick placement into execution of FORTRAN
applications.
This type of compiler is used heavily during the debugging phase of
the application.
At this stage of application development, the "load-and-go" compiler
optimizes the programmer's time ... not the program's time.
However, once parts of the application have been thoroughly debugged,
it may be advantageous to turn to a compiler which will optimize the
execution time of the executable code.
.np
&cmpname is a highly optimizing compiler based on the code generation
technology that was developed for &company's highly-praised C and C++
optimizing compilers.
&cmpname is a traditional compiler in the sense that it creates object
files which must be linked into an executable program.
.np
The
.us &product User's Guide
describes how to use &product with DOS, OS/2, Windows 3.x, Windows NT,
and Windows 95.
.*
.sect Acknowledgements
.*
.im gmlack
.np
We would like to thank IMSL of Houston, Texas for providing us
with copies of their Mathematics and Statistics libraries.
The IMSL
.us Math Library
is a collection of subprograms for mathematical problem solving and
the
.us Statistics Library
is a collection of subprograms for statistical analysis.
The self test procedures provided with these libraries proved to be an
immense help in testing &cmpname on the personal computer.
.np
We also used the "FORTRAN Compiler Validation System, Version 2.0" to
test the conformance of &cmpname with the full FORTRAN 77 language
standard.
This package is provided by the National Technical Information Service
of the U.S. Department of Commerce in Springfield, Virginia.
The validation system was developed by the Federal Software Testing
Center.
.np
If you find problems in the documentation or have some good
suggestions, we would like to hear from you.
.np
&pubdate..
.*
.sect Trademarks Used in this Manual
.*
.np
AutoCAD Development System is a trademark of Autodesk, Inc.
.*
.np
DOS/4G is a trademark of Tenberry Software, Inc.
.*
.np
IBM Developer's WorkFrame/2, Presentation Manager, and OS/2 are
trademarks of International Business Machines Corp.
IBM is a registered trademark of International Business Machines Corp.
.*
.np
Intel and Pentium are registered trademarks of Intel Corp.
.*
.np
Microsoft, Windows and Windows 95 are registered trademarks of
Microsoft Corp.
Windows NT is a trademark of Microsoft Corp.
.*
.np
NetWare, NetWare 386, and Novell are registered trademarks of Novell,
Inc.
.*
.np
Phar Lap, 286|DOS-Extender, and 386|DOS-Extender are trademarks of
Phar Lap Software, Inc.
.*
.np
QNX is a trademark of QNX Software Systems Ltd.
.*
.np
.im wtrdmrk
