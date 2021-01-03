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
#ifdef __WIN__
  #include "wwinhelp.h"
#else
  #include "viuihelp.h"
#endif

#include "clibext.h"


#ifndef __WIN__
#define HELPCMDS() \
    pick( "COMmandline",        "cmd.hlp" ) \
    pick( "KEYS",               "key.hlp" ) \
    pick( "REGularexpressions", "regexp.hlp" ) \
    pick( "SCRipts",            "script.hlp" ) \
    pick( "SETtings",           "set.hlp" ) \
    pick( "STARTing",           "start.hlp" )

static char HelpCmdTokens[] = {
    #define pick(t,h)   t "\0"
    HELPCMDS()
    #undef pick
};

char *helpFiles[] = {
    #define pick(t,h)   h,
    HELPCMDS()
    #undef pick
};
int nHelpFiles = sizeof( helpFiles ) / sizeof( char * );
#endif


/*
 * DoHelp - do help on specified topic
 */
#ifdef __WIN__
vi_rc DoHelpOnContext( void )
{
    //Until we have a global context string, use thi local
    DoHelp( "Contents" );
    return( ERR_NO_ERR );
}

vi_rc DoHelp( const char *data )
{
    // Use the windows help till we get one of our own
    LPSTR vi_chmfile = "editor.chm";
    LPSTR vi_helpfile = "editor.hlp";
#ifdef __NT__
    //LPSTR win_helpfile = "api32wh.hlp";
    LPSTR win_helpfile = "win32sdk.hlp";
#else
    LPSTR win_helpfile = "win31wh.hlp";
#endif

    if( strcmp( data, "OnHelp" ) == 0 ) {
        WWinHelp( root_window_id, NULL, HELP_HELPONHELP, (HELP_DATA)0 );
    } else if( strcmp( data, "Contents" ) == 0 ) {
        if( !WHtmlHelp( root_window_id, vi_chmfile, HELP_CONTENTS, (HELP_DATA)0 ) ) {
            WWinHelp( root_window_id, vi_helpfile, HELP_CONTENTS, (HELP_DATA)0 );
        }
    } else if( strcmp( data, "Search" ) == 0 ) {
        if( !WHtmlHelp( root_window_id, vi_chmfile, HELP_PARTIALKEY, (HELP_DATA)(LPCSTR)"" ) ) {
            WWinHelp( root_window_id, vi_helpfile, HELP_PARTIALKEY, (HELP_DATA)(LPCSTR)"" );
        }
    } else {
        WWinHelp( root_window_id, win_helpfile, HELP_KEY, (HELP_DATA)(LPCSTR)data );
    }
    return ( ERR_NO_ERR );
}

#else

vi_rc DoHelpOnContext( void )
{
    return( ERR_NO_ERR );
}

vi_rc DoHelp( const char *data )
{
    char        *hfile;
    const char  *tstr;
    int         token;
    vi_rc       rc;
    char        path[_MAX_PATH];
    char        tmp[MAX_STR];
    int         i;

    token = Tokenize( HelpCmdTokens, data, false );
    if( token == TOK_INVALID ) {
        if( data[0] == '\0' ) {
            strcpy( tmp, "Topics: " );
            for( i = 0; i < nHelpFiles; i++ ) {
                if( i != 0 ) {
                    strcat( tmp, ", " );
                }
                strcat( tmp, GetTokenString( HelpCmdTokens, i ) );
            }
            Message1( "%s", tmp );
        } else {
            Error( "No help on topic %s", data );
        }
        return( DO_NOT_CLEAR_MESSAGE_WINDOW );
    }
    hfile = helpFiles[token];
    GetFromEnv( hfile, path );
    if( path[0] == '\0' ) {
        Error( "Help file %s not found", hfile );
        return( DO_NOT_CLEAR_MESSAGE_WINDOW );
    }
    EditFlags.ViewOnly = true;
    rc = EditFile( path, false );
    EditFlags.ViewOnly = false;
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    tstr = GetTokenString( HelpCmdTokens, token );
    strcpy( tmp, tstr );
    strlwr( tmp );
    strcat( tmp, " Help" );
    tmp[0] = toupper( tmp[0] );
    CurrentFile->read_only = false;
    ReplaceString( &(CurrentFile->name), tmp );
    SetFileWindowTitle( current_window_id, CurrentInfo, true );
    DisplayFileStatus();
    return( ERR_NO_ERR );

} /* DoHelp */
#endif
