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


#ifndef weditbox_class
#define weditbox_class

#include "wcontrol.hpp"

#define EStyleDefault           0
#define EStyleInvisible         _ControlStyleToWStyle( GUI_EDIT_INVISIBLE )

WCLASS WEdit : public WControl {
    public:
        WEXPORT WEdit( WWindow *parent,
                       gui_control_class control,
                       const WRect& r,
                       const char *text, WStyle wstyle );
        WEXPORT WEdit( WStatDialog *parent, unsigned id, WStyle wstyle );
        WEXPORT ~WEdit();
        void WEXPORT select( int first=0, int last=32767 );
        int WEXPORT getSelectBounds( int *first, int *last );

    protected:
        virtual gui_control_class controlClass() { return GUI_EDIT; }
};

WCLASS WEditBox : public WEdit {
    public:
        WEXPORT WEditBox( WWindow *parent, const WRect& r,
                          const char *text=NULL,
                          WStyle wstyle=EStyleDefault );
        WEXPORT WEditBox( WStatDialog *parent, unsigned id,
                          WStyle wstyle=EStyleDefault );
        WEXPORT ~WEditBox();

    protected:
        virtual gui_control_class controlClass() { return GUI_EDIT; }
};

WCLASS WMultiLineEditBox : public WEdit {
    public:
        WEXPORT WMultiLineEditBox( WWindow *parent, const WRect& r,
                                   const char *text=NULL,
                                   WStyle wstyle=EStyleDefault );
        WEXPORT WMultiLineEditBox( WStatDialog *parent, unsigned id,
                                   WStyle wstyle=EStyleDefault );
        WEXPORT ~WMultiLineEditBox();

    protected:
        virtual gui_control_class controlClass() { return GUI_EDIT_MLE; }
};

#endif
