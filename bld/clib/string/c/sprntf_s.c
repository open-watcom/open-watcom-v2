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
* Description:  Implementation of sprintf_s() - safe formatted string output.
*
****************************************************************************/


#include "variety.h"
#include "widechar.h"
#include "saferlib.h"
#include <stdio.h>
#include <stdarg.h>
#include "libwchar.h"
#include "printf.h"


/*
 * mem_putc -- append a character to a string in memory, with overflow check
 */

typedef struct vsprtf_s_buf {
    CHAR_TYPE   *bufptr;
    rsize_t     chars_output;
    rsize_t     max_chars;
} vsprtf_s_buf;

static prtf_callback_t mem_putc; // set up calling convention
static void PRTF_CALLBACK mem_putc( PTR_SPECS specs, PRTF_CHAR_TYPE op_char )
{
    vsprtf_s_buf    *info;

    info = GET_SPECS_DEST( vsprtf_s_buf, specs );
    if( info->chars_output <= info->max_chars ) {
        *( info->bufptr++ ) = op_char;
        specs->_output_count++;
        info->chars_output++;
    }
}

_WCRTLINK int __F_NAME(sprintf_s,swprintf_s)( CHAR_TYPE * __restrict s, rsize_t n,
                                        const CHAR_TYPE * __restrict format, ... )
{
    va_list         args;
    vsprtf_s_buf    info;
    const char      *msg;
    int             rc = 0;

    /* First check the critical conditions; if any of those
     * is violated, return immediately and don't touch anything.
     */
    if( __check_constraint_nullptr_msg( msg, s )
     && __check_constraint_maxsize_msg( msg, n )
     && __check_constraint_zero_msg( msg, n ) ) {

        /* The buffer looks okay, try doing something useful */
        if( __check_constraint_nullptr_msg( msg, format ) ) {
            info.bufptr       = s;
            info.chars_output = 0;
            info.max_chars    = n - 1;
            msg = NULL;

            va_start( args, format );
            __F_NAME(__prtf_s,__wprtf_s)( &info, format, args, &msg, mem_putc );
            va_end( args );

            if( msg == NULL ) {
                if( info.chars_output <= info.max_chars ) {
                    s[info.chars_output] = NULLCHAR;
                    return( info.chars_output );
                }
                /* If we got here, the output buffer was too small */
                msg = "n < chars_output";
#ifdef __WIDECHAR__
                rc = -1;    /* Return value for swprintf_s is different! */
#endif
            }
        }
        /* Something went wrong, output buffer will contain empty string */
        *s = NULLCHAR;
    }
    __rtct_fail( __func__, msg, NULL );
    return( rc );
}
