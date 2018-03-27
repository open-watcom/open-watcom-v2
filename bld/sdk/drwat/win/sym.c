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


#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dos.h>
#include <malloc.h>
#include <io.h>
#include "drwatcom.h"
#include "walloca.h"
#include "sopen.h"
#include "dip.h"
#include "dipimp.h"
#include "dipcli.h"

//#define DEBUGOUT( x ) LBPrintf( ListBox, x );
#define DEBUGOUT( x )
static process_info     *curProcess;
static mod_handle       curModHdl;
static BOOL             dipIsLoaded;


#if 0
FILE *PathOpen( char *name, unsigned len, char *ext )
{
    char        path[ _MAX_PATH ];
    char        *realname;
    char        *filename;

    len = len;
    if( ext == NULL || *ext == '\0' ) {
        realname = name;
    } else {
        realname = MemAlloc( _MAX_PATH );
        filename = MemAlloc( _MAX_FNAME );
        _splitpath( name, NULL, NULL, filename, NULL );
        _makepath( realname, NULL, NULL, filename, ext );
        MemFree( realname );
        MemFree( filename );
    }
    _searchenv( realname, "PATH", path );
    if( *path == '\0' ) {
        return( NULL );
    } else {
        return( DIGCli( Open )( path, DIG_READ ) );
    }
}
#endif

/*
 * InitSymbols
 */
bool InitSymbols( void )
{
    int         rc;
    BOOL        diploaded;

    diploaded = FALSE;
    rc = DIPInit();
    if( rc & DS_ERR ) {
        RCMessageBox( NULL, STR_CANT_INIT_DIP, AppName,
                      MB_OK | MB_ICONEXCLAMATION );
        return( false );
    }
    if( LoadTheDips() ) {
        dipIsLoaded = TRUE;
        return( true );
    } else {
        return( false );
    }
}

/*
 * FiniSymbols
 */
void FiniSymbols( void )
{
    if( dipIsLoaded ) {
        DIPFini();
        FiniDipMsgs();
    }
}

/*
 * LoadDbgInfo
 */
bool LoadDbgInfo( void )
{
    dip_priority    priority;
    FILE            *fp;

    DEBUGOUT( "Enter LoadDbgInfo" );
    curModHdl = NO_MOD;
    curProcess = DIPCreateProcess();
    fp = DIGCli( Open )( DTModuleEntry.szExePath , DIG_READ );
    if( fp != NULL ) {
        DEBUGOUT( "File open OK" );
        for( priority = 0; (priority = DIPPriority( priority )) != 0; ) {
            curModHdl = DIPLoadInfo( fp, 0, priority );
            if( curModHdl != NO_MOD ) {
                break;
            }
        }
        DIGCli( Close )( fp );
        if( curModHdl != NO_MOD ) {
            DEBUGOUT( "debug info load OK" );
            DIPMapInfo( curModHdl, NULL );
            return( true );
        }
        DEBUGOUT( "curModHdl == NO_MOD" );
    }
    DEBUGOUT( "LoadDbgInfo Failed" );
    DIPDestroyProcess( curProcess );
    curProcess = NULL;
    return( false );
}

/*
 * doFindSymbol
 */
static BOOL doFindSymbol( ADDRESS *addr, syminfo *si, int getsrcinfo )
{
    sym_handle          *symhdl;
    cue_handle          *cueh;
    search_result       sr;
    location_list       ll;
    address             dipaddr;
    BOOL                ret;

    si->segnum = -1;
    si->name[0] = 0;
    if( !StatShowSymbols || curProcess == NULL ) {
        return( FALSE );
    }
    symhdl = MemAlloc( DIPHandleSize( HK_SYM ) );
    dipaddr.sect_id = 0;
    dipaddr.indirect = FALSE;
    dipaddr.mach.offset = addr->offset;
    dipaddr.mach.segment = addr->seg;
    sr = DIPAddrSym( NO_MOD, dipaddr, symhdl );
    switch( sr ) {
    case SR_CLOSEST:
        DIPSymLocation( symhdl, NULL, &ll );
        si->symoff = addr->offset - ll.e[0].u.addr.mach.offset;
        break;
    case SR_EXACT:
        si->symoff = 0;
        break;
    case SR_NONE:
        ret = FALSE;
        break;
    }
    if( sr != SR_NONE ) {
        DIPSymName( symhdl, NULL, SN_OBJECT, si->name, MAX_SYM_NAME );
//      DIPSymName( symhdl, NULL, SN_SOURCE, si->name, MAX_SYM_NAME );
        if( getsrcinfo ) {
            cueh = walloca( DIPHandleSize( HK_CUE ) );
            if( DIPAddrCue( NO_MOD, dipaddr, cueh ) == SR_NONE ) {
                ret = FALSE;
            } else {
                DIPCueFile( cueh, si->filename, MAX_FILE_NAME );
                si->linenum = DIPCueLine( cueh );
                ret = TRUE;
            }
        }
    }
    MemFree( symhdl );
    return( ret );
}

/*
 * FindWatSymbol
 */
RVALUE FindWatSymbol( ADDRESS *addr, syminfo *si, int getsrcinfo )
{
    if( doFindSymbol( addr, si, getsrcinfo ) ) {
        return( FOUND );
    } else {
        return( NO_INFO );
    }
}

/*
 * FindSymbol - locate a symbol for a name
 */
BOOL FindSymbol( ADDRESS *addr, syminfo *si )
{
    return( doFindSymbol( addr, si, FALSE ) );
}

/*
 * UnloadDbgInfo - cleanup DIP
 */
void UnloadDbgInfo( void )
{
    if( curModHdl != NO_MOD ) {
        DIPUnloadInfo( curModHdl );
        curModHdl = NO_MOD;
    }
    if( curProcess != NULL ) {
        DIPDestroyProcess( curProcess );
        curProcess = NULL;
    }
}
