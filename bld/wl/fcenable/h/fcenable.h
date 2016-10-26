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


/* definitions used throughout fcenable */
#include "watcom.h"
#include "bool.h"

#define MAX_OBJECT_REC_SIZE 4096

typedef unsigned char   byte;
typedef unsigned short  ushort;

// these are the results returned from ReadRec.

enum {
    ERROR = -1,
    OK = 0,
    ENDFILE,
    ENDMODULE,
    LIBRARY,
    OBJECT,
    ENDLIBRARY
};

typedef struct name_list    NAME_LIST;
typedef struct exclude_list EXCLUDE_LIST;

typedef struct name_list {
    NAME_LIST * next;
    unsigned    lnameidx;       // index of lname record which equals name
    char        name[1];
} name_list;

typedef struct exclude_list {
    EXCLUDE_LIST *  next;
    unsigned        lnameidx;   // index of segment lname record
    unsigned        segidx;     // index of segment
    unsigned long   start_off;  // starting offset
    unsigned long   end_off;    // ending offset;
    char            name[1];
} exclude_list;

extern int          PageLen;
extern int          InFile;
extern int          OutFile;
extern name_list    *ClassList;
extern name_list    *SegList;
extern exclude_list *ExcludeList;

// fcenable.c
extern int      CopyFile( char *, char * );
extern void     put( const char * );
extern void     putlen( const char *, unsigned );
extern void     LinkList( void **, void * );
extern void     FreeList( void * );
extern void     Warning( char * );
extern void     Error( char * );
extern void     IOError( char * );
extern int      QRead( int, void *, int );
extern int      QWrite( int, void *, int );
extern long     QSeek( int, long, int );

// mem.c
extern void     MemInit( void );
extern void     MemFini( void );
extern void     *MemAlloc( size_t );
extern void     MemFree( void * );

// records.c
extern void     *InitRecStuff( void );
extern void     FileCleanup( void );
extern void     CleanRecStuff( void );
extern void     FinalCleanup( void );
extern void     ProcessRec( void );
extern unsigned GetIndex( byte ** );
extern int      ReadRec( void );
extern void     BuildRecord( void *, unsigned );
extern void     FlushBuffer( void );
extern void     IndexRecord( unsigned );
