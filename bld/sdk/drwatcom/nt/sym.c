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


#include <io.h>
#include "drwatcom.h"
#include "mem.h"

/*
 * InitDip
 */
BOOL InitDip( void ) {

    if( DIPInit() & DS_ERR ) {
        RCMessageBox( NULL, STR_CANT_LOAD_DIP, AppName,
                    MB_OK | MB_ICONEXCLAMATION );
        return( FALSE );
    }
    return( LoadTheDips() );
}

/*
 * getMapSeg
 */
static WORD getMapSeg( ModuleNode *mod, DWORD offset ) {

    DWORD       best;
    DWORD       cur;
    DWORD       *offsets;

    best = -1;
    offsets = mod->syminfo->segoff;
    for( cur=0; cur < mod->syminfo->segcnt; cur++ ) {
        if( offsets[ cur ] < offset ) {
            if( best == -1 || offsets[ cur ] > offsets[ best ] ) {
                best = cur;
            }
        }
    }
    if( best != -1 ) best++;
    return( best );
}

/*
 * GetLineNum
 */
BOOL GetLineNum( ModuleNode *mod, DWORD offset, char *fname,
                 DWORD bufsize, DWORD *line )
{
    address     addr;
    cue_handle  *cue;

    if( mod->syminfo == NULL ) {
        return( FALSE );
    }
    cue = MemAlloc( DIPHandleSize( HK_CUE ) );
    addr.sect_id = 0;
    addr.indirect = FALSE;
    addr.mach.offset = offset;
    addr.mach.segment = getMapSeg( mod, offset );
    if( AddrCue( NO_MOD, addr, cue ) == SR_NONE ) {
        MemFree( cue );
        return( FALSE );
    }
    CueFile( cue, fname, bufsize );
    *line = CueLine( cue );
    MemFree( cue );
    return( TRUE );
}

/*
 * GetSymbolName
 */
BOOL GetSymbolName( ModuleNode *mod, DWORD offset, char *name,
                    DWORD *symoff )
{
    sym_handle          *symhdl;
    address             addr;
    search_result       sr;
    location_list       ll;

    if( mod->syminfo == NULL ) {
        return( FALSE );
    }
    addr.sect_id = 0;
    addr.indirect = FALSE;
    addr.mach.offset = offset;
    addr.mach.segment = getMapSeg( mod, offset );
    symhdl = MemAlloc( DIPHandleSize( HK_SYM ) );
    sr = AddrSym( NO_MOD, addr, symhdl );
    switch( sr ) {
    case SR_CLOSEST:
        SymLocation( symhdl, NULL, &ll );
        *symoff = offset - ll.e[0].u.addr.mach.offset;
        break;
    case SR_EXACT:
        *symoff = 0;
        break;
    case SR_NONE:
        MemFree( symhdl );
        return( FALSE );
    }
    SymName( symhdl, NULL, SN_OBJECT, name, MAX_SYM_NAME );
    MemFree( symhdl );
    return( TRUE );
}

/*
 * LoadDbgInfo
 */
BOOL LoadDbgInfo( ModuleNode *mod ) {
    unsigned            priority;

    if( !GetSegmentList( mod ) ) return( FALSE );
    mod->syminfo->procinfo = DIPCreateProcess();
    priority = 0;
    for( ;; ) {
        priority = DIPPriority( priority );
        if( priority == 0 ) break;
        mod->syminfo->hdl = DIPLoadInfo( (dip_fhandle)mod->fhdl, 0, priority );
        if( mod->syminfo->hdl != NO_MOD ) break;
    }
    if( mod->syminfo->hdl != NO_MOD ) {
        DIPMapInfo( mod->syminfo->hdl, mod );
    } else {
        return( FALSE );
    }
    return( TRUE );
}

/*
 * UnloadDbgInfo
 */
void UnloadDbgInfo( ModuleNode *mod ) {
    if( mod->syminfo != NULL ) {
        DIPUnloadInfo( mod->syminfo->hdl );
        DIPDestroyProcess( mod->syminfo->procinfo );
        MemFree( mod->syminfo );
        mod->syminfo = NULL;
    }
}

