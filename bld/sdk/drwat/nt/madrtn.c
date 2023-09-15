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


#include "drwatcom.h"
#include "srchmsg.h"
#include "madrtn.h"
#include "madx86.h"
#include "dip.h"
#include "madsys1.h"
#include "madcli.h"

msglist MADMsgs[] = {
    #define pick( r, e, j ) MAD_##r,(char *)(pointer_uint)r,
    #include "mad.str"
    #undef pick
    0 , NULL
};

unsigned MADCLIENTRY( String )( mad_string mstr, char *buff, unsigned buff_len )
{
    unsigned len;
    char *msg;

    msg = SrchMsg( mstr, MADMsgs, "String not found" );
    len = strlen( msg );
    if( buff_len > 0 ) {
        --buff_len;
        if( buff_len > len )
            buff_len = len;
        memcpy( buff, msg, buff_len );
        buff[buff_len]='\0';
    }
    return( len );
}

size_t MADCLIENTRY( RadixPrefix )( mad_radix radix, char *buff, size_t buff_len )
{
    char    msg[10];
    size_t  len;

    switch( radix ){
    case 16:
        strcpy( msg, "0x" );
        break;
    default:
        msg[0] = '\0';
        break;
    }

    len = strlen( msg );
    if( buff_len > 0 ) {
        --buff_len;
        if( buff_len > len )
            buff_len = len;
        memcpy( buff, msg, buff_len );
        buff[buff_len]='\0';
    }
    return( len );
}

void MADCLIENTRY( Notify )( mad_notify_type nt, const void *d )
{
    if( StatHdl == NULL ){
        return;
    }
    PostMessage( StatHdl, STAT_MAD_NOTIFY, (WPARAM)nt, (LPARAM)d );
}

mad_status MADCLIENTRY( AddrToString )( address a, mad_type_handle mth,
                            mad_label_kind lk, char *buff, unsigned buff_len )
{
    mad_type_info       mti;
    addr_ptr            item;
    mad_type_info       host_mti;

    /* unused parameters */ (void)lk;

    MADTypeInfo( mth, &mti );
    MADTypeInfoForHost( MTK_ADDRESS, sizeof( address ), &host_mti );
    MADTypeConvert( &host_mti, &a, &mti, &item, 0 );
    MADTypeToString( 16, &mti, &item, buff, &buff_len );
    return( MS_OK );
}

mad_status MADCLIENTRY( MemExpr )( const char *expr, mad_radix radix, address *a )
{
    /* unused parameters */ (void)expr; (void)radix; (void)a;

    //stub
    return( MS_OK );
}

void MADCLIENTRY( AddrSection )( address *addr )
{
    /* unused parameters */ (void)addr;

    //stub
}

mad_status      MADCLIENTRY( AddrOvlReturn )( address *addr )
{
    /* unused parameters */ (void)addr;

    //stub
    return( MS_FAIL );
}



mad_status MADCLIENTRY( AddString )( mad_string mstr, const char *str )
{
    /* unused parameters */ (void)mstr; (void)str;

    //stub
    return( MS_FAIL );
}

system_config *MADCLIENTRY( SystemConfig )( void )
{
    return( &SysConfig );
}

size_t MADCLIENTRY( ReadMem )( address a, size_t size, void *buff )
{
    DWORD bytesread;

    ReadProcessMemory( ProcessHdl, (void *)a.mach.offset, buff, size, &bytesread );
    return( bytesread );
}

size_t MADCLIENTRY( WriteMem )( address a, size_t size, const void *buff )
{
    DWORD byteswritten;

    WriteProcessMemory( ProcessHdl, (void *)a.mach.offset, buff, size, &byteswritten );
    return( byteswritten );
}

unsigned MADCLIENTRY( MachineData )( address addr, dig_info_type info_type, dig_elen in_size,
                                            const void *in, dig_elen out_size, void *out )
/********************************************************************************************
 * MADCliMachineData
 */
{
#if defined( _M_IX86 )
    /* unused parameters */ (void)addr; (void)info_type; (void)in_size; (void)in; (void)out_size;
#else
    /* unused parameters */ (void)addr; (void)info_type; (void)in_size; (void)in; (void)out_size; (void)out;
#endif

    switch( SysConfig.arch ) {
#if defined( _M_IX86 )
    case DIG_ARCH_X86:
        if( info_type == X86MD_ADDR_CHARACTERISTICS ) {
            *(x86_addrflags *)out = X86AC_BIG;
            return( sizeof( x86_addrflags ) );
        }
        break;
#elif defined( _M_X64 )
    case DIG_ARCH_X86:
    case DIG_ARCH_X64:
        if( info_type == X64MD_ADDR_CHARACTERISTICS ) {
            *(x64_addrflags *)out = ( SysConfig.arch == DIG_ARCH_X64 ) ? X64AC_BIG : 0;
            return( sizeof( x64_addrflags ) );
        }
        break;
#elif defined( __AXP__ )
  #if 0
    case DIG_ARCH_AXP:
        if( info_type == AXPMD_PDATA ) {
            memcpy( out, in, sizeof( axp_data ) );
            return( sizeof( axp_data ) );
        }
        break;
  #endif
#endif
    default:
        break;
    }
    return( 0 );
}

bool InitMADInfo( void )
{
    LocalGetSysConfig();
    if( MADInit() != MS_OK ) {
        return( false );
    }
    if ( MADActiveSet( SysConfig.arch ) != MS_OK ){
        return( false );
    }
    if( MADLoad( SysConfig.arch ) != MS_OK  ) {
        return( false );
    }
    if( !AllocMadDisasmData() ) {
        MADFini();
        return( false );
    }
    return( true );
}

void FiniMADInfo( void )
{
    DeAllocMadDisasmData();
    MADFini();
}

void GetCurrAddr(address *curr, mad_registers *regs )
{
    curr->sect_id=0;
    curr->indirect=0;
    MADRegSpecialGet( MSR_IP, regs, &(curr->mach) );
}

void AllocMadRegisters( mad_registers **regs )
{
    unsigned    new;

    new = MADRegistersSize();
    *regs= MemAlloc( new );
}

void DeAllocMadRegisters( mad_registers *regs )
{
    MemFree( regs );
}
