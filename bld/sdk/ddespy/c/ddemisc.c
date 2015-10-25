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
* Description:  Miscellaneous DDE helper functions.
*
****************************************************************************/


#include "wddespy.h"
#include <io.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

extern msglist DDEMsgs[1];

/*
 * LogHeader
 */
void LogHeader( FILE *f )
{
    char        buf[100];
    unsigned    i;
    time_t      tm;

    tm = time( NULL );
    for( i = 0; i < 80; i++ ) {
        buf[i] = '-';
    }
    buf[i] = '\0';
    fwrite( buf, 1, strlen( buf ), f );
    fwrite( "\r\n", 1, 2, f );
    RCsprintf( buf, STR_LOG_HEADER, asctime( localtime( &tm ) ) );
    fwrite( buf, 1, strlen( buf ), f );
    fwrite( "\r\n", 1, 2, f );
    for( i = 0; i < 80; i++ ) {
        buf[i] = '-';
    }
    buf[i] = '\0';
    fwrite( buf, 1, strlen( buf ), f );
    fwrite( "\r\n", 1, 2, f );

} /* LogHeader */

/*
 * DumpHeader
 */
void DumpHeader( FILE *fptr )
{
    time_t      tm;
    char        *str;

    tm = time( NULL );
    str = GetRCString( STR_EVENT_LIST_LINE );
    fprintf( fptr, str );
    str = GetRCString( STR_EVENT_LIST_HEADER );
    fprintf( fptr, str, ctime( &tm ) );
    str = GetRCString( STR_EVENT_LIST_LINE );
    fprintf( fptr, str );

} /* DumpHeader */

/*
 * InitGblStrings
 */
bool InitGblStrings( void )
{
    bool        ret;

    ret = true;
    AppName = AllocRCString( STR_APP_NAME );
    if( ret ) {
        ret = InitSrchTable( Instance, DDEMsgs );
    }
    return( ret );

} /* InitGblStrings */

/*
 * FiniGblStrings
 */
void FiniRCStrings( void )
{
    FreeRCString( AppName );

} /* FiniGblStrings */
