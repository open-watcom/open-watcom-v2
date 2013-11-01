/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2013 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Implementation of getpeername() for Linux.
*
****************************************************************************/

#include <sys/types.h>
#include <sys/socket.h>
#include "linuxsys.h"

_WCRTLINK int getpeername( int __s, struct sockaddr *__name, socklen_t *__namelen )
{
    unsigned long   args[3];

    args[0] = (unsigned long)__s;
    args[1] = (unsigned long)__name;
    args[2] = (unsigned long)__namelen;
    return( __socketcall( SYS_GETPEERNAME, args ) );
}
