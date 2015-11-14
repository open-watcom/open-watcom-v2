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
#include "uidef.h"

bool uibackground( char *fname )
{
    FILE *              f;
    int                 c;
    int                 attr;
    int                 i;
    PIXEL               buff[80];
    BUFFER              *bgndbuff;
    int                 row;

    if( fname == NULL ) {
        uiremovebackground();
        return( TRUE );
    }
    bgndbuff = uibackgroundbuffer();
    if( bgndbuff == NULL ) {
        return( FALSE );
    }
    f = fopen( fname, "rb" );
    if( f == NULL ) {
        return( FALSE );
    }
    i = 0;
    row = 0;
    while( (attr = getc( f )) != EOF ) {
        c = getc( f );
        if( c == '!' && attr == '!' ) {
            getc( f );
            getc( f );
            continue;
        }
        buff[i].ch = c;
        buff[i].attr = attr;
        if( ++i >= 80 ) {
            braw( bgndbuff, row, 0, buff, i );
            i = 0;
            ++row;
            if( row > UIData->height ) {
                break;
            }
        }
    }
    fclose( f );
    return( TRUE );
}
