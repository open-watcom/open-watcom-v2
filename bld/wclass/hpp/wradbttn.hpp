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


#ifndef wradiobutton_class
#define wradiobutton_class

#include "wboolsw.hpp"

#define RB_GROUP_FIRST  _ControlStyleToWStyle( GUI_AUTOMATIC | GUI_GROUP )
#define RB_NORMAL       _ControlStyleToWStyle( GUI_AUTOMATIC )
#define RB_GROUP_LAST   _ControlStyleToWStyle( GUI_AUTOMATIC | GUI_GROUP )

#define RStyleGroupFirst        RB_GROUP_FIRST
#define RStyleRadioButton       RB_NORMAL
#define RStyleGroupLast         RB_GROUP_LAST

WCLASS WRadioButton : public WBoolSwitch {
    public:
        WEXPORT WRadioButton( WWindow* parent, const WRect& r,
                              const char* text=NULL, WStyle s=RB_NORMAL );
        WEXPORT WRadioButton( WStatDialog* parent, WControlId id,
                              WStyle s=RB_NORMAL );
        WEXPORT ~WRadioButton();

    protected:
        virtual gui_control_class controlClass() { return( GUI_RADIO_BUTTON ); }
};

#endif
