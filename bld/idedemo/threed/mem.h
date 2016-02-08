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


#ifndef MEM_H
#define MEM_H
/*
 Description:
 ============
    This is the external include file for the local mmemory manager.
    It provides simple alloc/free mechanisms in WINDOWS DLLs.

*/

#include "mytypes.h"
#include "wpitypes.h"

extern void near * lmem_alloc( unsigned int );
extern void lmem_free( void * );
extern void near * lmem_realloc( void *, unsigned int );

extern void * gmem_alloc( unsigned int );
extern void gmem_free( void * );
extern void * gmem_realloc( void *, unsigned int );

#ifdef TRACK_MEM

/* functions to open and close the tracker */
extern void         tr_mem_open( WPI_INST, char *rpt_title );
extern void         tr_mem_close( void);

/* alloc, free, and realloc functions */
extern void near *  tr_lmem_alloc( unsigned int );
extern void         tr_lmem_free( void * );
extern void near *  tr_lmem_realloc( void *, unsigned int );
extern void *       tr_gmem_alloc( unsigned int );
extern void         tr_gmem_free( void * );
extern void *       tr_gmem_realloc( void *, unsigned int );

/* debugging routines */
extern void         tr_mem_report();
extern void         tr_mem_check( void );

#define _new( ptr, size ) ( ptr = tr_lmem_alloc( sizeof( *(ptr) ) * (size) ) )
#define _renew( ptr, size ) \
        ( ptr = tr_lmem_realloc( ptr, sizeof( *(ptr) ) * (size) ) )
#define _free( x ) tr_lmem_free( x )
#define _alloc( size ) tr_lmem_alloc( size )
#define _realloc( size ) tr_lmem_realloc( size )
#define _free_fn tr_lmem_free
#define _alloc_fn tr_lmem_alloc
#define _realloc_fn tr_lmem_realloc

#define _gnew( ptr, size ) ( ptr = tr_gmem_alloc( sizeof( *(ptr) ) * (size) ) )
#define _grenew( ptr, size ) \
        ( ptr = tr_gmem_realloc( ptr, sizeof( *(ptr) ) * (size) ) )
#define _gfree( x ) tr_gmem_free( x )
#define _galloc( size ) tr_gmem_alloc( size )
#define _grealloc( ptr, size ) tr_gmem_realloc( ptr, size )
#define _gfree_fn tr_gmem_free
#define _galloc_fn tr_gmem_alloc
#define _grealloc_fn tr_gmem_realloc

#else  // !TRACK_MEM

#define tr_mem_open( inst, title )
#define tr_mem_close()

#define _new( ptr, size ) ( ptr = lmem_alloc( sizeof( *(ptr) ) * (size) ) )
#define _renew( ptr, size ) \
        ( ptr = lmem_realloc( ptr, sizeof( *(ptr) ) * (size) ) )
#define _free( x ) lmem_free( x )
#define _alloc( size ) lmem_alloc( size )
#define _realloc( size ) lmem_realloc( size )
#define _free_fn lmem_free
#define _alloc_fn lmem_alloc
#define _realloc_fn lmem_realloc


#define _gnew( ptr, size ) ( ptr = gmem_alloc( sizeof( *(ptr) ) * (size) ) )
#define _grenew( ptr, size ) \
        ( ptr = gmem_realloc( ptr, sizeof( *(ptr) ) * (size) ) )
#define _gfree( x ) gmem_free( x )
#define _galloc( size ) gmem_alloc( size )
#define _grealloc( ptr, size ) gmem_realloc( ptr, size )
#define _gfree_fn gmem_free
#define _galloc_fn gmem_alloc
#define _grealloc_fn gmem_realloc

#endif  // !TRACK_MEM

#define _free_check( x ) if( (x) != NULL ) _free( (x) )
#define _gfree_check( x ) if( (x) != NULL ) _gfree( (x) )

#endif
