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
* Description:  Log file support.
*
****************************************************************************/


#include "dbgdefn.h"
#include "dbglit.h"
#include "dbgio.h"
#include "dbgtoken.h"
#include "dbgerr.h"
#include <string.h>


extern void         ReqEOC( void );
extern void         Scan( void );
extern unsigned int ScanCmd( char * );
extern bool         ScanItem( bool, char **, unsigned int * );
extern bool         ScanEOC( void );
extern char         *GetCmdName( int );


extern tokens       CurrToken;
extern char         *TxtBuff;



static handle       LogHndl;


/*
 * IsLogging -- return true if there is a log file open
 */

#ifdef DEADCODE
bool IsLogging( void )
{
    return( LogHndl != NIL_HANDLE );
}
#endif


/*
 * LogInit -- initialize logging
 */

void LogInit( void )
{
    LogHndl = NIL_HANDLE;
}


/*
 * LogFini -- finish logging
 */

void LogFini( void )
{
    if( LogHndl != NIL_HANDLE ) {
        FileClose( LogHndl );
        LogHndl = NIL_HANDLE;
    }
}


/*
 * LogLine -- put a string followed by a newline in the log file
 */

void LogLine( char *str )
{
    int   len;

    if( LogHndl == NIL_HANDLE ) return;
    len = strlen( str );
    if( WriteText( LogHndl, str, len ) != len ) {
        LogFini();
    }
}


/*
 * LogPut -- put string to log file
 */

#ifdef DEADCODE
void LogPut( char *str )
{
    int   len;

    if( LogHndl == NIL_HANDLE ) return;
    len = strlen( str );
    if( WriteStream( LogHndl, str, len ) != len ) {
        LogFini();
    }
}
#endif


static void OpenLog( open_access mode )
{
    char   *start;
    unsigned len;

    if( LogHndl != NIL_HANDLE ) {
        Error( ERR_NONE, LIT( ERR_LOG_STARTED ) );
    } else if( !ScanItem( TRUE, &start, &len ) ) {
        Error( ERR_LOC, LIT( ERR_WANT_FILENAME ) );
    } else {
        if( len > TXT_LEN ) len = TXT_LEN;
        ReqEOC();
        memcpy( TxtBuff, start, len );
        TxtBuff[len] = NULLCHAR;
        LogHndl = FileOpen( TxtBuff, mode );
        if( LogHndl == NIL_HANDLE ) {
            Error( ERR_NONE, LIT( ERR_FILE_NOT_OPEN ), TxtBuff );
        }
    }
}

/*
 * LogAppend -- start logging to file
 */

OVL_EXTERN void LogAppend( void )
{
    OpenLog( OP_WRITE | OP_CREATE | OP_APPEND );
}


/*
 * LogStart -- start logging to file
 */

void LogStart( void )
{
    OpenLog( OP_WRITE | OP_CREATE | OP_TRUNC );
}


/*
 * LogEnd -- end logging to file
 */

void LogEnd( void )
{
    if( LogHndl != NIL_HANDLE ) {
        FileClose( LogHndl );
        LogHndl = NIL_HANDLE;
    } else {
        Error( ERR_NONE, LIT( ERR_LOG_NOT_STARTED ) );
    }
}


/*
 * BadLog -- handle bad log command
 */

OVL_EXTERN void BadLog( void )
{
    Error( ERR_LOC, LIT( ERR_BAD_OPTION ), GetCmdName( CMD_LOG ) );
}


static char LogNameTab[] = {
    "Append\0"
    "Start\0"
};


static void (* const LogJmpTab[])( void ) = {
    &BadLog,
    &LogAppend,
    &LogStart
};


/*
 * ProcLog -- process log command
 */

void ProcLog( void )
{
    if( ScanEOC() ) {
        LogEnd();
    } else if( CurrToken == T_GT ) {
        Scan();
        LogAppend();
    } else if( CurrToken == T_DIV ) {
        Scan();
        (*LogJmpTab[ ScanCmd( LogNameTab ) ])();
    } else {
        LogStart();
    }
}
