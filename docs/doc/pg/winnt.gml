.chap Windows NT Programming Overview
.*
.np
.ix 'Windows NT' 'programming overview'
Windows NT supports both non-windowed character-mode applications and
windowed Graphical User Interface (GUI) applications.
In addition, Windows NT supports Dynamic Link Libraries and
applications with multiple threads of execution.
.np
.ix 'Windows NT'
.ix 'NT development'
We have supplied all the necessary tools for native development on
Windows NT.
You can also cross develop for Windows NT using either the DOS-hosted
compilers and tools, the Windows 95-hosted compilers and tools, or the
OS/2-hosted compilers and tools.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.np
Note - If you are on the host with operating system other then 32-bit 
Windows, you should setup INCLUDE environment variable correctly 
to compile for 32-bit Windows target.
.np
You can do that by command (DOS, OS/2, Windows 3.x)
.np
set INCLUDE=%WATCOM%\h;%WATCOM%\h\nt
.np
or by command (LINUX)
.np
export INCLUDE=$WATCOM/h:$WATCOM/h/nt
.np
.do end
Testing and debugging of your Windows NT application must be done on
Windows NT or Windows 95.
.np
.ix 'Phar Lap TNT'
.ix 'TNT'
If you are creating a character-mode application, you may also be
interested in a special DOS extender from Phar Lap (TNT) that can run
your Windows NT character-mode application under DOS.
.*
.if '&lang' eq 'C/C++' .do begin
.*
.section Windows NT Programming Note
.*
.np
.ix 'Windows NT' 'programming notes'
.ix 'structure alignment'
.ix 'enums'
When doing Win32 programming, you should use the /ei and /zp4 options
to compile C and C++ code with the &company compilers since this
adjusts the compilers to match the default Microsoft compiler
behaviour.
Some Microsoft software relies on the default behaviour of their own
compiler regarding the treatment of enums and structure packing
alignment.
.do end
.*
.section Windows NT Character-mode Versus GUI
.*
.np
.ix 'Windows NT' 'character-mode applications'
.ix 'Windows NT' 'GUI applications'
Basically, there are two classes of &lang applications that can run in
a windowed environment like Windows NT.
.np
The first are those &lang applications that do not use any of the
Win32 API functions; they are strictly &lang applications that do
not rely on the features of a particular operating system.
.if '&defwinnt' eq '1' .do begin
&company gives you two choices when porting these kinds of
applications to Windows NT.
.np
.begbull
.bull
You may choose to create a character-mode application that makes no
use of the windowing capabilities of the system. In this case Application 
must be created as Windows NT Character-mode Application.
.np
.bull
Or, you may choose to make use of &company's default windowing system
in which application output will be directed to one or more windows.
The latter can give somewhat of a GUI look-and-feel to an application
what wasn't designed for the GUI environment. In this case Application 
must be created as Windows NT Non-GUI Application.
.endbull
.do end
.el .do begin
.np
.begbull
.bull
This Application must be created as Windows NT Character-mode Application.
.endbull
.do end
.np
The second class of &lang applications are those that actually call
Win32 API functions directly.
These are applications that have been tailored for the Win32
operating environment.
.if '&lang' eq 'C/C++' .do begin
There can occure two application types.
.begbull
.bull
First one uses GUI interface then it must be created as Windows NT GUI Application.
.np
.bull
Second one uses only character console (no GUI) then it must be created as Windows NT Character-mode Application
.endbull
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.begbull
.bull
&product does not provide direct support for these types of
applications.
While we do provide include files that map out 16-bit Windows
structures and the interface to 16-bit Windows API calls, we do not
provide this for Win32 API.
The Win32 application developer must create these as required.
.np
.bull
An alternate solution, for those so-inclined, is to develop the GUI
part of the interface in C and call these functions from FORTRAN code.
.endbull
.do end
.np
A subsequent chapters deal with the creation of different application types for Windows NT target.
