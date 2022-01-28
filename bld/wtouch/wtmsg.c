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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#if defined( __WATCOMC__ ) || !defined( __UNIX__ )
#include <process.h>
#endif
#include "bool.h"
#include "wtmsg.h"
#include "wressetr.h"
#include "wresset2.h"
#include "wreslang.h"

#include "clibext.h"


static  HANDLE_INFO     hInstance = { 0 };
static  unsigned        MsgShift;

bool MsgInit( void )
/******************/
{
    char        name[_MAX_PATH];
    char        dummy[MAX_RESOURCE_SIZE];

    hInstance.status = 0;
    if( _cmdname( name ) != NULL && OpenResFile( &hInstance, name ) ) {
        MsgShift = _WResLanguage() * MSG_LANG_SPACING;
        if( MsgGet( MSG_USAGE_BASE, dummy ) ) {
            return( true );
        }
    }
    CloseResFile( &hInstance );
    puts( NO_RES_MESSAGE );
    return( false );
}

bool MsgGet( int resourceid, char *buffer )
/*****************************************/
{
    if( hInstance.status == 0 || WResLoadString( &hInstance, resourceid + MsgShift, (lpstr)buffer, MAX_RESOURCE_SIZE ) <= 0 ) {
        buffer[0] = '\0';
        return( false );
    }
    return( true );
}

static char *strApp( char *dest, const char *src )
/************************************************/
{
    while( (*dest = *src) != '\0' ) {
        ++dest;
        ++src;
    }
    return( dest );
}

void MsgSubStr( int resourceid, char *buff, char *p )
/***************************************************/
{
    char        msgbuff[MAX_RESOURCE_SIZE];
    char        *src;
    char        *dest;
    char        ch;

    MsgGet( resourceid, msgbuff );
    src = msgbuff;
    dest = buff;
    for(;;) {
        ch = *src++;
        if( ch == '\0' )
            break;
        if( ch != '%' ) {
            *dest++ = ch;
        } else {
            ch = *src++;
            switch( ch ) {
            case 's' :
                dest = strApp( dest, p );
                break;
            case 'Z' :
                {
                    char    tmpbuff[MAX_RESOURCE_SIZE];

                    MsgGet( MSG_SYS_ERR_0+errno, tmpbuff );
                    dest = strApp( dest, tmpbuff );
                }
                break;
            default :
                *dest++ = ch;
                break;
            }
        }
    }
    *dest = '\0';
}

void MsgFini( void )
/******************/
{
    CloseResFile( &hInstance );
}
