/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
#include <setjmp.h>
#include "expr.h"
#include "srcwin.h"
#include "wwinhelp.h"


static char WinCmdTokens[] = {
    #define pick(t,e)   t "\0"
    WINCMDS()
    #undef pick
};

#define HELPTOKENS() \
    pick( HELP_KEY ) \
    pick( HELP_PARTIALKEY )

static char WinHelpTokens[] = {
    #define pick(t)   #t "\0"
    HELPTOKENS()
    #undef pick
};

enum {
    #define pick(t)   WIN ## t,
    HELPTOKENS()
    #undef pick
};

/*
 * GetDWORD - get a dword from a string
 */
bool GetDWORD( const char **str, DWORD *res )
{
    char        hdlstr[MAX_STR];

    *str = GetNextWord1( *str, hdlstr );
    if( *hdlstr == '\0'  ) {
        return( false );
    }
    *res = strtoul( hdlstr, NULL, 10 );
    return( true );

} /* GetDWORD */

/*
 * RunWindowsCommand - try to run a Windows specific command
 */
bool RunWindowsCommand( const char *cmd, vi_rc *result, vars_list *vl )
{
    char        str[MAX_INPUT_LINE];
    char        tmp[MAX_INPUT_LINE];
    char        *ext;
    int         token;
    bool        rc;
    DWORD       left, top, width, height;

    if( cmd == NULL ) {
        return( false );
    }
    cmd = GetNextWord1( cmd, tmp );
    if( *tmp == '\0' ) {
        return( false );
    }
    token = Tokenize( WinCmdTokens, tmp, false );
    if( token == TOK_INVALID ) {
        return( false );
    }
    switch( token ) {
    case T_SETMAINSIZE:
        if( vl != NULL ) {
            cmd = Expand( tmp, cmd, vl );
        }
        *result = ERR_INVALID_COMMAND;
        if( !GetDWORD( &cmd, &left ) ) {
            return( true );
        }
        if( !GetDWORD( &cmd, &top ) ) {
            return( true );
        }
        if( !GetDWORD( &cmd, &width ) ) {
            return( true );
        }
        if( !GetDWORD( &cmd, &height ) ) {
            return( true );
        }
        ShowWindow( root_window_id, SW_SHOWNORMAL );
        MoveWindow( root_window_id, left, top, width, height, TRUE );
        *result = ERR_NO_ERR;
        return( true );

    case T_TAKEFOCUS:
#ifdef __NT__
        SetForegroundWindow( root_window_id );
#else
        SetActiveWindow( root_window_id );
#endif
        SetFocus( root_window_id );
        *result = ERR_NO_ERR;
        return( true );

    case T_MINIMIZE:
        ShowWindow( root_window_id, SW_SHOWMINNOACTIVE );
        *result = ERR_NO_ERR;
        return( true );

    case T_RESTORE:
        SetFocus( root_window_id );
        SetActiveWindow( root_window_id );
        BringWindowToTop( root_window_id );
        ShowWindow( root_window_id, SW_RESTORE );
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
                cmd = Expand( tmp, cmd, vl );
            }
            if( PromptThisFileForSave( cmd ) ) {
                *result = ERR_NO_ERR;
                return( true );
            } else {
                *result = ERR_FILE_MODIFIED;
                return( false );
            }
        }
    case T_INPUT_BOOL:
        if( vl != NULL ) {
            cmd = Expand( tmp, cmd, vl );
        }
        SetWindowPos( root_window_id, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
        if( MessageBox( root_window_id, cmd, EditorName, MB_OKCANCEL ) == IDOK ) {
            *result = ERR_NO_ERR;
            SetWindowPos( root_window_id, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
            SetWindowPos( root_window_id, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
            return( true );
        }
        SetWindowPos( root_window_id, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
        SetWindowPos( root_window_id, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
        *result = ERR_FILE_MODIFIED;
        return( true );
    case T_QUERY_FILE:
        if( QueryFile( cmd ) ) {
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
        rc = EditFile( cmd, false ) == ERR_NO_ERR;
#ifdef __NT__
        SetForegroundWindow( root_window_id );
#else
        SetActiveWindow( root_window_id );
#endif
        SetFocus( root_window_id );
        return( rc );
    case T_LOCATE:
        *result = 1;
        rc = LocateCmd( cmd ) == ERR_NO_ERR;
#ifdef __NT__
        SetForegroundWindow( root_window_id );
#else
        SetActiveWindow( root_window_id );
#endif
        SetFocus( root_window_id );
        return( rc );
    case T_WINHELP:
        if( vl != NULL ) {
            cmd = Expand( tmp, cmd, vl );
        }
        *result = ERR_INVALID_COMMAND;
        cmd = GetNextWord1( cmd, str );
        if( *str == '\0' ) {
            return( true );
        }
        token = Tokenize( WinHelpTokens, str, false );
        if( token == TOK_INVALID ) {
            return( true );
        }
        cmd = GetNextWord1( cmd, str );
        if( *str == '\0' ) {
            return( true );
        }
        ext = strstr( str, ".hlp" );
        if( ext != NULL ) {
            strcpy( ext, ".chm" );
        }
        switch( token ) {
        case WINHELP_KEY:
            if( *cmd == '\0' ) {
                return( true );
            }
            if( !WHtmlHelp( root_window_id, str, HELP_KEY, (HELP_DATA)(LPCSTR)cmd ) ) {
                if( ext != NULL ) {
                    strcpy( ext, ".hlp" );
                }
                WWinHelp( root_window_id, str, HELP_KEY, (HELP_DATA)(LPCSTR)cmd );
            }
            break;
        case WINHELP_PARTIALKEY:
            if( *cmd == '\0' ) {
                return( true );
            }
            if( !WHtmlHelp( root_window_id, str, HELP_PARTIALKEY, (HELP_DATA)(LPCSTR)cmd ) ) {
                if( ext != NULL ) {
                    strcpy( ext, ".hlp" );
                }
                WWinHelp( root_window_id, str, HELP_PARTIALKEY, (HELP_DATA)(LPCSTR)cmd );
            }
            break;
        }
        *result = ERR_NO_ERR;
        return( true );
    }
    if( vl != NULL ) {
        if( RunDDECommand( token, cmd, tmp, result, vl ) ) {
            return( true );
        }
    }
    return( false );

} /* RunWindowsCommand */
