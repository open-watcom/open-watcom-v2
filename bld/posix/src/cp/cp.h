/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  cp header file.
*
****************************************************************************/


#ifndef _CP_INCLUDED
#define _CP_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <direct.h>
#ifdef __NT__
    #include <windows.h>
#elif defined( __OS2__ )
    #define  INCL_DOSFILEMGR
    #define  INCL_DOSERRORS
    #define  INCL_DOSMISC
    #include <os2.h>
    #include <dos.h>
#elif defined( __DOS__ )
    #include <dos.h>
#endif
#include "bool.h"
#include "lineprt.h"
#include "misc.h"

#ifdef _M_I86
#define __FAR   __far
#else
#define __FAR
#endif

typedef struct {
    unsigned   yy;
    unsigned   dd;
    unsigned   mm;
    unsigned   hr;
    unsigned   min;
    unsigned   sec;
} timedate;

#ifdef __NT__
typedef DWORD           fattrs;
typedef HANDLE          fhandle;
typedef DWORD           fsize;
typedef WORD            fdt;
#else
typedef unsigned        fattrs;
typedef int             fhandle;
typedef unsigned        fsize;
typedef unsigned short  fdt;
#endif

/* constants */
#define MAXIOBUFFER     (512 * 127)

typedef struct dd {
  char          *dir;
  char          sdir[13];
  struct dd     *next;
} dirs;

typedef struct mem_block {
    struct mem_block    *next;
    union {
        char __FAR      *buffer;
        long            xmemaddr;
    } where;
    fsize               buffsize;
    unsigned            in_memory   :1;
    unsigned            in_xmemory  :1;
} mem_block;

typedef struct ctrl_block {
    struct ctrl_block   *next;
    struct ctrl_block   *prev;
    char                *inname;
    char                *outname;
    long                bytes_pending;
    fhandle             inhandle;
    fhandle             outhandle;
    mem_block           *head;
    mem_block           *curr;
    fdt                 t;
    fdt                 d;
    fattrs              srcattr;
} ctrl_block;

typedef struct file_list {
   struct file_list __FAR   *next;
   char                     name[13];
} file_list;

/* global data */
extern unsigned long    TotalTime;
extern clock_t          StartTime;
extern int              ErrorNumber;
extern unsigned         DumpCnt;
extern unsigned         FileCnt;
extern unsigned         TotalFiles;
extern unsigned         DirCnt;
extern unsigned         TotalDirs;
extern long             TotalBytes;
extern ctrl_block       *CBHead;
extern ctrl_block       *CBTail;
extern char             *CtrlBlocks;
extern char __FAR       *Buffer;
extern char __FAR       *FileList;
extern char             OutOfMemory[];
extern char             InvalidTDStr[];
extern timedate         before_t_d;
extern timedate         after_t_d;
extern bool             rflag;
extern bool             iflag;
extern bool             npflag;
extern bool             aflag;
extern bool             fflag;
extern bool             tflag2;
extern bool             Tflag1;
extern bool             dflag2;
extern bool             Dflag1;
extern bool             sflag;
extern bool             xflag;
extern bool             todflag;
extern bool             pattrflag;
extern bool             rxflag;

/* routines */
extern void         MemInit( void );
extern void         DoCP( char *, char * );
extern void         CopyOneFile( char *, char *);
extern int          GrabFile( char *, struct stat *, char *, fattrs );
extern void         FlushMemoryBlocks( void );
extern void __FAR   *FarAlloc( size_t );
extern void         *NearAlloc( size_t );
extern void         FarFree( void __FAR *ptr );
extern void         NearFree( void *ptr );
extern void         MemFini( void );
extern void         IOError( int );
#if defined( __OS2__ ) && defined( __386__ )
extern void         OS2Error( int );
#endif

#endif
