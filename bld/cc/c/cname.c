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
* Description:  Compare two names.
*
****************************************************************************/


#include "cvars.h"

// Set of functions for processing Names 4-bytes at a time for efficiency
// especially on RISC machines (particularly Alpha since it does not have
// byte access instructions).

static unsigned_32 DwordMasks[] = {
#ifdef __BIG_ENDIAN__
    0x00000000,
    0xFF000000,
    0xFFFF0000,
    0xFFFFFF00,
    0xFFFFFFFF
#else
    0x00000000,
    0x000000FF,
    0x0000FFFF,
    0x00FFFFFF,
    0xFFFFFFFF
#endif
};


// len includes the trailing nullchar

int NameCmp( const void *p1, const void *p2, int len )
{
    const unsigned_32 *p = p1;
    const unsigned_32 *q = p2;

    while( len > sizeof(unsigned_32) ) {
        if( *p != *q )  return( -1 );   // indicate names not equal
        ++p;
        ++q;
        len -= sizeof(unsigned_32);
    }
    return( ((*p ^ *q) & DwordMasks[ len ]) != 0 );
}
