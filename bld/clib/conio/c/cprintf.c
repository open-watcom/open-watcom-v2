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


#include "widechar.h"
#include "variety.h"
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <stdarg.h>
#include "printf.h"

/*
 * con_putc -- output character to console
 */
static slib_callback_t con_putc; // setup calling convention
static void __SLIB_CALLBACK con_putc( SPECS __SLIB *specs, OUTC_PARM op_char )
{
    if( (UCHAR_TYPE)op_char == putch( (UCHAR_TYPE)op_char ) ) {
        specs->_output_count++;
    }
}


_WCRTLINK int vcprintf( const char *format, va_list arg )
{
    return( __prtf( NULL, format, arg, con_putc ) );
}


_WCRTLINK int cprintf( const char *format, ... )
{
    auto va_list    args;

    va_start( args, format );
    return( __prtf( NULL, format, args, con_putc ) );
}

