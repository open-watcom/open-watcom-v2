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


#include "wmsgdlg.hpp"
#include "wresstr.hpp"
#include "wstring.hpp"

extern "C" {
        #include <stdio.h>
        #include <stdarg.h>
}

#define MAX_MESSAGE 500


MsgRetType WMessageDialog::domessage( WWindow *parent,
                                             const char *text,
                                             const char *caption,
                                             unsigned type ) {
/************************************************************/

    if( caption == NULL ) {
        if( type & GUI_EXCLAMATION ) {  // don't call warnings errors
            caption = "Warning";
        } else if( type & GUI_STOP ) {
            caption = "Error";
        } else {
            caption = "Information";
        }
    }
    gui_window *handle = NULL;
    if( parent != NULL ) {
        handle = parent->handle();
    }
    MsgRetType ret = (MsgRetType)GUIDisplayMessage( handle,
                                                    (char *)text,
                                                    (char *)caption,
                                                    (gui_message_type)type );
    return( ret );
}


MsgRetType WEXPORT WMessageDialog::messagef( WWindow* parent, MsgLevel level,
                                             MsgButtons button,
                                             const char *caption,
                                             const char *text, ... ) {
/********************************************************************/

    MsgRetType rc;
    char* txt = new char[MAX_MESSAGE+1];
    va_list args;
    va_start( args, text );
    vsprintf( (char *)txt, text, args );
    unsigned type = (unsigned)level | (unsigned)button;
    rc = domessage( parent, txt, caption, type );
    delete []txt;
    return( rc );
}


MsgRetType WEXPORT WMessageDialog::message( WWindow* parent, MsgLevel level,
                                            MsgButtons button,
                                            const char *text,
                                            const char *caption ) {
/*****************************************************************/

    unsigned type;

    type = (unsigned)level | (unsigned)button;
    return( domessage( parent, text, caption, type ) );
}

MsgRetType WEXPORT WMessageDialog::messager( WWindow* parent, MsgLevel level,
                                             MsgButtons button,
                                             uint text_id,
                                             uint caption_id ) {
/*****************************************************************/

    unsigned type;
    WResStr  text( text_id );
    WResStr  caption( caption_id );

    type = (unsigned)level | (unsigned)button;
    return( domessage( parent, text, caption, type ) );
}

void WEXPORT WMessageDialog::info( WWindow* parent, const char *text, ... ) {
/***************************************************************************/

    char* txt = new char[MAX_MESSAGE+1];
    va_list args;
    va_start( args, text );
    vsprintf( txt, text, args );
    domessage( parent, txt, NULL, MsgOk );
    delete []txt;
}
