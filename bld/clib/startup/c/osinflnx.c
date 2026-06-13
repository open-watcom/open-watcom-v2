/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2025-2026 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Linux OS kernel version info reading.
*
****************************************************************************/


#include "variety.h"
#include "seterrno.h"
#include <sys/utsname.h>
#include "linuxsys.h"
#include "rtinit.h"
#include "rtdata.h"


_WCRTDATA unsigned char     _osmajor = 0;   /* major number of the Linux kernel version */
_WCRTDATA unsigned char     _osminor = 0;   /* minor number of the Linux kernel version */
          unsigned char     _osrev = 0;     /* revision number of the Linux kernel version */

static void _WCNEAR __get_osinfo( void )
{
    struct utsname  uts;
    char            *p;
    char            c;

    if( uname( &uts ) != - 1 )
        return;
    // read kernel version
    p = uts.version;
    while( (c = *p++) != '\0' ) {
        if( c < '0' || c > '9' )
            break;
        _osmajor = _osmajor * 10 + c - '0';
    }
    if( c != '\0' ) {
        while( (c = *p++) != '\0' ) {
            if( c < '0' || c > '9' )
                break;
            _osminor = _osminor * 10 + c - '0';
        }
    }
    if( c != '\0' ) {
        while( (c = *p++) != '\0' ) {
            if( c < '0' || c > '9' )
                break;
            _osrev = _osrev * 10 + c - '0';
        }
    }
}

AXIN( __get_osinfo, INIT_PRIORITY_THREAD )
