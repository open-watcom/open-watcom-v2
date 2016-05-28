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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#if defined( __WATCOMC__ ) || !defined( __UNIX__ )
#include <process.h>
#endif
#include "wio.h"
#include "bool.h"
#include "wtmsg.h"
#include "wressetr.h"
#include "wresset2.h"
#include "wreslang.h"
#include "clibext.h"


static  HANDLE_INFO     hInstance = { 0 };
static  unsigned        MsgShift;

#define NO_RES_MESSAGE "Error: could not open message resource file.\r\n"
#define NO_RES_SIZE (sizeof( NO_RES_MESSAGE ) - 1)


static WResFileOffset resSeek( WResFileID handle, WResFileOffset position, int where )
/* fool the resource compiler into thinking that the resource information
 * starts at offset 0 */
{
    if( where == SEEK_SET ) {
        return( lseek( handle, position + FileShift, where ) - FileShift );
    } else {
        return( lseek( handle, position, where ) );
    }
}

WResSetRtns( open, close, read, write, resSeek, tell, malloc, free );

int MsgInit( void )
/******************/
{
    char        name[_MAX_PATH];
    char        dummy[MAX_RESOURCE_SIZE];

    hInstance.handle = NIL_HANDLE;
    if( _cmdname( name ) != NULL && !OpenResFile( &hInstance, name ) ) {
        if( !FindResources( &hInstance ) && !InitResources( &hInstance ) ) {
            MsgShift = _WResLanguage() * MSG_LANG_SPACING;
            if( MsgGet( MSG_USAGE_BASE, dummy ) ) {
                return( true );
            }
        }
    }
    write( STDOUT_FILENO, NO_RES_MESSAGE, NO_RES_SIZE );
    MsgFini();
    return( false );
}

int MsgGet( int resourceid, char *buffer )
/****************************************/
{
    if( LoadString( &hInstance, resourceid + MsgShift, (LPSTR)buffer, MAX_RESOURCE_SIZE ) <= 0 ) {
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
        if( ch == '\0' ) break;
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
    if( hInstance.handle != NIL_HANDLE ) {
        CloseResFile( &hInstance );
        hInstance.handle = NIL_HANDLE;
    }
}
