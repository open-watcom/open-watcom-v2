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
* Description:  WGML message definition.
*               adapted from wlink file (bld\wl\c\wlnkmsg.c)
*
****************************************************************************/

#ifdef __WATCOMC__
#pragma disable_message( 128 );
#endif

#define __STDC_WANT_LIB_EXT1__  1      /* use safer C library              */

#include "wgml.h"
#include "bool.h"
#include "wio.h"
#include "wressetr.h"
#include "wresset2.h"
#include "wreslang.h"

#ifdef __WATCOMC__
#pragma enable_message( 128 );
#endif

HANDLE_INFO Instance;

static int WGMLItself;
static unsigned MsgShift;               // 0 = english, 1000 for japanese


/***************************************************************************/
/*  Special seek routine, because resource file does not start at offset 0 */
/*  of wgml.exe                                                            */
/***************************************************************************/

static long res_seeek( WResFileID handle, long position, int where )
/******************************************************************/
{
    if( ( where == SEEK_SET ) && ( handle == WGMLItself ) ) {
        return( lseek( handle, position + FileShift, where ) - FileShift );
    } else {
        return( lseek( handle, position, where ) );
    }
}

WResSetRtns( open, close, read, write, res_seeek, tell, mem_alloc, mem_free );

/***************************************************************************/
/*  initialize messages from resource file                                 */
/***************************************************************************/

int init_msgs( void )
{
    int         error;
    char        fname[_MAX_PATH];

    error = FALSE;
    if( _cmdname( fname ) == NULL ) {
        error = TRUE;
    } else {
        error = OpenResFile( &Instance, fname );
        WGMLItself = Instance.handle;
        if( !error ) {
            error = FindResources( &Instance );
            if( !error ) {
                error = InitResources( &Instance );
            }
        }
        MsgShift = _WResLanguage() * MSG_LANG_SPACING;
        if( !error && !get_msg( ERR_DUMMY, fname, sizeof( fname ) ) ) {
            error = TRUE;
        }
    }
    if( error ) {
        if( Instance.handle != NIL_HANDLE )
            CloseResFile( &Instance );
        out_msg( "Resources not found\n" );
        g_suicide();
    }
    return( 1 );
}


/***************************************************************************/
/*  get a msg text string                                                  */
/***************************************************************************/

int get_msg( msg_ids resid, char *buff, size_t buff_len )
{
    if( WResLoadString( &Instance, resid + MsgShift, buff, buff_len ) != 0 ) {
        buff[0] = '\0';
        return( 0 );
    }
    return( 1 );
}

/***************************************************************************/
/*  end of msg processing                                                  */
/***************************************************************************/

void fini_msgs( void )
{
    CloseResFile( &Instance );
}
