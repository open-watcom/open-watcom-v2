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


#include "guiwind.h"

typedef struct MessageTypes {
    UINT                style;
    gui_message_type    type;
} MessageTypes;

static MessageTypes Types[] = {
 {  MB_OK,                      GUI_OK                  },
 {  MB_OKCANCEL,                GUI_OK_CANCEL           },
 {  MB_YESNO,                   GUI_YES_NO              },
 {  MB_YESNOCANCEL,             GUI_YES_NO_CANCEL       },
 {  MB_RETRYCANCEL,             GUI_RETRY_CANCEL        },
 {  MB_ABORTRETRYIGNORE,        GUI_ABORT_RETRY_IGNORE  },
 {  MB_ICONINFORMATION,         GUI_INFORMATION         },
 {  MB_ICONSTOP,                GUI_STOP                },
 {  MB_ICONEXCLAMATION,         GUI_EXCLAMATION         },
 {  MB_ICONQUESTION,            GUI_QUESTION            },
 {  MB_SYSTEMMODAL,             GUI_SYSTEMMODAL         }
};

#define NUM_STYLES ( sizeof( Types ) / sizeof( MessageTypes ) )

/*
 * GUIDisplayMessage - display a message with requested types of icon and
 *                      return user's response.
 */

gui_message_return GUIDisplayMessage( gui_window *wnd,
                                      const char *message, const char *title,
                                      gui_message_type type )
{
    UINT                style;
    int                 ret;
    gui_message_return  ret_code;
    HWND                hwnd;
    int                 i;


    if( ( message == NULL ) || ( title == NULL ) ) {
        return( GUI_RET_ABORT );
    }
    style = 0;
    for( i = 0; i < NUM_STYLES; i++ ) {
        if( type & Types[i].type ) {
            style |= Types[i].style;
        }
    }

    style |= MB_MOVEABLE;

    hwnd = NULLHANDLE;
    if( wnd != NULL ) {
        hwnd = wnd->hwnd;
    }

    ret_code = GUI_RET_ABORT;

    ret = _wpi_messagebox( hwnd, (LPSTR)message, title, style );

    switch( ret ) {
    case WPI_IDABORT :
        ret_code = GUI_RET_ABORT;
        break;
    case WPI_IDCANCEL :
        ret_code = GUI_RET_CANCEL;
        break;
    case WPI_IDIGNORE :
        ret_code = GUI_RET_IGNORE;
        break;
    case WPI_IDNO :
        ret_code = GUI_RET_NO;
        break;
    case WPI_IDOK :
        ret_code = GUI_RET_OK;
        break;
    case WPI_IDRETRY :
        ret_code = GUI_RET_RETRY;
        break;
    case WPI_IDYES :
        ret_code = GUI_RET_YES;
        break;
    }

    return( ret_code );
}
