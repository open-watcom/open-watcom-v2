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
* Description:  Implementation of getservbyname() for Linux.
*
* Author: J. Armstrong
*
****************************************************************************/


#include "variety.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>

#include "rterrno.h"

#define SAFE_SAME_STR(x, y)  (x != NULL && y != NULL && strcmp(x,y) == 0)

_WCRTLINK struct servent *getservbyname( const char *name, const char *proto )
{
    struct servent *ret;

    if( name == NULL ) {
        _RWD_errno = EINVAL;
        return( NULL );
    }

    setservent( 1 );

    do {

        ret = getservent();

    } while( ret != NULL &&
             !( SAFE_SAME_STR( name, ret->s_name ) &&
               ( proto == NULL || SAFE_SAME_STR( proto, ret->s_proto ) ) ) );

    endservent();

    return( ret );
}
