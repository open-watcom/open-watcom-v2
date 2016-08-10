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


extern sio_data         *CurrSIOData;


void * DIGCLIENT DIGCliAlloc( size_t amount )
/*******************************************/
{
    return( _MALLOC( amount ) );
}



void * DIGCLIENT DIGCliRealloc( void * p, size_t amount )
/*******************************************************/
{
    return( _REALLOC( p, amount ) );
}



void DIGCLIENT DIGCliFree( void * p )
/***********************************/
{
    _FREE( p );
}



dig_fhandle DIGCLIENT DIGCliOpen( const char * name, dig_open mode )
/******************************************************************/
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
    return( (dig_fhandle)fh );
}



unsigned long DIGCLIENT DIGCliSeek( dig_fhandle dfh, unsigned long p, dig_seek k )
/********************************************************************************/
{
    return( lseek( (int)dfh, p, k ) );
}



size_t DIGCLIENT DIGCliRead( dig_fhandle dfh, void * b , size_t s )
/*****************************************************************/
{
    return( BigRead( (int)dfh, b, s ) );
}



size_t DIGCLIENT DIGCliWrite( dig_fhandle dfh, const void * b, size_t s )
/***********************************************************************/
{
#ifdef _WIN64
    size_t      total;
    unsigned    write_len;
    unsigned    amount;

    amount = INT_MAX;
    total = 0;
    while( s > 0 ) {
        if( amount > s )
            amount = (unsigned)s;
        write_len = write( (int)dfh, b, amount );
        if( write_len == (unsigned)-1 ) {
            return( DIG_RW_ERROR );
        }
        total += write_len;
        if( write_len != amount ) {
            return( total );
        }
        b = (char *)b + amount;
        s -= amount;
    }
    return( total );
#else
    return( write( (int)dfh, b, s ) );
#endif
}



void DIGCLIENT DIGCliClose( dig_fhandle dfh )
/*******************************************/
{
    close( (int)dfh );
}



void DIGCLIENT DIGCliRemove( const char * name, dig_open mode )
/*************************************************************/
{
    mode=mode;
    remove( name );
}

unsigned DIGCLIENT DIGCliMachineData( address addr, unsigned info_type,
                        dig_elen in_size,  const void *in,
                        dig_elen out_size, void *out )
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
