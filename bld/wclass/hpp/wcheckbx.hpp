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


#ifndef wcheckbox_class
#define wcheckbox_class

#include "wboolsw.hpp"

#define CStyleDefault 0
#define CStyleThreeState    _ControlStyleToWStyle( GUI_CONTROL_3STATE )

WCLASS WCheckBox : public WBoolSwitch {
    public:
        WEXPORT WCheckBox( WWindow* parent, const WRect& r,
                           const char *text, WStyle style=CStyleDefault );
        WEXPORT WCheckBox( WStatDialog* parent, unsigned id,
                           WStyle style=CStyleDefault );
        WEXPORT WCheckBox( WWindow* parent, const WRect& r,
                           WStyle style=CStyleDefault );
        WEXPORT ~WCheckBox();

    protected:
        virtual gui_control_class controlClass() { return( GUI_CHECK_BOX ); }
};

enum WCheckState3 {
    WCSNotChecked =     GUI_NOT_CHECKED,
    WCSChecked =        GUI_CHECKED,
    WCSIndeterminant =  GUI_INDETERMINANT
};

WCLASS WThreeState : public WButton {
    public:
        WEXPORT WThreeState( WWindow* parent, const WRect& r,
                             const char *text, WStyle style=CStyleThreeState );
        WEXPORT WThreeState( WStatDialog* parent, unsigned id,
                             WStyle style=CStyleThreeState );
        WEXPORT ~WThreeState();

    WCheckState3 checkState();
    void         setCheckState( WCheckState3 state );

    protected:
        virtual gui_control_class controlClass() { return( GUI_CHECK_BOX ); }
};

#endif
