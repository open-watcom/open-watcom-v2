.* (c) Portions Copyright 1990, 1992 by Tenberry Software, Inc.
.*
.chap *refid=linexe Linear Executables
.*
.np
.ix 'linear executable'
To build a linear executable, compile and link it as described in the
chapter entitled "Creating 32-bit &dos4gprd Executables".
.ix 'Instant-D'
.ix '&dos4gexe'
The resulting file will not run independently: you can run it under
the &dbgname, Tenberry Software Instant-D debugger, or with the
standalone "&dos4gexe".
.*
.section The Linear Executable Format
.*
.np
.ix 'linear executable'
.ix 'executable' 'linear'
.ix 'LE format'
&dos4g works with files that use the Linear Executable (LE) file
format.
The format represents a protected-mode program in the context of a
32-bit 386 runtime environment with linear to physical address
translation hardware enabled.
It uses a flat address space.
.np
.ix 'segmented executable'
.ix 'executable' 'segmented'
.ix 'NE format'
This file format is similar to the Segmented Executable (NE) format
used in OS/2 1.x and MS Windows.
Both support Dynamic Linking, Resources, and are geared toward
protected-mode programs.
Both formats use tables of "counted ASCII" names, and they use similar
relocation formats.
.np
.ix 'stub program'
Both formats begin with a DOS style stub program that sophisticated
loaders skip.
This stub program executes when the &dos4g loader is not present,
displaying the message,
.us This program cannot run in DOS mode.
.np
:cmt. When your program is ready for distribution, you replace this stub
:cmt. program with a stub that calls &dos4gnam..
When the &lnkname is used to link a &dos4g application, it automatically
replaces the default stub program with one that calls &dos4gnam..
.*
.beglevel
.*
.section The Stub Program
.*
.np
The stub at the beginning of a linear executable is a real-mode
program that you can modify as you like.
For example, you can:
.begbull
.bull
make the stub program do a checksum on the "&dos4gexe" file to make
sure it's the correct version.
:cmt. .bull
:cmt. change the name of the "&dos4gexe" file, to connect it with your
:cmt. product and as another way to ensure that the correct version is
:cmt. called.
:cmt. .bull
:cmt. change the banner that is displayed at startup.
:cmt. (You must display the Tenberry Software copyright message, but you do
:cmt. not have to show it at startup time.)
.bull
copy protect your program.
.bull
specify a search path for the "&dos4gexe" file.
.bull
add command line arguments.
.endbull
.np
The SRC directory contains source code for a sample stub program.
.ix 'WSTUB.C'
"WSTUB.C" is a simple example, a good base to start from when you
construct your own stub.
Please note that you will require a 16-bit C compiler to compile a new
stub program.
Following is the code in "WSTUB.C":
.ix 'environment variables' 'DOS4GPATH'
.ix 'DOS4GPATH environment variable'
.code begin
#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <errno.h>
#include <string.h>

/* Add environment strings to be searched here */
char *paths_to_check[] = {
        "DOS4GPATH",
        "PATH"};

.code break
char *dos4g_path()
{
    static char fullpath[80];
    int i;

    for( i = 0;
         i < sizeof( paths_to_check ) / sizeof( paths_to_check[0] );
         i++ ) {
        _searchenv( "dos4gw.exe", paths_to_check[i], fullpath );
        if( fullpath[0] ) return( &fullpath );
    }
    for( i = 0;
         i < sizeof( paths_to_check ) / sizeof( paths_to_check[0] );
         i++ ) {
        _searchenv( "dos4g.exe", paths_to_check[i], fullpath );
        if( fullpath[0] ) return( &fullpath );
    }
    return( "dos4gw.exe" );
}

.code break
main( int argc, char *argv[] )
{
    char        *av[4];
    auto char   cmdline[128];

    av[0] = dos4g_path();               /* Locate the DOS/4G loader */
    av[1] = argv[0];                    /* name of executable to run */
    av[2] = getcmd( cmdline );          /* command line */
    av[3] = NULL;                       /* end of list */
#ifdef QUIET
    putenv( "DOS4G=QUIET" );    /* disables DOS/4G Copyright banner */
#endif
    execvp( av[0], av );
    puts( "Stub exec failed:" );
    puts( av[0] );
    puts( strerror( errno ) );
    exit( 1 );                  /* indicate error */
}
.code end
.if '&lang' eq 'FORTRAN 77' .do begin
.np
If you do not have a C compiler, you can create an assembly language version
of the above sample stub program and use it to create your own version of the
stub program.
.do end
.*
.endlevel
.*
.section Memory Use
.*
.np
.ix '&dos4gprd' 'memory use'
This section explains how a &dos4g application uses the memory on a
386-based PC/AT.
The basic memory layout of an AT machine consists of 640KB of DOS
memory, 384KB of upper memory, and an undetermined amount of
extended memory.
DOS memory and upper memory together compose real memory, the memory
that can be addressed when the processor is running in real mode.
:cmt. :image xoff='0.0i' depth='4.0i' file='RSI\MEM1' text='Basic Memory Layout'.
.figure *depth='4.0' *scale=100 *file='mem1' Basic Memory Layout
.np
Under &dos4g, the first megabyte of physical memory &mdash the real
memory &mdash is mapped as a shared linear address space.
This allows your application to use absolute addresses in real memory,
to access video RAM or BIOS ROM, for example.
Because the real memory is available to all processes, you are not
guaranteed to be able to allocate a particular area in real memory:
another process may have allocated it already.
.np
Most code and data is placed in a paged linear address space starting
at 4MB.
The linear address space starts at 4MB, the first address in the
second page table, to avoid conflicts with VCPI system software.
.np
This split mapping &mdash an executable that is linked to start at 4MB in
the linear address space, with the first MB in the address space
mapped to the first MB of physical memory &mdash is called a
.us split flat model.
.np
The illustration below shows the layout of physical memory on the
left, and the layout of the linear address space on the right.
:cmt. :image xoff='0.0i' depth='4.0i' file='RSI\MEM2' text='Physical Memory/Linear Address Space'.
.figure *depth='4.0' *scale=100 *file='mem2' Physical Memory/Linear Address Space
.np
The 1KB label in the diagram indicates the top of the real-mode
interrupt vectors.
4KB marks the end of the first page.
