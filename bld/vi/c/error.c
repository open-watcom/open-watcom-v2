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


#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "vi.h"
#include "win.h"

static void ReadErrorMsgData( void );

/*
 * FatalError - process fatal error
 */
void FatalError( int err )
{
    char *str;

    SetCursorOnScreen( (int)WindMaxHeight -1, 0 );
    if( err == ERR_NO_MEMORY ) {
        str = "Out of memory";
    } else {
        str = GetErrorMsg( err );
    }
    MyPrintf("%s (fatal)\n", str );
    ExitEditor( -1 );

} /* FatalError */

/*
 * Die - unusual termination
 */
void Die( const char *str, ... )
{
    va_list     al;

    SetCursorOnScreen(  (int) WindMaxHeight - 1, 0 );
    MyPrintf("Failure: ");
    va_start( al, str );
    MyVPrintf( str, al );
    va_end( al );
    MyPrintf("\n");
    ExitEditor( -1 );

} /* Die */

static char strBuff[25];
static char readMsgData = FALSE;
static int errCnt;
static char *errorList;

/*
 * GetErrorMsg - return pointer to message
 */
char *GetErrorMsg( int err )
{
    char        *msg;

    if( !readMsgData ) {
        readErrorMsgData();
    }
    LastError = err;
    if( EditFlags.InputKeyMapMode ) {
        DoneInputKeyMap();
        EditFlags.NoInputWindow = FALSE;
        EditFlags.Dotable = FALSE;
    }
    if( err < 0 || err > errCnt ) {
        MySprintf( strBuff,"Err no. %d (no msg)",err);
        return( strBuff );
    }
    msg = GetTokenString( errorList, err );
    if( msg == NULL ) {
        MySprintf( strBuff,"Err no. %d (no msg)",err);
        return( strBuff );
    }
    return( msg );

} /* GetErrorMsg */

/*
 * Error - print an error message in the message window
 */
void Error( char *str, ... )
{
    va_list     al;
    char        tmp[MAX_STR];

    if( MessageWindow != NO_WINDOW ) {
        WindowAuxUpdate( MessageWindow, WIND_INFO_TEXT_COLOR,
                            messagew_info.hilight.foreground );
        WindowAuxUpdate( MessageWindow, WIND_INFO_BACKGROUND_COLOR,
                            messagew_info.hilight.background );
        va_start( al, str );
        MyVSprintf( tmp, str, al );
        va_end( al );

        SourceError( tmp );
        Message1( "%s", tmp );

        WindowAuxUpdate( MessageWindow, WIND_INFO_TEXT_COLOR,
                            messagew_info.text.foreground );
        WindowAuxUpdate( MessageWindow, WIND_INFO_BACKGROUND_COLOR,
                            messagew_info.text.background );
        MyBeep();
    } else {
        va_start( al, str );
#ifndef __WIN__
        MyVPrintf( str, al );
#endif
        va_end( al );
    }


} /* Error */

/*
 * readErrorMsgData - do just that
 */
static void readErrorMsgData( void )
{
    int         *vals;
    int         rc,cnt;
    char        *buff;

    rc = ReadDataFile( "errmsg.dat", &cnt, &buff, &vals, FALSE );
    if( rc ) {
        return;
    }
    errCnt = cnt;
    errorList = buff;
    readMsgData = TRUE;

    // we dont really need the values
    MemFree( vals );

} /* readErrorMsgData */

void ErrorFini( void ){
    MemFree( errorList );
    MemFree( ErrorTokens );
    MemFree( ErrorValues );
}
