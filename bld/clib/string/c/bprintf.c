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
#include "printf.h"

struct  buf_limit {
        CHAR_TYPE       *bufptr;
        size_t          bufsize;
};

/*
 * buf_putc -- append a character to a string in memory
 */
static slib_callback_t buf_putc; // setup calling convention
static void __SLIB_CALLBACK buf_putc( SPECS __SLIB *specs, int op_char )
    {
        struct buf_limit *bufinfo;

        bufinfo = (struct buf_limit *)specs->_o._dest;
        if( specs->_o._output_count < bufinfo->bufsize ) {
            *( bufinfo->bufptr++ ) = op_char;
            specs->_o._output_count++;
        }
    }


_WCRTLINK int __F_NAME(_vbprintf,_vbwprintf) ( CHAR_TYPE *s, size_t bufsize,
                const CHAR_TYPE *format, va_list arg)
    {
        register int len;
        auto    struct buf_limit bufinfo;
        slib_callback_t *tmp;

        bufinfo.bufptr  = s;
        bufinfo.bufsize = bufsize - 1;
#if defined(__386__) && defined(__QNX__)
        /* avoid some segment relocations for 32-bit QNX */
        tmp = (void (*)())buf_putc;
#else
        tmp = buf_putc;
#endif
        len = __F_NAME(__prtf,__wprtf)( &bufinfo, format, arg, tmp );
        s[len] = '\0';
        return( len );
    }


_WCRTLINK int __F_NAME(_bprintf,_bwprintf) ( CHAR_TYPE *dest, size_t bufsize,
                const CHAR_TYPE *format, ... )
    {
        auto    va_list         args;

        va_start( args, format );
        return( __F_NAME(_vbprintf,_vbwprintf)( dest, bufsize, format, args ) );
    }
