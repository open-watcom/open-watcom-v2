/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  Global variables for wmake.
*
****************************************************************************/


#include "make.h"
#if defined( DEVELOPMENT ) || defined( INTERNAL_VERSION )
    /* just because this is compiled with -d2 - we get all the debugging
     * info we'll ever need this way :)
     */
#   include "macros.h"
#   include "mcache.h"
#   include "mmemory.h"
#   include "mexec.h"
#   include "mhash.h"
#   include "mmisc.h"
#   include "mrcmsg.h"
#   include "msg.h"
#   include "mparse.h"
#   include "mpreproc.h"
#   include "mstream.h"
#   include "msuffix.h"
#   include "mlex.h"
#   include "mtarget.h"
#   include "mupdate.h"
#   include "mvecstr.h"
#if defined( __DOS__ )
#   include "tinyio.h"
#endif
#endif
#include "banner.h"


struct Glob Glob;

/*
 * Be careful that these declarations doesn't exceed 2048 characters.
 * Note that this is just one big string - not an array of strings.
 */
const char FAR *BuiltIns = {
    "__MAKEOPTS__=%s\n"
    "__MAKEFILES__=\n"
    "__VERSION__=" BANSTR( _BANVER ) "\n"
#ifdef DLLS_IMPLEMENTED
    "__LOADDLL__=\n"
#endif

#if defined( __DOS__ )
    "__MSDOS__=\n"

#elif defined( __NT__ )
    "__NT__=\n"
    #if defined( _M_X64 ) || defined( __x86_64__ ) || defined( __amd64__ ) || defined( __amd64 )
        "__NTX64__=\n"
    #elif defined( _M_IX86 ) || defined( __i386 ) || defined( __i386__ )
        "__NT386__=\n"
    #elif defined( __AXP__ )
        "__NTAXP__=\n"
    #else
        #error Unknown CPU architecture
    #endif

#elif defined( __OS2__ )
    "__OS2__=\n"

#elif defined( __QNX__ )
    "__QNX__=\n"
    "__UNIX__=\n"

#elif defined( __SOLARIS__ ) || defined( __SunOS ) || defined( __sun )
    "__SOLARIS__=\n"
    "__UNIX__=\n"

#elif defined( __OSX__ ) || defined( __APPLE__ )
    "__OSX__=\n"
    "__BSD__=\n"
    "__UNIX__=\n"
    #if defined( _M_X64 ) || defined( __x86_64__ ) || defined( __amd64__ ) || defined( __amd64 )
        "__BSDX64__=\n"
        "__OSXX64__=\n"
    #elif defined( _M_IX86 ) || defined( __i386 ) || defined( __i386__ )
        "__BSDX386__=\n"
        "__OSXX386__=\n"
    #elif defined( __PPC__ ) || defined( __ppc__ ) || defined( __powerpc__ )
        "__BSDPPC__=\n"
        "__OSXPPC__=\n"
    #elif defined( _M_ARM ) || defined( __ARM__ ) || defined( __arm__ )
        "__BSDARM__=\n"
        "__OSXARM__=\n"
    #else
        #error Unknown CPU architecture
    #endif

#elif defined( __LINUX__ ) || defined( __linux__ )
    "__LINUX__=\n"
    "__UNIX__=\n"
    #if defined( _M_X64 ) || defined( __x86_64__ ) || defined( __amd64__ ) || defined( __amd64 )
        "__LINUXX64__=\n"
    #elif defined( _M_IX86 ) || defined( __i386 ) || defined( __i386__ )
        "__LINUX386__=\n"
    #elif defined( __PPC__ ) || defined( __ppc__ ) || defined( __powerpc__ )
        "__LINUXPPC__=\n"
    #elif defined( __MIPS__ ) || defined( __mips__ )
        "__LINUXMIPS__=\n"
    #elif defined( _M_ARM ) || defined( __ARM__ ) || defined( __arm__ )
        "__LINUXARM__=\n"
    #else
        #error Unknown CPU architecture
    #endif

#elif defined( __BSD__ ) || defined( __FreeBSD__ )
    "__BSD__=\n"
    "__UNIX__=\n"
    #if defined( _M_X64 ) || defined( __x86_64__ ) || defined( __amd64__ ) || defined( __amd64 )
        "__BSDX64__=\n"
    #elif defined( _M_IX86 ) || defined( __i386 ) || defined( __i386__ )
        "__BSD386__=\n"
    #elif defined( __PPC__ ) || defined( __ppc__ ) || defined( __powerpc__ )
        "__BSDPPC__=\n"
    #else
        #error Unknown CPU architecture
    #endif

#elif defined( __HAIKU__ ) || defined( __haiku__ )
    "__HAIKU__=\n"
    "__UNIX__=\n"
    #if defined( _M_X64 ) || defined( __x86_64__ ) || defined( __amd64__ ) || defined( __amd64 )
        "__HAIKUX64__=\n"
    #elif defined( _M_IX86 ) || defined( __i386 ) || defined( __i386__ )
        "__HAIKU386__=\n"
    #elif defined( __PPC__ ) || defined( __ppc__ ) || defined( __powerpc__ )
        "__HAIKUPPC__=\n"
    #else
        #error Unknown CPU architecture
    #endif

#endif
};

