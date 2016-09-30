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
* Description:  Message resource access routines.
*
****************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#if defined( __WATCOMC__ )
    #include <process.h>
#endif
#include "wio.h"
#include "wressetr.h"
#include "wresset2.h"
#include "wreslang.h"
#include "msg.h"

#include "clibext.h"


#define NO_RES_MESSAGE "Error: could not open message resource file\r\n"
#define NO_RES_SIZE (sizeof(NO_RES_MESSAGE)-1)

static HANDLE_INFO      hInstance = {0};
static unsigned         MsgShift;
static bool             res_failure = true;

static WResFileOffset res_seek( WResFileID handle, WResFileOffset position, int where )
{
    if( where == SEEK_SET ) {
        return( lseek( handle, position + WResFileShift, where ) - WResFileShift );
    } else {
        return( lseek( handle, position, where ) );
    }
}

WResSetRtns( open, close, read, write, res_seek, tell, malloc, free );

bool MsgInit( void )
{
    char            name[_MAX_PATH];

    hInstance.handle = NIL_HANDLE;
    if( _cmdname( name ) != NULL && !OpenResFile( &hInstance, name ) ) {
        res_failure = false;
        if( !FindResources( &hInstance ) && !InitResources( &hInstance ) ) {
            MsgShift = _WResLanguage() * MSG_LANG_SPACING;
            if( MsgGet( WDIS_LITERAL_BASE, name ) ) {
                return( true );
            }
        }
        MsgFini();
    }
    write( STDOUT_FILENO, NO_RES_MESSAGE, NO_RES_SIZE );
    res_failure = true;
    return( false );
}

bool MsgGet( int resourceid, char *buffer )
{
    if( res_failure || WResLoadString( &hInstance, resourceid + MsgShift, (LPSTR)buffer, MAX_RESOURCE_SIZE ) <= 0 ) {
        buffer[0] = '\0';
        return( false );
    }
    return( true );
}

void MsgFini( void )
{
    if( hInstance.handle != NIL_HANDLE ) {
        CloseResFile( &hInstance );
        hInstance.handle = NIL_HANDLE;
    }
}
