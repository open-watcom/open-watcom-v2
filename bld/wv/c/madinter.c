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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <stddef.h>
#include <ctype.h>
#include "bool.h"
#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbglit.h"
#include "dbgmem.h"
#include "dbgio.h"
#include "dbgerr.h"
#include "dui.h"
#include "dbgscan.h"
#include "madinter.h"
#include "madcli.h"
#include "dbgutil.h"
#include "dbgovl.h"
#include "dbgparse.h"
#include "dbgtrace.h"
#include "dbgtback.h"
#include "remcore.h"
#include "removl.h"
#include "dbgreg.h"
#include "dbgset.h"
#include "dbgupdt.h"
#include "dbgmad.h"

#include "clibext.h"


extern void             SetMADMenuItems( void );

static mad_status       MADStatus;

/*
 * Client support routines
 */

void MADCLIENTRY( AddrSection )( address *addr )
{
    AddrSection( addr, OVL_MAP_CURR );
}

mad_status MADCLIENTRY( AddrOvlReturn )( address *addr )
{
    return( FixOvlRetAddr( addr ) ? MS_OK : MS_FAIL );
}

mad_status MADCLIENTRY( AddrToString )( address a, mad_type_handle mth,
                        mad_label_kind lk, char *buff, size_t buff_len )
{
    char        *p;

    RemoteOvlTransAddr( &a );
    switch( lk ) {
    case MLK_CODE:
        p = CnvAddr( a, CAO_NO_PLUS, false, buff, buff_len );
        break;
    default:
        p = CnvAddr( a, CAO_NORMAL_PLUS, false, buff, buff_len );
        break;
    }
    if( p != NULL )
        return( MS_OK );
    AddrTypeToString( &a, mth, buff, buff_len );
    return( MS_FAIL );
}

mad_status MADCLIENTRY( MemExpr )( const char *expr, mad_radix radix, address *a )
{
    const char  *old_scan;
    mad_radix   old_radix;

    old_radix = SetCurrRadix( radix );
    old_scan = ReScan( expr );
    EvalExpr( 0 );   /* memory expression */
    MakeMemoryAddr( true, EXPR_DATA, a );
    SetCurrRadix( old_radix );
    ReScan( old_scan );
    return( MS_OK );
}


size_t MADCLIENTRY( ReadMem )( address a, size_t size, void *buff )
{
    return( ProgPeek( a, buff, size ) );
}

size_t MADCLIENTRY( WriteMem )( address a, size_t size, const void *buff )
{
    return( ProgPoke( a, buff, size ) );
}

