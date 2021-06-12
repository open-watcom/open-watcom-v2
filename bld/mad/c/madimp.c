/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  MAD imports interface.
*
****************************************************************************/


#include <stdio.h>
#if defined( __WINDOWS__)
#include <stdlib.h>
#include <windows.h>
#include <i86.h>
#endif
#include "madcli.h"
#include "madimp.h"

#if defined( __WATCOMC__ )
    #pragma aux MADLOAD "*"
#endif

mad_client_routines     *MADClient;

mad_imp_routines        MadImpInterface = {
    MAD_MAJOR,
    MAD_MINOR,
    sizeof( MadImpInterface ),

    MADImp( Init ),
    MADImp( Fini ),
    MADImp( StateSize ),
    MADImp( StateInit ),
    MADImp( StateSet ),
    MADImp( StateCopy ),

    MADImp( AddrAdd ),
    MADImp( AddrComp ),
    MADImp( AddrDiff ),
    MADImp( AddrMap ),
    MADImp( AddrFlat ),
    MADImp( AddrInterrupt ),

    MADImp( TypeWalk ),
    MADImp( TypeName ),
    MADImp( TypePreferredRadix ),
    MADImp( TypeForDIPType ),
    MADImp( TypeInfo ),
    MADImp( TypeDefault ),
    MADImp( TypeConvert ),
    MADImp( TypeToString ),

    MADImp( RegistersSize ),
    MADImp( RegistersHost ),
    MADImp( RegistersTarget ),
    MADImp( RegSetWalk ),
    MADImp( RegSetName ),
    MADImp( RegSetLevel ),
    MADImp( RegSetDisplayGrouping ),
    MADImp( RegSetDisplayGetPiece ),
    MADImp( RegSetDisplayModify ),
    MADImp( RegSetDisplayToggleList ),
    MADImp( RegSetDisplayToggle ),
    MADImp( RegModified ),
    MADImp( RegInspectAddr ),
    MADImp( RegWalk ),
    MADImp( RegSpecialGet ),
    MADImp( RegSpecialSet ),
    MADImp( RegSpecialName ),
    MADImp( RegFromContextItem ),
    MADImp( RegUpdateStart ),
    MADImp( RegUpdateEnd ),

    MADImp( CallStackGrowsUp ),
    MADImp( CallTypeList ),
    MADImp( CallBuildFrame ),
    MADImp( CallReturnReg ),
    MADImp( CallParmRegList ),

    MADImp( DisasmDataSize ),
    MADImp( DisasmNameMax ),
    MADImp( Disasm ),
    MADImp( DisasmFormat ),
    MADImp( DisasmInsSize ),
    MADImp( DisasmInsUndoable ),
    MADImp( DisasmControl ),
    MADImp( DisasmInspectAddr ),
    MADImp( DisasmMemRefWalk ),
    MADImp( DisasmToggleList ),
    MADImp( DisasmToggle ),

    MADImp( TraceSize ),
    MADImp( TraceInit ),
    MADImp( TraceOne ),
    MADImp( TraceHaveRecursed ),
    MADImp( TraceSimulate ),
    MADImp( TraceFini ),

    MADImp( UnexpectedBreak ),
    MADImp( DisasmInsNext ),
    MADImp( CallUpStackSize ),
    MADImp( CallUpStackInit ),
    MADImp( CallUpStackLevel ),
};

#define FIRST_IMP_FUNC      Init

#if defined( __WATCOMC__ ) && defined( __386__ )
/* WD looks for this symbol to determine module bitness */
int __nullarea;
#pragma aux __nullarea "*";
#endif

#if defined( __WINDOWS__)
static HANDLE       TaskId;
static HINSTANCE    ThisInst;
#endif

#if defined( __WATCOMC__ ) && ( defined( __DOS__ ) || defined( __UNIX__ ) )
const char __based( __segname( "_CODE" ) ) Signature[4] = "MAD";
#endif

DIG_DLLEXPORT mad_imp_routines * DIGENTRY MADLOAD( mad_status *status, mad_client_routines *client )
{
    MADClient = client;
    *status = MS_OK;
    return( &MadImpInterface );
}

void    *MCAlloc( size_t amount )
{
    return( MADClient->Alloc( amount ) );
}

void    *MCRealloc( void *p, size_t amount )
{
    return( MADClient->Realloc( p, amount ) );
}

void    MCFree( void *p )
{
    MADClient->Free( p );
}

FILE *MCOpen( const char *name, dig_open mode )
{
    return( MADClient->Open( name, mode ) );
}

