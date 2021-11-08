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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "variety.h"
#include "widechar.h"
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <stdarg.h>
#include "printf.h"

/*
 * con_putc -- output character to console
 */
static prtf_callback_t con_putc; // setup calling convention
static void PRTF_CALLBACK con_putc( PTR_SPECS specs, PRTF_CHAR_TYPE op_char )
{
    if( (UCHAR_TYPE)op_char == putch( (UCHAR_TYPE)op_char ) ) {
        specs->_output_count++;
    }
}


_WCRTLINK int vcprintf( const char *format, va_list args )
{
    return( __prtf( NULL, format, args, con_putc ) );
}


_WCRTLINK int cprintf( const char *format, ... )
{
    va_list     args;
    int         ret;

    va_start( args, format );
    ret = __prtf( NULL, format, args, con_putc );
    va_end( args );
    return( ret );
}

