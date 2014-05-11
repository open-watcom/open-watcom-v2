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


#include <stdio.h>
#include <string.h>
#include "watcom.h"
#include "types.h"

#define COL_MAX         77

extern void PrintUint16Flags( uint_16 flag, char * flagonlist[],
                    char * flagofflist[], unsigned column )
/**************************************************************/
{
    bool        first;
    int         bit;
    char        *printstr;
    unsigned    curcol;

    first = true;
    curcol = column;

    for (bit = 15; bit >= 0; bit--) {
        if (flag & (0x01 << bit)) {
            printstr = flagonlist[ bit ];
        } else {
            printstr = flagofflist[ bit ];
        }

        if (printstr != NULL) {
            if (first) {
                first = false;
            } else {
                fputc( '|', stdout );
                curcol ++;
            }
            curcol += (unsigned)strlen( printstr );
            if( curcol > COL_MAX ) {
                fputc( '\n', stdout );
                fprintf( stdout, "%*s", column - 1, "" );
                curcol = column;
            }
            fputs( printstr, stdout );
        }
    }

    fputc( '\n', stdout );
}
