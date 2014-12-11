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
* Description:  Implementation of vsprintf() - formatted string output.
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
#include <stdio.h>
#include <stdarg.h>
#include <limits.h>
#include "printf.h"


/*
 * mem_putc -- append a character to a string in memory
 */
#ifdef __WIDECHAR__

struct vswprtf_buf {
    CHAR_TYPE   *bufptr;
    int         chars_output;
    int         max_chars;
};

static slib_callback_t mem_putc; // setup calling convention
static void __SLIB_CALLBACK mem_putc( SPECS __SLIB *specs, int op_char )
{
    struct vswprtf_buf  *info;

    info = (struct vswprtf_buf*) specs->_dest;
    if( info->chars_output + 1 <= info->max_chars ) {
        *( info->bufptr++ ) = op_char;
        specs->_output_count++;
        info->chars_output++;
    }
}

#else

static slib_callback_t mem_putc; // setup calling convention
static void __SLIB_CALLBACK mem_putc( SPECS __SLIB *specs, int op_char )
{
    *( specs->_dest++ ) = op_char;
    specs->_output_count++;
}

#endif


#ifdef __WIDECHAR__
_WCRTLINK int vswprintf( CHAR_TYPE *dest, size_t n, const CHAR_TYPE *format, va_list arg )
{
    auto struct vswprtf_buf info;

    if( n != 0 ) {
        info.bufptr = dest;
        info.chars_output = 0;
        info.max_chars = n - 1;
        __wprtf( &info, format, arg, mem_putc );
        dest[info.chars_output] = NULLCHAR;
    }
    return( info.chars_output );
}
#endif

_WCRTLINK int __F_NAME(vsprintf,_vswprintf) ( CHAR_TYPE *dest, const CHAR_TYPE *format, va_list arg )
{
#ifndef __WIDECHAR__
    register int            len;
#else
    auto struct vswprtf_buf info;
#endif

#ifdef __WIDECHAR__
    info.bufptr = dest;
    info.chars_output = 0;
    info.max_chars = INT_MAX;
    __wprtf( &info, format, arg, mem_putc );
    dest[info.chars_output] = NULLCHAR;
    return( info.chars_output );
#else
    len = __prtf( dest, format, arg, mem_putc );
    dest[len] = NULLCHAR;
    return( len );
#endif
}
