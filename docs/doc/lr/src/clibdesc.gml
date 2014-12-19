.np
Each of the functions or macros in the C Library is described in this
chapter.
Each description consists of a number of subsections:
.in 0
.begnote $setptnt &INDlvl
.*
.note Synopsis:
.*
This subsection gives the header files that should be included within
a source file that references the function or macro.
It also shows an appropriate declaration for the function or for a
function that could be substituted for a macro.
This declaration is not included in your program; only the header
file(s) should be included.
.pp
When a pointer argument is passed to a function and that function does
not modify the item indicated by that pointer, the argument is shown
with
.kw const
before the argument.
For example,
.millust begin
const char *string
.millust end
.pc
indicates that the array pointed at by
.arg string
is not changed.
.*
.note Constraints:
.*
This subsection describes Runtime-constraints for Safer C Library functions.
.*
.note Safer C:
.*
This subsection points to the Safer C version of the described "unsafe" function.
.*
.note Description:
.*
This subsection is a description of the function or macro.
.*
.note Returns:
.*
This subsection describes the return value (if any)
for the function or macro.
.*
.note Errors:
.*
This subsection describes the possible
.kw errno
values.
.*
.note See Also:
.*
This optional subsection provides a list of related functions or
macros.
.*
.note Example:
.*
This optional subsection consists of one or more examples of
the use of the function.
The examples are often just fragments of code (not complete programs)
for illustration purposes.
:cmt. .pp
:cmt. The examples often use the
:cmt. .kw printf
:cmt. function.
:cmt. Normally, the header file
:cmt. .hdrfile stdio.h
:cmt. would be included when that function is used.
:cmt. This file is not shown in the examples, unless it is otherwise
:cmt. required.
.*
.note Classification:
.*
This subsection provides an indication of where the function or macro
is commonly found.
The following notation is used:
.begpoint $break $setptnt 12
.point ISO C
.ix 'ISO C classification'
These functions or macros are defined by the ISO/ANSI C standard
ISO/IEC 9899:1989.
.point ISO C90
.ix 'ISO C90 classification'
These functions or macros are defined by the ISO/ANSI C standard
ISO/IEC 9899:1990.
.point ISO C95
.ix 'ISO C95 classification'
These functions or macros are defined by the ISO/ANSI C standard
ISO/IEC 9899:1990/AMD 1:1995.
.point ISO C99
.ix 'ISO C99 classification'
These functions or macros are defined by the ISO/ANSI C standard
ISO/IEC 9899:1999.
.point POSIX 1003.1
.ix 'POSIX classification'
.ix 'POSIX 1003.1 classification'
The functions or macros are not defined by the ISO C standard.
These functions are specified in the document
.us IEEE Standard Portable Operating System Interface
.us for Computer Environments
(IEEE Draft Standard 1003.1-1990).
.point POSIX 1003.2
.ix 'POSIX 1003.2 classification'
These functions or macros are not defined by the ISO C standard.
These functions are specified in the document
.us Shell and Utility Application Interface
.us for Computer Operating System Environments
(IEEE Computer Society Working Group 1003.2).
.point POSIX 1003.4
.ix 'POSIX 1003.4 classification'
These functions or macros are not defined by the ISO C standard.
These functions are specified in the document
.us Realtime Extensions
.us for Computer Operating System Environments
(IEEE Computer Society Working Group 1003.4).
.point Intel
.ix 'Intel classification'
These functions or macros are neither ISO C nor POSIX.
It performs a function related to the Intel x86 architecture.
It may be found in other implementations of C for personal computers
using Intel chips.
Use these functions with caution, if portability is a consideration.
.if '&machsys' eq 'QNX' .do begin
.point QNX
.ix 'QNX classification'
These functions or macros are neither ISO C nor POSIX.
They perform a function related to QNX.
They may be found in other implementations of C for personal computers
with QNX.
Use these functions with caution, if portability is a consideration.
.point UNIX
.ix 'UNIX classification'
These functions exist on some UNIX systems but are outside of the
POSIX or ISO C standards.
.do end
.el .do begin
.point BIOS
.ix 'BIOS classification'
These functions access a service of the BIOS found in IBM Personal
Computers and compatibles.
These functions should not be used if portability is a consideration.
.point DOS
.ix 'DOS classification'
These functions or macros are neither ISO C nor POSIX.
They perform a function related to DOS.
They may be found in other implementations of C for personal computers
with DOS.
Use these functions with caution, if portability is a consideration.
.point OS/2
.ix 'OS/2 classification'
These functions are specific to OS/2.
.point PC Graphics
.ix 'PC Graphics classification'
These functions are part of the PC graphics library.
.point Windows
.ix 'Windows classification'
These functions are specific to Microsoft Windows.
.do end
.point WATCOM
.ix 'WATCOM classification'
These functions or macros are neither ISO C nor POSIX.
They may be found in other implementations of the C language, but
caution should be used if portability is a consideration.
.*
.point TR 24731
.ix 'TR 24731 classification'
These functions are "safer" versions of normal C library functions.
They perform more checks on parameters and should be used in preference over their "unsafe" version.
.endpoint
.*
.if '&machsys' ne 'WIN32' .do begin
.note Systems:
.*
This subsection provides an indication of where the function or macro
is supported.
The following notation is used:
.begpoint $break $setptnt 12
.point All
This function is available on all systems
(we do not include RDOS, Linux, Netware or DOS/PM in this category).
.point DOS
This function is available on both 16-bit DOS and 32-bit extended DOS.
.point DOS/16
This function is available on 16-bit, real-mode DOS.
.point DOS/32
This function is available on 32-bit, protected-mode extended DOS.
.point DOS/PM
This 16-bit DOS protected-mode function is supported under Phar Lap's
286|DOS-Extender "RUN286".
The function is found in one of &company's 16-bit protected-mode DOS
libraries (DOSPM*.LIB under the 16-bit OS2 subdirectory).
.point Linux
This function is available on the Linux operating system for Intel 80386
and upwards compatible systems.
.point MACRO
This function is implemented as a macro (#define) on all systems.
.point Math
This function is a math function.
Math functions are available on all systems.
.point Netware
This function is available on the 32-bit Novell Netware operating
system.
.point OS/2 1.x
This function is available on IBM OS/2 1.x, a 16-bit protected-mode
system for Intel 80286 and upwards compatible systems.
.np
When "(MT)" appears after OS/2, it refers to the
.filename CLIBMTL
library which supports multi-threaded applications.
.np
When "(DL)" appears after OS/2, it refers to the
.filename CLIBDLL
library which supports creation of Dynamic Link Libraries.
.np
When "(all)" appears after "OS/2 1", it means all versions of the OS/2
1.x libraries.
.np
If a function is missing from the OS/2 library, it may be found in
&company's 16-bit protected-mode DOS libraries (DOSPM*.LIB) for
Phar Lap's 286|DOS-Extender (RUN286).
.point OS/2-32
This function is available on 32-bit IBM OS/2, a protected-mode
system for Intel 80386 and upwards compatible systems.
.point RDOS
This function is available on RDOS operating system.
.point QNX
This function is available on QNX Software Systems' 16 or 32-bit
operating systems.
.point QNX/16
This function is available on QNX Software Systems' 16-bit operating
system.
.point QNX/32
This function is available on QNX Software Systems' 32-bit operating
system.
.point Windows
This function is available on 16-bit, protected-mode Windows 3.x.
.point Win386
This function is available on Microsoft Windows 3.x, using &company's
Windows Extender for 32-bit protected-mode applications running on
Intel 386 or upward compatible systems.
.point Win32
This function is available on 32-bit Microsoft Windows platforms
(Windows 95, Windows 98, Windows NT, Windows 2000, etc.).
It may also be available for Windows 3.x using Win32s support.
.endpoint
.do end
.endnote
.in &INDlvl