size_t  MCRead( FILE *fp, void *d, size_t l )
{
    return( MADClient->Read( fp, d, l ) );
}

void    MCClose( FILE *fp )
{
    MADClient->Close( fp );
}

size_t  MCReadMem( address a, size_t size, void *buff )
{
    return( MADClient->ReadMem( a, size, buff ) );
}

size_t  MCWriteMem( address a, size_t size, const void *buff )
{
    return( MADClient->WriteMem( a, size, buff ) );
}

size_t  MCString( mad_string id, char *buff, size_t buff_size )
{
    return( MADClient->String( id, buff, buff_size ) );
}

mad_status  MCAddString( mad_string id, const char *buff )
{
    return( MADClient->AddString( id, buff ) );
}

size_t  MCRadixPrefix( mad_radix radix, char *buff, size_t buff_size )
{
    return( MADClient->RadixPrefix( radix, buff, buff_size ) );
}

void    MCNotify( mad_notify_type nt, const void *d )
{
    MADClient->Notify( nt, d );
}

unsigned    MCMachineData( address a, dig_info_type info_type, dig_elen in_size, const void *in, dig_elen out_size, void *out )
{
    return( MADClient->MachineData( a, info_type, in_size, in, out_size, out ) );
}

mad_status  MCAddrToString( address a, mad_type_handle mth, mad_label_kind lk, char *buff, size_t buff_size )
{
    return( MADClient->AddrToString( a, mth, lk, buff, buff_size ) );
}

mad_status  MCMemExpr( const char *expr, mad_radix radix, address *a )
{
    return( MADClient->MemExpr( expr, radix, a ) );
}

void    MCAddrSection( address *a )
{
    MADClient->AddrSection( a );
}

mad_status  MCAddrOvlReturn( address *a )
{
    return( MADClient->AddrOvlReturn( a ) );
}

system_config   *MCSystemConfig( void )
{
    return( MADClient->SystemConfig() );
}

mad_status  MCTypeInfoForHost( mad_type_kind tk, int size, mad_type_info *mti )
{
    return( MADClient->TypeInfoForHost( tk, size, mti ) );
}

mad_status  MCTypeConvert( const mad_type_info *in_mti, const void *in_d, const mad_type_info *out_mti, void *out_d, addr_seg seg )
{
    return( MADClient->TypeConvert( in_mti, in_d, out_mti, out_d, seg ) );
}

mad_status  MCTypeToString( mad_radix radix, const mad_type_info *mti, const void *data, char *buff, size_t *buff_size_p )
{
    return( MADClient->TypeToString( radix, mti, data, buff, buff_size_p ) );
}

void    MCStatus( mad_status ms )
{
    MCNotify( MNT_ERROR, &ms );
}

#if defined( __WINDOWS__)

#ifdef DEBUGGING
void Say( char *buff )
{
    MessageBox( (HWND)NULL, buff, "IMP", MB_OK | MB_ICONHAND | MB_SYSTEMMODAL );
}
#endif

void DIGENTRY MADUNLOAD( void )
{
    PostAppMessage( TaskId, WM_QUIT, 0, 0 );
}

int PASCAL WinMain( HINSTANCE this_inst, HINSTANCE prev_inst, LPSTR cmdline, int cmdshow )
/*****************************************************************************************

    Initialization, message loop.
*/
{
    MSG                 msg;
    FARPROC             *func;
    unsigned            count;
    mad_link_block      __far *link;
    unsigned            seg;
    unsigned            off;

    prev_inst = prev_inst;
    cmdshow = cmdshow;
    seg = strtoul( cmdline, &cmdline, 16 );
    off = strtoul( cmdline, NULL, 16 );
    if( seg == 0 || off == 0 )
        return( 1 );
    link = _MK_FP( seg, off );
    TaskId = GetCurrentTask();
    ThisInst = this_inst;
    func = (FARPROC *)&MadImpInterface.FIRST_IMP_FUNC;
    count = ( sizeof( mad_imp_routines ) - offsetof( mad_imp_routines, FIRST_IMP_FUNC ) ) / sizeof( FARPROC );
    while( count != 0 ) {
        if( *func != NULL ) {
            *func = MakeProcInstance( *func, this_inst );
        }
        ++func;
        --count;
    }
    link->load = (mad_init_func *)MakeProcInstance( (FARPROC)MADLOAD, this_inst );
    link->unload = (mad_fini_func *)MakeProcInstance( (FARPROC)MADUNLOAD, this_inst );
    while( GetMessage( &msg, NULL, 0, 0 ) ) {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }
    return( 0 );
}

#endif
