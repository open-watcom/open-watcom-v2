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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


/*@H************************ < COMPRESS HEADER > ****************************
*   $@(#) compress.c,v 4.3 88/12/26 08:00:00 don Release ^                  *
*                                                                           *
*   compress : compress.h <global defines, etc>                             *
*                                                                           *
*   port by  : Donald J. Gloistein                                          *
*                                                                           *
*   Source, Documentation, Object Code:                                     *
*   released to Public Domain.  This code is based on code as documented    *
*   below in release notes.                                                 *
*                                                                           *
*---------------------------  Module Description  --------------------------*
*   THIS HEADER CONTAINS MUCH IMPLEMENTATION INFORMATION AND ASSUMPTIONS    *
*   PLEASE PRINT IT OUT AND READ IT BEFORE COMPILING CODE FOR YOURSELF      *
*                                                                           *
*   This header supports a number of compiler defines and predefines.       *
*   Rather than explain all of them, please print the header and read the   *
*   notes. Also the unix and xenix makefiles are commented for the          *
*   various options. There continues to have a lot of Dos specific info in  *
*   the header. This is to help on 16 bit Msdos machines to get their       *
*   compiler to work properly. I make no appology for that, as this port    *
*   began as a way to implement 16 bit compress on a segmented MsDos machine*
*                                                                           *
*   However, for Unix and Xenix, all you should have to define is -DXENIX   *
*   or -DUNIX and compile. There may be a problem with whether your library *
*   supports alloc() or malloc(), but there is a define for that, also.     *
*                                                                           *
*   This header can be maintained to keep up with the different compilers   *
*   and systems. As distributed in don Release, the files will compile with *
*   no changes under Microsoft version 5.1 C compiler, and Xenix C compiler *
*   which is the Microsoft version 4 ported. If you are going to bind the   *
*   code for use in MsDos and OS/2 machines, then you must uncomment the    *
*   #define BIND in this header. Otherwise, this distribution of source     *
*   detect Msdos and Xenix predefines from the compiler and adjust.         *
*                                                                           *
*--------------------------- Implementation Notes --------------------------*
*                                                                           *
*   compiled with : compress.fns                                            *
*
*   NOTE!!!  Defaults of this code now are completely Unix, even for the    *
*            msdos ports.  That means that the program works as a filter,   *
*            and will just sit there waiting for input from stdin if you    *
*            issue just the command name. You must use -h or -? to get the  *
*            full help screen now. Also, it will unlink (kill) as a default *
*            on successful compression and decompression. That means the    *
*            source file will be erased.                                    *
*            These defaults are changed with the FILTER and KEEPFLAG        *
*            defines.                                                       *
*                                                                           *
*   NOTE!!!  Compiler predefines were taken out of the compress.h header.   *
*            You must either specify them on compile or uncomment the       *
*            compiler define in this header. Compiling without doing these  *
*            will result in a program that does unspecified actions.        *
*   problems:                                                               *
*            The inpath and outpath is a bit kludged. It should work okay.  *
*            Let me know if you have problems, especially under Unix.       *
*                                                                           *
*   CAUTION: The bound version will run on Dos 2.x, but you must use the    *
*            name compress.exe.  If you rename the file, it will not run    *
*            The unbound version will run on Dos 2.x with the name changed  *
*            but due to the dos version, will not detect its own name.      *
*                                                                           *
*   CAUTION: Non MsDos users. You must modify the _MAX_PATH defines for     *
*            your operating system if it is different from the assumed      *
*            standard.                                                      *
*                                                                           *
*   CAUTION: I have used a number of defines to make it possible to compile *
*            properly under a number of bit sizes and adjust for the memory *
*            allocation scheme needed. If you do not use a dos system,      *
*            PLEASE pay attention to the defines for MAXSEG_64 and the one  *
*            called SMALLMODEL. The SMALLMODEL define is set in the header  *
*            but if you don't have a compiler that triggers the MAXSEG_64   *
*            define, you may end up with bad pointers. Becareful.           *
*                                                                           *
*   Header for files using version 4 compress routines define MAIN          *
*   in the file with defining instance of the global variables.             *
*   There are a number of compilers for MsDos and Unix/Xenix.               *
*   So the user must define the actions required.                           *
*                                                                           *
*                                                                           *
*   Defines:  This header file contains most of the system wide defines.    *
*             the purpose for this was to consolodate compiler differences  *
*             into one area that is easily changed.                         *
*                                                                           *
*   define MAXBITS= if you want a different maximum bits. 16 bits will now  *
*   run in about 400K of memory.                                            *
*   define BIND if you are going to use Microsoft bind.exe program on the   *
*   executable.                                                             *
*                                                                           *
*   define MSDOS if you are compiling under MsDos or PcDos and your compiler*
*   does not predefine it.                                                  *
*                                                                           *
*     Initials ---- Name ---------------------------------                  *
*      DjG          Donald J. Gloistein, current port to MsDos 16 bit       *
*                   Plus many others, see rev.hst file for full list        *
*      LvR          Lyle V. Rains, many thanks for improved implementation  *
*                   of the compression and decompression routines.          *
*************************************************************************@H*/


