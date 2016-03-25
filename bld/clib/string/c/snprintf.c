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
* Description:  Implementation of snprintf().
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
#include <stdio.h>
#include <stdarg.h>
#include "printf.h"

struct buf_limit {
    CHAR_TYPE   *bufptr;
    size_t      bufsize;
};

/*
 * buf_putc -- append a character to a string in memory
 */
static slib_callback_t buf_putc; // setup calling convention
static void __SLIB_CALLBACK buf_putc( SPECS __SLIB *specs, OUTC_PARM op_char )
{
    struct buf_limit    *bufinfo;

    bufinfo = (struct buf_limit *)specs->_dest;
    if( specs->_output_count < bufinfo->bufsize ) {
        *( bufinfo->bufptr++ ) = op_char;
    }
    specs->_output_count++;
}

/*
 * buf_count_putc -- only count characters to be output
 */
static slib_callback_t buf_count_putc; // setup calling convention
static void __SLIB_CALLBACK buf_count_putc( SPECS __SLIB *specs, OUTC_PARM op_char )
{
    specs->_output_count++;
}


_WCRTLINK int __F_NAME(vsnprintf,vsnwprintf)( CHAR_TYPE *s, size_t bufsize,
                                              const CHAR_TYPE *format, va_list arg )
{
    int                 len;
    struct buf_limit    bufinfo;

    bufinfo.bufptr  = s;
    bufinfo.bufsize = bufsize - 1;
    if( bufsize == 0 )
        len = __F_NAME(__prtf,__wprtf)( &bufinfo, format, arg, buf_count_putc );
    else {
        len = __F_NAME(__prtf,__wprtf)( &bufinfo, format, arg, buf_putc );
        s[(( len >= 0 ) && ( len < bufsize )) ? len : bufsize - 1] = NULLCHAR;
    }
    return( len );
}


_WCRTLINK int __F_NAME(snprintf,snwprintf)( CHAR_TYPE *dest, size_t bufsize,
                                            const CHAR_TYPE *format, ... )
{
    va_list         args;

    va_start( args, format );
    return( __F_NAME(vsnprintf,vsnwprintf)( dest, bufsize, format, args ) );
}
