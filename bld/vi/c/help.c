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
#ifdef __WIN__
  #include "wwinhelp.h"
#endif


#ifndef __WIN__
static char *helpCmds =
    "COMmandline\0"
    "KEYS\0"
    "REGularexpressions\0"
    "SCRipts\0"
    "SETtings\0"
    "STARTing\0"
    "\0";
#endif

char *helpFiles[] = {
    "cmd.hlp",
    "key.hlp",
    "regexp.hlp",
    "script.hlp",
    "set.hlp",
    "start.hlp"
};
int nHelpFiles = sizeof( helpFiles ) / sizeof( char * );


/*
 * DoHelp - do help on specified topic
 */
#ifdef __WIN__
vi_rc DoHelpOnContext( void )
{
    //Until we have a global context string, use thi local
    char *context_str = "Contents";
    DoHelp( context_str );
    return( ERR_NO_ERR );
}

vi_rc DoHelp( char *data )
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

    RemoveLeadingSpaces( data );
    if( !strcmp( data, "OnHelp" ) ) {
        WWinHelp( Root, NULL, HELP_HELPONHELP, 0L );
    } else if( !strcmp( data, "Contents" ) ) {
        if( !WHtmlHelp( Root, vi_chmfile, HELP_CONTENTS, 0L ) ) {
            WWinHelp( Root, vi_helpfile, HELP_CONTENTS, 0L );
        }
    } else if( !strcmp( data, "Search" ) ) {
        if( !WHtmlHelp( Root, vi_chmfile, HELP_PARTIALKEY, (DWORD)(LPSTR)"" ) ) {
            WWinHelp( Root, vi_helpfile, HELP_PARTIALKEY, (DWORD)(LPSTR)"" );
        }
    } else {
        WWinHelp( Root, win_helpfile, HELP_KEY, (DWORD)(LPSTR)data );
    }
    return ( ERR_NO_ERR );
}

#else

vi_rc DoHelpOnContext( void )
{
    return( ERR_NO_ERR );
}

vi_rc DoHelp( char *data )
{
    char        *hfile;
    char        *tstr;
    int         token;
    vi_rc       rc;
    char        path[FILENAME_MAX];
    char        tmp[MAX_STR];
    int         i;

    RemoveLeadingSpaces( data );
    token = Tokenize( helpCmds, data, FALSE );
    if( token < 0 ) {
        if( data[0] == 0 ) {
            strcpy( tmp, "Topics: " );
            for( i = 0; i < nHelpFiles; i++ ) {
                if( i != 0 ) {
                    strcat( tmp, ", " );
                }
                strcat( tmp, GetTokenString( helpCmds, i ) );
            }
            Message1( "%s", tmp );
        } else {
            Error( "No help on topic %s", data );
        }
        return( DO_NOT_CLEAR_MESSAGE_WINDOW );
    }
    hfile = helpFiles[token];
    GetFromEnv( hfile, path );
    if( path[0] == 0 ) {
        Error( "Help file %s not found", hfile );
        return( DO_NOT_CLEAR_MESSAGE_WINDOW );
    }
    EditFlags.ViewOnly = TRUE;
    rc = EditFile( path, FALSE );
    EditFlags.ViewOnly = FALSE;
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    tstr = GetTokenString( helpCmds, token );
    strcpy( tmp, tstr );
    strlwr( tmp );
    strcat( tmp, " Help" );
    tmp[0] = toupper( tmp[0] );
    CurrentFile->read_only = FALSE;
    AddString2( &(CurrentFile->name), tmp );
    SetFileWindowTitle( CurrentWindow, CurrentInfo, TRUE );
    DisplayFileStatus();
    return( ERR_NO_ERR );

} /* DoHelp */
#endif
