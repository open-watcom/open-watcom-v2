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


/*
   CP.H - cp header file

   Date         By              Reason
   ====         ==              ======
   17-aug-90    Craig Eisler    defined
   19-oct-91    Craig Eisler    more work
   11-nov-91    Craig Eisler    cleaned up
   28-jan-92    Craig Eisler    display time
   25-mar-92    Craig Eisler    NT port
   18-jun-92    Greg Bentz      OS/2 2.0 port
   20-jan-92    Craig Eisler    added rxflag
 */
#ifndef _CP_INCLUDED
#define _CP_INCLUDED

#if defined(__OS_nt__) || defined(__OS_os2v2__) || defined(__OS_alpha__)
#define __FAR
#else
#define __FAR   far
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
    unsigned char in_memory:1;
    unsigned char in_xmemory:1;
} mem_block;

typedef struct ctrl_block {
    struct ctrl_block *next,*prev;
    char *inname,*outname;
    long bytes_pending;
    int  inhandle,outhandle;
    mem_block *head,*curr;
    unsigned short t,d;
    char srcattr;
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
extern int rflag,iflag,npflag,aflag,fflag,tflag,Tflag,dflag,Dflag,sflag,xflag;
extern int todflag,pattrflag,rxflag;

/* routines */
void MemInit( void );
void DoCP( char *, char * );
void CopyOneFile( char *, char *);
int GrabFile( char *, void *, char *, char );
void FlushMemoryBlocks( void );
void __FAR *FarAlloc( unsigned );
void *NearAlloc( unsigned );
void FarFree( void __FAR *ptr );
void NearFree( void *ptr );
void MemFini( void );
void IOError( int );
#if defined(__OS_os2v2__)
void OS2Error( int );
#endif

#endif
