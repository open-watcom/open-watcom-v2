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
    if TRMEM is defined, trmem functions are used which will help tracking
    memory usage.
*/

#include <stdlib.h>
#include <malloc.h>

#ifdef _WASM_

#include "fatal.h"
#ifdef TRMEM
#include "memutil.h"
#endif

extern void     Fatal( unsigned, ... );

extern void     heap( char * );

void *AsmRealloc( void *ptr, size_t size )
{
    void        *new;

#ifdef TRMEM
    new = MemRealloc( ptr, size );
#else
    new = realloc( ptr, size );
    if( ptr == NULL ) {
        Fatal( MSG_OUT_OF_MEMORY );
    }
#endif
    return new;
}
#endif

void *AsmAlloc( size_t amount )
{
#ifdef _WASM_
    void        *ptr;

#ifdef TRMEM
    ptr = MemAlloc( amount );
#else
    ptr = malloc( amount );
    if( ptr == NULL ) {
        Fatal( MSG_OUT_OF_MEMORY );
    }
#endif
    return( ptr );
#else
    return( malloc( amount ) );
#endif
}

void AsmFree( void *ptr )
{
    if( ptr == NULL ) return;
#ifdef TRMEM
    MemFree( ptr );
#else
    free( ptr );
#endif
}
