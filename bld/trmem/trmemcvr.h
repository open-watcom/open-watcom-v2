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
* Description:  Cover routines to access the trmem memory tracker for
*               programs that have no special needs.
*
****************************************************************************/


/*

The assumptions that these routines make are:
    - memory alloction is done through malloc, free, realloc
    - error messages are printed to stderr using write
    - all trmem checks are performed (all flags are set on)
    - all memory allocation throughout these routines uses the same memory
            tracker (i.e. same memory handle)

To use these routines compile trmemcvr.c into your program with the following
compile options:  -DTRMEM -of+

If you leave off these options, trmemcvr will use malloc, free, realloc
and strdup directly.

No special options are required for routines that call the routine in trmemcvr.

*/


#ifndef _TRMEMCVR_H_INCLUDED
#define _TRMEMCVR_H_INCLUDED

#include <stddef.h>

/* open and close the tracker */
extern void     TRMemOpen( void );
extern void     TRMemRedirect( int );
extern void     TRMemClose( void );

/* change all calls to malloc, free, realloc and strdup with calls to these */
extern void *   TRMemAlloc( size_t size );
extern void     TRMemFree( void * ptr );
extern void *   TRMemRealloc( void * ptr, size_t size );
extern char *   TRMemStrdup( const char * str );


/* the rest of these functions are only available if trmemcvr was compiled */
/* with the options memtioned above */

/* print the current and peak usage to stderr */
extern void     TRMemPrtUsage( void );

/* print a list of the currently used blocks */
extern unsigned TRMemPrtList( void );

/* check that ptr is valid */
extern int      TRMemValidate( void * ptr );

/* check that the heap is valid */
extern int      TRMemValidateAll( void );

/* check that the len locations starting at start are properly allocated */
extern int      TRMemChkRange( void * start, size_t len );

#endif
