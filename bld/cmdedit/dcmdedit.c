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
#include "cmdedit.h"
#include <process.h>
#include <dos.h>

#ifdef DOS
    unsigned char far * far AliasList;
#else
    unsigned char far * AliasList;
#endif

extern void InitRetrieve( char far * );
extern void StringIn( char far *, LENGTH far *, int, int );

main() {
    LENGTH l;
    int row,col;
    int written;
    static char near buff[LINE_WIDTH];

    getcmd( buff );
    InitRetrieve( buff );
    for( ;; ) {
        VioGetCurPos( (int PASPTR *)&row, (int PASPTR *)&col, 0 );
        VioWrtCharStr( (char PASPTR *)"hello>", 6, row, col, 0 );
        VioSetCurPos( row, col+6, 0 );
        l.input = LINE_WIDTH;
        StringIn( &buff, &l, 1, 5 );
        if( l.output == 1 && buff[0] == 'q' ) break;
        DosWrite( 1, (char far *)"\r\n", 2, (int PASPTR *)&written );
        DosWrite( 1, (char far *)buff, l.output, (int PASPTR *)&written );
        DosWrite( 1, (char far *)"\r\n", 2, (int PASPTR *)&written );
    }
}
