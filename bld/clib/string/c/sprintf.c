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


#include "variety.h"
#include "widechar.h"
#include <stdio.h>
#include <stdarg.h>
#include <limits.h>
#include "printf.h"


//#ifdef __WIDECHAR__
//_WCRTLINK int _usprintf( wchar_t *dest, const wchar_t *format, ... )
//{
//    va_list             args;
//
//    va_start( args, format );
//    return( vswprintf( dest, INT_MAX, format, args ) );
//}
//#endif


#ifdef __WIDECHAR__
_WCRTLINK int swprintf( CHAR_TYPE *dest, size_t n, const CHAR_TYPE *format, ... )
{
    auto    va_list         args;

    va_start( args, format );
    return( vswprintf( dest, n, format, args ) );
}
#endif

_WCRTLINK int __F_NAME(sprintf,_swprintf) ( CHAR_TYPE *dest, const CHAR_TYPE *format, ... )
{
    auto    va_list         args;

    va_start( args, format );
    #ifdef __WIDECHAR__
        return( _vswprintf( dest, format, args ) );
    #else
        return( vsprintf( dest, format, args ) );
    #endif
}
