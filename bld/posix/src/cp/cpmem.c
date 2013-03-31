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
* Description:  cp memory handler.
*
****************************************************************************/


#include <stdio.h>
#include <stddef.h>
#include <malloc.h>
#include <dos.h>
#include "cp.h"

long near_allocs,far_allocs;
/*
 * MemInit - initialize memory
 */
void MemInit( void )
{
#if defined( _M_I86 )
    _nheapgrow();
#endif
    near_allocs=0L;
    far_allocs=0L;

} /* MemInit */


/*
 * MemFini - memory cleanup
 */
void MemFini( void )
{
    if( near_allocs != 0L ) {
        printf("\n%ld unfreed near blocks!\n", near_allocs );
    }
    if( far_allocs != 0L ) {
        printf("\n%ld unfreed far blocks!\n", far_allocs );
    }

} /* MemFini */

#if !defined( _M_I86 )
/*
 * FarAlloc - allocate from far heap
 */
void *FarAlloc( size_t size )
{
    void        *ptr;

    ptr = malloc( size );
    if( ptr == NULL ) {
        return( NULL );
    }

    far_allocs++;
    return( ptr );
} /* FarAlloc */
/*
 * FarFree - free some far memory
 */
void FarFree( void *ptr )
{
    free( ptr );
    far_allocs--;

} /* FarFree */
#else
/*
 * FarAlloc - allocate from far heap
 */
void far *FarAlloc( size_t size )
{
    void        far *tmp;

    tmp = _fmalloc( size );
    if( tmp == (void far *) NULL ) {
        return( (void far *) NULL );
    }

    if( FP_SEG( tmp ) == FP_SEG( (void far *) &tmp ) ) {
        _ffree( tmp );
        return( (void far *) NULL );
    }

    far_allocs++;
    return( tmp );

} /* FarAlloc */

/*
 * FarFree - free some far memory
 */
void FarFree( void far *ptr )
{
    _ffree( ptr );
    far_allocs--;

} /* FarFree */
#endif

/*
 * NearAlloc - allocation of memory; flush mem if we run out
 */
void *NearAlloc( size_t size )
{
    void        *tmp;

    tmp = malloc( size );
    if( tmp == NULL ) {
        FlushMemoryBlocks();
        tmp = malloc( size );
        if( tmp == NULL ) {
            Die( OutOfMemory );
        }
    }
    near_allocs++;
    return( tmp );

} /* NearAlloc */

/*
 * NearFree - release near memory
 */
void NearFree( void *ptr )
{
    free( ptr );
    near_allocs--;

} /* NearFree */
