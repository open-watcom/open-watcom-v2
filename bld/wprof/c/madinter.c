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
* Description:  MAD interface client callback routines.
*
****************************************************************************/


#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include "bool.h"
#include "wio.h"
#define MD_x86
#define MD_axp
#include "madcli.h"
#include "mad.h"
#include "madregs.h"

#include "dip.h"

#include "aui.h"
#include "common.h"
#include "sampinfo.h"
#include "msg.h"
#include "madinter.h"
#include "support.h"
#include "wpdata.h"


mad_status              MADStatus;

system_config           DefSysConfig =
    { X86_386, X86_387, 1, 0, DIG_OS_IDUNNO, 12, DIG_ARCH_X86 };

/*
 * Client support routines
 */

void MADCLIENTRY( AddrSection )( address *addr )
{
    MapAddressIntoSection( addr );
}

mad_status MADCLIENTRY( AddrOvlReturn )( address *addr )
{
    /* unused parameters */ (void)addr;

    /* never called */
    return( MS_FAIL );
}

mad_status MADCLIENTRY( AddrToString )( address a, mad_type_handle th,
                        mad_label_kind lk, char *buff, size_t buff_len )
{
    mad_type_info       mti;
    addr_ptr            item;
    mad_type_info       host_mti;

    /* unused parameters */ (void)lk;

    if( CnvAddr( a, buff, buff_len ) )
        return( MS_OK );
    MADTypeInfo( th, &mti );
    MADTypeInfoForHost( MTK_ADDRESS, sizeof( address ), &host_mti );
    MADTypeConvert( &host_mti, &a, &mti, &item, 0 );
    MADTypeToString( 16, &mti, &item, buff, &buff_len );
    return( MS_FAIL );
}

mad_status MADCLIENTRY( MemExpr )( const char *expr, mad_radix radix, address *a )
{
    /* unused parameters */ (void)expr; (void)radix; (void)a;

    /* never called */
    return( MS_FAIL );
}


size_t MADCLIENTRY( ReadMem )( address a, size_t size, void *buff )
{
    size_t      len;
    unsigned_8  *d;

    d = buff;
    SetExeOffset( a );
    for( len = 0; len < size; ++len ) {
        d[len] = GetDataByte();
    }
    return( len );
}

size_t MADCLIENTRY( WriteMem )( address a, size_t size, const void *buff )
{
    /* unused parameters */ (void)a; (void)size; (void)buff;

    return( 0 );
}

size_t MADCLIENTRY( String )( mad_string mstr, char *buff, size_t buff_len )
{
    //MAD: need to resourcify
    static const char *strings[] = {
        #define pick( e, es, js ) es,
        #include "mad.str"
        #undef pick
    };
    size_t  len;

    len = strlen( strings[mstr] );
    if( buff_len > 0 ) {
        --buff_len;
        if( buff_len > len )
            buff_len = len;
        memcpy( buff, strings[mstr], buff_len );
        buff[buff_len] = '\0';
    }
    return( len );
}

mad_status MADCLIENTRY( AddString )( mad_string mstr, const char *str )
{
    /* unused parameters */ (void)mstr; (void)str;

    //MAD: NYI
    return( MS_FAIL );
}

size_t MADCLIENTRY( RadixPrefix )( mad_radix radix, char *buff, size_t buff_len )
{
    /* unused parameters */ (void)radix; (void)buff; (void)buff_len;

    return( 0 );
}

void MADCLIENTRY( Notify )( mad_notify_type nt, const void *d )
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

system_config *MADCLIENTRY( SystemConfig )( void )
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

static void ReportMADFailure( mad_status ms )
{
    dig_arch    arch;
    char        buff[256];

    if( CurrSIOData->config.arch == DIG_ARCH_NIL ) {
        /* we're in deep do do */
        fatal( LIT( LMS_RECURSIVE_MAD_FAILURE ) );
    }
    arch = CurrSIOData->config.arch;
    MADNameFile( arch, buff, sizeof( buff ) );
    CurrSIOData->config.arch = DIG_ARCH_NIL;
    /* this deregisters the MAD, and sets the active one to the dummy */
    MADRegister( arch, NULL, NULL );
    switch( ms & ~MS_ERR ) {
    case MS_UNREGISTERED_MAD:
        ErrorMsg( LIT( LMS_UNREGISTERED_MAD ), arch );
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

void SetCurrentMAD( dig_arch arch )
{
    mad_status          ms;

    if( MADActiveSet( arch ) != arch ) {
        if( MADLoaded( arch ) != MS_OK ) {
            ms = MADLoad( arch );
            if( ms != MS_OK ) {
                ReportMADFailure( ms );
            }
            AsmSize();
        }
    }
}
