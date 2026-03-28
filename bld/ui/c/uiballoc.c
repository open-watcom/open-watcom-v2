/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2026 The Open Watcom Contributors. All Rights Reserved.
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


#include <stdio.h>
#if defined( _M_IX86 )
    #include <i86.h>
  #if defined( _M_I86 ) && ( defined( __SMALL__ ) || defined( __MEDIUM__ ) )
    #include <malloc.h>
  #endif
#endif
#include "uidef.h"
#include "uifar.h"
#include "memfuncs.h"

#include "clibext.h"


void intern bframe( BUFFER *bptr )
/********************************/
{
    bptr->origin += bptr->increment + 1;
}


void intern bunframe( BUFFER *bptr )
/**********************************/
{
    /* this must be called before bfree for all framed buffers */

    bptr->origin -= bptr->increment + 1;
}


bool intern balloc( BUFFER *bptr, uisize height, uisize width )
/*************************************************************/
{
    LP_PIXEL    ptr;

    bptr->increment = width;
#if defined( __DOS__ ) && !defined( _M_I86 )
    ptr = MemAlloc( sizeof( *ptr ) * height * width );
    if( ptr != NULL ) {
        /* convert ptr to far if necessary: use DS for segment value */
        bptr->origin = ptr;
        return( true );
    }
    /* use 0 for segment value */
    bptr->origin = NULL;
    return( false );
#elif defined( _M_I86 ) && ( defined( __SMALL__ ) || defined( __MEDIUM__ ) )
    ptr = _fmalloc( sizeof( *ptr ) * height * width );
    bptr->origin = ptr;
    return( ptr != NULL );
#else
    ptr = MemAlloc( sizeof( *ptr ) * height * width );
    bptr->origin = ptr;
    return( ptr != NULL );
#endif
}


void intern bfree( BUFFER *bptr )
/*******************************/
{
#if defined( __DOS__ ) && !defined( _M_I86 )
    /* On extended DOS, we are throwing away the segment part. However,
     * since the memory should have been allocated by balloc() above,
     * this is safe because we synthesized the segment part to begin with.
     */
    MemFree( (void *)_FP_OFF( bptr->origin ) );
#elif defined( _M_I86 ) && ( defined( __SMALL__ ) || defined( __MEDIUM__ ) )
    _ffree( bptr->origin );
#else
    MemFree( (void *)bptr->origin );
#endif
}
