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
* Description:  DIP imports interface.
*
****************************************************************************/


#include <stdio.h>
#include <string.h>
#if defined( __WINDOWS__ )
#include <stdlib.h>
#include <windows.h>
#include <i86.h>
#endif
#include "bool.h"
#include "diptypes.h"
#include "dipimp.h"

#ifndef DIP_PRIORITY
#define DIP_PRIORITY DIP_PRIOR_NORMAL
#endif

#if defined( __WATCOMC__ )
    #pragma aux DIPLOAD "*"
#endif

address                 NilAddr;
dip_client_routines     *DIPClient;

dip_imp_routines        ImpInterface = {
    DIP_MAJOR,
    DIP_MINOR,
    DIP_PRIORITY,
    DIPImp( Name ),

    DIPImp( HandleSize ),
    DIPImp( MoreMem ),
    DIPImp( Shutdown ),
    DIPImp( Cancel ),

    DIPImp( LoadInfo ),
    DIPImp( MapInfo ),
    DIPImp( UnloadInfo ),

    DIPImp( WalkModList ),
    DIPImp( ModName ),
    DIPImp( ModSrcLang ),
    DIPImp( ModInfo ),
    DIPImp( ModDefault ),
    DIPImp( AddrMod ),
    DIPImp( ModAddr ),

    DIPImp( WalkTypeList ),
    DIPImp( TypeMod ),
    DIPImp( TypeInfo ),
    DIPImp( OldTypeBase ),
    DIPImp( TypeArrayInfo ),
    DIPImp( TypeProcInfo ),
    DIPImp( TypePtrAddrSpace ),
    DIPImp( TypeThunkAdjust ),
    DIPImp( TypeCmp ),
    DIPImp( TypeName ),

    DIPImp( WalkSymList ),
    DIPImp( SymMod ),
    DIPImp( SymName ),
    DIPImp( SymType ),
    DIPImp( SymLocation ),
    DIPImp( SymValue ),
    DIPImp( SymInfo ),
    DIPImp( SymParmLocation ),
    DIPImp( SymObjType ),
    DIPImp( SymObjLocation ),
    DIPImp( AddrSym ),
    DIPImp( LookupSym ),
    DIPImp( AddrScope ),
    DIPImp( ScopeOuter ),
    DIPImp( SymCmp ),

    DIPImp( WalkFileList ),
    DIPImp( CueMod ),
    DIPImp( CueFile ),
    DIPImp( CueFileId ),
    DIPImp( CueAdjust ),
    DIPImp( CueLine ),
    DIPImp( CueColumn ),
    DIPImp( CueAddr ),
    DIPImp( LineCue ),
    DIPImp( AddrCue ),
    DIPImp( CueCmp ),

    DIPImp( TypeBase ),

    DIPImp( TypeAddRef ),
    DIPImp( TypeRelease ),
    DIPImp( TypeFreeAll ),

    DIPImp( SymAddRef ),
    DIPImp( SymRelease ),
    DIPImp( SymFreeAll ),

    DIPImp( WalkSymListEx ),
    DIPImp( LookupSymEx ),
};

#define FIRST_IMP_FUNC      HandleSize

#if defined( __WINDOWS__ )
static HINSTANCE    ThisInst;
static HANDLE       TaskId;
#endif

#if defined( __WATCOMC__ ) && ( defined( __DOS__ ) || defined( __UNIX__ ) )
const char __based( __segname( "_CODE" ) ) Signature[4] = "DIP";
#endif

DIG_DLLEXPORT dip_imp_routines * DIGENTRY DIPLOAD( dip_status *ds, dip_client_routines *client )
{
    DIPClient = client;
    *ds = DIPImp( Startup )();
    if( *ds & DS_ERR )
        return( NULL );
    return( &ImpInterface );
}

void *DCAlloc( size_t amount )
{
    return( DIPClient->Alloc( amount ) );
}

void *DCAllocZ( size_t amount )
{
    void *p = DIPClient->Alloc( amount );
    if( p ) {
        memset( p, 0, amount );
    }
    return( p );
}

void *DCRealloc( void *p, size_t amount )
{
    return( DIPClient->Realloc( p, amount ) );
}

