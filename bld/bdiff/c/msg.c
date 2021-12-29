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
* Description:  Binary patch message output.
*
****************************************************************************/


#include "bdiff.h"
#ifdef __WATCOMC__
#include <process.h>
#endif
#include "wressetr.h"
#include "wresset2.h"
#include "wreslang.h"
#include "msg.h"

#include "clibext.h"


static  HANDLE_INFO     hInstance = { 0 };
static  unsigned        MsgShift;

bool GetMsg( char *buffer, int resourceid )
{
    if( hInstance.status == 0 || WResLoadString( &hInstance, resourceid + MsgShift, (lpstr)buffer, MAX_RESOURCE_SIZE ) <= 0 ) {
        buffer[0] = '\0';
        return( false );
    }
    return( true );
}

bool MsgInit( void )
{
    char        name[_MAX_PATH];
    char        msgbuf[MAX_RESOURCE_SIZE];

    hInstance.status = 0;
    if( _cmdname( name ) != NULL && OpenResFile( &hInstance, name ) ) {
        MsgShift = _WResLanguage() * MSG_LANG_SPACING;
        if( GetMsg( msgbuf, MSG_USAGE_BASE ) ) {
            return( true );
        }
    }
    CloseResFile( &hInstance );
    puts( NO_RES_MESSAGE );
    return( false );
}

static void OrderMsg( int order[], int num_arg, char *msg_ptr )
{
    int         i = 0;

    while( (msg_ptr = strpbrk( msg_ptr, "%")) != NULL ) {
        if( i >= num_arg )
            break;
        msg_ptr++;
        if( isdigit( *msg_ptr ) ) {
            order[i++] = atoi( msg_ptr ) - 1;
        }
        msg_ptr++;
    }
}

void MsgPrintf( int resourceid, va_list args )
{
    char        msgbuf[MAX_RESOURCE_SIZE];
    int         order[3] = { 0, 0, 0 };
    char        *argbuf[3];
    int         i;

    for( i = 0; i < 3; i++ ) {
        argbuf[i] = va_arg( args, char * );
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
    CloseResFile( &hInstance );
}

static void Err( int format, va_list args )
{
    char        msgbuf[MAX_RESOURCE_SIZE];

    GetMsg( msgbuf, MSG_ERROR );
    printf( msgbuf );
    MsgPrintf( format, args);
}

void PatchError( int format, ... )
{
    va_list     args;

    va_start( args, format );
    Err( format, args );
    puts( "" );
    va_end( args );
    MsgFini();
    exit( EXIT_FAILURE );
}

void FilePatchError( int format, ... )
{
    va_list     args;
    int         err;

    va_start( args, format );
    err = errno;
    Err( format, args );
    printf( ": %s\n", strerror( err ) );
    va_end( args );
    MsgFini();
    exit( EXIT_FAILURE );
}

void FileCheck( FILE *fd, const char *name )
{
    if( fd == NULL ) {
        FilePatchError( ERR_CANT_OPEN, name );
    }
}

void SeekCheck( int rc, const char *name )
{
    if( rc != 0 ) {
        FilePatchError( ERR_IO_ERROR, name );
    }
}
