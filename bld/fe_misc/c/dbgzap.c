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


// DBGZAP -- memory zap functions for debugging
//
//
// Note: must be compiled with -dheader="name" where "name" is a header file
//       containing definitions for DbgVerify
//
// 96/04/15 -- J.W.Welch        - swiped from C++ project

#include <string.h>

#include header
#include "dbgzap.h"


unsigned char DbgZapChars[2] = { 0xa0, 0xf0 };   // zap characters

#define NEXT_ZAP( index, v ) \
        { \
            unsigned char __t; \
            v = DbgZapChars[ index ]; \
            __t = v + 1; \
            v &= 0xf0; \
            v |= __t & 0x0f; \
            DbgZapChars[ index ] = v; \
        }



void* DbgZapMem(                // ZAP MEMORY
    void* tgt,                  // - target
    int zap_chr,                // - zap character
    unsigned size )             // - size
{
    return memset( tgt, zap_chr, size );
}


void* DbgZapAlloc(              // ZAP ALLOCATED MEMORY
    void* tgt,                  // - target
    unsigned size )             // - size
{
    unsigned char   z;

    NEXT_ZAP( 0, z );
    return DbgZapMem( tgt, z, size );
}


void* DbgZapFreed(              // ZAP FREED MEMORY
    void* tgt,                  // - target
    unsigned size )             // - size
{
    unsigned char   z;

    NEXT_ZAP( 1, z );
    return DbgZapMem( tgt, z, size );
}

int DbgZapQuery(                // QUERY WHETHER MEMORY IS STILL ZAPPED
    void *tgt,                  // - target
    unsigned size )             // - size
{
    unsigned char *c = tgt;
    unsigned char *p = c + ( size - 1 );
    unsigned char *s = c + sizeof( void * );

    while( p > s ) {
        if( *p != *s ) {
            return( 0 );
        }
        --p;
    }
    return( 1 );
}

void DbgZapVerify(              // VERIFY MEMORY IS STILL ZAPPED
    void *tgt,                  // - target
    unsigned size )             // - size
{
    DbgAssert( DbgZapQuery( tgt, size ) != 0 );
}
