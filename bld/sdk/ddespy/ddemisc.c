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


#include "wddespy.h"
#include <io.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

extern msglist DDEMsgs[1];

void LogHeader( int f ) {

    char        buf[100];
    unsigned    i;
    time_t      tm;

    tm = time( NULL );
    for( i=0; i< 80; i++ ) {
        buf[i] = '-';
    }
    buf[i] = '\0';
    write( f, buf, strlen( buf ) );
    write( f, "\r\n", 2 );
    RCsprintf( buf, STR_LOG_HEADER, asctime( localtime( &tm ) ) );
    write( f, buf, strlen( buf ) );
    write( f, "\r\n", 2 );
    for( i=0; i< 80; i++ ) {
        buf[i] = '-';
    }
    buf[i] = '\0';
    write( f, buf, strlen( buf ) );
    write( f, "\r\n", 2 );
}

void DumpHeader( FILE *fptr ) {

    time_t      tm;
    char        *str;

    tm = time( NULL );
    str = GetRCString( STR_EVENT_LIST_LINE );
    fprintf( fptr, str );
    str = GetRCString( STR_EVENT_LIST_HEADER );
    fprintf( fptr, str, ctime( &tm ) );
    str = GetRCString( STR_EVENT_LIST_LINE );
    fprintf( fptr, str );
}

BOOL InitGblStrings( void ) {
    BOOL        ret;

    ret = TRUE;
    AppName = AllocRCString( STR_APP_NAME );
    if( ret ) {
        ret = InitSrchTable( Instance, DDEMsgs );
    }
    return( ret );
}

void FiniRCStrings( void ) {
    FreeRCString( AppName );
}
