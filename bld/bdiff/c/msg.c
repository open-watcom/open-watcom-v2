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


#include "bdiff.h"
#include "wressetr.h"
#include "wresset2.h"
#include "wreslang.h"
#include "msg.h"

#include "clibext.h"

#define NO_RES_MESSAGE "Error: could not open message resource file."

static  HANDLE_INFO     hInstance = { 0 };
static  unsigned        MsgShift;

static WResFileOffset res_seek( WResFileID handle, WResFileOffset position, int where )
/* fool the resource compiler into thinking that the resource information
 * starts at offset 0 */
{
    if( where == SEEK_SET ) {
        return( lseek( handle, position + FileShift, where ) - FileShift );
    } else {
        return( lseek( handle, position, where ) );
    }
}

WResSetRtns( open, close, read, write, res_seek, tell, bdiff_malloc, bdiff_free );

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
    char        name[_MAX_PATH];

    hInstance.handle = NIL_HANDLE;
    if( _cmdname( name ) != NULL && !OpenResFile( &hInstance, name ) ) {
        if( !FindResources( &hInstance ) && !InitResources( &hInstance ) ) {
            MsgShift = _WResLanguage() * MSG_LANG_SPACING;
            if( GetMsg( name, MSG_USAGE_FIRST ) ) {
                return( 1 );
            }
        }
    }
    printf( "%s\n", NO_RES_MESSAGE );
    MsgFini();
    return( 0 );
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
    printf( "\n" );
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

void ReadCheck( size_t rc, size_t size, const char *name )
{
    if( rc != size ) {
        FilePatchError( ERR_IO_ERROR, name );
    }
}
