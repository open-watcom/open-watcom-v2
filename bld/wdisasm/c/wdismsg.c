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
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <process.h>

#include "watcom.h"
#include "wdismsg.h"
#include "wressetr.h"
#include "wreslang.h"
#include "disasm.h"


#define NIL_HANDLE      ((int)-1)

static  HANDLE_INFO     hInstance = { 0 };
static  unsigned        MsgShift;
extern  long            FileShift;

#define NO_RES_MESSAGE "Error: could not open message resource file.\r\n"
#define NO_RES_SIZE (sizeof(NO_RES_MESSAGE)-1)

#define EXE_EXT         ".exe"


static long res_seek( int handle, long position, int where )
/* fool the resource compiler into thinking that the resource information
 * starts at offset 0 */
{
    if( where == SEEK_SET ) {
        return( lseek( handle, position + FileShift, where ) - FileShift );
    } else {
        return( lseek( handle, position, where ) );
    }
}

WResSetRtns( open, close, read, write, res_seek, tell, malloc, free );

int MsgInit()
{
    int         initerror;
    char        name[_MAX_PATH];

    hInstance.handle = NIL_HANDLE;
    if( _cmdname( name ) == NULL ) {
        initerror = 1;
    } else {
        OpenResFile( &hInstance, name );
        if( hInstance.handle == NIL_HANDLE ) {
            initerror = 1;
        } else {
            initerror = FindResources( &hInstance );
            if( !initerror ) {
                initerror = InitResources( &hInstance );
            }
        }
    }
    MsgShift = WResLanguage() * MSG_LANG_SPACING;
    if( !initerror && !MsgGet( MSG_USE_BASE, name ) ) {
        initerror = 1;
    }
    if( initerror ) {
        write( STDOUT_FILENO, NO_RES_MESSAGE, NO_RES_SIZE );
        MsgFini();
        return( 0 );
    }
    return( 1 );
}

int MsgGet( int resourceid, char *buffer )
{
    if ( LoadString( &hInstance, resourceid + MsgShift,
                (LPSTR) buffer, MAX_RESOURCE_SIZE ) != 0 ) {
        buffer[0] = NULLCHAR;
        return( 0 );
    }
    return( 1 );
}

void MsgPutUsage()
{
    char        msg_buff[MAX_RESOURCE_SIZE];
    int         i;
    char        only_newline;
//    int         previous_null = 0;

    for( i = MSG_USE_BASE;; i++ ) {
        only_newline = 0;
        MsgGet( i, msg_buff );
        if( (msg_buff[ 0 ] == '~')&&(msg_buff[ 1 ] == 0 ) ) {
            only_newline = 1;
        }
#if 0
This section of the code is killed for now.  If, sometime in the future,
there are multiple pages of usage messages for this program, include the code
and add a waitforkey() function.
        if( previous_null ) {
            if( msg_buff[0] != '\0' ) {
                waitforkey();
                if( only_newline ) {
                    puts( "" );
                } else {
                    puts( msg_buff );
                }
                previous_null = 0;
            } else break;

        } else if( msg_buff[0] == '\0' ) {
            previous_null = 1;
        } else {
#else
        if( msg_buff[ 0 ] == 0 ) {
            break;
        }
#endif
            if( only_newline ) {
                puts( "" );
            } else {
                puts( msg_buff );
            }
//        }
    }
}

void MsgFini()
{
    if( hInstance.handle != NIL_HANDLE ) {
        CloseResFile( &hInstance );
        hInstance.handle = NIL_HANDLE;
    }
}

void MsgSubStr( char *strptr, char *para, char specifier )
{
    char        buff[MAX_RESOURCE_SIZE];
    char        *src;
    int         i;

    src = strptr;
    for( i = 0; *src != NULLCHAR; src++ ) {
        if( i == MAX_RESOURCE_SIZE - 1 ) break;
        if( *src != '%' ) {
            buff[i++] = *src;
        } else {
            if( *(src+1) == specifier ) {
                while( *para != NULLCHAR ) {
                    buff[i++] = *para++;
                    if( i == MAX_RESOURCE_SIZE - 1 ) break;
                }
                src++;
            } else if( specifier == '1' ) { /*keep '%' for next MsgSubStr*/
                buff[i++] = *src;
                buff[i++] = *(++src);
            } else if( *(src+1) == '%' ) {
                buff[i++] = *(++src);
            }
        }
    }
    buff[i] = NULLCHAR;
    strcpy( strptr, buff );
}

void MsgChgeSpec( char *strptr, char specifier )
{
    int         i;

    for( i = 0; strptr[i] != NULLCHAR; i++ ) {
        if( strptr[i] == '%' && strptr[i+1] == specifier ) {
            strptr[i+1] = 's';
            break;
        }
    }
}
