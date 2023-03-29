/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2023      The Open Watcom Contributors. All Rights Reserved.
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


#include "wtokfile.hpp"

WString& WTokenFile::token( WString& tok, bool* quoted, bool* eol )
{
    tok = "";
    _quoted = false;
    _eol = false;
    char ch = 0;
    for(;;) {
        ch = getch();
        if( !isspace( ch ) ) {
            break;
        }
    }
    if( ch == '"' ) {
        _quoted = true;
        ch = getch();
        for(;;) {
            if( ch == '"' )
                break;
            if( ch == '\\' ) {
                ch = getch();
                if( ch == 'n' ) {
                    tok.concat( 13 );
                    tok.concat( 10 );
                } else if( ch == 'r' ) {
                    tok.concat( 13 );
                } else if( ch == 't' ) {
                    tok.concat( 9 );
                } else {
                    tok.concat( ch );
                }
            } else {
                tok.concat( ch );
            }
            ch = getch();
        }
    } else {
        for(;;) {
            if( ch == 0 || ch == 10 )
                break;
            if( isspace( ch ) || ch == ',' )
                break;
            tok.concat( ch );
            ch = getch();
        }
        ungetch( ch );
    }
    for(;;) {
        ch = getch();
        if( ch == 0 || ch == 10 ) {
            if( quoted != NULL )
                *quoted = _quoted;
            _eol = true;
            if( eol != NULL )
                *eol = _eol;
            return( tok );
        }
        if( !isspace( ch ) ) {
            break;
        }
    }
    if( ch != ',' ) {
        ungetch( ch );
    }
    if( quoted != NULL )
        *quoted = _quoted;
    if( eol != NULL )
        *eol = _eol;
    return( tok );
}

void WTokenFile::flushLine( WString& tok )
{
    for( _eol = false; !_eol; ) {
        token( tok );
    }
}
