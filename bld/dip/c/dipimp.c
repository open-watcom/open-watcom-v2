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


#include <stddef.h>
#include "dipimp.h"

address                 NilAddr;
dip_client_routines     *Client;


#ifndef DIP_PRIORITY
#define DIP_PRIORITY DP_NORMAL
#endif

dip_status DIPENTRY DIPImpOldTypeBase(imp_image_handle *ii, imp_type_handle *it,
                 imp_type_handle *base );

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


#if defined(__386__)
#pragma aux DIPLOAD "*"
#elif defined( __WINDOWS__)

#include <stdlib.h>
#include <windows.h>
#include <i86.h>

#pragma aux (pascal) DIPLOAD export

typedef void (DIPENTRY INTER_FUNC)();

static HANDLE TaskId;
static HANDLE ThisInst;

extern dip_imp_routines *DIPLOAD( dip_status *, dip_client_routines * );

#ifdef DEBUGGING
void Say( char *buff )
{
    MessageBox( (HWND) NULL, buff, "IMP",
            MB_OK | MB_ICONHAND | MB_SYSTEMMODAL );
}
#endif

void DIPENTRY DIPUNLOAD()
{
    PostAppMessage( TaskId, WM_QUIT, 0, 0 );
}

int PASCAL WinMain( HANDLE this_inst, HANDLE prev_inst,
                    LPSTR cmdline, int cmdshow )
/***********************************************

    Initialization, message loop.
*/
{
    MSG                 msg;
    INTER_FUNC          **func;
    unsigned            count;
    struct {
        INTER_FUNC      *load;
        INTER_FUNC      *unload;
    }                   *link;
    unsigned            seg;
    unsigned            off;


    prev_inst = prev_inst;
    cmdshow = cmdshow;
    seg = strtoul( cmdline, &cmdline, 16 );
    off = strtoul( cmdline, NULL, 16 );
    if( seg == 0 || off == 0 ) return( 1 );
    link = MK_FP( seg, off );
    TaskId = GetCurrentTask();
    ThisInst = this_inst;
    func = (INTER_FUNC **)&ImpInterface.handle_size;
    count = (sizeof(dip_imp_routines)-offsetof(dip_imp_routines,handle_size))
                / sizeof( INTER_FUNC * );
    while( count != 0 ) {
        *func = (INTER_FUNC *)MakeProcInstance( (FARPROC)*func, this_inst );
        ++func;
        --count;
    }
    link->load = (INTER_FUNC *)MakeProcInstance( (FARPROC)DIPLOAD, this_inst );
    link->unload = (INTER_FUNC *)MakeProcInstance( (FARPROC)DIPUNLOAD, this_inst );
    while( GetMessage( &msg, NULL, NULL, NULL ) ) {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }

    return( 0 );
}
#elif defined(M_I86)
#pragma aux DIPLOAD "*" loadds
#elif defined(__AXP__)
/* nothing to do */
#else
#error DIPIMP.C not configured for system
#endif

#if defined(__DOS__) || defined(__QNX__)
    const char __based( __segname( "_CODE" ) ) Signature[4] = "DIP";
#endif

dip_imp_routines *DIPLOAD( dip_status *status, dip_client_routines *client )
{
    Client = client;
#if defined(__WINDOWS__) && !defined(__386__)
    {
        dip_status (DIPENTRY *start)(void);

        start = (INTER_FUNC *)MakeProcInstance( (FARPROC)DIPImpStartup, ThisInst );
        *status = start();
        FreeProcInstance( (FARPROC)start );
    }
#else
    *status = DIPImpStartup();
#endif
    if( *status & DS_ERR ) return( NULL );
    return( &ImpInterface );
}

void *DCAlloc( unsigned amount )
{
    return( Client->alloc( amount ) );
}

void *DCRealloc( void *p, unsigned amount )
{
    return( Client->realloc( p, amount ) );
}

void DCFree( void *p )
{
    Client->free( p );
}

void DCMapAddr( addr_ptr *a, void *d )
{
    Client->map_addr( a, d );
}

imp_sym_handle *DCSymCreate( imp_image_handle *ii, void *d )
{
    return( Client->sym_create( ii, d ) );
}

dip_status DCItemLocation( location_context *lc, context_item ci,
                        location_list *ll )
{
    return( Client->item_location( lc, ci, ll ) );
}

dip_status DCAssignLocation( location_list *dst, location_list *src,
                        unsigned long len )
{
    return( Client->assign_location( dst, src, len ) );
}

dip_status DCSameAddrSpace( address a, address b )
{
    return( Client->same_addr_space( a, b ) );
}

void DCAddrSection( address *a )
{
    Client->addr_section( a );
}

dig_fhandle DCOpen( char *path, dig_open flags )
{
    return( Client->open( path, flags ) );
}

unsigned long DCSeek( dig_fhandle h, unsigned long p, dig_seek w )
{
    return( Client->seek( h, p, w ) );
}

unsigned DCRead( dig_fhandle h, void *b, unsigned s )
{
    return( Client->read( h, b, s ) );
}

unsigned DCWrite( dig_fhandle h, void *b, unsigned s )
{
    return( Client->write( h, b, s ) );
}

void DCClose( dig_fhandle h )
{
    Client->close( h );
}

void DCRemove( char *path, dig_open flags )
{
    Client->remove( path, flags );
}

void DCStatus( dip_status status )
{
    Client->status( status );
}

mad_handle DCCurrMAD( void )
{
    /* check for old client */
    if( Client->sizeof_struct < offsetof(dip_client_routines,curr_mad) ) return( MAD_X86 );
    return( Client->curr_mad() );
}

unsigned        DCMachineData( address a, unsigned info_type,
                                unsigned in_size,  void *in,
                                unsigned out_size, void *out )
{
    if( Client->sizeof_struct < offsetof(dip_client_routines,DIGCliMachineData) ) return( 0 );
    return( Client->DIGCliMachineData( a, info_type, in_size, in, out_size, out ) );
}

dip_status DIPENTRY DIPImpOldTypeBase(imp_image_handle *ii, imp_type_handle *it,
                 imp_type_handle *base )
{
    return( ImpInterface.type_base( ii, it, base, NULL, NULL ) );
}
