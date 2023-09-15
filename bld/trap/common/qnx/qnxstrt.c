/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  QNX trap file startup and cover routines.
*
****************************************************************************/


#include <stdlib.h>
#include <malloc.h>     /* necessary for near/far versions */
#include <sys/types.h>
#include <signal.h>
#include "trpld.h"
#include "trpcomm.h"
#include "qnxstrt.h"

void                        *_slib_func[2];
char                        **dbg_environ;  /* pointer to environment strings */
static const trap_callbacks *Client;
static const trap_requests  ImpInterface = { TrapInit, TrapRequest, TrapFini } ;

#ifdef __WATCOMC__
const char __based( __segname( "_CODE" ) ) Signature[4] = { TRAPSIG };
#endif

const trap_requests *TrapLoad( const trap_callbacks *client )
{
    Client = client;
    if( Client->len <= offsetof(trap_callbacks,signal) )
        return( NULL );
    dbg_environ = *Client->environp;
    _slib_func[0] = Client->_slib_func[0];
    _slib_func[1] = Client->_slib_func[1];
    return( &ImpInterface );
}

void __near *_nmalloc( unsigned size )
{
    return( malloc( size ) );
}

void *malloc( unsigned size )
{
    return( Client->malloc( size ) );
}

void __near *_nrealloc( void __near *ptr, unsigned size )
{
    return( realloc( ptr, size ) );
}

void *realloc( void *ptr, unsigned size )
{
    return( Client->realloc( ptr, size ) );
}

void _nfree( void __near *ptr )
{
    free( ptr );
}

void free( void *ptr )
{
    Client->free( ptr );
}

char *getenv( const char *name )
{
    return( Client->getenv( name ) );
}

void    (*signal( int __sig, void (*__func)(int) ))(int)
{
    return( Client->signal( __sig, __func ) );
}
