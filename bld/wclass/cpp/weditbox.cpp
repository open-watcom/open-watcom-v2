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


#include "weditbox.hpp"


//      Define Edit Box
//      ===============


WEXPORT WEditBox::WEditBox( WWindow *p, const WRect& r, const char *text,
                            WStyle wstyle )
        : WEdit( p, GUI_EDIT, r, text, wstyle ) {
/***********************************************/

}

WEXPORT WEditBox::WEditBox( WStatDialog *p, WControlId id,
                            WStyle wstyle )
        : WEdit( p, id, wstyle ) {
/********************************/

}


//      Define Multiline Edit Box
//      =========================


WEXPORT WMultiLineEditBox::WMultiLineEditBox( WWindow *p, const WRect& r,
                                              const char *text,
                                              WStyle wstyle )
        : WEdit( p, GUI_EDIT_MLE, r, text,
                 wstyle | _ControlStyleToWStyle( GUI_STYLE_CONTROL_WANTRETURN ) ) {
/***************************************************************************/

}

WEXPORT WMultiLineEditBox::WMultiLineEditBox( WStatDialog *p, WControlId id,
                                              WStyle wstyle )
        : WEdit( p, id,
                 wstyle | _ControlStyleToWStyle( GUI_STYLE_CONTROL_WANTRETURN ) ) {
/***************************************************************************/

}


//      Define General Edit Box
//      =======================


WEXPORT WEdit::WEdit( WWindow *parent, gui_control_class control_class,
                      const WRect& r, const char *text, WStyle wstyle )
        : WControl( parent, control_class, r, text, wstyle ) {
/************************************************************/

}

WEXPORT WEdit::WEdit( WStatDialog *parent, WControlId id,
                      WStyle wstyle )
        : WControl( parent, id, wstyle ) {
/****************************************/

}


void WEXPORT WEdit::select( int first, int last ) {
/*************************************************/

    GUISetEditSelect( parent()->handle(), controlId(), first, last );
}


int WEXPORT WEdit::getSelectBounds( int *first, int *last ) {
/***********************************************************/

    GUIGetEditSelect( parent()->handle(), controlId(), first, last );
    return( *last - *first );
}


#ifdef __WATCOMC__
// Complain about defining trivial destructor inside class
#if !defined( BOOTSTRAP )
#pragma disable_message( P656 )
#pragma disable_message( P657 )
#else
#pragma disable_message( 656 )
#pragma disable_message( 657 )
#endif
#endif

WEXPORT WEditBox::~WEditBox() {
/*****************************/

}

WEXPORT WEdit::~WEdit() {
/***********************/

}

WEXPORT WMultiLineEditBox::~WMultiLineEditBox() {
/***********************************************/

}
