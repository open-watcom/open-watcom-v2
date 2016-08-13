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
* Description:  DIP imports interface.
*
****************************************************************************/


#include <string.h>
#if defined( __WINDOWS__ )
#include <stdlib.h>
#include <windows.h>
#include <i86.h>
#endif
#include "diptypes.h"
#include "dipimp.h"

#ifndef DIP_PRIORITY
#define DIP_PRIORITY DIP_PRIOR_NORMAL
#endif

#if defined( __WATCOMC__ )
  #if defined( __WINDOWS__ )
  #elif defined( _M_I86 )
    #pragma aux DIPLOAD "*" __loadds
  #else
    #pragma aux DIPLOAD "*"
  #endif
#endif

address                 NilAddr;
dip_client_routines     *DIPClient;

dip_imp_routines        ImpInterface = {
    DIP_MAJOR,
    DIP_MINOR,
    DIP_PRIORITY,
    DIPImpName,

    DIPImpQueryHandleSize,
    DIPImpMoreMem,
    DIPImpShutdown,
    DIPImpCancel,

    DIPImpLoadInfo,
    DIPImpMapInfo,
    DIPImpUnloadInfo,

    DIPImpWalkModList,
    DIPImpModName,
    DIPImpModSrcLang,
    DIPImpModInfo,
    DIPImpModDefault,
    DIPImpAddrMod,
    DIPImpModAddr,

    DIPImpWalkTypeList,
    DIPImpTypeMod,
    DIPImpTypeInfo,
    DIPImpOldTypeBase,
    DIPImpTypeArrayInfo,
    DIPImpTypeProcInfo,
    DIPImpTypePtrAddrSpace,
    DIPImpTypeThunkAdjust,
    DIPImpTypeCmp,
    DIPImpTypeName,

    DIPImpWalkSymList,
    DIPImpSymMod,
    DIPImpSymName,
    DIPImpSymType,
    DIPImpSymLocation,
    DIPImpSymValue,
    DIPImpSymInfo,
    DIPImpSymParmLocation,
    DIPImpSymObjType,
    DIPImpSymObjLocation,
    DIPImpAddrSym,
    DIPImpLookupSym,
    DIPImpAddrScope,
    DIPImpScopeOuter,
    DIPImpSymCmp,

    DIPImpWalkFileList,
    DIPImpCueMod,
    DIPImpCueFile,
    DIPImpCueFileId,
    DIPImpCueAdjust,
    DIPImpCueLine,
    DIPImpCueColumn,
    DIPImpCueAddr,
    DIPImpLineCue,
    DIPImpAddrCue,
    DIPImpCueCmp,

    DIPImpTypeBase,

    DIPImpTypeAddRef,
    DIPImpTypeRelease,
    DIPImpTypeFreeAll,

    DIPImpSymAddRef,
    DIPImpSymRelease,
    DIPImpSymFreeAll,

    DIPImpWalkSymListEx,
    DIPImpLookupSymEx,
};

#if defined( __WINDOWS__ )
static HINSTANCE    ThisInst;
static HANDLE       TaskId;
#endif

#if defined( __WATCOMC__ ) && ( defined( __DOS__ ) || defined( __UNIX__ ) )
const char __based( __segname( "_CODE" ) ) Signature[4] = "DIP";
#endif

