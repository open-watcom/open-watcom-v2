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


#include "vabout.hpp"
#include "wpshbttn.hpp"
#include "wtext.hpp"
#include "wmetrics.hpp"

extern const char* _viperDesc[];

VAbout::VAbout( WWindow* parent )
    : WDialog( parent, WRect( 160, 100, 380, 180 ), "WATCOM IDE" )
{
}

void VAbout::initialize()
{
    int fw = WSystemMetrics::dialogFrameWidth();
    int fh = WSystemMetrics::dialogFrameHeight();
    int wid = 0;
    int yoff = fh;
    for( int i=0; _viperDesc[i] != NULL; i++ ) {
        int w = getTextExtentX( _viperDesc[i] );
        int h = getTextExtentY( _viperDesc[i] );
        if( w > 0 ) {
            if( wid < w ) wid = w;
            WText* t1 = new WText( this, WRect(fw, yoff, w, h), _viperDesc[i] );
//          WText* t1 = new WText( this, WRect(fw, yoff, w, h), _viperDesc[i], TextStyleCentre );
            yoff += h * 5/4;
            t1->show();
        }
    }
    wid += fw * 2;

    static const char ok[] = { "OK" };
    int w = getTextExtentX( ok ) * 3;
    int h = getTextExtentY( ok ) * 3/2;
    int xoff = (wid - w) / 2;
    WDefPushButton* bOk = new WDefPushButton( this, WRect( xoff, yoff, w, h), "OK" );
    yoff += h * 5/4;
    bOk->onClick( this, (cbw)&VAbout::okButton );
    bOk->show();

    shrink();
    centre();

    show();
    bOk->setFocus();
}

void VAbout::okButton( WWindow* )
{
    quit( true );
}

#ifdef __WATCOMC__
// Complain about defining trivial destructor inside class
// definition only for warning levels above 8 
#pragma warning 656 9
#endif
  
VAbout::~VAbout()
{
}

