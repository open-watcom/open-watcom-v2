/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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


#include "wname.hpp"


WEXPORT WName::WName( WWindow* parent, const WRect& r, WWindow* client,
                      char* text )
        : WText( parent, r, text )
        , _client( client ) {
/***************************/

    if( text != NULL ) {
        size_t icount = strlen( text );
        for( size_t i = 0; i < icount; i++ ) {
            if( text[i] == '&' ) {
                int key = text[i + 1];
                if( (key != '\0') && (key != '&') ) {
                    parent->addAccelKey( key, this, (bcbk)&WName::hotKey );
                }
            }
        }
    }
}


bool WName::hotKey( WKeyCode ) {
/******************************/

    if( _client != NULL ) {
        _client->setFocus();
        return( true );
    }
    return( false );
}


#ifdef __WATCOMC__
// Complain about defining trivial destructor inside class
#pragma disable_message( 656 )
#endif

WEXPORT WName::~WName() {
/***********************/

}
