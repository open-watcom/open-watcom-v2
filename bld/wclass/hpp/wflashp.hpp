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


#ifndef flashpage_class
#define flashpage_class

#include "wwindow.hpp"
#include "whotspot.hpp"
#include "wstrlist.hpp"

#define INTERVAL_OK_BUTTON (-1)

WCLASS WTimer;
WCLASS WButton;

class WFlashPage : public WWindow
{
    public:
        WFlashPage( WWindow* parent, WHotSpots* hs, int idx, int interval=0 );
        WFlashPage( WWindow* parent, WHotSpots* hs, int idx, const char* title, int interval=0 );
        WFlashPage( WWindow* parent, WHotSpots* hs, int idx, const char* title, const char* text[], int interval=0 );
        ~WFlashPage();

        bool    paint();

    private:
        void            init( int interval, const char *title );
        int             _idx;
        WTimer*         _flashTimer;
        void            flashTimer( WTimer* timer, DWORD );
        void            okButton( WWindow * );
        WStringList     _text;
        int             _height;
        int             _width;
        WHotSpots*      _hotSpot;
};

#endif
