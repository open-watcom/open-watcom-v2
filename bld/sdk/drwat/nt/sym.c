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


#include "drwatcom.h"
#include <io.h>
#include "mad.h"


/*
 * InitDip
 */
bool InitDip( void )
{
    if( DIPInit() & DS_ERR ) {
        RCMessageBox( NULL, STR_CANT_LOAD_DIP, AppName, MB_OK | MB_ICONEXCLAMATION );
        return( false );
    }
    return( LoadTheDips() );
}

/*
 * GetLineNum
 */
bool GetLineNum( address *addr, char *fname, DWORD bufsize, DWORD *line )
{
    cue_handle  *cueh;

    cueh = alloca( DIPHandleSize( HK_CUE ) );
    if( DIPAddrCue( NO_MOD, *addr, cueh ) == SR_NONE ) {
        return( false );
    }
    DIPCueFile( cueh, fname, bufsize );
    *line = DIPCueLine( cueh );
    return( true );
}



/*
 * GetSymbolName
 */
bool GetSymbolName( address *addr, char *name, DWORD *symoff )
{
    sym_handle          *symhdl;
    search_result       sr;
    location_list       ll;

    symhdl = MemAlloc( DIPHandleSize( HK_SYM ) );
    sr = DIPAddrSym( NO_MOD, *addr, symhdl );
    switch( sr ) {
    case SR_CLOSEST:
        DIPSymLocation( symhdl, NULL, &ll );
        *symoff = MADAddrDiff( addr,&(ll.e[0].u.addr), MAF_FULL );
        break;
    case SR_EXACT:
        *symoff = 0;
        break;
    case SR_NONE:
        MemFree( symhdl );
        return( false );
    }
    DIPSymName( symhdl, NULL, SNT_OBJECT, name, MAX_SYM_NAME );
    MemFree( symhdl );
    return( true );
}

/*
 * LoadDbgInfo
 */
bool LoadDbgInfo( ModuleNode *mod )
{
    dip_priority    priority;

    if( !GetSegmentList( mod ) )
        return( false );
    mod->syminfo->procinfo = DIPCreateProcess();
    for( priority = 0; (priority = DIPPriority( priority )) != 0; ) {
        mod->syminfo->hdl = DIPLoadInfo( mod->fp, 0, priority );
        if( mod->syminfo->hdl != NO_MOD ) {
            break;
        }
    }
    if( mod->syminfo->hdl == NO_MOD )
        return( false );
    return( true );
}

/*
 * UnloadDbgInfo
 */
void UnloadDbgInfo( ModuleNode *mod )
{
    if( mod->syminfo != NULL ) {
        DIPUnloadInfo( mod->syminfo->hdl );
        DIPDestroyProcess( mod->syminfo->procinfo );
        MemFree( mod->syminfo );
        mod->syminfo = NULL;
    }
}
