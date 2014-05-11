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
* Description:  Helper functions for using strings loaded from resources.
*
****************************************************************************/


#include "precomp.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "mem.h"
#include "ldstr.h"

#define ALLOC_STRING( x )       AllocRCString( x )
#define FREE_STRING( x )        FreeRCString( x )

/*
 * RCsprintf - do sprintf with a format string from a resource
 */
int RCsprintf( char *buf, MSGID fmtid, ... )
{
    va_list     al;
    char        *fmtstr;
    int         ret;

    va_start( al, fmtid );
    fmtstr = ALLOC_STRING( fmtid );
    ret = vsprintf( buf, fmtstr, al );
    FREE_STRING( fmtstr );
    va_end( al );
    return( ret );

} /* RCsprintf */

/*
 * RCfprintf - do fprintf with a format string from a resource
 */
void RCfprintf( FILE *fp, MSGID strid, ... )
{
    va_list     al;
    char        *str;

    va_start( al, strid );
    str = ALLOC_STRING( strid );
    vfprintf( fp, str, al );
    FREE_STRING( str );
    va_end( al );

} /* RCfprintf */

/*
 * RCvfprintf - do vfprintf with a format string from a resource
 */
void RCvfprintf( FILE *fp, MSGID strid, va_list al )
{
    char        *str;

    str = ALLOC_STRING( strid );
    vfprintf( fp, str, al );
    FREE_STRING( str );

} /* RCvfprintf */

/*
 * RCMessageBox - display a message box with a string from a resource
 */
int RCMessageBox( HWND hwnd, MSGID msgid, char *title, UINT type )
{
    char        *msg;
    int         ret;

    msg = ALLOC_STRING( msgid );
    ret = MessageBox( hwnd, msg, title, type );
    FREE_STRING( msg );
    return( ret );

} /* RCMessageBox */
