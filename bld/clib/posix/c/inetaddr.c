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
* Description:  Implementation of inet_addr() for little-endian cpus.
*
****************************************************************************/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

_WCRTLINK uint32_t inet_addr(const char *cp)
{
    uint32_t ret, val;
    int shift = 0;

    ret = val = 0;
    if ( *cp ) do {
        if ( *cp >= '0' && *cp <= '9' ) {
            val = val*10 + (*cp - '0');
        } else if ( *cp == '.' || *cp == '\0' ) {
            if ( val > 255 )
                return INADDR_NONE;
            ret |= ( val << shift );
            shift += 8;
            val = 0;
        } else {
            return INADDR_NONE;
        }
        cp++;
    } while ( cp[-1] );
    if ( shift != 32 )
        return INADDR_NONE;
    return ret;
}