#ifdef UNIX
#define NPROTO
#define COMP40           /* take this out for a little more speed */
#ifdef ALLOC
char *alloc();
#define ALLOCATE(x,y)   alloc((unsigned int)x*y)
#else
char *malloc();
#define ALLOCATE(x,y)   malloc((unsigned int)x*y)
#endif
#define FREEIT(ptr)     free(ptr)
#define setbinary(fp)
#define NO_SETVBUF      /* most don't support setvbuf() function */
#endif

                        /* NOTE: This program will not work on the */
                        /* IBM/PC version of MINIX due to the small */
                        /* memory model restrictions.  There is no */
                        /* such thing as a FAR pointer in MINIX-PC */
#ifdef MINIX            /* Unix V7 clone for Atari ST */
#define strchr  index
#define strrchr rindex
#define DFLTBITS 13     /* compatible with original MINIX compress, max=16 */
#ifndef KEEPFLAG
#define KEEPFLAG 1      /* compatible with original MINIX compress */
                        /* may be redefined from the Makefile if desired */
#endif
#ifndef VERBOSE
#define VERBOSE true    /* compatible with original MINIX compress */
                        /* may be redefined from the Makefile if desired */
#endif
#define NPROTO
#define SIGTYPE int     /* MINIX defines this as pointer to int */
                        /* for the return from a signal         */
#endif

                          /* Microsoft C compiler  v 4.0-5.1 */
                          /* MSC is defined in makefile.msc        */
#ifdef MSC
#define FAR far
#define MAXSEG_64K
#define NO_REVSEARCH
#define CONST const
#define ALLOCTYPE void
#ifdef _M_I86SM
#define SMALLMODEL        /* compiled in small model */
#endif
#define setbinary(fp)    setmode(fileno((fp)), O_BINARY)
#endif

                          /* Mark Williams C for Atari ST (V3.0.5) */
                          /* MWC is defined in makefile.mwc        */
#ifdef MWC
#define MSDOS
#define NOSIGNAL
#define DFLTBITS 14
#define NPROTO
/* #define MAXSEG_64K */  /* use this if your compiler has a problem with */
                          /* indexing arrays larger than 64k              */
#define NO_SETVBUF
#define NO_REVSEARCH
#define ALLOCATE(x,y)   lcalloc((unsigned long)(x),((unsigned long)(unsigned)(y)))
#define FREEIT(ptr)     free(ptr)
#define setbinary(fp)    ((fp)->_ff &= ~_FASCII)
#endif

                          /* Sozobon or Alcyon (4.14) C for Atari ST */
                          /* used with the dLibs standard library    */
#ifdef ALCYON
#define SOZOBON 1
#endif
#ifdef SOZOBON
#define MSDOS
#define NOSIGNAL
#define DFLTBITS 14
#define NPROTO
/* #define MAXSEG_64K */  /* use this if your compiler has a problem with */
                          /* indexing arrays larger than 64k              */
#define ALLOCATE(x,y)   lalloc((unsigned long)(x) * ((unsigned long)(y)))
#define FREEIT(ptr)     free(ptr)
#define setbinary(fp)    ((fp)->_flag |= _IOBIN)
#define FILTER  false
#endif


#ifdef __ZTC__                 /* Zortech compiler           */
#define setbinary(fp)    ((fp)->_flag&=~_IOTRAN)
#define NO_REVSEARCH
#define CONST const
#define MAXSEG_64K
#endif

#ifdef XENIX
#define setbinary(fp)
#define FAR far
#define CONST
#define SIGTYPE int           /* xenix defines this as pointer to int */
                              /* for the return from a signal         */
