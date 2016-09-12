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
* Description:  DIG interface client callback routines.
*
****************************************************************************/


#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include "wio.h"
#include "common.h"
#include "dip.h"
#include "dipimp.h"
#include "dipcli.h"
#include "diptypes.h"
#include "sampinfo.h"
#include "msg.h"
#include "myassert.h"
#include "support.h"
#include "utils.h"

#define MD_x86
#define MD_axp
#include "madcli.h"
#include "mad.h"
#include "madregs.h"

#include "clibext.h"


extern sio_data         *CurrSIOData;

void *DIGCLIENTRY( Alloc )( size_t amount )
/*****************************************/
{
    return( _MALLOC( amount ) );
}

void *DIGCLIENTRY( Realloc )( void * p, size_t amount )
/*****************************************************/
{
    return( _REALLOC( p, amount ) );
}

void DIGCLIENTRY( Free )( void * p )
/**********************************/
{
    _FREE( p );
}

dig_fhandle DIGCLIENTRY( Open )( const char * name, dig_open mode )
/*****************************************************************/
{
    int             fh;
    int             access;

    access = O_BINARY;
    if( mode & DIG_TRUNC ) {
        access |= O_TRUNC | O_CREAT;
    }
    if( mode & DIG_READ ) {
        access |= O_RDONLY;
    }
    if( mode & DIG_WRITE ) {
        if( access & DIG_READ ) {
            access &= ~O_RDONLY;
            access |= O_RDWR;
        } else {
            access |= O_WRONLY | O_CREAT;
        }
    }
    if( mode & DIG_CREATE ) {
        access |= O_CREAT;
    }
    if( mode & DIG_APPEND ) {
        access |= O_APPEND;
    }
    fh = open( name, access );
    if( fh == -1 )
        return( DIG_NIL_HANDLE );
    return( PH2DFH( fh ) );
}

unsigned long DIGCLIENTRY( Seek )( dig_fhandle dfh, unsigned long p, dig_seek k )
/*******************************************************************************/
{
    return( lseek( DFH2PH( dfh ), p, k ) );
}

size_t DIGCLIENTRY( Read )( dig_fhandle dfh, void * b , size_t s )
/****************************************************************/
{
#if defined( __QNX__ )
    return( BigRead( DFH2PH( dfh ), b, s ) );
#else
    return( read( DFH2PH( dfh ), b, s ) );
#endif
}

size_t DIGCLIENTRY( Write )( dig_fhandle dfh, const void * b, size_t s )
/**********************************************************************/
{
#if defined( __QNX__ )
    return( BigWrite( DFH2PH( dfh ), b, s ) );
#else
    return( write( DFH2PH( dfh ), b, s ) );
#endif
}

void DIGCLIENTRY( Close )( dig_fhandle dfh )
/******************************************/
{
    close( DFH2PH( dfh ) );
}

void DIGCLIENTRY( Remove )( const char * name, dig_open mode )
/************************************************************/
{
    mode=mode;
    remove( name );
}

unsigned DIGCLIENTRY( MachineData )( address addr, unsigned info_type,
                        dig_elen in_size,  const void *in,
                        dig_elen out_size, void *out )
/********************************************************************/
{
    enum x86_addr_characteristics       *d;

    info_type = info_type; in_size = in_size; in = in; out_size = out_size;
    switch( CurrSIOData->config.mad ) {
    case MAD_X86:
        d = out;
        *d = 0;
        if( IsX86BigAddr( addr ) ) {
            *d |= X86AC_BIG;
        };
        if( IsX86RealAddr( addr ) ) {
            *d |= X86AC_REAL;
        }
        return( sizeof( *d ) );
    /* add other machines here */
    }
    return( 0 );
}
