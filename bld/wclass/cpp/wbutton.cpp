/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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


#include "wbutton.hpp"


WEXPORT WButton::WButton( WWindow *parent, gui_control_class control_class,
                          const WRect &r, const char *text, WStyle wstyle )
        : WControl( parent, control_class, r, text, wstyle )
        , _clickClient( NULL )
        , _click( NULL )
        , _dblClickClient( NULL )
        , _dblClick( NULL )
        , _tagPtr( NULL ) {
/*************************/

    if( text != NULL ) {
        size_t icount = strlen( text );
        for( size_t i = 0; i < icount; ++i ) {
            if( text[i] == '&' ) {
                int key = text[i + 1];
                if( key != '\0' ) {
                    parent->addAccelKey( key, this, (bcbk)&WButton::hotKey );
                }
            }
        }
    }
}

WEXPORT WButton::WButton( WStatDialog *parent, WControlId id, WStyle wstyle )
        : WControl( parent, id, wstyle )
        , _clickClient( NULL )
        , _click( NULL )
        , _dblClickClient( NULL )
        , _dblClick( NULL )
        , _tagPtr( NULL ) {
/*************************/

#if 0 // NYI -- DAGLO
    if( text != NULL ) {
        size_t icount = strlen( text );
        for( size_t i = 0; i < icount; ++i ) {
            if( text[i] == '&' ) {
                int key = text[i + 1];
                if( key != '\0' ) {
                    parent->addAccelKey( key, this, (bcbk)&WButton::hotKey );
                }
            }
        }
    }
#endif
}



void WEXPORT WButton::onClick( WObject *client, cbw click ) {
/***********************************************************/

    _clickClient = client;
    _click = click;
}


void WEXPORT WButton::onDblClick( WObject *client, cbw click ) {
/**************************************************************/

    _dblClickClient = client;
    _dblClick = click;
}


bool WEXPORT WButton::hotKey( WKeyCode ) {
/****************************************/

    if( _clickClient && _click ) {
        (_clickClient->*_click)( this );
        return( true );
    }
    return( false );
}


bool WButton::processMsg( gui_event gui_ev )
/******************************************/
{
    switch( gui_ev ) {
    case GUI_CONTROL_CLICKED:
        if( _clickClient && _click ) {
            (_clickClient->*_click)( this );
            return( true );
        }
        break;
    case GUI_CONTROL_DCLICKED:
        if( _dblClickClient && _dblClick ) {
            (_dblClickClient->*_dblClick)( this );
            return( true );
        }
        break;
    }
    return( false );
}


#ifdef __WATCOMC__
// Complain about defining trivial destructor inside class
#if !defined( BOOTSTRAP )
#pragma disable_message( P656 )
#else
#pragma disable_message( 656 )
#endif
#endif

WEXPORT WButton::~WButton() {
/***************************/

}
