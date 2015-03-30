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


#include "wcheckbx.hpp"


WEXPORT WCheckBox::WCheckBox( WWindow* parent, const WRect& r,
                              const char *text, WStyle style )
        : WBoolSwitch( parent, GUI_CHECK_BOX, r, text, style ) {
/**************************************************************/

}


WEXPORT WCheckBox::WCheckBox( WWindow* parent, const WRect& r, WStyle style )
        : WBoolSwitch( parent, GUI_CHECK_BOX, r, NULL, style ) {
/**************************************************************/

}


WEXPORT WCheckBox::WCheckBox( WStatDialog* parent, gui_ctl_id id,
                              WStyle style )
        : WBoolSwitch( parent, id, style ) {
/******************************************/

}

WEXPORT WThreeState::WThreeState( WWindow* parent, const WRect& r,
                                  const char *text, WStyle style )
        : WButton( parent, GUI_CHECK_BOX, r, text, style ) {
/**********************************************************/

}

WEXPORT WThreeState::WThreeState( WStatDialog* parent, gui_ctl_id id,
                                  WStyle style )
        : WButton( parent, id, style ) {
/**************************************/

}

WCheckState3 WThreeState::checkState() {
/**************************************/

    return( (WCheckState3) GUIIsChecked( parent()->handle(), controlId() ) );
}

void WThreeState::setCheckState( WCheckState3 state ) {
/*****************************************************/

    GUISetChecked( parent()->handle(), controlId(), state );
}

#ifdef __WATCOMC__
// Complain about defining trivial destructor inside class
// definition only for warning levels above 8 
#pragma warning 656 9
#endif

WEXPORT WCheckBox::~WCheckBox() {
/*******************************/

}

WEXPORT WThreeState::~WThreeState() {
/***********************************/

}
