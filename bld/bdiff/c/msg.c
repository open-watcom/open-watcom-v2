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
* Description:  Binary patch message output.
*
****************************************************************************/


#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <process.h>
#include <sys/types.h>
#include "wio.h"

#include "bdiff.h"
#include "wressetr.h"
#include "wresset2.h"
#include "wreslang.h"
#include "watcom.h"
#include "msg.h"

#define STDOUT_HANDLE   ((int)1)

static  HANDLE_INFO     hInstance = { 0 };
static  unsigned        MsgShift;
extern  long            FileShift;

#define NO_RES_MESSAGE "Error: could not open message resource file.\r\n"
#define NO_RES_SIZE (sizeof(NO_RES_MESSAGE)-1)


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

int GetMsg( char *buffer, int resourceid )
{
    if( !LoadString( &hInstance, resourceid + MsgShift, (LPSTR)buffer, MAX_RESOURCE_SIZE ) == 0 ) {
        buffer[0] = '\0';
        return( 0 );
    }
    return( 1 );
}

int MsgInit( void )
{
    int         initerror;
    char        name[_MAX_PATH];

    hInstance.handle = NIL_HANDLE;
    if( _cmdname( name ) == NULL ) {
        initerror = 1;
    } else {
        initerror = OpenResFile( &hInstance, name );
        if( !initerror ) {
            initerror = FindResources( &hInstance );
            if( !initerror ) {
                initerror = InitResources( &hInstance );
            }
        }
    }
    MsgShift = _WResLanguage() * MSG_LANG_SPACING;
    if( !initerror && !GetMsg( name, MSG_USAGE_FIRST ) ) {
        initerror = 1;
    }
    if( initerror ) {
        write( STDOUT_FILENO, NO_RES_MESSAGE, NO_RES_SIZE );
        MsgFini();
        return( 0 );
    }
    return( 1 );
}

static void OrderMsg ( int order[], int num_arg, char *msg_ptr )
{
    int         i = 0;

    msg_ptr = strpbrk( msg_ptr, "%" );
    while( msg_ptr != NULL && i < num_arg ) {
        msg_ptr++;
        if( isdigit( *msg_ptr ) ) {
            order[i] = atoi( msg_ptr ) - 1;
            i++;
        }
        msg_ptr++;
        msg_ptr = strpbrk( msg_ptr, "%" );
    }
}

void MsgPrintf( int resourceid, va_list arglist )
{
    char        msgbuf[MAX_RESOURCE_SIZE];
    int         order[3] = { 0, 0, 0 };
    char        *argbuf[3];
    int         i;

    for( i = 0; i < 3; i++ ) {
        argbuf[i] = va_arg( arglist, char * );
    }
    GetMsg( msgbuf, resourceid );
    OrderMsg( order, 3, msgbuf );
    printf( msgbuf, argbuf[order[0]], argbuf[order[1]], argbuf[order[2]] );
}

void Message( int format, ... )
{
    va_list     args;

    va_start( args, format );
    MsgPrintf( format, args );
    va_end( args );
}

void MsgFini( void )
{
    if( hInstance.handle != NIL_HANDLE ) {
        CloseResFile( &hInstance );
        hInstance.handle = NIL_HANDLE;
    }
}
