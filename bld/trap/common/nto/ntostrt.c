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
* Description:  QNX Neutrino trap file startup code.
*
****************************************************************************/


#include <stdlib.h>
#include "trpimp.h"
#include "trptypxx.h"
#include "ntostrt.h"


char                            **dbg_environ;  /* pointer to parent's environment table */
const trap_callbacks            *Client;
extern const trap_requests      ImpInterface;

const trap_requests *TrapLoad( trap_callbacks *client )
{
    Client = client;
    if( Client->len <= offsetof(trap_callbacks,signal) ) {
        return( NULL );
    }
    dbg_environ = *Client->environ;
    return( &ImpInterface );
}

const trap_requests ImpInterface = { TrapInit, TrapRequest, TrapFini } ;

#if !defined( BUILTIN_TRAP_FILE )

void *_nmalloc( unsigned size )
{
    return( malloc( size ) );
}

void *malloc( unsigned size )
{
    return( Client->malloc( size ) );
}

void *_nrealloc( void *ptr, unsigned size )
{
    return( realloc( ptr, size ) );
}

void *realloc( void *ptr, unsigned size )
{
    return( Client->realloc( ptr, size ) );
}

void _nfree( void *ptr )
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

void (*signal( int __sig, void (*__func)( int ) ))( int )
{
    return( Client->signal( __sig, __func ) );
}
#endif
