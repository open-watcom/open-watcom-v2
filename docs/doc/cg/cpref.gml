.preface Preface
.*
.np
&cmpcname is an implementation of
ISO/ANSI 9899:1990 Programming Language C.
The standard was developed by the ANSI X3J11 Technical Committee
on the C Programming Language.
In addition to the full C language standard, the compiler supports
numerous extensions for the Intel 80x86-based personal computer
environment.
The compiler is also partially compliant with the ISO/IEC 9899:1999
Programming Language C standard.
.if '&lang' eq 'C/C++' .do begin
.np
&cmppname is an implementation of the Draft Proposed International
Standard for Information Systems Programming Language C++ (ANSI X3J16,
ISO WG21).
In addition to the full C++ language standard, the compiler supports
numerous extensions for the Intel 80x86-based personal computer
environment.
.do end
.np
&company is well known for its language processors having developed,
over the last decade, compilers and interpreters for the APL, BASIC,
COBOL, FORTRAN and Pascal programming languages.
From the start, &company has been committed to developing portable
software products.
These products have been implemented on a variety of processor
architectures including the IBM 370, the Intel 8086 family, the
Motorola 6809 and 68000, the MOS 6502, and the Digital PDP11 and
VAX.
In most cases, the tools necessary for porting to these environments
had to be created first.
Invariably, a code generator had to be written.
Assemblers, linkers and debuggers had to be created when none were
available or when existing ones were inadequate.
.np
Over the years, much research has gone into developing the "ultimate"
code generator for the Intel 8086 family.
We have continually looked for new ways to improve the quality of the
emitted code, never being quite satisfied with the results.
Several major revisions, including some entirely new approaches to
code generation, have ensued over the years.
Our latest version employs state of the art techniques to produce very
high quality code for the 8086 family.
We introduced the C compiler in 1987, satisfied that we had a C
software development system that would be of major benefit to those
developing applications in C for the IBM PC and compatibles.
.if '&alpha' eq 'AXP' .do begin
.np
In 1996, &company introduced &product for the DEC Alpha AXP platform.
This version shares the same code generation technology that made the
Intel version the best optimizing compiler for 8086 platforms in the
world.
.do end
.np
The
.if '&target' eq 'QNX' .do begin
.us &product Compiler and Tools User's Guide for QNX
describes how to use &product on Intel 80x86-based personal computers
to build QNX applications.
.do end
.el .do begin
.us &product User's Guide
describes how to use &product on Intel 80x86-based personal computers
with DOS, Windows, Windows NT, or OS/2.
.if '&alpha' eq 'AXP' .do begin
It also describes how to use &product on DEC Alpha AXP computers
running Windows NT.
.do end
.do end
.*
.if '&target' eq 'QNX' .do begin
The User's Guide is covers the following topics:
.begbull
.bull
The &product compiler including compiler options,
benchmarking,
include file processing,
the preprocessor,
predefined macros and keywords,
based pointers,
precompiled headers,
and libraries
.bull
16-bit memory models, calling conventions, and pragmas
.bull
32-bit memory models, calling conventions, and pragmas
.bull
In-line assembly language
.bull
The &lnkname
.bull
The &libname
.bull
The &asmname
.bull
The &disname
.bull
The &stripname
.bull
Environment Variables
.bull
C Diagnostic Messages
.bull
C++ Diagnostic Messages
.bull
&product Run-Time Messages
.endbull
.np
Additional copies of this documentation may be ordered from:
.illust begin
        QNX Software Systems Ltd.
        175 Terence Matthews Crescent
        Kanata, Ontario
        CANADA  K2M 1W8
        Phone: 613-591-0931
        Fax:   613-591-3579
.illust end
.do end
.*
.sect Acknowledgements
.*
.im gmlack
.*
.np
The Plum Hall Validation Suite for &lang has been invaluable in
verifying the conformance of the &product compilers to the
.if '&lang' eq 'C' .do begin
ISO C Language Standard.
.do end
.if '&lang' eq 'C/C++' .do begin
ISO C Language Standard and the Draft Proposed C++ Language Standard.
.do end
.np
Many users have provided valuable feedback on earlier versions of
the &product compilers and related tools.
Their comments were greatly appreciated.
If you find problems in the documentation or have some good
suggestions, we would like to hear from you.
.np
&pubdate..
.*
.sect Trademarks Used in this Manual
.*
.if '&target' ne 'QNX' .do begin
.if '&alpha' eq 'AXP' .do begin
.np
Alpha AXP, AXP, DEC and Digital
are trademarks of Digital Equipment Corporation.
.do end
.*
.np
.tmarkd4g
.*
.np
High C is a trademark of MetaWare, Inc.
.*
.np
.if '&target' eq 'QNX' .do begin
OS/2 is a trademark of International Business Machines Corp.
.do end
.el .do begin
IBM Developer's Toolkit, Presentation Manager, and OS/2 are
trademarks of International Business Machines Corp.
.do end
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
Phar Lap, 286|DOS-Extender and 386|DOS-Extender are trademarks of
Phar Lap Software, Inc.
.*
.np
.tmarkqnx
.*
.np
.tmarkunix
.*
.np
.im wtrdmrk
