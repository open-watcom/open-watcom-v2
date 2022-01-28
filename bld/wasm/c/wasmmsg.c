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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "asmglob.h"
#ifdef __WATCOMC__
  #include <process.h>
#endif
#include "banner.h"
//#include "wasmmsg.h"

#include "clibext.h"


enum {
    MSG_USAGE_COUNT = 0
    #define pick(num,etext,jtext) + 1
    #include "usage.gh"
    #undef pick
};

#if defined( INCL_MSGTEXT )

static const char *txtmsgs[] = {
    #define pick(num,etext,jtext) {etext},
    #include "wasmc.msg"
    #include "wasms.msg"
    #include "usage.gh"
    #undef pick
};

#else

#include "wressetr.h"
#include "wresset2.h"
#include "wreslang.h"

static  HANDLE_INFO     hInstance = { 0 };
static  unsigned        MsgShift;

#endif

static const char *FingerMsg[] = {
    banner1w( "Assembler", _WASM_VERSION_ ),
    banner2,
    banner2a( 1992 ),
    banner3,
    banner3a,
/* NB: it is disabled due to incompatibility with DOS/4G 2.x */
#if 0
#ifdef D32_NAME
    "DOS4/G Run-Time " D32_VERSION " " D32_COPYRIGHT,
#endif
#endif
    0
};

int PrintBanner( void )
{
    int     count = 0;

    if( !Options.banner_printed ) {
        Options.banner_printed = true;
        if( !Options.quiet ) {
            while( FingerMsg[count] != NULL ) {
                puts( FingerMsg[count++] );
            }
        }
    }
    return( count );
}

void PrintfUsage( void )
{
    char        msg_buff[MAX_MESSAGE_SIZE];
    int         msg;

    PrintBanner();
    if( !Options.quiet ) {
        puts( "" );
    }
    for( msg = MSG_USAGE_BASE; msg < MSG_USAGE_BASE + MSG_USAGE_COUNT; msg++ ) {
        MsgGet( msg, msg_buff );
        puts( msg_buff );
    }
}

void MsgPrintf( unsigned resourceid )
{
    char        msgbuf[MAX_MESSAGE_SIZE];

    PrintBanner();
    MsgGet( resourceid, msgbuf );
    printf( "%s", msgbuf );
}

void MsgPrintf1( unsigned resourceid, const char *token )
{
    char        msgbuf[MAX_MESSAGE_SIZE];

    PrintBanner();
    MsgGet( resourceid, msgbuf );
    printf( msgbuf, token );
}

bool MsgInit( void )
{
#if !defined( INCL_MSGTEXT )
    char        name[_MAX_PATH];

    hInstance.status = 0;
    if( _cmdname( name ) != NULL && OpenResFile( &hInstance, name ) ) {
        MsgShift = _WResLanguage() * MSG_LANG_SPACING;
        if( MsgGet( MSG_USAGE_BASE, name ) ) {
            return( true );
        }
    }
    CloseResFile( &hInstance );
    puts( NO_RES_MESSAGE );
    return( false );
#else
    return( true );
#endif
}

void MsgFini( void )
{
#if !defined( INCL_MSGTEXT )
    CloseResFile( &hInstance );
#endif
}

bool MsgGet( unsigned id, char *buffer )
{
#if defined( INCL_MSGTEXT )
    strncpy( buffer, txtmsgs[id], MAX_MESSAGE_SIZE - 1 );
    buffer[MAX_MESSAGE_SIZE - 1] = '\0';
#else
    if( hInstance.status == 0 || WResLoadString( &hInstance, id + MsgShift, (lpstr)buffer, MAX_MESSAGE_SIZE ) <= 0 ) {
        buffer[0] = '\0';
        return( false );
    }
#endif
    return( true );
}