#ifdef _M_I286
#define MAXSEG_64K
#endif
#define NO_SETVBUF            /* evidently xenix chokes on the large buff*/
#endif                        /* really needs to be fine tuned           */

#ifdef MCH_AMIGA
#define CONST
#define MAXSEG_64K             /* Manx C compiler limitation */
#define NO_SETVBUF
#endif

#ifdef vms
#define NO_SETVBUF
#endif

#ifdef __TURBOC__
#define MSDOS
#define MAXSEG_64
#define NO_REVSEARCH
#ifdef __SMALL__
#define SMALLMODEL
#endif
#define CONST const
#define FAR             far
#define setbinary(fp)   setmode(fileno((fp)), O_BINARY)
#endif

#ifdef __WATCOMC__
#ifndef MSDOS
 #define MSDOS
#endif
#define NO_REVSEARCH
#define CONST const
#include <unistd.h>
#include <stdlib.h>
#if defined( __QNX__ )
 #include <utime.h>
#else
 #include <sys/utime.h>
#endif
#endif

/* FILTER  if you want the program to operate as a unix type filter */
/*  if not defined true, then issuing command without parameters will */
/*  print a usage and help information                              */
/*  Use -DFILTER=0 to deactivate filter operation                   */
#ifndef FILTER
#define FILTER  true
#endif

/* KEEPFLAG determines the default action on successful completion */
/* Unix convention is false (erase input file)                     */
/* Use -DKEEPFLAG=1 to keep files as default or change here        */
/* if you don't set it before here and you are compiling the debug */
/* version, then files will be kept.                               */

#ifndef KEEPFLAG
#ifdef NDEBUG
#define KEEPFLAG false
#else
#define KEEPFLAG true
#endif
#endif


/* Does your compiler support extended prototyping?  */
/* Uncomment the following if your compiler does not.*/
/* support full prototyping, such as:                */
/*      char *emalloc(unsigned,int);                 */
/* if defined it will use the following:             */
/*      char *emalloc();                             */
/* #define NPROTO */

/* putting the include files in one location */

#include <ctype.h>

#ifdef MINIX
#define assert(x)
extern char *index(), *rindex(), *strcat(), *strcpy(), *strncat(), *strncpy();
#else
#include <assert.h>
#include <string.h>
#endif

#ifndef NOSIGNAL
#include <signal.h>
#endif

#ifdef MWC
#include <stdlib.h>
#include <types.h>
#include <stat.h>
#else
#ifdef SOZOBON
#include <stdio.h>
#include <stat.h>
#include <limits.h>
#include <malloc.h>
#include <errno.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#endif
#endif

#ifdef M_XENIX
#include <fcntl.h>
#endif


#ifdef MSC
#include <stdlib.h>
#include <io.h>
#include <sys/utime.h>
#include <fcntl.h>
#include <limits.h>
#endif

#ifdef __TURBOC__
#include <stdlib.h>
#include <io.h>
#include <fcntl.h>
#include <limits.h>
#endif

#ifndef NOSIGNAL
#ifndef SIGTYPE
#define SIGTYPE void
#endif
#ifndef SIG_ERR
#define SIG_ERR (SIGTYPE(*)())-1
#endif
#endif

/* This is for Microsoft C v5.1 to compile and be bound  */
/* for use with os/2.  Also if compiled just for os/2    */
/* The signal function is different between protected    */
/* and real mode.                                        */

/* #define BIND */

#ifdef INCL_DOSPROCESS
#define ISOS2 true
#endif

#ifdef BIND
#define ISOS2 true
#endif

/*  The following is defined in MSC 5.1 stdlib.h
    Replace with appropriate values for your system.
    These values are used for MSDos system.
*/
#ifndef _MAX_PATH
#define _MAX_PATH  144      /* max. length of full pathname       */
#define _MAX_DRIVE   3      /* max. length of drive component     */
#define _MAX_DIR   130      /* max. length of path component      */
#define _MAX_FNAME   9      /* max. length of file name component */
#define _MAX_EXT     5      /* max. length of extension component */
#endif