const char FAR *SuffixList = {
    ".SUFFIXES: "
        ".exe .nlm .dsk .lan .exp "             /* executable extensions */
        ".lib .obj "                            /* object file extensions */
        ".i "                                   /* preprocessor output exts */
        ".asm .c .cpp .cxx .cc .for .pas .cob " /* source file extensions */
        ".h .hpp .hxx .hh .fi .mif .inc\n"      /* include file extensions */
};

const char FAR *MSSuffixList = {
    ".SUFFIXES: "
        ".exe .obj .asm .c .cpp .cxx .bas .cbl .for .f .f90 .pas .res .rc"
};


const char FAR *UNIXSuffixList = {
    ".SUFFIXES: "
        ".exe .obj .c .y .l .f"
};

const char FAR *POSIXSuffixList = {
    ".SUFFIXES: "
        ".o .c .y .l .a .sh .f"
};

/*
 * Be careful that this doesn't exceed 2048 characters.  Note that this is just
 * one big string - not an array of strings.
 */
const char FAR* MSBuiltIn = {
    "AS=ml\n"
    "BC=bc\n"
    "CC=cl\n"
    "COBOL=cobol\n"
    "CPP=cl\n"
    "CXX=cl\n"
    "FOR=fl\n"
    "PASCAL=pl\n"
    "RC=rc\n"
    ".asm.exe:\n"
    "    $(AS) $(AFLAGS) $*.asm\n"
    ".asm.obj:\n"
    "    $(AS) $(AFLAGS) /c $*.asm\n"
    ".c.exe:\n"
    "    $(CC) $(CFLAGS) $*.c\n"
    ".c.obj:\n"
    "    $(CC) $(CFLAGS) /c $*.c\n"
    ".cpp.exe:\n"
    "    $(CPP) $(CPPFLAGS) $*.cpp\n"
    ".cpp.obj:\n"
    "    $(CPP) $(CPPFLAGS) /c $*.cpp\n"
    ".cxx.exe:\n"
    "    $(CXX) $(CXXFLAGS) $*.cxx\n"
    ".cxx.obj:\n"
    "    $(CXX) $(CXXFLAGS) $*.cxx\n"
    ".bas.obj:\n"
    "    $(BC) $(BFLAGS) $*.bas\n"
    ".cbl.exe:\n"
    "    $(COBOL) $(COBFLAGS) $*.cbl, $*.exe;\n"
    ".cbl.obj:\n"
    "    $(COBOL) $(COBFLAGS) $*.cbl;\n"
    ".f.exe:\n"
    "    $(FOR) $(FFLAGS) $*.f\n"
    ".f.obj:\n"
    "    $(FOR) /c $(FFLAGS) $*.f\n"
    ".f90.exe:\n"
    "    $(FOR) $(FFLAGS) $*.f90\n"
    ".f90.obj:\n"
    "    $(FOR) /c $(FFLAGS) $*.f90\n"
    ".for.exe:\n"
    "    $(FOR) $(FFLAGS) $*.for\n"
    ".for.obj:\n"
    "    $(FOR) /c $(FFLAGS) $*.for\n"
    ".pas.exe:\n"
    "    $(PASCAL) $(PFLAGS) $*.pas\n"
    ".pas.obj:\n"
    "    $(PASCAL) /c $(PFLAGS) $*.pas\n"
    ".rc.res:\n"
    "    $(RC) $(RFLAGS) /r $*\n"

};