size_t MADCLIENTRY( String )( mad_string mstr, char *buff, size_t buff_len )
{
    static  char ** strings[] = {
        #define pick( e, es, js ) LITREF_ENG( e ),
        #include "mad.str"
        #undef pick
    };

    size_t      len;

    len = strlen( *strings[mstr] );
    if( buff_len > 0 ) {
        --buff_len;
        if( buff_len > len )
            buff_len = len;
        memcpy( buff, *strings[mstr], buff_len );
        buff[buff_len] = NULLCHAR;
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
    const char          *start;
    size_t              len;

    if( radix == CurrRadix )
        return( 0 );
    FindRadixSpec( radix, &start, &len );
    if( buff_len > 0 ) {
        --buff_len;
        if( buff_len > len )
            buff_len = len;
        memcpy( buff, start, buff_len );
        buff[buff_len] = NULLCHAR;
    }
    return( len );
}

void MADCLIENTRY( Notify )( mad_notify_type nt, const void *d )
{
    switch( nt ) {
    case MNT_ERROR:
        MADStatus = *(mad_status *)d;
        break;
    case MNT_MODIFY_REG:
        DbgUpdate( UP_REG_CHANGE );
        break;
    case MNT_MODIFY_IP:
        DbgUpdate( UP_CSIP_CHANGE );
        break;
    case MNT_MODIFY_SP:
    case MNT_MODIFY_FP:
        break;
    case MNT_EXECUTE_TOUCH_SCREEN_BUFF:
        _SwitchOn( SW_TOUCH_SCREEN_BUFF );
        break;
    case MNT_EXECUTE_LONG:
        _SwitchOn( SW_EXECUTE_LONG );
        break;
    case MNT_REDRAW_DISASM:
        DbgUpdate( UP_ASM_RESIZE );
        break;
    case MNT_REDRAW_REG:
        DbgUpdate( UP_REG_RESIZE );
        break;
    }
}

system_config *MADCLIENTRY( SystemConfig )( void )
{
    return( &SysConfig );
}

unsigned MADCLIENTRY( MachineData )( address addr, dig_info_type info_type, dig_elen in_size,
                                        const void *in, dig_elen out_size, void *out )
{
    return( RemoteMachineData( addr, info_type, in_size, in, out_size, out ) );
}

/*
 *      Debugger routines
 */

void InitMADInfo( void )
{
    if( MADInit() != MS_OK ) {
        StartupErr( LIT_ENG( STARTUP_MAD_Not_Init ) );
    }
}

void FiniMADInfo( void )
{
    MADFini();
}

mad_type_handle GetMADTypeHandleDefaultAt( address a, mad_type_kind mtk )
{
    return( MADTypeDefault( mtk, MAF_FULL, &DbgRegs->mr, &a ) );
}

void    GetMADTypeDefaultAt( address a, mad_type_kind mtk, mad_type_info *mti )
{
    MADTypeInfo( GetMADTypeHandleDefaultAt( a, mtk ), mti );
}

void    GetMADTypeDefault( mad_type_kind mtk, mad_type_info *mti )
{
    GetMADTypeDefaultAt( GetRegIP(), mtk, mti );
}

void ReportMADFailure( mad_status ms )
{
    dig_arch    arch_old;
    char        buff[TXT_LEN];

    if( SysConfig.arch == DIG_ARCH_NIL ) {
        /* we're in deep do do */
        StartupErr( LIT_ENG( LMS_RECURSIVE_MAD_FAILURE ) );
    }
    arch_old = SysConfig.arch;
    MADBaseName( arch_old, buff, sizeof( buff ) );
    SysConfig.arch = DIG_ARCH_NIL;
    /* this deregisters the MAD, and sets the active one to the dummy */
    MADRegister( arch_old, NULL, NULL );
    switch( ms & ~MS_ERR ) {
    case MS_UNREGISTERED_MAD:
        ErrorRet( ERR_NONE, LIT_ENG( LMS_UNREGISTERED_MAD ), arch_old );
        break;
    case MS_INVALID_MAD_VERSION:
        ErrorRet( ERR_NONE, LIT_ENG( LMS_INVALID_MAD_VERSION ), buff );
        break;
    case MS_INVALID_STATE:
        ErrorRet( ERR_INTERNAL, "MAD state" );
        break;
    case MS_FOPEN_FAILED:
        ErrorRet( ERR_NONE, LIT_ENG( ERR_FILE_NOT_OPEN ), buff );
        break;
    case MS_FREAD_FAILED:
    case MS_FSEEK_FAILED:
        ErrorRet( ERR_NONE, LIT_ENG( LMS_FREAD_FAILED ), buff );
        break;
    case MS_NO_MEM:
        ErrorRet( ERR_NONE, LIT_ENG( ERR_NO_MEMORY ) );
        break;
    case MS_INVALID_MAD:
    default:
        ErrorRet( ERR_NONE, LIT_ENG( LMS_INVALID_MAD ), buff );
        break;
    }
}

void CheckMADChange( void )
{
    mad_status          ms;
    mad_type_info       mti;

    if( MADActiveSet( SysConfig.arch ) != SysConfig.arch ) {
        if( MADLoaded( SysConfig.arch ) != MS_OK ) {
            ms = MADLoad( SysConfig.arch );
            if( ms != MS_OK ) {
                ReportMADFailure( ms );
            }
        }
        DbgUpdate( UP_MAD_CHANGE );
        ResizeTraceData();
        ResizeRegData();
        PendingToggles();
        if( MADCallStackGrowsUp() == MS_OK ) {
            _SwitchOn( SW_STACK_GROWS_UP );
        } else {
            _SwitchOff( SW_STACK_GROWS_UP );
        }
        GetMADTypeDefault( MTK_ADDRESS, &mti );
        if( mti.a.seg.bits != 0 ) {
            _SwitchOn( SW_HAVE_SEGMENTS );
        } else {
            _SwitchOff( SW_HAVE_SEGMENTS );
        }
        SetMADMenuItems();
        ClearMachineDataCache();
    }
}

static size_t doNormalizedString( char *p )
{
//    char        *start;
    char        *d;

//    start = p;
    d = p;
    while( *p != NULLCHAR ) {
        switch( *p ) {
        case ' ':
        case '\t':
            *d++ = '_';
            break;
        case '&':
            break;
        default:
            *d++ = *p;
            break;
        }
        ++p;
    }
    *d = NULLCHAR;
    return( d - p );
}

size_t GetMADNormalizedString( mad_string ms, char *buff, size_t buff_len )
{
    MADCli( String )( ms, buff, buff_len );
    return( doNormalizedString( buff ) );
}

size_t GetMADTypeNameForCmd( mad_type_handle mth, char *buff, size_t buff_len )
{
    return( GetMADNormalizedString( MADTypeName( mth ), buff, buff_len ) );
}

struct find_handle {
    unsigned            bits;
    mad_type_handle     mth;
};

static walk_result CheckOneHandle( mad_type_handle mth, void *d )
{
    struct find_handle  *fd = d;
    mad_type_info       mti;

    MADTypeInfo( mth, &mti );
    if( mti.b.bits != fd->bits )
        return( WR_CONTINUE );
    fd->mth = mth;
    return( WR_STOP );
}

mad_type_handle FindMADTypeHandle( mad_type_kind tk, unsigned size )
{
    struct find_handle  data;

    data.bits = BYTES2BITS( size );
    data.mth = MAD_NIL_TYPE_HANDLE;
    MADTypeWalk( tk, CheckOneHandle, &data );
    return( data.mth );
}

struct find_mad {
    const char      *name;
    unsigned        len;
    dig_arch        arch;
};

static walk_result FindTheMad( dig_arch arch, void *d )
{
    struct find_mad     *fd = d;
    char                buff[80];
//    char                *p;

    MADBaseName( arch, buff, sizeof( buff ) );
//    p = SkipPathInfo( buff, 0 );
    SkipPathInfo( buff, 0 );
    if( strnicmp( buff, fd->name, fd->len ) == 0 ) {
        fd->arch = arch;
        return( WR_STOP );
    }
    MADDescription( arch, buff, sizeof( buff ) );
    doNormalizedString( buff );
    if( strnicmp( buff, fd->name, fd->len ) == 0 ) {
        fd->arch = arch;
        return( WR_STOP );
    }
    return( WR_CONTINUE );
}

dig_arch FindMAD( const char *name, unsigned len )
{
    struct find_mad     data;

    data.name = name;
    data.len = len;
    data.arch = DIG_ARCH_NIL;
    MADWalk( FindTheMad, &data );
    return( data.arch );
}
