.chap Introduction to &product
.*
.np
Welcome to the &product &ver..&rev development system. &product
is an Open Source successor to commercial compilers previously
marketed by Sybase, Powersoft and originally WATCOM International
Corp.
.np
Version &ver..&rev of &product is a professional, optimizing,
multi-platform
.if '&lang' eq 'C/C++' .do begin
C and C++
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
FORTRAN 77
.do end
compiler with a comprehensive suite of development tools for
developing and debugging both 16-bit and 32-bit applications for
DOS,
extended DOS,
Novell NLMs,
16-bit OS/2,
32-bit OS/2,
Windows 3.x,
Windows 95/98/Me,
Win32s,
and
Windows NT/2000/XP (Win32).
.np
You should read the entire contents of this booklet, as it contains
information on new programs and modifications that have been made
since the previous release.
.np
.abox begin
.bd Special NOTE to users of previous versions!
.bd See the section entitled
:HDREF refid='diffs'
.bd to determine if you need to recompile your application.
.abox end
.*
.section What is in version &ver..&rev of &product?
.*
.np
Version &ver..&rev incorporates the features professional developers have
been demanding:
.begpoint $break $setptnt 5
.ix 'product overview'
.ix 'overview'
.point Open, Multi-target Integrated Development Environment
The IDE allows you to easily edit, compile, link, debug and build
applications for 16-bit systems like DOS, OS/2 1.x, and Windows 3.x
and 32-bit systems like extended DOS, Novell NLMs, OS/2,
Windows 3.x (Win32s), Windows 95/98/Me, and Windows NT/2000/XP.
Projects can be made up of multiple targets which permit a project to
include EXEs and DLLs.
The IDE produces makefiles for the project which can be viewed and
edited with a text editor.
The IDE is hosted under Windows 3.x, Windows 95/98/Me, Windows NT/2000/XP,
and 32-bit OS/2.
.cp 10
.point The Widest Range of Intel x86 Platforms
.begnote
.note Host Platforms
.ix 'platforms supported'
.ix 'host platforms supported'
.begbull $compact
.bull
DOS (command line)
.bull
32-bit OS/2 (IDE and command line)
.bull
Windows 3.x (IDE)
.bull
Windows 95/98/Me (IDE and command line)
.bull
Windows NT/2000/XP (IDE and command line)
.endbull
.note 16-bit Target Platforms
.ix 'target platforms supported'
.begbull $compact
.bull
DOS
.bull
Windows 3.x
.bull
OS/2 1.x
.endbull
.note 32-bit Target Platforms
.begbull $compact
.bull
Extended DOS
.bull
Win32s
.bull
Windows 95/98/Me
.bull
Windows NT/2000/XP
.bull
32-bit OS/2
.bull
Novell NLMs
.endbull
.endnote
.point Cross-Platform Development Tools
.ix 'cross-platform'
The core tools in the package permit cross-platform development that
allows developers to exploit the advanced features of today's popular
32-bit operating systems, including Windows 95/98/Me, Windows NT/2000/XP,
and OS/2. Cross-platform support allows you to develop on a host development
environment for execution on a different target system.
.point Multi-Platform Debugger
The new debugger advances developer productivity. New features include
redesigned interface, ability to set breakpoints on nested function
calls, improved C++ and DLL debugging, reverse execution, and
configurable interface. Graphical versions of the debugger are
available under Windows 3.x, Windows 95/98/Me, Windows NT/2000/XP, and
32-bit OS/2. Character versions of the debugger are available under DOS,
Windows 3.x, Windows NT/2000/XP, and 32-bit OS/2.
For VIDEO fans, we have kept the command line compatibility from the
original debugger.
.if '&lang' eq 'FORTRAN 77' .do begin
:cmt. .point Class Browser
:cmt. The Browser lets you visually navigate the subroutines,
:cmt. functions, and variables of your FORTRAN 77 application.
.do end
.if '&lang' eq 'C/C++' .do begin
.point Class Browser
.ix 'class browser'
The Browser lets you visually navigate the object hierarchies,
functions, variable types, and constants of your C/C++ application.
.do end
.point Performance Analysis
.ix 'performance analysis'
The &smpname and &prfname are performance analysis tools that
locate heavily used sections of code so that you may focus your
efforts on these areas and improve your application's performance.
.point Editor
.ix 'editor'
The &edname is a context sensitive source editor, integrated into the
Windows 3.x, Windows 95/98/Me and Windows NT/2000/XP version of the IDE.
.point Graphical Development Tools
.ix 'GUI tools'
&product includes a suite of graphical development tools to aid
development of Windows 3.x, Windows 95/98/Me and Windows NT/2000/XP
applications. The development tools include:
.begnote $setptnt 12
.note Resource Editors
.ix 'resource editors'
Enable you to create resources for your 16-bit and 32-bit Windows
applications. For 32-bit OS/2 PM development, &product interoperates
with IBM's OS/2 Developer's Toolkit (available from IBM).
These tools have been seamlessly integrated into the
IDE. The resource compiler allows you to incorporate these resources
into your application.
.note Resource Compiler
.ix 'resource compiler'
Produces a compiled resource file from a source file.
.note Zoom
.ix 'zoom'
Magnifies selected sections of your screen.
.note Heap Walker
.ix 'heap walker'
Displays memory usage for testing and debugging purposes.
.note Spy
.ix 'spy'
Monitors messages passed between your application and Windows.
.note DDESpy
.ix 'DDE spy'
Monitors all DDE activity occurring in the system.
.note &drwc
.ix '&drwc'
Enables you to debug your program by examining both the program and
the system after an exception occurs; monitors native applications
running under Windows 3.x, Windows 95/98/Me or Windows NT/2000/XP.
.endnote
.point Assembler
An assembler is included in the package.
It is compatible with a subset of the Microsoft macro assembler (MASM).
.if '&lang' eq 'C/C++' .do begin
.point C++ Class Libraries
.ix 'class libraries'
&product includes container and stream class libraries.
.do end
:cmt.The container class libraries have been improved significantly and feature....
.point Royalty-free 32-bit DOS Extender
.ix 'DOS extender'
&product includes the DOS/4GW 32-bit DOS extender by Tenberry Software
with royalty-free run-time and virtual memory support up to 32MB.
.point Support for wide range of DOS Extenders
&product allows you to develop and debug applications based on the
following DOS extender technology: CauseWay DOS Extender, Tenberry Software's
DOS/4G and Phar Lap's TNT DOS Extender.
You can also develop applications using DOS/32A and FlashTek's DOS Extender
but, currently, there is no support for debugging these applications.
.point Sample programs and applications
&product includes a large set of sample applications to demonstrate
the integrated development environment.
.endpoint
.*
.section Technical Support and Services
.*
.np
.ix 'technical support'
We are committed to ensuring that our products perform as they were
designed.
Although a significant amount of testing has gone into this product,
you may encounter errors in the software or documentation.
Technical support is provided on an informal basis through the
&product newsgroups. Please visit http://www.openwatcom.org/ for more
information.
.*
.beglevel
.*
.section Resources at Your Fingertips
.*
.np
.ix 'self-help'
&product contains many resources to help you find answers to your
questions. The documentation is the first place to start. With each
release of the product, we update the manuals to answer the most
frequently asked questions. Most of this information is also
accessible through on-line help.
.np
.ix 'read-me file'
The "README" file in the main product directory contains up-to-date
information that recently became available.
.np
Answers to frequently asked questions are available on the Open Watcom
World Wide Web server (http://www.openwatcom.org/).
.*
.section Contacting Technical Support
.*
.np
.ix 'technical support'
Our technical support is available to help resolve technical defects
in the software. Note that all support is currently informal and free.
The following are ways to contact technical support.
.begnote
.*
.note Newsgroups
The easiest way to get support is through the Open Watcom newsgroups
at news://news.openwatcom.org/.
.*
.note World Wide Web
You can also submit bug reports or enhancement requests through the
Open Watcom bug tracking system at http://bugzilla.openwatcom.org/.
.*
.endnote
.np
.*
.section Information Technical Support Will Need to Help You
.*
.np
The more information you can provide to technical support,
the faster they can help you solve your problem. A detailed description
of the problem, short sample program, and a summary of steps to duplicate
the problem (including compiler and linker options) are essential. Concise
problem reports allow technical support to quickly pinpoint the problem
and offer a resolution. Here is a list of information that will help
technical support solve the problem:
.begpoint $break
.point Contact information
We would like your name, as well as telephone and fax numbers where
you can be reached during the day.
.point Product information
Please tell us the product name and exact version number.
.point Hardware configuration
Please tell us what type of processor you are using (e.g., 2.2GHz
Intel Pentium 4), how much memory is present, what kind of graphics
adapter you are using, and how much memory it has.
.point Software configuration
Please tell us what operating system and version you are using.
.point Concise problem report with short sample program
Please provide a complete description of the problem and the steps to
reproduce it. A small, self-contained program example with compile and
link options is ideal.
.endpoint
.*
.section Suggested Reading
.*
.np
There are a number of good books and references that can help you
answer your questions. Following is a list of some of the books and
documents we feel might be helpful. This is by no means an exhaustive
list. Contact your local bookstore for additional information.
.*
.beglevel
.* ----------------------------------------------------------------
.if '&lang' eq 'C/C++' .do begin
.*
.section C Programmers
.*
.begnote
.note The C Programming Language, 2nd Edition
.br
Brian W. Kernighan and Dennis M.Ritchie; Prentice Hall, 1988.
.*
.note C DiskTutor
.br
L. John Ribar; Osborne McGraw-Hill, 1992.
.endnote
.do end
.* ----------------------------------------------------------------
.if '&lang' eq 'C/C++' .do begin
.*
.section C++ Programmers
.*
.begnote
.*
.note C++ Primer, 2nd Edition
.br
Stanley B. Lippman; Addison-Wesley Publishing Company, 1991.
.*
.note Teach Yourself C++ in 21 Days
.br
Jesse Liberty; Sams Publishing, 1994.
.endnote
.do end
.* ----------------------------------------------------------------
.*
.section DOS Developers
.*
.begnote
.*
.note PC Interrupts, Second Edition
.br
Ralf Brown and Jim Kyle; Addison-Wesley Publishing Company, 1994.
.*
.note Relocatable Object Module Format Specification, V1.1
.br
.ix 'OMF specification'
The Tool Interface Standards (TIS) OMF specification can be obtained from
the Open Watcom website.
Here is the URL.
.code begin
http://www.openwatcom.org/ftp/devel/docs/omf.pdf
.code end
.np
This file contains a PDF version of the TIS OMF V1.1 specification.
.endnote
.* ----------------------------------------------------------------
.*
.section Extended DOS Developers
.*
.begnote
.*
.note Extending DOS&mdash.A Programmer's Guide to Protected-Mode DOS, 2nd Edition
.br
Ray Duncan, et al; Addison-Wesley Publishing Company, 1992.
.*
.note DOS Protected-Mode Interface (DPMI) Specification
.br
.ix 'DPMI specification'
The DPMI 1.0 specification can be obtained from the Open Watcom website.
Here is the URL.
.code begin
http://www.openwatcom.org/ftp/devel/docs/dpmi10.pdf
.code end
.np
This file contains a PDF version of the DPMI 1.0 specification.
.endnote
.* ----------------------------------------------------------------
.*
.section Windows 3.x Developers
.*
.begnote
.*
.note Microsoft Windows Programmer's Reference
.br
Microsoft Corporation; Microsoft Press, 1990.
.*
.note Programming Windows 3.1, Third Edition
.br
Charles Petzold; Microsoft Press, 1992.
.*
.note Windows Programming Primer Plus
.br
Jim Conger; Waite Group Press, 1992.
.endnote
.* ----------------------------------------------------------------
.*
.section Windows NT Developers
.*
.begnote
.*
.note Advanced Windows NT
.br
Jeffrey Richter; Microsoft Press. 1994.
.*
.note Inside Windows NT
.br
Helen Custer; Microsoft Press. 1993.
.*
.note Microsoft Win32 Programmer's Reference, Volume One
.br
Microsoft Corporation; Microsoft Press, 1993.
.endnote
.* ----------------------------------------------------------------
.*
.section OS/2 Developers
.*
.begnote
.*
.note The Design of OS/2
.br
H.M. Deitel and M.S. Kogan; Addison-Wesley Publishing Company, 1992.
.*
.note OS/2 Warp Unleashed, Deluxe Edition
.br
David Moskowitz and David Kerr, et al; Sams Publishing, 1995.
.*
.endnote
.* ----------------------------------------------------------------
.if '&lang' eq 'C/C++' .do begin
.*
.section Virtual Device Driver Developers
.*
.begnote
.*
.note Writing Windows Virtual Device Drivers
.br
David Thielen and Bryan Woodruff; Addison-Wesley Publishing Company,
1994.
.endnote
.do end
.*
.endlevel
:cmt..*
:cmt..section Documentation Conventions
:cmt..*
:cmt..np
:cmt.Monospaced fonts are used to denote how type appears on the computer screen.
:cmt..np
:cmt.Bold
:cmt..np
:cmt.ALL CAPS
:cmt..np
:cmt.Italics
:cmt..np
:cmt.Throughout our manuals you may notice that for our multi-platform tools we
:cmt.have selected Windows as our default environment. Our multi-platform tools will
:cmt.appear similar when running under OS/2 unless otherwise noted.
