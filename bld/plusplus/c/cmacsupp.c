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


#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "plusplus.h"
#include "preproc.h"



void MacroOffsetAddChar(                // MacroOffset: ADD A CHARACTER
    unsigned *mlen,                     // - length of MacroOffset
    unsigned nchar, ... )               // - no. of chars to insert
{
    unsigned clen;
    unsigned i;
    char *p;
    va_list args;

    clen = *mlen;
    MacroOverflow( clen + nchar, clen );
    p = MacroOffset + clen;
    va_start( args, nchar );
    for( i = 0; i < nchar; i++ ) {
        *p = va_arg( args, int );
        ++clen;
        ++p;
    }
    va_end( args );
    *mlen = clen;
}


void MacroOffsetAddMem(            // MacroOffset: ADD A SEQUENCE OF BYTES
    unsigned *mlen,                // - Macro Offset Length
    const char *buff,              // - bytes to be added
    unsigned len )                 // - number of bytes
{
    unsigned clen;

    clen = *mlen;
    MacroOverflow( clen + len, clen );
    memcpy( MacroOffset + clen, buff, len );
    *mlen += len;
}
