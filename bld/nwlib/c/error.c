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
static  int             Res_Flag;
static  unsigned        MsgShift;

static long res_seek( WResFileID handle, long position, int where )
/* fool the resource compiler into thinking that the resource information
 * starts at offset 0 */
{
    if( where == SEEK_SET ) {
        return( lseek( handle, position + FileShift, where ) - FileShift );
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
        if( !FindResources( &hInstance ) && !InitResources( &hInstance ) ) {
            MsgShift = _WResLanguage() * MSG_LANG_SPACING;
            Res_Flag = EXIT_SUCCESS;
            return;
        }
        CloseResFile( &hInstance );
    }
    Res_Flag = EXIT_FAILURE;
    FatalResError();
}

void MsgGet( int resourceid, char *buffer )
{
    if( LoadString( &hInstance, resourceid + MsgShift, (LPSTR)buffer, MAX_ERROR_SIZE ) != 0 ) {
        buffer[ 0 ] = '\0';
    }
}

void FiniMsg( void )
{
    if( Res_Flag == EXIT_SUCCESS ) {
        if( CloseResFile( &hInstance ) ) {
            Res_Flag = EXIT_FAILURE;
            longjmp( Env, 1 );
        }
    }
}
#endif
