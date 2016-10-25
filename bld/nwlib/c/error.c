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
* Description:  Message output for librarian.
*
****************************************************************************/


#include "wlib.h"
#ifdef __WATCOMC__
#include <process.h>
#endif
#include "wio.h"

#include "clibext.h"


#if defined( INCL_MSGTEXT )

static char *msg_text_array[] = {
    "",
    #include "incltext.gh"
};

void InitMsg( void ) {}

void MsgGet( int resourceid, char *buffer )
{
    strcpy( buffer, msg_text_array[resourceid] );
}

void FiniMsg( void ) {}

#else

#include "wressetr.h"
#include "wresset2.h"
#include "wreslang.h"
#if !defined( IDE_PGM )
#include "clibint.h"
#endif

static  HANDLE_INFO     hInstance = { 0 };
static  unsigned        MsgShift;
static  bool            res_failure = true;

static WResFileOffset res_seek( WResFileID handle, WResFileOffset position, int where )
/* fool the resource compiler into thinking that the resource information
 * starts at offset 0 */
{
    if( where == SEEK_SET ) {
        return( lseek( handle, position + WResFileShift, where ) - WResFileShift );
    } else {
        return( lseek( handle, position, where ) );
    }
}

WResSetRtns( open, close, read, write, res_seek, tell, MemAllocGlobal, MemFreeGlobal );

void InitMsg( void )
{
#if defined( IDE_PGM ) || !defined( __WATCOMC__ )
    char    imageName[_MAX_PATH];
#else
    char    *imageName;
#endif

    hInstance.handle = NIL_HANDLE;
#if defined( IDE_PGM )
    _cmdname( imageName );
#elif !defined( __WATCOMC__ )
    get_dllname( imageName, sizeof( imageName ) );
#else
    imageName = _LpDllName;
#endif
    if( !OpenResFile( &hInstance, imageName ) ) {
        res_failure = false;
        if( !FindResources( &hInstance ) && !InitResources( &hInstance ) ) {
            MsgShift = _WResLanguage() * MSG_LANG_SPACING;
            return;
        }
        CloseResFile( &hInstance );
    }
    res_failure = true;
    FatalResError();
}

void MsgGet( int resourceid, char *buffer )
{
    if( res_failure || WResLoadString( &hInstance, resourceid + MsgShift, (LPSTR)buffer, MAX_ERROR_SIZE ) <= 0 ) {
        buffer[0] = '\0';
    }
}

void FiniMsg( void )
{
    if( hInstance.handle != NIL_HANDLE ) {
        if( CloseResFile( &hInstance ) ) {
            hInstance.handle = NIL_HANDLE;
            res_failure = true;
            longjmp( Env, 1 );
        }
    }
}
#endif
