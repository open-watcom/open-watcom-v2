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
* Description:  WHEN YOU FIGURE OUT WHAT THIS MODULE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "auipvt.h"
#include <string.h>
#include <ctype.h>

static  char            *IdChars = WND_ALNUM_STR "_$";

extern  char            *WndSetIDChars( a_window *wnd, char *id_chars )
{
    char        *old;

    old = wnd->select_chars;
    wnd->select_chars = id_chars;
    return( old );
}

extern  bool            WndIDChar( a_window *wnd, char ch )
{
    char        *p;

    p = wnd->select_chars;
    if( p == NULL ) {
        p = IdChars;
    }
    while( *p != '\0' ) {
        if( *p == WND_ALNUM_CHAR ) {
            if( p[1] != WND_ALNUM_CHAR ) {
                if( isalnum( ch ) ) return( TRUE );
            } else {
                ++p;
            }
        }
        if( ch == *p ) return( TRUE );
        ++p;
    }
    return( FALSE );
}


extern  bool            WndKeyChar( char ch )
{
    return( isprint( ch ) );
}
