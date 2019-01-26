/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2017 The Open Watcom Contributors. All Rights Reserved.
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
#include "aui.h"
#include "sampinfo.h"
#include "msg.h"
#include "myassert.h"
#include "support.h"
#include "utils.h"
#include "wpdata.h"
#include "memutil.h"

#define MD_x86
#define MD_axp
#include "madcli.h"
#include "mad.h"
#include "madregs.h"

#include "clibext.h"


void *DIGCLIENTRY( Alloc )( size_t amount )
/*****************************************/
{
    return( ProfAlloc( amount ) );
}

void *DIGCLIENTRY( Realloc )( void *p, size_t amount )
/****************************************************/
{
    return( ProfRealloc( p, amount ) );
}

void DIGCLIENTRY( Free )( void *p )
/*********************************/
{
    ProfFree( p );
}

FILE * DIGCLIENTRY( Open )( const char *name, dig_open mode )
/***********************************************************/
{
    const char  *access;

    if( mode & DIG_APPEND ) {
        access = "ab";
    } else if( mode & (DIG_WRITE | DIG_CREATE) ) {
        access = "wb";
    } else {
        access = "rb";
    }
    return( fopen( name, access ) );
}

int DIGCLIENTRY( Seek )( FILE *fp, unsigned long p, dig_seek k )
/**************************************************************/
{
    return( fseek( fp, p, k ) );
}

unsigned long DIGCLIENTRY( Tell )( FILE *fp )
/*******************************************/
{
    return( ftell( fp ) );
}

size_t DIGCLIENTRY( Read )( FILE *fp, void *b , size_t s )
/********************************************************/
{
    return( fread( b, 1, s, fp ) );
}

size_t DIGCLIENTRY( Write )( FILE *fp, const void *b, size_t s )
/**************************************************************/
{
    return( fwrite( b, 1, s, fp ) );
}

void DIGCLIENTRY( Close )( FILE *fp )
/***********************************/
{
    fclose( fp );
}

void DIGCLIENTRY( Remove )( const char *name, dig_open mode )
/***********************************************************/
{
    /* unused parameters */ (void)mode;

    remove( name );
}

unsigned DIGCLIENTRY( MachineData )( address addr, unsigned info_type,
                        dig_elen in_size,  const void *in,
                        dig_elen out_size, void *out )
/********************************************************************/
{
    enum x86_addr_characteristics       *d;

    /* unused parameters */ (void)info_type; (void)in_size; (void)in; (void)out_size;

    switch( CurrSIOData->config.arch ) {
    case DIG_ARCH_X86:
        d = out;
        *d = 0;
        if( IsX86BigAddr( addr ) ) {
            *d |= X86AC_BIG;
        }
        if( IsX86RealAddr( addr ) ) {
            *d |= X86AC_REAL;
        }
        return( sizeof( *d ) );
    /* add other machines here */
    }
    return( 0 );
}