/* the following tells the system that the maximum segment is 64k */
/* if your compiler is not one of these and has this limitation   */
/* Because of this, this code should compile with minimum porting */
/* in the COMPUSI.XEN module to most unix systems.                */
/* This is also used to keep array indexing to 16 bit integer     */
/* if not predefined in compiler implementation, you must define  */
/* it separately if applicable to your compiler/system            */
/* #define MAXSEG_64K                                             */

/* put this in if you are compiling in small code */
/* model and your compiler does not predefine it  */
/* this is for CPU' with 64k segment limitation.  */
/* Use this define for small code, it is used by  */
/* the header to decide on value for NEARHEAP     */
/* #define SMALLMODEL */

/* does your system use far pointers ? if you want it enabled keep this */
/* if you have segment limit and compile in larger than 13 bits         */
/* then you will have to use compact or large model if your compiler    */
/* does not support far pointer keyword.                                */

#ifndef FAR
#define FAR
#endif

/* What type does the alloc() function return, char or void? */

#ifndef ALLOCTYPE
#define ALLOCTYPE char
#endif

/* If you use compusi.dos and your computer supports the unix */
/* file links and the link code is set properly in the stat() */
/* then define the following:                                 */
/* #define USE_LINKS */

/* Does your run time library support the ANSI functions for:*/

/* reverse string set search?  strrpbrk() if so:             */
/*#define NO_REVSEARCH*//* dos module uses this function. */
/* #define NO_REVSEARCH */

/* Does your library include strrchr()? If not define this:  */
/*#define NO_STRRCHR*//* unix/xenix module uses this function*/

/* Does your library include strchr()? If not define this:   */
/*#define NO_STRCHR*//* dos module uses this function.       */

/* definition for const key word if supported */
#ifndef CONST
#define CONST
#endif


/*  And now for some typedefs */
typedef unsigned short CODE;
typedef unsigned char UCHAR;
typedef unsigned int HASH;
typedef int FLAG;

  /*
   * You can define the value of MAXBITS to be anything betweeen MINBITS
   * and MAXMAXBITS.  This is will determine the maximum memory you will
   * use and how the tables will be handled.  I recommend you just leave
   * it at MAXMAXBITS, because you can define DFLTBITS in compiling the
   * module COMPRESS.C to set the default, and you can vary the number
   * of bits at runtime by using the -b switch.
   */

   /*
   * The only reason to change MAXBITS is if you absolutely must have
   * faster performance. If you specify 14 bits, the tables will not
   * be split; at 13 bits, you can fit in the MSDOS small memory model
   * and allocate tables in near heap.
   * This value is available to other modules through the variable maxbits.
   */

#define INITBITS    9
#define MINBITS     12
#define MAXMAXBITS  16

#ifndef MAXBITS
#define MAXBITS    MAXMAXBITS
#endif

#if (MAXBITS > MAXMAXBITS)
#undef MAXBITS
#define MAXBITS    MAXMAXBITS
#endif

#if (MAXBITS < MINBITS)
#undef MAXBITS
#define MAXBITS  MINBITS
#endif

  /* You should define DFLTBITS to be the default compression code
   * bit length you desire on your system.
   * (I define mine in the compiler command line in my Makefile.LvR)
   * (I leave mine alone and keep to the maximum. DjG)
   */

#ifndef DFLTBITS
#define DFLTBITS MAXBITS
#endif
#if (DFLTBITS < MINBITS)
#undef DFLTBITS
#define DFLTBITS MINBITS
#endif
#if (DFLTBITS > MAXBITS)
#undef DFLTBITS
#define DFLTBITS MAXBITS
#endif

/* THIS IS TO COMPILE A 13 BIT MODEL IN SMALL MEMORY AND NEAR HEAP */
/* AS SET UP IT WILL WORK WITH MICROSOFT C COMPILER                */

#if (MAXBITS < 14)
#ifdef SMALLMODEL
#define NEARHEAP true
#undef FAR
#define FAR
#endif
#endif

/* correcting for different types of pointer arithmatic */
/* probably won't have to change it                     */
#define NULLPTR(type)   ((type FAR *) NULL)


/* in making this program portable the following allocation and          */
/* free functions are called, with the following parameters:             */
/*        ALLOCTYPE FAR *emalloc(unsigned int x, int y)                  */
/*                 void  efree(ALLOCTYPE FAR *ptr)                       */
/* you must define the allocation function and the free function         */
/* keep in mind that the casts must be correct for your compiler         */
/* NOTE these are the two functions to change for allocating pointers to */
/* far data space if you are not using Microsoft C v.5.1                 */
/* Consult your compiler manual and find the low level function that     */
/* returns a far pointer when compiled in the small model.               */
/* if your compiler does not support that, you will have to compile with */
/* a model that defaults to far pointers to data (compact or large model)*/
/* HERE ARE SOME SAMPLE PREDEFINED ONES                                  */

