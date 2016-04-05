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


#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "bool.h"
#include "wrmaini.h"
#include "wrmsg.h"
#include "errprt.h"
#include "rcldstr.h"

#define ERRPRT_BUFFER 512
static char buf[ERRPRT_BUFFER];

static void WRDisplayRCMsg( const char *msg )
{
    char        *title;

    title = WRAllocRCString( WR_WRCMSG );

    MessageBox( (HWND)NULL, msg, title,
                MB_ICONEXCLAMATION | MB_OK | MB_APPLMODAL );

    if( title != NULL ) {
        WRFreeRCString( title );
    }
}

int RcMsgFprintf( FILE *fp, OutPutInfo *info, const char *format, ... )
{
    int         err;
    va_list     args;
    char        *fmt;
    char        *p;

    fp = fp;
    p = buf;
    if( info->flags & OUTFLAG_FILE ) {
        err = sprintf( p, "%s(%d): ", info->file, info->lineno );
        if( err < 0 ) {
            return( err );
        }
        p += err;
    }
    switch( info->severity ) {
    case SEV_WARNING:
        fmt = "Warning! %d: ";
        break;
    case SEV_ERROR:
        fmt = "Error! %d: ";
        break;
    case SEV_FATAL_ERR:
        fmt = "Fatal Error! %d: ";
        break;
    default:
        fmt = "%d: ";
        break;
    }
    err = sprintf( p, fmt, info->errid );
    if( err < 0 ) {
        return( err );
    }
    p += err;
    va_start( args, format );
    err = vsprintf( p, format, args );
    va_end( args );
    if( err < 0 ) {
        return( err );
    }
    p += err;
    if( p > buf ) {
        WRDisplayRCMsg ( buf );
    }
    return( p - buf );
}

bool GetRcMsg( unsigned resid, char *buff, int buff_len )
{
    if( LoadString( WRGetInstance(), resid, buff, buff_len ) != 0 ) {
        buff[0] = '\0';
        return( false );
    }
    return( true );
}

void InitOutPutInfo( OutPutInfo *info )
{
    info->flags = 0;
}
