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


// editsess.cpp - implementation of class EditSession
// this class is used _only_ for debugging purposes

#include <wtext.hpp>
#include "symbol.h"
#include "editsess.h"


EditSession::EditSession( int x, int y, Symbol & symbol )
        : WBRWindow( WRect( x, y, 350, 150 ), "" )
{
    char buff[ MAX_DISP_NAME_LEN ];
    WString s;
    ulong len;
    ulong line;
    uint  col;
    char * file = symbol.defSourceFile();
    char * src = symbol.defSourceFile();

    s.printf( "Edit: %s", src );
    setText(s);

    symbol.filePosition( line, col );
    len = strlen( symbol.copyBaseName(buff) );

    s.printf("I am pretending to be an editor opened on file %s (%lu,%u) - %lu",
                src, line, col, len );
    (new WText( this, WRect(0,0,650,150), s ))->show();
    show();
}

EditSession::EditSession( int x, int y, Reference * ref )
        : WBRWindow( WRect( x, y, 350, 150 ), "" )
{
    WString s( "Edit: " );
    s.concat( ref->name() );
    setText(s);

    ulong len = ref->length();
    ulong col = ref->column();

    s.printf( "I am pretending to be an editor opened on file %s (%lu,%lu) - %u",
                ref->sourceFile(), ref->line(), col, len );
    (new WText( this, WRect(0,0,650,150), s ))->show();
    show();
}