#ifdef MSC
#include <malloc.h>
#ifdef NEARHEAP
#define ALLOCATE(x,y)   malloc((size_t)((x)*(y)))
#define FREEIT(ptr)     free((ptr))
#else
#define ALLOCATE(x,y)   _fmalloc((size_t)((x)*(y)))
#define FREEIT(ptr)     _ffree((ptr))
#endif
#endif

#ifdef HUGE
#include <malloc.h>
#define ALLOCATE(x,y)   halloc((long)(x),(size_t)(y))
#define FREEIT(ptr)     hfree(ptr)
#endif

#ifdef __TURBOC__
#include <alloc.h>
#ifdef NEARHEAP
#define ALLOCATE(x,y)  malloc((unsigned int)((x)*(y)))
#define FREEIT(x)      free(x)
#else
#define ALLOCATE(x,y)  farmalloc((unsigned long)((x)*(y)))
#define FREEIT(x)    farfree(x)
#endif
#endif

/* default allocation function, in segmented addressing, must return */
/* a far pointer or compile with far pointer data as default         */
#ifndef ALLOCATE
#include <malloc.h>
#define ALLOCATE(x,y)   malloc((unsigned int)x*y)
#define FREEIT(ptr)     free((ptr))
#endif


# ifdef MAXSEG_64K
#   if  MAXBITS > 14
#       define SPLIT_HT   true
#   else
#       define SPLIT_HT   0
#   endif
# else
#   define SPLIT_HT   0
# endif

# ifdef MAXSEG_64K
#   if  MAXBITS > 15
#       define SPLIT_PFX   true
#   else
#       define SPLIT_PFX   0
#   endif
# else
#   define SPLIT_PFX   0
# endif

#ifndef BUFSIZ
#define BUFSIZ 512
#endif

#ifdef NO_SETBUF
#define NO_SETVBUF
#endif

#ifdef NO_SETVBUF
#   ifndef NO_SETBUF
#       define setvbuf(fp,buf,mode,size) setbuf((fp),(buf))
#       define ZBUFSIZE BUFSIZ
#       define XBUFSIZE BUFSIZ
#   else
#       define setvbuf(fp,buf,mode,size)
#       define ZBUFSIZE (1)
#       define XBUFSIZE (1)
#   endif
#else
#   ifdef NEARHEAP
#       define XBUFSIZE    (0xC00)
#       define ZBUFSIZE    (0x1800)
#   else
#       define XBUFSIZE    (0x3000)      /* 12k bytes */
#       define ZBUFSIZE    (0x6000)      /* 24k bytes */
#   endif
#endif

#define UNUSED      ((CODE)0)   /* Indicates hash table value unused    */
#define CLEAR       ((CODE)256) /* Code requesting table to be cleared  */
#define FIRSTFREE ((CODE)(CLEAR+1))/* First free code for token encoding */
#define MAXTOKLEN   512         /* Max chars in token; size of buffer   */
#define OK          0           /* Result codes from functions:         */
#ifdef vms
#define ERROR       0x10000004  /* General unspecified error            */
#define NORMAL      1           /* No error                             */
#define unlink(x)   remove(x)
#else
#    define NORMAL      0
#    ifdef ERROR
#        if (ERROR == NORMAL)
#            define ERROR   1   /* force redefine if (ERROR == NORMAL)  */
#        endif
#    else
#        define ERROR       1
#    endif
#endif
#define SIGNAL_ERROR -1         /*   signal function error              */
#define NOMEM       2           /*   Ran out of memory                  */
#define TOKTOOBIG   3           /*   Token longer than MAXTOKLEN chars  */
#define READERR     4           /*   I/O error on input                 */
#define WRITEERR    5           /*   I/O error on output                */
#define INFILEBAD   6           /*   Infile not in compressed format    */
#define CODEBAD     7           /*   Infile contained a bad token code  */
#define TABLEBAD    8           /*   The tables got corrupted (!)       */
#define NOSAVING    9           /*   no saving in file size             */
#define NOTOPENED  10           /*   output file couldn't be opened     */
#define YES         1
#define NO          0


