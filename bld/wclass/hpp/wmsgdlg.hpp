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


#ifndef wmessagedialog_class
#define wmessagedialog_class

#include "wdialog.hpp"

typedef enum msgLevel {
          MsgPlain      = 0
        , MsgError      = GUI_STOP
        , MsgWarning    = GUI_EXCLAMATION
        , MsgInfo       = GUI_INFORMATION
        , MsgQuestion   = GUI_QUESTION
} MsgLevel;

typedef enum msgButtons {
        MsgOk           = GUI_OK,
        MsgOkCancel     = GUI_OK_CANCEL,
        MsgRetryCancel  = GUI_RETRY_CANCEL,
        MsgYesNo        = GUI_YES_NO,
        MsgYesNoCancel  = GUI_YES_NO_CANCEL
} MsgButtons;

typedef enum msgRetType {
        MsgRetAbort     = GUI_RET_ABORT,
        MsgRetCancel    = GUI_RET_CANCEL,
        MsgRetIgnore    = GUI_RET_IGNORE,
        MsgRetNo        = GUI_RET_NO,
        MsgRetOk        = GUI_RET_OK,
        MsgRetRetry     = GUI_RET_RETRY,
        MsgRetYes       = GUI_RET_YES
} MsgRetType;

WCLASS WMessageDialog : public WDialog {
    public:
        WMessageDialog() {}
        ~WMessageDialog() {}
        static MsgRetType WEXPORT messagef( WWindow* parent, MsgLevel level,
                                MsgButtons button, const char *caption,
                                const char *text, ... );
        static MsgRetType WEXPORT message( WWindow* parent, MsgLevel level,
                                MsgButtons button, const char *text,
                                const char *caption = NULL );
        static MsgRetType WEXPORT messager( WWindow* parent, MsgLevel level,
                                MsgButtons button, WResourceId text_id,
                                WResourceId caption_id );
        static void WEXPORT info( WWindow* parent, const char *text, ... );
    private:
        static MsgRetType domessage( WWindow* parent, const char *text,
                                const char *caption, unsigned type );
};

#endif
