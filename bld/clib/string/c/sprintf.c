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
* Description:  Implementation of sprintf() - formatted output to string.
*
****************************************************************************/


#include "variety.h"
#include "widechar.h"
#include <stdio.h>
#include <stdarg.h>
#include <limits.h>


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
    va_list         args;
    int             ret;

    va_start( args, format );
    ret = vswprintf( dest, n, format, args );
    va_end( args );
    return( ret );
}
#endif

_WCRTLINK int __F_NAME(sprintf,_swprintf) ( CHAR_TYPE *dest, const CHAR_TYPE *format, ... )
{
    va_list         args;
    int             ret;

    va_start( args, format );
#ifdef __WIDECHAR__
    ret = _vswprintf( dest, format, args );
#else
    ret = vsprintf( dest, format, args );
#endif
    va_end( args );
    return( ret );
}
