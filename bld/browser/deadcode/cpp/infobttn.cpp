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


#include <wrect.hpp>
#include "wbrwin.h"
#include "symbol.h"
#include "infobttn.h"
#include "util.h"
#include "assure.h"

InfoButton::InfoButton()
    :WPushButton( NULL, WRect(0,0,0,0), NULL )
    ,_info(NULL)
    ,_showInfo(NULL)
{}

InfoButton::InfoButton( char * text, WBRWindow * prnt, int x, int y,
                        int &w, int &h )
    :WPushButton( prnt, WRect(x,y,w,h), text )
    ,_info(NULL)
    ,_showInfo(NULL)
{
    setup(text,w,h);
    show();
}

InfoButton::InfoButton( Symbol * info, WBRWindow * prnt, int x, int y,
                        int & w, int & h )
    :WPushButton( prnt, WRect(x,y,w,h), NULL )
    ,_info( info )
    ,_showInfo(NULL)
{
    char        buff[ MAX_DISP_NAME_LEN ];

    REQUIRE( _info != NULL, "InfoButton ctor got info not a sym" );
    char * name = _info->copyBaseName( buff );
    setText( name );
    setup( name, w, h );
    show();
}

void InfoButton::setup( const char* text, int & w, int & h)
{
    w = getTextExtentX( text );
    h = getTextExtentY( text );
    w += 10;
    h *= 7;
    h /= 4;
    size(w,h);
    setText((char*)text); // text is not changed - honest!
}

InfoButton::~InfoButton()
{
    if( _info ) {
        delete _info;
    }
}

void InfoButton::setInfoRqstCB( InfoRqstCB callback, WObject * clnt )
/*******************************************************************/
{
    if( _info != NULL ) {
        _showInfo = callback;
        _myclient = clnt;
        onClick( this, (cbw)&InfoButton::clicked );
    }
}

void InfoButton::clicked( WWindow * )
/***********************************/
{
    (_myclient->*_showInfo)( _info );
}
