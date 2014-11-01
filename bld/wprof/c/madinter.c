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
* Description:  MAD interface client callback routines.
*
****************************************************************************/


#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include "wio.h"
#define MD_x86
#define MD_axp
#include "madcli.h"
#include "mad.h"
#include "madregs.h"

#include "dip.h"

#include "common.h"
#include "sampinfo.h"
#include "msg.h"


extern void     ErrorMsg(char *msg,... );
extern void     fatal(char *msg,... );
extern void     MapAddressIntoSection(address *addr);
extern bool     IsX86BigAddr( address );
extern bool     IsX86RealAddr( address );
extern void     SetExeOffset( address );
extern int_16   GetDataByte( void );
extern bool     CnvAddr( address addr, char *buff, unsigned len );
extern void     AsmSize( void );
extern void     AsmFini( void );

extern sio_data         *CurrSIOData;

mad_status              MADStatus;

system_config           DefSysConfig =
    { X86_386, X86_387, 1, 0, MAD_OS_IDUNNO, 12, MAD_X86 };

/*
 * Client support routines
 */

void DIGCLIENT MADCliAddrSection( address *addr )
{
    MapAddressIntoSection( addr );
}

mad_status      DIGCLIENT MADCliAddrOvlReturn( address *addr )
{
    /* never called */
    return( MS_FAIL );
}

mad_status      DIGCLIENT MADCliAddrToString( address a, mad_type_handle th,
                            mad_label_kind lk, char *buff, unsigned max )
{
    mad_type_info       mti;
    unsigned            mad_max = max;
    addr_ptr            item;
    mad_type_info       host;

    if( CnvAddr( a, buff, max ) ) return( MS_OK );
    MADTypeInfo( th, &mti );
    MADTypeInfoForHost( MTK_ADDRESS, sizeof( address ), &host );
    MADTypeConvert( &host, &a, &mti, &item, 0 );
    MADTypeToString( 16, &mti, &item, buff, &mad_max );
    return( MS_FAIL );
}

mad_status      DIGCLIENT MADCliMemExpr( const char *start, unsigned len, unsigned radix, address *a )
{
    /* never called */
    return( MS_FAIL );
}


unsigned        DIGCLIENT MADCliReadMem( address a, unsigned size, void *buff )
{
    unsigned    len;
    unsigned_8  *d;

    d = buff;
    SetExeOffset( a );
    for( len = 0; len < size; ++len ) {
        d[len] = GetDataByte();
    }
    return( len );
}

unsigned        DIGCLIENT MADCliWriteMem( address a, unsigned size, const void *buff )
{
    return( 0 );
}

unsigned        DIGCLIENT MADCliString( mad_string mstr, unsigned max, char *buff )
{
    //MAD: need to resourcify
    static const char *strings[] = {
        #define pick( e, es, js ) es,
        #include "mad.str"
        #undef pick
    };

    unsigned    len;

    len = strlen( strings[mstr] );
    if( max > 0 ) {
        --max;
        if( max > len ) max = len;
        memcpy( buff, strings[mstr], max );
        buff[max] = '\0';
    }
    return( len );
}

mad_status      DIGCLIENT MADCliAddString( mad_string mstr, const char *str )
{
    //MAD: NYI
    return( MS_FAIL );
}

unsigned        DIGCLIENT MADCliRadixPrefix( unsigned radix, unsigned max, char *buff )
{
    return( 0 );
}

void            DIGCLIENT MADCliNotify( mad_notify_type nt, const void *d )
{
    switch( nt ) {
    case MNT_ERROR:
        MADStatus = *(mad_status *)d;
        break;
    case MNT_MODIFY_REG:
    case MNT_MODIFY_IP:
    case MNT_MODIFY_SP:
    case MNT_MODIFY_FP:
    case MNT_EXECUTE_TOUCH_SCREEN_BUFF:
    case MNT_EXECUTE_LONG:
        /* never gonna happen, my friend */
        break;
    }
}

system_config   *DIGCLIENT MADCliSystemConfig( void )
{
    return( &CurrSIOData->config );
}

/*
 *      Debugger routines
 */

void InitMADInfo( void )
{
    if( MADInit() != MS_OK ) {
        fatal( LIT( Mad_Init_Failed ) );
    }
}

void FiniMADInfo( void )
{
    MADFini();
    AsmFini();
}

void ReportMADFailure( mad_status ms )
{
    mad_handle  old;
    char        buff[256];

    if( CurrSIOData->config.mad == MAD_NIL ) {
        /* we're in deep do do */
        fatal( LIT( LMS_RECURSIVE_MAD_FAILURE ) );
    }
    old = CurrSIOData->config.mad;
    MADNameFile( old, buff, sizeof( buff ) );
    CurrSIOData->config.mad = MAD_NIL;
    /* this deregisters the MAD, and sets the active one to the dummy */
    MADRegister( old, NULL, NULL );
    switch( ms & ~MS_ERR ) {
    case MS_UNREGISTERED_MAD:
        ErrorMsg( LIT( LMS_UNREGISTERED_MAD ), old );
        break;
    case MS_INVALID_MAD:
        ErrorMsg( LIT( LMS_INVALID_MAD ), buff );
        break;
    case MS_INVALID_MAD_VERSION:
        ErrorMsg( LIT( LMS_INVALID_MAD_VERSION ), buff );
        break;
    case MS_INVALID_STATE:
        ErrorMsg( LIT( Assertion_Failed ), "MAD state" );
        break;
    case MS_FOPEN_FAILED:
        ErrorMsg( LIT( LDS_FOPEN_FAILED ) );
        break;
    case MS_FREAD_FAILED:
        ErrorMsg( LIT( LDS_FREAD_FAILED ) );
        break;
    case MS_FSEEK_FAILED:
        ErrorMsg( LIT( LDS_FSEEK_FAILED ) );
        break;
    case MS_NO_MEM:
        ErrorMsg( LIT( Memfull ) );
        break;
    }
}

void SetCurrentMAD( mad_handle mad )
{
    mad_status          ms;

    if( MADActiveSet( mad ) != mad ) {
        if( MADLoaded( mad ) != MS_OK ) {
            ms = MADLoad( mad );
            if( ms != MS_OK ) {
                ReportMADFailure( ms );
            }
            AsmSize();
        }
    }
}
