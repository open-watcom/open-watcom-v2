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


#include <unistd.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <process.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "wstrip.h"
#include "banner.h"
#include "wressetr.h"
#include "wreslang.h"

#define RESOURCE_MAX_SIZE       128
#define NIL_HANDLE      ((int)-1)

#define NO_RES_MESSAGE "Error: could not open message resource file.\r\n"
#define NO_RES_SIZE (sizeof(NO_RES_MESSAGE)-1)


static  HANDLE_INFO     hInstance = { 0 };
static  int             Res_Flag;
static  unsigned        MsgShift;
extern  long            FileShift;

static void Outs( int nl, char *s )
{
    write( STDOUT_FILENO, s, strlen( s ) );
    if( nl ) write( STDOUT_FILENO, "\r\n", 2 );
}

static void Outc( char c )
{
    write( STDOUT_FILENO, &c, 1 );
}

void Banner()
{
    Outs( 1, banner1w( "Executable Strip Utility", _WSTRIP_VERSION_ ) );
    Outs( 1, banner2( "1988" ) );
    Outs( 1, banner3 );
}

void Usage( void )
{
    char        msg_buffer[RESOURCE_MAX_SIZE];
    int         i;

    for( i = MSG_USE_BASE;; i++ ) {
        Msg_Get( i, msg_buffer );
        if( ( msg_buffer[ 0 ] == '.' ) && ( msg_buffer[ 1 ] == 0 ) ) break;
        Outs( 1, msg_buffer );
    }
    Msg_Fini();
    exit( -1 );
}


void Fatal( int reason, char *insert )
/* the reason doesn't have to be good */
{
    char        msg_buffer[RESOURCE_MAX_SIZE];
    int         i = 0;

    Msg_Get( reason, msg_buffer );
    while( msg_buffer[i] != '\0' ) {
        if( msg_buffer[i] == '%' ) {
            if( msg_buffer[i+1] == 's' ) {
                Outs( 0, insert );
            } else {
                Outc( msg_buffer[i+1] );
            }
            i++;
        } else {
            Outc( msg_buffer[i] );
        }
        i++;
    }
    Msg_Get( MSG_WSTRIP_ABORT, msg_buffer );
    Outs( 1, msg_buffer );
    Msg_Fini();
    exit( -1 );
}


static long res_seek( int handle, long position, int where )
/* fool the resource compiler into thinking that the resource information
 * starts at offset 0 */
{
    if( where == SEEK_SET ) {
        return( lseek( handle, position + FileShift, where ) - FileShift );
    } else {
        return( lseek( handle, position, where ) );
    }
}


WResSetRtns( open, close, read, write, res_seek, tell, malloc, free );


static int Msg_Get( int resourceid, char *buffer )
{
    if ( LoadString( &hInstance, resourceid + MsgShift,
                (LPSTR) buffer, RESOURCE_MAX_SIZE ) != 0 ) {
        buffer[0] = '\0';
        return( 0 );
    }
    return( 1 );
}


int Msg_Init()
{
    int         initerror;
    char        name[_MAX_PATH];

    if( _cmdname( name ) == NULL ) {
        initerror = 1;
    } else {
        hInstance.filename = name;
        OpenResFile( &hInstance );
        if( hInstance.handle == NIL_HANDLE ) {
            initerror = 1;
        } else {
            initerror = FindResources( &hInstance );
        }
        if( !initerror ) {
            initerror = InitResources( &hInstance );
        }
    }
    if( initerror ) {
        Res_Flag = EXIT_FAILURE;
        write( STDOUT_FILENO, NO_RES_MESSAGE, NO_RES_SIZE );
    } else {
        Res_Flag = EXIT_SUCCESS;
    }
    MsgShift = WResLanguage() * MSG_LANG_SPACING;
    if( !initerror && !Msg_Get( MSG_USE_BASE, name ) ) {
        Res_Flag = EXIT_FAILURE;
        write( STDOUT_FILENO, NO_RES_MESSAGE, NO_RES_SIZE );
    }
    return Res_Flag;
}


int Msg_Fini()
{
    int     retcode = EXIT_SUCCESS;

    if( Res_Flag == EXIT_SUCCESS ) {
        if ( CloseResFile( &hInstance ) != -1 ) {
            Res_Flag = EXIT_FAILURE;
        } else {
            retcode = EXIT_FAILURE;
        }
    }
    return retcode;
}