#include "compress.fns"

/* defines opening mode for files */
/* and suffixes for compressed file */

#ifdef MSDOS
#define WRITE_FILE_TYPE "wb"
#define READ_FILE_TYPE "rb"
#define SUFFIX "Z"
#else
#ifdef vms
#define SUFFIX "_Z"
#define WRITE_FILE_TYPE "wb"
#define READ_FILE_TYPE "rb"
#else
#define WRITE_FILE_TYPE "w"
#define READ_FILE_TYPE "r"
#define SUFFIX ".Z"
#endif
#endif


/* The VERBOSE flag defines the default value of the verbose variable */
/* If it's not already defined, we set it to false here since most */
/* systems set the default that way.  -Dal */
#ifndef VERBOSE
#define VERBOSE false
#endif


/* Defines for third byte of header */
#define BIT_MASK    0x1f
#define BLOCK_MASK  0x80
/* Masks 0x40 and 0x20 are free.  I think 0x20 should mean that there is
   a fourth header byte (for expansion).
*/


#define CHECK_GAP 10000L     /* ratio check interval */

#ifdef MAIN
UCHAR magic_header[] = { 0x1F,0x9D };  /* 1F 9D */

char rcs_ident[] = "@(#) compress,v 4.3 88/12/26 08:00:00 don Release $";

#ifdef MWC
long _stksize = 20000L;     /* set MWC's stack to 20,000 */
#ifdef MWC_NAME           /* if defined in makefile set _cmdname for */
char _cmdname[]=MWC_NAME;   /* compress,zcat,uncomp check for desktop and */
#endif                      /* dumb shells */
#endif
#ifdef SOZOBON
long _STKSIZ = 20000L;          /* set runtime stack to 20,000 bytes */
#ifndef CMDNAME
#define CMDNAME "compress"
#endif
char _cmdname[]=CMDNAME;        /* force command name */
#endif

int overwrite = 0;          /* Do not overwrite unless given -f flag */
int maxbits = DFLTBITS;     /* user settable max # bits/code */

int exit_stat = 0;
int keep = KEEPFLAG;            /* True = don't kill file */
int keep_error = false;     /* True = keep output file even if error exist */
char *prog_name;
char ifname[_MAX_DIR];
char inpath[_MAX_DIR];
char ofname [_MAX_DIR];
char outpath[_MAX_DIR];
int is_list = false;            /* flag for file parameters */
char endchar[1];
char xbuf[XBUFSIZE];
char zbuf[ZBUFSIZE];
#ifdef MSDOS
char separator[] = "\\";
#else
char separator[] = "/";
#endif
int nomagic = false;  /* Use a 3-byte magic number header, unless old file */
int zcat_flg = false; /* Write output on stdout, suppress messages */
int quiet = !VERBOSE; /* don't tell me about compression */
/*
 * block compression parameters -- after all codes are used up,
 * and compression rate changes, start over.
 */
int block_compress = BLOCK_MASK;
#ifdef COMP40
long int ratio = 0L;
long checkpoint = CHECK_GAP;
#endif

/* force the overwrite */
int force = 0;

#ifndef NDEBUG
int verbose = VERBOSE;
int debug = false;
#endif /* !NDEBUG */

#ifndef NOSIGNAL
SIGTYPE (*bgnd_flag)();
#endif

int do_decomp = false;

#else               /* not defining instance */

extern UCHAR magic_header[];
extern char rcs_ident[];
#ifdef MWC
extern long _stksize;
#endif
extern int overwrite;
extern int maxbits;


extern int exit_stat;
extern int keep;
extern int keep_error;
extern char *prog_name;
extern char inpath[];
extern char outpath[];
extern int is_list;
extern char endchar[];
extern char xbuf[];
extern char zbuf[];
extern char ifname[];
extern char ofname[];
extern char separator[];
extern int nomagic;
extern int zcat_flg;
extern int quiet;
extern int block_compress;
#ifdef COMP40
extern long int ratio;
extern long checkpoint;
#endif
extern int force;

#ifndef NDEBUG
extern int verbose;
extern int debug;
#endif /* !NDEBUG */

#ifndef NOSIGNAL
extern SIGTYPE (*bgnd_flag)();
#endif
extern int do_decomp;
#endif
