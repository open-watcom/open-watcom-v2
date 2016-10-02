/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2016 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Implementation of gethostbyaddr
*
* Author: J. Armstrong
*
****************************************************************************/

#include "variety.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include "rtdata.h"
#include "rterrno.h"
#include "thread.h"


_WCRTLINK struct hostent *gethostbyaddr(const void *addr, socklen_t len, int type)
{
    struct hostent *ret;
    int i;

    if(addr == NULL) {
        _RWD_errno = EINVAL;
        return( NULL );
    }

    sethostent( 1 );

    ret = gethostent();
    while(ret != NULL) {
        if(ret->h_addrtype == type && ret->h_length == (int)len && ret->h_addr_list != NULL)
        {
            for( i=0; ret->h_addr_list[i] != NULL; i++ ) {
                if(memcmp(ret->h_addr_list[i], addr, len) == 0)
                    goto gethostbyaddr_cleanup;
            }
        }

        ret = gethostent();
    }

gethostbyaddr_cleanup:

    endhostent();

    return( ret );
}
