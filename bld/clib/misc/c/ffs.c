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
* Description:  Implementation of ffs().
*
****************************************************************************/


#include "variety.h"
#include <strings.h>

#if defined(__386__)

/* Note that we can get away with 'add al,cl' because we know that
 * eax will always be a lot less than 256.
 */
extern unsigned int __ffs( int value );
#pragma aux __ffs =     \
    "bsf    eax,edx"    \
    "setne  cl"         \
    "add    al,cl"      \
    parm [edx] value [eax] modify [ecx];

#endif

_WCRTLINK int ffs( int value )
{
#if defined(__386__) && defined(__WATCOMC__)
    return( __ffs( value ) );
#else
    int     index;

    if( value == 0 ) {
        return( 0 );
    }

    index = 1;
    /* We know this loop will exit because at least one bit is non-zero */
    while( (value & 1) == 0 ) {
        ++index;
        value >>= 1;
    }
    return( index );
#endif
}
