/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2008-2013 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Implementation of recvfrom() for Linux.
*
****************************************************************************/


#include "variety.h"
#include <sys/types.h>
#include <sys/socket.h>
#include "linuxsys.h"

_WCRTLINK int recvfrom(int s, void *buf, size_t len, int flags, struct sockaddr *from, socklen_t *fromlen)
{
    unsigned long args[6];
    args[0] = (unsigned long)s;
    args[1] = (unsigned long)buf;
    args[2] = (unsigned long)len;
    args[3] = (unsigned long)flags;
    args[4] = (unsigned long)from;
    args[5] = (unsigned long)fromlen;
    return( __socketcall( SYS_RECVFROM, args ) );
}

