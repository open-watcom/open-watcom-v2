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


#ifndef wabout_class
#define wabout_class

#include "wdialog.hpp"
#include "whotspot.hpp"
#include "wstrlist.hpp"

WCLASS WButton;

class WAbout : public WDialog {
    public:
        WAbout( WWindow* parent, WHotSpots* hs, int idx );
        WAbout( WWindow* parent, WHotSpots* hs, int idx, const char* title );
        WAbout( WWindow* parent, WHotSpots* hs, int idx, const char* title, const char* text[] );
        ~WAbout();

        virtual void initialize();
        bool    paint();

    private:
        int             _idx;
        void            okButton( WWindow * );
        WStringList     _text;
        const char      **_textArray;
        const char      *_title;
        int             _height;
        int             _width;
        WHotSpots*      _hotSpot;
};

#endif
