/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2016 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Implementation of inet_makeaddr() for RDOS and Linux.
*
****************************************************************************/

#include "variety.h"
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

_WCRTLINK struct in_addr inet_makeaddr( int net, int __host )
{
    unsigned long addr;

    if (net < 128) {
        addr = (net << IN_CLASSA_NSHIFT) | (__host & IN_CLASSA_HOST);
    } else if (net < 65536) {
        addr = (net << IN_CLASSB_NSHIFT) | (__host & IN_CLASSB_HOST);
    } else if (net < 16777216L) {
        addr = (net << IN_CLASSC_NSHIFT) | (__host & IN_CLASSC_HOST);
    } else {
        addr = (net | __host);
    }

    addr = htonl( addr );
    return( *(struct in_addr*) &addr );
}