void DCFree( void *p )
{
    DIPClient->Free( p );
}

void DCMapAddr( addr_ptr *a, void *d )
{
    DIPClient->MapAddr( a, d );
}

imp_sym_handle *DCSymCreate( imp_image_handle *iih, void *d )
{
    return( DIPClient->SymCreate( iih, d ) );
}

dip_status DCItemLocation( location_context *lc, context_item ci, location_list *ll )
{
    return( DIPClient->ItemLocation( lc, ci, ll ) );
}

dip_status DCAssignLocation( location_list *dst, const location_list *src, unsigned long len )
{
    return( DIPClient->AssignLocation( dst, src, len ) );
}

dip_status DCSameAddrSpace( address a, address b )
{
    return( DIPClient->SameAddrSpace( a, b ) );
}

void DCAddrSection( address *a )
{
    DIPClient->AddrSection( a );
}

FILE *DCOpen( const char *path, dig_open flags )
{
    return( DIPClient->Open( path, flags ) );
}

int DCSeek( FILE *fp, unsigned long p, dig_seek w )
{
    return( DIPClient->Seek( fp, p, w ) );
}

unsigned long DCTell( FILE *fp )
{
    return( DIPClient->Tell( fp ) );
}

size_t DCRead( FILE *fp, void *b, size_t s )
{
    return( DIPClient->Read( fp, b, s ) );
}

dip_status DCReadAt( FILE *fp, void *b, size_t s, unsigned long p )
{
    if( DIPClient->Seek( fp, p, DIG_ORG ) ) {
        return( DS_ERR | DS_FSEEK_FAILED );
    }
    if( DIPClient->Read( fp, b, s ) != s ) {
        return( DS_ERR | DS_FREAD_FAILED );
    }
    return( DS_OK );
}

size_t DCWrite( FILE *fp, const void *b, size_t s )
{
    return( DIPClient->Write( fp, b, s ) );
}

void DCClose( FILE *fp )
{
    DIPClient->Close( fp );
}

void DCRemove( const char *path, dig_open flags )
{
    DIPClient->Remove( path, flags );
}

void DCStatus( dip_status ds )
{
    DIPClient->Status( ds );
}

dig_arch DCCurrArch( void )
{
    /* check for old client */
    if( DIPClient->sizeof_struct < offsetof(dip_client_routines,CurrArch) )
        return( DIG_ARCH_X86 );
    return( DIPClient->CurrArch() );
}

unsigned DCMachineData( address a, dig_info_type info_type,
                                dig_elen in_size,  const void *in,
                                dig_elen out_size, void *out )
{
    if( DIPClient->sizeof_struct < offsetof(dip_client_routines,MachineData) )
        return( 0 );
    return( DIPClient->MachineData( a, info_type, in_size, in, out_size, out ) );
}

dip_status DIPIMPENTRY( OldTypeBase )(imp_image_handle *iih, imp_type_handle *ith, imp_type_handle *base_ith )
{
    return( ImpInterface.TypeBase( iih, ith, base_ith, NULL, NULL ) );
}

#if defined( __WINDOWS__ )

#ifdef DEBUGGING
void Say( const char *buff )
{
    MessageBox( (HWND)NULL, buff, "IMP", MB_OK | MB_ICONHAND | MB_SYSTEMMODAL );
}
#endif

void DIGENTRY DIPUNLOAD( void )
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
    dip_link_block      __far *link;
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
    func = (FARPROC *)&ImpInterface.FIRST_IMP_FUNC;
    count = ( sizeof( dip_imp_routines ) - offsetof( dip_imp_routines, FIRST_IMP_FUNC ) ) / sizeof( FARPROC );
    while( count != 0 ) {
        if( *func != NULL ) {
            *func = MakeProcInstance( *func, this_inst );
        }
        ++func;
        --count;
    }
    link->load = (dip_init_func *)MakeProcInstance( (FARPROC)DIPLOAD, this_inst );
    link->unload = (dip_fini_func *)MakeProcInstance( (FARPROC)DIPUNLOAD, this_inst );
    while( GetMessage( &msg, NULL, 0, 0 ) ) {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }
    return( 0 );
}

#endif
