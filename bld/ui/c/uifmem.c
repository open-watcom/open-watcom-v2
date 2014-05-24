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
* Description:  Allocate and free far memory.
*
****************************************************************************/


#include "uidef.h"
#if defined( HAVE_FAR ) && defined( __386__ )
  #include <i86.h>
#endif

void __FAR * global uifaralloc( size_t size )
/*******************************************/
{
    void        *ptr;

    ptr = uimalloc( size );
    if( ptr != NULL ) {
        /* convert ptr to far if necessary: use DS for segment value */
        return( ptr );
    }
    /* use 0 for segment value */
    return( NULL );
}


void global uifarfree( void __FAR *ptr )
/**************************************/
{
#if defined( HAVE_FAR ) && defined( __386__ )
    /* On extended DOS, we are throwing away the segment part. However,
     * since the memory should have been allocated by uifaralloc() above,
     * this is safe because we synthesized the segment part to begin with.
     */
    uifree( (void *)FP_OFF( ptr ) );
#else
    uifree( (void *)ptr );
#endif
}
