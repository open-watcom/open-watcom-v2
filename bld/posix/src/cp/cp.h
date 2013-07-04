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
* Description:  cp header file.
*
****************************************************************************/


#ifndef _CP_INCLUDED
#define _CP_INCLUDED

#include <sys/stat.h>

#ifdef _M_I86
#define __FAR   far
#else
#define __FAR
#if !defined(__WATCOMC__) && ( defined(__NT__) )
#include <windows.h>
#endif
#endif

#include "lineprt.h"
#include "misc.h"

typedef struct {
unsigned   yy;
unsigned   dd;
unsigned   mm;
unsigned   hr;
unsigned   min;
unsigned   sec;
} timedate;

#define TRUE    1
#define FALSE   0

/* constants */
#define MAXIOBUFFER     512*127

typedef struct dd {
  char *dir;
  char sdir[13];
  struct dd *next;
} dirs;

typedef struct mem_block {
    struct mem_block *next;
    union {
            char __FAR *buffer;
            long xmemaddr;
    } where;
    unsigned buffsize;
    unsigned in_memory:1;
    unsigned in_xmemory:1;
} mem_block;

typedef struct ctrl_block {
    struct ctrl_block *next,*prev;
    char *inname,*outname;
    long bytes_pending;
#if !defined(__WATCOMC__) && ( defined(__NT__) )
    HANDLE  inhandle,outhandle;
#else
    int  inhandle,outhandle;
#endif
    mem_block *head,*curr;
    unsigned short t,d;
    unsigned srcattr;
} ctrl_block;

typedef struct file_list {
   struct file_list __FAR *next;
   char name[13];
} file_list;

/* global data */
extern unsigned long StartTime, TotalTime;
extern int ErrorNumber;
extern unsigned DumpCnt,FileCnt,TotalFiles,DirCnt,TotalDirs;
extern long TotalBytes;
extern ctrl_block *CBHead,*CBTail;
extern char *CtrlBlocks;
extern char __FAR *Buffer;
extern char __FAR *FileList;
extern char OutOfMemory[],InvalidTDStr[];
extern timedate before_t_d,after_t_d;
extern int rflag,iflag,npflag,aflag,fflag,tflag2,Tflag1,dflag2,Dflag1,sflag,xflag;
extern int todflag,pattrflag,rxflag;

/* routines */
void MemInit( void );
void DoCP( char *, char * );
void CopyOneFile( char *, char *);
int GrabFile( char *, struct stat *, char *, unsigned );
void FlushMemoryBlocks( void );
void __FAR *FarAlloc( size_t );
void *NearAlloc( size_t );
void FarFree( void __FAR *ptr );
void NearFree( void *ptr );
void MemFini( void );
void IOError( int );
#if defined( __OS2__ ) && defined( __386__ )
void OS2Error( int );
#endif

#endif
