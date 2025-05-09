/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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
    /*
     * just because this is compiled with -d2 - we get all the debugging
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
#endif
#include "banner.h"


struct Glob Glob;

/*
 * Be careful that these declarations doesn't exceed 2048 characters.
 * Note that this is just one big string - not an array of strings.
 */
const char FAR BuiltIns[] = {
    "__MAKEOPTS__=%s\n"
    "__MAKEFILES__=\n"
    "__VERSION__=" _MACROSTR( _BLDVER ) "\n"
#ifdef DLLS_IMPLEMENTED
    "__LOADDLL__=\n"
#endif

#if defined( __WATCOMC__ )

#if defined( __DOS__ )
    "__MSDOS__=\n"
    "__DOS__=\n"

#elif defined( __NT__ )
    "__NT__=\n"
    #if defined( _M_X64 )
        "__NTX64__=\n"
    #elif defined( _M_IX86 )
        "__NT386__=\n"
    #elif defined( __AXP__ )
        "__NTAXP__=\n"
    #elif defined( __MIPS__ )
        "__NTMIPS__=\n"
    #else
        #error Unknown CPU architecture
    #endif

#elif defined( __OS2__ )
    "__OS2__=\n"

#elif defined( __QNX__ )
    "__QNX__=\n"
    "__UNIX__=\n"

#elif defined( __RDOS__ )
    "__RDOS__=\n"

#elif defined( __LINUX__ )
    "__LINUX__=\n"
    "__UNIX__=\n"
    #if defined( _M_X64 )
        "__LINUXX64__=\n"
    #elif defined( _M_IX86 )
        "__LINUX386__=\n"
    #elif defined( __PPC__ )
        "__LINUXPPC__=\n"
    #elif defined( __MIPS__ )
        "__LINUXMIPS__=\n"
    #else
        #error Unknown CPU architecture
    #endif
#endif

#else /* non-WATCOM toolchains */

#if defined( __NT__ )
    "__NT__=\n"
    #if defined( _M_X64 ) || defined( __x86_64__ ) || defined( __amd64__ ) || defined( __amd64 )
        "__NTX64__=\n"
    #elif defined( _M_IX86 ) || defined( __i386 ) || defined( __i386__ )
        "__NT386__=\n"
    #elif defined( __AXP__ )
        "__NTAXP__=\n"
    #elif defined( __MIPS__ ) || defined( _MIPS_ )
        "__NTMIPS__=\n"
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
    #elif defined( _M_ARM64 ) || defined( __arm64__ ) || defined( __aarch64__ )
        "__LINUXARM64__=\n"
    #else
        #error Unknown CPU architecture
    #endif

#elif defined( __SOLARIS__ ) || defined( __SunOS ) || defined( __sun )
    "__SOLARIS__=\n"
    "__UNIX__=\n"

#elif defined( __OSX__ ) || defined( __APPLE__ )
    "__OSX__=\n"
    "__UNIX__=\n"
    #if defined( _M_X64 ) || defined( __x86_64__ ) || defined( __amd64__ ) || defined( __amd64 )
        "__OSXX64__=\n"
    #elif defined( _M_IX86 ) || defined( __i386 ) || defined( __i386__ )
        "__OSX386__=\n"
    #elif defined( __PPC__ ) || defined( __ppc__ ) || defined( __powerpc__ )
        "__OSXPPC__=\n"
    #elif defined( _M_ARM ) || defined( __ARM__ ) || defined( __arm__ )
        "__OSXARM__=\n"
    #elif defined( _M_ARM64 ) || defined( __arm64__ ) || defined( __aarch64__ )
        "__OSXARM64__=\n"
    #else
        #error Unknown CPU architecture
    #endif

#elif defined( __BSD__ ) \
     || defined( __FREEBSD__ ) || defined( __FreeBSD__ ) \
     || defined( __DRAGONFLY__ ) || defined( __DragonFly__ ) \
     || defined( __NETBSD__ ) || defined( __NetBSD__ ) \
     || defined( __OPENBSD__ ) || defined( __OpenBSD__ )
    "__BSD__=\n"
    "__UNIX__=\n"
    #if defined( __FREEBSD__ ) || defined( __FreeBSD__ )
        "__FREEBSD__=\n"
    #elif defined( __DRAGONFLY__ ) || defined( __DragonFly__ )
        "__DRAGONFLY__=\n"
    #elif defined( __NETBSD__ ) || defined( __NetBSD__ )
        "__NETBSD__=\n"
    #elif defined( __OPENBSD__ ) || defined( __OpenBSD__ )
        "__OPENBSD__=\n"
    #endif
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

#endif /* non-WATCOM toolchains */
};

const char FAR SuffixList[] = {
    ".SUFFIXES: "
        ".exe .nlm .dsk .lan .exp "             /* executable extensions */
        ".lib .obj .o "                         /* object file extensions */
        ".i "                                   /* preprocessor output exts */
        ".asm .c .cpp .cxx .cc .for .pas .cob " /* source file extensions */
        ".h .hpp .hxx .hh .fi .mif .inc\n"      /* include file extensions */
};

const char FAR MSSuffixList[] = {
    ".SUFFIXES: "
        ".exe .obj .o .asm .c .cpp .cxx .bas .cbl .for .f .f90 .pas .res .rc"
};


const char FAR UNIXSuffixList[] = {
    ".SUFFIXES: "
        ".exe .obj .o .c .y .l .f"
};

const char FAR POSIXSuffixList[] = {
    ".SUFFIXES: "
        ".o .obj .c .y .l .a .sh .f"
};

/*
 * Be careful that this doesn't exceed 2048 characters.  Note that this is just
 * one big string - not an array of strings.
 */
const char FAR MSBuiltIn[] = {
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

const char FAR UNIXBuiltIn[] = {
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

/*
 * The following definitions are taken from SUSv3
 */
const char FAR POSIXBuiltIn[] = {
    /*
     * Predefined Macros
     */
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
    /*
     * Single suffix rules
     */
#if 0
    ".c:\n"
    "    $(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<\n"
    ".f:\n"
    "    $(FC) $(FFLAGS) $(LDFLAGS) -o $@ $<\n"
    ".sh:\n"
    "    cp $< $@\n"
    "    chmod a+x $@\n"
#endif
    /*
     * Double suffix rules
     */
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
 * The program 'cretype.exe' is used to create file isarray.gh
 * used by this table.
 *
 * NOTE:
 *  this code depends on STRM_T definition in mstream.h
 *
 *  hold this file in sync with STRM_T definition in mstream.h
 */
const UINT8 IsArray[] = {
    0,      /* -4 STRM_TMP_LEX_START    */
    0,      /* -3 STRM_TMP_EOL          */
    0,      /* -2 STRM_MAGIC            */
    0,      /* -1 STRM_END              */
    #include "isarray.gh"
};
