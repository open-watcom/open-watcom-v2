.chap Creating Windows NT POSIX Applications
.*
.np
.ix 'POSIX applications'
This chapter describes how to compile and link POSIX applications for
Windows NT.
There are a number of issues to consider.
.autopoint
.point
&company does not provide its own POSIX libraries.
.ix 'POSIX libraries'
.ix 'Microsoft Win32 SDK'
.ix 'Win32 SDK'
You must use those included with the Microsoft Win32 SDK.
They are
.fi libcpsx.lib,
.fi psxdll.lib
and
.fi psxrtl.lib.
If you installed the Win32 SDK component when you installed the
&company software, you will find these libraries in the
.fi %&pathvarup%\lib386\nt
directory.
.point
.ix 'cdecl'
Since you will be using Microsoft POSIX libraries compiled by the
Microsoft compiler, you must follow the calling conventions used by
Microsoft (i.e., the
.mono __cdecl
convention).
The &company compiler can generate these calling conventions provided
that the POSIX library routines are all properly prototyped.
.point
&company does not provide its own header files for use with the
Microsoft POSIX libraries.
The Microsoft Win32 SDK includes only a subset of the headers required
for calling the POSIX library routines.
If you installed the Win32 SDK component when you installed the
&company software, you will find these headers in the
.fi %&pathvarup%\sdk\posix\h
and
.fi %&pathvarup%\sdk\posix\h\sys
directories.
Take a look at these directories to see what is and what is not
included.
.point
If you have the Microsoft compiler, then you will likely have access to
the missing header files.
If you do not have the Microsoft compiler, then you will have to define
prototypes for any of the POSIX library routines that you use for which
no prototypes are defined in any of the POSIX header files.
.point
There is one exception to the generation of the
.mono __cdecl
calling convention for appropriately prototyped functions.
This is the
.mono main
function.
Since many Microsoft sample programs inappropriately declare the
.mono main
function as
.mono __cdecl,
it was necessary to make a special case in the &company compilers
to ignore the
.mono __cdecl
attribute when used for this entry point.
To work around this problem, a special pragma is used.
This is shown in the following example.
.point
Since we are going to use the Microsoft POSIX libraries rather than
the &company libraries, we will use the "zl" compile option to
instruct the &company compiler not to include references to &company
libraries in the object files.
.endpoint
.np
To illustrate the creation of a POSIX application, we will use a
simple example.
This program displays an identifying banner and then displays its
arguments one at a time.
.exam begin
[POSIXSMP.C]
#include <unistd.h>

// The Win32 SDK doesn't provide a complete set of
// headers for the libraries (e.g., no stdio.h).

extern int __cdecl printf( char *, ... );

// Note: the "__cdecl" attribute is ignored for main().

int __cdecl main( int argc, char **argv )
{
    int i;
    printf( "POSIX sample program\n" );
    for( i = 0 ; i < argc ; i++ ) {
        printf( "%d: %s\n", i, argv[i] );
    }
    return 0;
}

// Since the "__cdecl" attribute is ignored,
// make sure that parms go on the stack for main
// and that main gets the _ in the right place by
// using a pragma to do so.

#pragma aux main "_*" parm [];

// The compiler emits references to these symbols,
// so make sure they get defined here to prevent
// unresolved references.

int _cstart_;
#pragma aux _cstart_ "*";
int __argc;
#pragma aux __argc "*";
.exam end
.np
The example program illustrates some of the special considerations
required for using the Microsoft POSIX libraries rather than the
&company libraries.
There are also some special link time issues and these are addressed
in the following sample "makefile".
.tinyexam begin
[MAKEFILE]
posixsmp.exe : posixsmp.c posix.add makefile.
  set nt_include=
  set include=$(%&pathvar.)\sdk\posix\h;$(%&pathvar.)\sdk\posix\h\sys
  wcc386 -bt=nt -oaxt -zl posixsmp.c
  wlink @posix.add file posixsmp sys nt_posix option map

posix.add :
  %create posix.add
  %append posix.add system begin nt_posix
  %append posix.add   option osname='Windows NT character-mode posix'
  %append posix.add   libpath %&pathvarup.%\lib386\nt
  %append posix.add   option nodefaultlib
  %append posix.add   option start=___PosixProcessStartup
  %append posix.add   lib { libcpsx.lib psxrtl.lib psxdll.lib }
  %append posix.add   format windows nt ^
  %append posix.add   runtime posix
  %append posix.add end
.tinyexam end
.np
A new "nt_posix" system is defined in the
.fi posix.add
file.
This file is generated automatically by the makefile.
.np
That is about all there is to creating a Windows NT POSIX application.
One final note - make sure when using the Microsoft headers that all
the library routines that you use are declared as
.mono __cdecl;
otherwise your application will not run correctly.
