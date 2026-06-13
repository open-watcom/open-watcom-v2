/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2026 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Linux uname() implementation.
*
****************************************************************************/


#include "variety.h"
#include <string.h>
#include <unistd.h>
#include <sys/utsname.h>
#include "seterrno.h"
#include "linuxsys.h"


#define  _OLD_UTSNAME_LENGTH    9

_WCRTLINK int uname( struct utsname *__name )
{
    syscall_res     res;

    if( sys_call1( SYS_uname, (u_long)__name ) == 0 )
        return( 0 );
    if( sys_call1( SYS_olduname, (u_long)__name ) == 0 ) {
        __name->domainname[0] = '\0';
        return( 0 );
    }
    if( (res = sys_call1( SYS_oldolduname, (u_long)__name )) != 0 ) {
        __syscall_return( int, res );
    }
    __name->domainname[0] = '\0';
    strcpy( __name->machine, (char *)__name + 4 * _OLD_UTSNAME_LENGTH );
    strcpy( __name->version, (char *)__name + 3 * _OLD_UTSNAME_LENGTH );
    strcpy( __name->release, (char *)__name + 2 * _OLD_UTSNAME_LENGTH );
    strcpy( __name->nodename, (char *)__name + 1 * _OLD_UTSNAME_LENGTH );
    return( 0 );
}
