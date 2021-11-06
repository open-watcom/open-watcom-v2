/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Implementation of _rotl().
*
****************************************************************************/


#undef __INLINE_FUNCTIONS__
#include "variety.h"
#include <stdlib.h>
#include <limits.h>


#if defined( _M_IX86 )
extern unsigned int __rotl( unsigned int value, unsigned int shift );
#if defined(_M_I86)
#pragma aux __rotl = \
        "rol ax,cl" \
    __parm      [__ax] [__cx] \
    __value     [__ax] \
    __modify    [__cx]
#else
#pragma aux __rotl = \
        "rol eax,cl" \
    __parm      [__eax] [__ecx] \
    __value     [__eax] \
    __modify    [__ecx]
#endif
#endif

_WCRTLINK unsigned int _rotl( unsigned int value, unsigned int shift )
{
#if defined( _M_IX86 )
    return( __rotl( value, shift ) );
#else
    unsigned int    tmp;

    tmp = value;
    value = value << shift;
    tmp = tmp >> ((sizeof( tmp ) * CHAR_BIT) - shift);
    value = value | tmp;
    return( value );
#endif
}