DIG_DLLEXPORT dip_imp_routines * DIGENTRY DIPLOAD( dip_status *status, dip_client_routines *client )
{
    DIPClient = client;
#if defined( __WINDOWS__ )
    {
        FARPROC start;

        start = MakeProcInstance( (FARPROC)DIPImpStartup, ThisInst );
        *status = ((dip_status(DIGENTRY*)(void)) start)();
        FreeProcInstance( start );
    }
#else
    *status = DIPImpStartup();
#endif
    if( *status & DS_ERR )
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

imp_sym_handle *DCSymCreate( imp_image_handle *ii, void *d )
{
    return( DIPClient->SymCreate( ii, d ) );
}

dip_status DCItemLocation( location_context *lc, context_item ci, location_list *ll )
{
    return( DIPClient->ItemLocation( lc, ci, ll ) );
}

dip_status DCAssignLocation( location_list *dst, location_list *src, unsigned long len )
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

dig_fhandle DCOpen( const char *path, dig_open flags )
{
    return( DIPClient->Open( path, flags ) );
}

unsigned long DCSeek( dig_fhandle dfh, unsigned long p, dig_seek w )
{
    return( DIPClient->Seek( dfh, p, w ) );
}

size_t DCRead( dig_fhandle dfh, void *b, size_t s )
{
    return( DIPClient->Read( dfh, b, s ) );
}

dip_status DCReadAt( dig_fhandle dfh, void *b, size_t s, unsigned long p )
{
    if( DIPClient->Seek( dfh, p, DIG_ORG ) != p ) {
        return( DS_ERR | DS_FSEEK_FAILED );
    }
    if( DIPClient->Read( dfh, b, s ) != s ) {
        return( DS_ERR | DS_FREAD_FAILED );
    }
    return( DS_OK );
}

size_t DCWrite( dig_fhandle dfh, const void *b, size_t s )
{
    return( DIPClient->Write( dfh, b, s ) );
}

void DCClose( dig_fhandle dfh )
{
    DIPClient->Close( dfh );
}

void DCRemove( const char *path, dig_open flags )
{
    DIPClient->Remove( path, flags );
}

void DCStatus( dip_status status )
{
    DIPClient->Status( status );
}

dig_mad DCCurrMAD( void )
{
    /* check for old client */
    if( DIPClient->sizeof_struct < offsetof(dip_client_routines,CurrMAD) )
        return( MAD_X86 );
    return( DIPClient->CurrMAD() );
}

unsigned        DCMachineData( address a, dig_info_type info_type,
                                dig_elen in_size,  const void *in,
                                dig_elen out_size, void *out )
{
    if( DIPClient->sizeof_struct < offsetof(dip_client_routines,MachineData) )
        return( 0 );
    return( DIPClient->MachineData( a, info_type, in_size, in, out_size, out ) );
}

dip_status DIGENTRY DIPImpOldTypeBase(imp_image_handle *ii, imp_type_handle *it, imp_type_handle *base )
{
    return( ImpInterface.TypeBase( ii, it, base, NULL, NULL ) );
}

#if defined( __WINDOWS__ )

typedef void (DIGENTRY INTER_FUNC)();

#ifdef DEBUGGING
void Say( const char *buff )
{
    MessageBox( (HWND)NULL, buff, "IMP", MB_OK | MB_ICONHAND | MB_SYSTEMMODAL );
}
#endif

DIG_DLLEXPORT void DIGENTRY DIPUNLOAD( void )
{
    PostAppMessage( TaskId, WM_QUIT, 0, 0 );
}

int PASCAL WinMain( HINSTANCE this_inst, HINSTANCE prev_inst, LPSTR cmdline, int cmdshow )
/*****************************************************************************************

    Initialization, message loop.
*/
{
    MSG                 msg;
    INTER_FUNC          **func;
    unsigned            count;
    struct {
        dip_init_func   *load;
        dip_fini_func   *unload;
    }                   *link;
    unsigned            seg;
    unsigned            off;

    prev_inst = prev_inst;
    cmdshow = cmdshow;
    seg = strtoul( cmdline, &cmdline, 16 );
    off = strtoul( cmdline, NULL, 16 );
    if( seg == 0 || off == 0 )
        return( 1 );
    link = MK_FP( seg, off );
    TaskId = GetCurrentTask();
    ThisInst = this_inst;
    func = (INTER_FUNC **)&ImpInterface.QueryHandleSize;
    count = ( sizeof( dip_imp_routines ) - offsetof( dip_imp_routines, QueryHandleSize ) ) / sizeof( INTER_FUNC * );
    while( count != 0 ) {
        *func = (INTER_FUNC *)MakeProcInstance( (FARPROC)*func, this_inst );
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
