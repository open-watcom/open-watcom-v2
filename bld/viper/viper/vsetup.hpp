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


#ifndef vsetup_class
#define vsetup_class

#include "wdialog.hpp"
#include "mstate.hpp"

WCLASS MTool;
WCLASS WComboBox;
WCLASS WPushButton;
WCLASS MSwitch;
WCLASS VSetup : public WDialog
{
        public:
                VSetup( WWindow* parent, MTool* tool, WString& tagMask, WVList* states, const char* text, SwMode mode );
                ~VSetup();
                void initialize();
                void okButton( WWindow* );
                void cancelButton( WWindow* );
                void resetCfgButton( WWindow* );
                void resetDevButton( WWindow* );
                void resetRelButton( WWindow* );
                void prevButton( WWindow* );
                void nextButton( WWindow* );
                void pickFamily( WWindow* );
                bool process();
        private:
                MTool*          _tool;
                WString         _tagMask;
                WVList*         _states;
                WVList          _screens;
                SwMode          _mode;
                int             _curIndex;
                WString         _text;
                WComboBox*      _combo;
                WPushButton*    _prev;
                WPushButton*    _next;
                WVList          _families;      //contains WVLists of SwitchMaps
                MState* findState( MSwitch* sw, SwMode mode );
                void initControls( WVList* swList, SwMode mode, bool useStates );
                void enableButtons();
                void hideFamily( int index );
                void showFamily( int index );
                void shift( WWindow* w, int xoff, int yoff );
                void shiftFamily( int index, int xoff, int yoff );
};

#endif
