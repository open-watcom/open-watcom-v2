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


#include "vi.h"
#include <malloc.h>
#include <setjmp.h>
#include "expr.h"
#include "srcwin.h"
#include "wwinhelp.h"

static char winTokens[] = {
    "DDEINIT\0"
    "DDESERVER\0"
    "CREATEDDESTRing\0"
    "DELETEDDESTRing\0"
    "DDECONnect\0"
    "DDEDISconnect\0"
    "DDEREQuest\0"
    "DDEPOKE\0"
    "DDERET\0"
    "DDECREATEDATAHandle\0"
    "DDEGETData\0"
    "DDEQUERYSTRING\0"
    "TAKEFOCUS\0"
    "MINIMIZE\0"
    "RESTORE\0"
    "EXIT\0"
    "SETMAINSIZE\0"
    "UPDATEWINDOWS\0"
    "WINHELP\0"
    "PROMPTFORSAVE\0"
    "PROMPTTHISFILEFORSAVE\0"
    "QUERYFILE\0"
    "INPUTBOOL\0"
    "EDITFILE\0"
    "LOCATE\0"
    "\0"
};

static char helpTokens[] = {
    "HELP_KEY\0"
    "HELP_PARTIALKEY\0"
    "0"
};
enum {
    WINHELP_KEY,
    WINHELP_PARTIALKEY
};

/*
 * GetDWORD - get a dword from a string
 */
bool GetDWORD( char *str, LPVOID res )
{
    char        hdlstr[MAX_STR];

    if( NextWord1( str, hdlstr ) <= 0  ) {
        return( false );
    }
    *((DWORD *)res) = strtoul( hdlstr, NULL, 10 );
    return( true );

} /* GetDWORD */

/*
 * RunWindowsCommand - try to run a Windows specific command
 */
bool RunWindowsCommand( char *cmd, vi_rc *result, vlist *vl )
{
    char        str[MAX_INPUT_LINE];
    char        tmp[MAX_INPUT_LINE];
    char        tmp2[MAX_INPUT_LINE];
    char        *ext;
    int         token;
    bool        rc;
    DWORD       left, top, width, height;

    if( tmp == NULL || str == NULL ) {
        return( false );
    }
    strcpy( str, cmd );
    RemoveLeadingSpaces( str );
    if( NextWord1( str, tmp ) <= 0 ) {
        return( false );
    }
    token = Tokenize( winTokens, tmp, false );
    if( token == -1 ) {
        return( false );
    }
    switch( token ) {
    case T_SETMAINSIZE:
        if( vl != NULL ) {
            Expand( str, str, vl );
        }
        *result = ERR_INVALID_COMMAND;
        if( !GetDWORD( str, &left ) ){
            return( true );
        }
        if( !GetDWORD( str, &top ) ){
            return( true );
        }
        if( !GetDWORD( str, &width ) ){
            return( true );
        }
        if( !GetDWORD( str, &height ) ){
            return( true );
        }
        ShowWindow( Root, SW_SHOWNORMAL );
        MoveWindow( Root, left, top, width, height, TRUE );
        *result = ERR_NO_ERR;
        return( true );

    case T_TAKEFOCUS:
#ifdef __NT__
        SetForegroundWindow( Root );
#else
        SetActiveWindow( Root );
#endif
        SetFocus( Root );
        *result = ERR_NO_ERR;
        return( true );

    case T_MINIMIZE:
        ShowWindow( Root, SW_SHOWMINNOACTIVE );
        *result = ERR_NO_ERR;
        return( true );

    case T_RESTORE:
        SetFocus( Root );
        SetActiveWindow( Root );
        BringWindowToTop( Root );
        ShowWindow( Root, SW_RESTORE );
        *result = ERR_NO_ERR;
        return( true );

    case T_EXIT:
        if( ExitWithPrompt( true, false ) ) {
            *result = ERR_NO_ERR;
            return( true );
        } else {
            *result = ERR_FILE_MODIFIED;
            return( false );
        }
    case T_PROMPT_FOR_SAVE:
        {
            if( PromptFilesForSave() ) {
                *result = ERR_NO_ERR;
                return( true );
            } else {
                *result = ERR_FILE_MODIFIED;
                return( false );
            }
        }
    case T_PROMPT_THIS_FILE_FOR_SAVE:
        {
            if( vl != NULL ) {
                Expand( str, str, vl );
            }
            if( PromptThisFileForSave( str ) ) {
                *result = ERR_NO_ERR;
                return( true );
            } else {
                *result = ERR_FILE_MODIFIED;
                return( false );
            }
        }
    case T_INPUT_BOOL:
        if( vl != NULL ) {
            Expand( str, str, vl );
        }
        SetWindowPos( Root, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
        if( MessageBox( Root, str, EditorName, MB_OKCANCEL ) == IDOK ) {
            *result = ERR_NO_ERR;
            SetWindowPos( Root, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
            SetWindowPos( Root, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
            return( true );
        }
        SetWindowPos( Root, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
        SetWindowPos( Root, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
        *result = ERR_FILE_MODIFIED;
        return( true );
    case T_QUERY_FILE:
        if( QueryFile( str ) ) {
            *result = 1;
        } else {
            *result = 0;
        }
        return( true );
    case T_UPDATEWINDOWS:
        DCUpdateAll();
        *result = ERR_NO_ERR;
        return( true );
    case T_EDITFILE:
        *result = 1;
        rc = EditFile( str, false ) == ERR_NO_ERR;
#ifdef __NT__
        SetForegroundWindow( Root );
#else
        SetActiveWindow( Root );
#endif
        SetFocus( Root );
        return( rc );
    case T_LOCATE:
        *result = 1;
        rc = LocateCmd( str ) == ERR_NO_ERR;
#ifdef __NT__
        SetForegroundWindow( Root );
#else
        SetActiveWindow( Root );
#endif
        SetFocus( Root );
        return( rc );
    case T_WINHELP:
        if( vl != NULL ) {
            Expand( str, str, vl );
        }
        *result = ERR_INVALID_COMMAND;
        if( NextWord1( str, tmp ) <= 0 ) {
            return( true );
        }
        token = Tokenize( helpTokens, tmp, false );
        if( token == -1 ) {
            return( true );
        }
        if( NextWord1( str, tmp ) <= 0 ) {
            return( true );
        }
        RemoveLeadingSpaces( str );
        strcpy( tmp2, tmp );
        ext = strstr( tmp2, ".hlp" );
        if( ext != NULL ) {
            strcpy( ext, ".chm" );
        }
        switch( token ) {
        case WINHELP_KEY:
            if( str[0] == 0 ) {
                return( true );
            }
            if( !WHtmlHelp( Root, tmp2, HELP_KEY, (HELP_DATA)str ) ) {
                WWinHelp( Root, tmp, HELP_KEY, (HELP_DATA)str );
            }
            break;
        case WINHELP_PARTIALKEY:
            if( str[0] == 0 ) {
                return( true );
            }
            if( !WHtmlHelp( Root, tmp2, HELP_PARTIALKEY, (HELP_DATA)str ) ) {
                WWinHelp( Root, tmp, HELP_PARTIALKEY, (HELP_DATA)str );
            }
            break;
        }
        *result = ERR_NO_ERR;
        return( true );
    }
    if( vl != NULL ) {
        if( RunDDECommand( token, str, tmp, result, vl ) ) {
            return( true );
        }
    }
    return( false );

} /* RunWindowsCommand */