const char FAR* UNIXBuiltIn = {
     "YACC=yacc\n"
     "YFLAGS=\n"
     "LEX=lex\n"
     "LFLAGS=\n"
     "LDFLAGS=\n"
     "CC=cl\n"
     "FC=fl\n"
     "CFLAGS=-nologo\n"
     ".c.exe:\n"
     "    $(CC) $(CFLAGS) $(LDFLAGS) $<\n"
     ".f.exe:\n"
     "    $(FC) $(FFLAGS) $(LDFLAGS) $<\n"
     ".c.obj:\n"
     "    $(CC) $(CFLAGS) -c $<\n"
     ".f.obj:\n"
     "    $(FC) $(FFLAGS) -c $<\n"
     ".y.obj:\n"
     "    $(YACC) $(YFLAGS) $<\n"
     "    $(CC) $(CFLAGS) -c y.tab.c\n"
     "    del y.tab.c\n"
     "    move y.tab.obj $@\n"
     ".l.obj:\n"
     "    $(LEX) $(LFLAGS) $<\n"
     "    $(CC) $(CFLAGS) -c lex.yy.c\n"
     "    del lex.yy.c\n"
     "    move lex.yy.obj $@\n"
     ".y.c:\n"
     "    $(YACC) $(YFLAGS) $<\n"
     "    move y.tab.c $@\n"
     ".l.c:\n"
     "    $(LEX) $(LFLAGS) $<\n"
     "    move lex.yy.c $@\n"
};

/* The following definitions are taken from SUSv3 */
const char FAR* POSIXBuiltIn = {
    /* Predefined Macros */
    "MAKE=make\n"
    "AR=ar\n"
    "ARFLAGS=-rv\n"
    "YACC=yacc\n"
    "YFLAGS=\n"
    "LEX=lex\n"
    "LFLAGS=\n"
    "LDFLAGS=\n"
    "CC=owcc\n"     /* SUSv3 says 'CC=c99' */
    "CFLAGS=-O\n"
    "FC=fort77\n"
    "FFLAGS=-O 1\n"
    /* Single suffix rules */
#if 0
    ".c:\n"
    "    $(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<\n"
    ".f:\n"
    "    $(FC) $(FFLAGS) $(LDFLAGS) -o $@ $<\n"
    ".sh:\n"
    "    cp $< $@\n"
    "    chmod a+x $@\n"
#endif
    /* Double suffix rules */
    ".c.o:\n"
    "    $(CC) $(CFLAGS) -c $<\n"
    ".f.o:\n"
    "    $(FC) $(FFLAGS) -c $<\n"
    ".y.o:\n"
    "    $(YACC) $(YFLAGS) $<\n"
    "    $(CC) $(CFLAGS) -c y.tab.c\n"
    "    rm -f y.tab.c\n"
    "    mv y.tab.o $@\n"
    ".l.o:\n"
    "    $(LEX) $(LFLAGS) $<\n"
    "    $(CC) $(CFLAGS) -c lex.yy.c\n"
    "    rm -f lex.yy.c\n"
    "    mv lex.yy.o $@\n"
    ".y.c:\n"
    "    $(YACC) $(YFLAGS) $<\n"
    "    mv y.tab.c $@\n"
    ".l.c:\n"
    "    $(LEX) $(LFLAGS) $<\n"
    "    mv lex.yy.c $@\n"
    ".c.a:\n"
    "    $(CC) -c $(CFLAGS) $<\n"
    "    $(AR) $(ARFLAGS) $@ $*.o\n"
    "    rm -f $*.o\n"
    ".f.a:\n"
    "    $(FC) -c $(FFLAGS) $<\n"
    "    $(AR) $(ARFLAGS) $@ $*.o\n"
    "    rm -f $*.o\n"
};

/*
 * This is the table indexed by users of the is... functions.
 * The program 'cretype.exe' is used to rebuild this table.
 */
const UINT8 IsArray[] = {
#include "isarray.gh"
};
