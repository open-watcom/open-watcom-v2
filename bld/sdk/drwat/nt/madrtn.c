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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <windows.h>
#include "srchmsg.h"
#include "madrtn.h"
#include "madx86.h"

#include <string.h>
#include "dip.h"
#include <malloc.h>
#include "madsys.h"
#include "drwatcom.h"

msglist MADMsgs[] = {
    #define pick( r, e, j ) MAD_##r,(char *)r,
    #include "mad.str"
    #undef pick
    0 , (char *)-1
};

unsigned DIGCLIENT MADCliString( mad_string mstr, unsigned max, char *buff )
{
    unsigned len;
    char *msg;

    msg = SrchMsg( mstr, MADMsgs, "String not found" );
    len = strlen( msg );
    if( max > 0 ) {
        --max;
        if( max > len ) max = len;
        memcpy( buff, msg, max ); //strncpy not good here since it always writes
        buff[max]='\0';           //max char
    }
    return len;
}

unsigned DIGCLIENT MADCliRadixPrefix( unsigned radix, unsigned max, char *buff )
{
    char msg[10];
    int len;

    switch( radix ){
        case 16:
            strcpy( msg, "0x" );
            break;
        default:
            msg[0] = '\0';
            break;
    }

    len = strlen( msg );
    if( max > 0 ) {
        --max;
        if( max > len ) max = len;
        memcpy( buff, msg, max ); //strncpy not good here since it always writes
        buff[max]='\0';
    }
    return len;
}

void DIGCLIENT MADCliNotify( mad_notify_type nt, void *d )
{
    if( StatHdl == NULL ){
        return;
    }
    PostMessage( StatHdl, STAT_MAD_NOTIFY, (WPARAM)nt, (LPARAM)d );
}

unsigned DIGCLIENT DIGCliMachineData( address addr, unsigned info_type,
                        unsigned in_size,  const void *in,
                        unsigned out_size, void *out )
{
    enum x86_addr_characteristics       *a_char;

    switch( SysConfig.mad ) {
    case MAD_X86:
        a_char = out;
        *a_char = X86AC_BIG;
        return( sizeof( *a_char ) );
    }
    return( 0 );
}

mad_status DIGCLIENT MADCliAddrToString( address a, mad_type_handle th,
                            mad_label_kind lk, unsigned max, char *buff )
{
    mad_type_info       mti;
    unsigned            mad_max;
    addr_ptr            item;
    mad_type_info       host;

    mad_max = max;
    MADTypeInfo( th, &mti );
    MADTypeInfoForHost( MTK_ADDRESS, sizeof( address ), &host );
    MADTypeConvert( &host, &a, &mti, &item, 0 );
    MADTypeToString( 16, &mti, &item, &mad_max, buff );
    return( MS_OK );
}

mad_status DIGCLIENT MADCliMemExpr( char *start, unsigned len, unsigned radix, address *a )
{
    //stub
    return( MS_OK );
}

void DIGCLIENT MADCliAddrSection( address *addr )
{
    //stub
}

mad_status      DIGCLIENT MADCliAddrOvlReturn( address *addr )
{
    //stub
    return( MS_FAIL );
}



mad_status DIGCLIENT MADCliAddString( mad_string mstr, const char *str )
{
    //stub
    return( MS_FAIL );
}

system_config *DIGCLIENT MADCliSystemConfig( void )
{
    return &SysConfig;
}

BOOL InitMADInfo(void)
{
    GetSysConfig();
    if( MADInit() != MS_OK ) {
        return( FALSE );
    }
    if ( MADActiveSet( SysConfig.mad ) != MS_OK ){
        return( FALSE );
    }
    if( MADLoad(SysConfig.mad) != MS_OK  ) {
        return( FALSE );
    }
    if( AllocMadDisasmData() == FALSE ) {
        MADFini();
        return( FALSE );
    }
    return TRUE;
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
