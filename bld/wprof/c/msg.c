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
* Description:  Profiler error message display routines.
*
****************************************************************************/


#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include "wio.h"
#include "common.h"
#include "myassert.h"
#include "msg.h"
#include "aui.h"
#include "utils.h"

#define MAX_MSG_LEN     200

extern  bool    WPWndInitDone;


STATIC void doErr( char * msg, va_list args )
/*******************************************/
{
    char        *dest;
    char        *str_arg;
    size_t      str_len;
    size_t      msg_len;
    char        num_buff[11];
    char        buff[MAX_MSG_LEN + 3];

    msg_len = 0;
    dest = buff;
    while( *msg != '\0' ) {
        if( *msg != '%' ) {
            *dest++ = *msg;
            msg_len++;
        } else {
            ++msg;
/**/        myassert( *msg != 0 );
            if( *msg == '%' ) {
                *dest++ = '%';
                msg_len++;
            } else {
                if( *msg == 's' ) {
                    str_arg = va_arg( args, char * );
                    str_len = strlen( str_arg );
                } else if( *msg == 'd' ) {
                    sprintf( num_buff, "%d", va_arg( args, int ) );
                    str_arg = num_buff;
                    str_len = strlen( str_arg );
                } else {
                    str_arg = msg - 1;
                    str_len = 2;
                }
                if( msg_len + str_len > MAX_MSG_LEN ) {
                    str_len = MAX_MSG_LEN - msg_len;
                }
                memcpy( dest, str_arg, str_len );
                msg_len += str_len;
                dest += str_len;
            }
        }
        if( msg_len == MAX_MSG_LEN ) 
            break;
        ++msg;
    }
    *dest++ = '\r';
    *dest = 0;
    if( WPWndInitDone ) {
        Ring();
        WndDisplayMessage( buff, "Error", GUI_INFORMATION );
    } else {
        *--dest = '\n';
        write( STDOUT_FILENO, buff, strlen( buff ) );
    }
}


void ErrorMsg( char * msg, ... )
/******************************/
{
    va_list     args;

    va_start( args, msg );
    doErr( msg, args );
    va_end( args );
}


void fatal( char * msg, ... )
/***************************/
{
    va_list     args;

    va_start( args, msg );
    doErr( msg, args );
    va_end( args );
    if( WPWndInitDone ) {
        GUICleanup();
        GUIMemClose();
    }
    exit( 1 );
}
