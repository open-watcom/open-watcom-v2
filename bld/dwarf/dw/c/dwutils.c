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


#include <watcom.h>
#include "dwutils.h"

/*
    These are client independant utility functions.
*/

char *LEB128(
    char *                      buf,
    dw_sconst                   value )
{
    uint_8                      byte;

    /* we can only handle an arithmetic right shift */
    if( value >= 0 ) {
        for(;;) {
            byte = value & 0x7f;
            value >>= 7;
            if( value == 0 && ( byte & 0x40 ) == 0 ) break;
            *buf++ = byte | 0x80;
        }
    } else {
        for(;;) {
            byte = value & 0x7f;
            value >>= 7;
            if( value == -1 && ( byte & 0x40 ) ) break;
            *buf++ = byte | 0x80;
        }
    }
    *buf++ = byte;
    return( buf );
}


char *ULEB128(
    char *                      buf,
    dw_uconst                   value )
{
    uint_8                      byte;

    for(;;) {
        byte = value & 0x7f;
        value >>= 7;
        if( value == 0 ) break;
        *buf++ = byte | 0x80;
    }
    *buf++ = byte;
    return( buf );
}
