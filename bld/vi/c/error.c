/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Error processing, fatal and non-fatal.
*
****************************************************************************/


#include "vi.h"
#include "win.h"
#include "myprtf.h"

#define ERROR_COUNT (sizeof( errorList ) / sizeof( errorList[0] ))

static char     *errorTokens = NULL;
static int      *errorValues = NULL;
static bool     errorTokensLoaded = false;
static char     strBuff[25];
static const char   *errorList[] = {
    #define pick(n,t,i) t,
    #include "_errs.h"
    #undef pick
};

/*
 * StartupError - process fatal startup error
 */
void StartupError( vi_rc err )
{
    MyPrintf( "%s (fatal)\n", GetErrorMsg( err ) );

    FiniMem();

    /* Do not call ExitEditor() because almost nothing is initialized yet. */
    exit( -1 );
    // never return
} /* StartupError */

/*
 * FatalError - process fatal error
 */
void FatalError( vi_rc err )
{
    SetPosToMessageLine();
    MyPrintf( "%s (fatal)\n", GetErrorMsg( err ) );
    ExitEditor( -1 );

} /* FatalError */

/*
 * Die - unusual termination
 */
void Die( const char *str, ... )
{
    va_list     args;

    SetPosToMessageLine();
    MyPrintf( "Failure: " );
    va_start( args, str );
    MyVPrintf( str, args );
    va_end( args );
    MyPrintf( "\n" );
    ExitEditor( -1 );

} /* Die */

/*
 * GetErrorMsg - return pointer to message
 */
const char *GetErrorMsg( vi_rc err )
{
    LastError = err;
    if( EditFlags.InputKeyMapMode ) {
        DoneInputKeyMap();
        EditFlags.NoInputWindow = false;
        EditFlags.Dotable = false;
    }
    if( err < 0 || err >= ERROR_COUNT ) {
        MySprintf( strBuff, "Err no. %d (no msg)", err );
        return( strBuff );
    }
    return( errorList[err] );

} /* GetErrorMsg */

/*
 * Error - print an error message in the message window
 */
void Error( const char *str, ... )
{
    va_list     args;
    char        tmp[MAX_STR];

    if( !BAD_ID( message_window_id ) ) {
        WindowAuxUpdate( message_window_id, WIND_INFO_TEXT_COLOR,
                            messagew_info.hilight_style.foreground );
        WindowAuxUpdate( message_window_id, WIND_INFO_BACKGROUND_COLOR,
                            messagew_info.hilight_style.background );
        va_start( args, str );
        MyVSprintf( tmp, str, args );
        va_end( args );

        SourceError( tmp );
        Message1( "%s", tmp );

        WindowAuxUpdate( message_window_id, WIND_INFO_TEXT_COLOR,
                            messagew_info.text_style.foreground );
        WindowAuxUpdate( message_window_id, WIND_INFO_BACKGROUND_COLOR,
                            messagew_info.text_style.background );
        MyBeep();
    } else {
        va_start( args, str );
#ifndef __WIN__
        MyVPrintf( str, args );
        MyPrintf( "\n" );
#endif
        va_end( args );
    }

} /* Error */

/*
 * ErrorBox - show an error message in a dialog box
 */
void ErrorBox( const char *str, ... )
{
    va_list     args;
    char        tmp[MAX_STR];

    if( !BAD_ID( message_window_id ) ) {
        WindowAuxUpdate( message_window_id, WIND_INFO_TEXT_COLOR,
                            messagew_info.hilight_style.foreground );
        WindowAuxUpdate( message_window_id, WIND_INFO_BACKGROUND_COLOR,
                            messagew_info.hilight_style.background );
        va_start( args, str );
        MyVSprintf( tmp, str, args );
        va_end( args );

        SourceError( tmp );
        Message1Box( "%s", tmp );

        WindowAuxUpdate( message_window_id, WIND_INFO_TEXT_COLOR,
                            messagew_info.text_style.foreground );
        WindowAuxUpdate( message_window_id, WIND_INFO_BACKGROUND_COLOR,
                            messagew_info.text_style.background );
        MyBeep();
    } else {
        va_start( args, str );
#ifndef __WIN__
        MyVPrintf( str, args );
        MyPrintf( "\n" );
#endif
        va_end( args );
    }

} /* Error */


static bool err_alloc( int cnt )
{
    errorValues = _MemAllocArray( int, cnt );
    return( true );
}

static bool err_save( int i, const char *buff )
{
    errorValues[i] = atoi( buff );
    return( true );
}

/*
 * ReadErrorTokens - do just that
 */
vi_rc ReadErrorTokens( void )
{
    vi_rc       rc;

    if( errorTokensLoaded ) {
        return( ERR_NO_ERR );
    }

    rc = ReadDataFile( "error.dat", &errorTokens, err_alloc, err_save, true );
    if( rc == ERR_NO_ERR ) {
        errorTokensLoaded = true;
    } else if( rc == ERR_FILE_NOT_FOUND ) {
        rc = ERR_SRC_NO_ERROR_DATA;
    }
    return( rc );

} /* ReadErrorTokens */

/*
 * GetErrorTokenValue
 */
vi_rc GetErrorTokenValue( int *value, const char *str )
{
    int     i;
    vi_rc   rc;

    rc = ReadErrorTokens();
    if( rc == ERR_NO_ERR ) {
        i = Tokenize( errorTokens, str, true );
        if( i != TOK_INVALID ) {
            *value = errorValues[i];
        } else {
            rc = NO_NUMBER;
        }
    }
    return( rc );

} /* GetErrorTokenValue */

void ErrorFini( void )
{
    MemFree( errorTokens );
    MemFree( errorValues );
    errorTokensLoaded = false;

} /* ErrorFini */
