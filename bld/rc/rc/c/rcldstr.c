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

#if defined( __WATCOMC__ ) || !defined( __UNIX__ )
#include <process.h>
#endif
#include "wio.h"
#include "global.h"
#include "layer0.h"
#include "errors.h"
#include "rcldstr.h"
#include "wresset2.h"
#include "iortns.h"

extern HANDLE_INFO  Instance;

#if defined( INCL_MSGTEXT )

static char *StringTable[] = {
    "",                             // message ID's start at 1
    #include "incltext.gh"
};

int InitRcMsgs( const char *fname )
{
    fname = fname;
    return( 1 );
}

int GetRcMsg( unsigned resid, char *buff, unsigned buff_len )
{
    buff_len = buff_len;
    strcpy( buff, StringTable[resid] );
    return( 1 );
}

void FiniRcMsgs( void ) {}

#else

#include "wreslang.h"

static unsigned MsgShift;

int InitRcMsgs( const char *fname )
{
    int         error;
    WResFileID  (* oldopen)(const char *, int, ...);
    char        testbuf[1];

    error = FALSE;

    /* swap open functions so this file handle is not buffered.
        * This makes it easier for layer0 to fool WRES into thinking
        * that the resource information starts at offset 0 */
    oldopen = WResRtns.open;
    WResRtns.open = open;
    OpenResFile( &Instance, fname );
    WResRtns.open = oldopen;

    if( Instance.handle == -1 )
        error = TRUE;
    if( !error ) {
        RegisterOpenFile( Instance.handle );
        error = FindResources( &Instance );
    }
    if( !error ) {
        error = InitResources( &Instance );
    }
    MsgShift = _WResLanguage() * MSG_LANG_SPACING;
    if( !error && !GetRcMsg( USAGE_MSG_FIRST, testbuf, sizeof( testbuf ) ) ) {
        error = TRUE;
    }
    if( error ) {
        if( Instance.handle != -1 )
            CloseResFile( &Instance );
        RcFatalError( ERR_RCSTR_NOT_FOUND );
    }
    return( 1 );
}

int GetRcMsg( unsigned resid, char *buff, unsigned buff_len )
{
    if( WResLoadString( &Instance, resid + MsgShift, buff, buff_len ) != 0 ) {
        buff[0] = '\0';
        return( 0 );
    }
    return( 1 );
}

void FiniRcMsgs( void )
{
    CloseResFile( &Instance );
}

#endif
