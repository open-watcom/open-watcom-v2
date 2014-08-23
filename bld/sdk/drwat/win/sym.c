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
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dos.h>
#include <malloc.h>
#include <io.h>
#include "bool.h"
#include "sopen.h"
#include "drwatcom.h"
#include "dip.h"
#include "dipcli.h"

//#define DEBUGOUT( x ) LBPrintf( ListBox, x );
#define DEBUGOUT( x )
static process_info     *curProcess;
static mod_handle       curModHdl;
static dig_fhandle      curFileHdl;
static BOOL             dipIsLoaded;



dig_fhandle PathOpen( char *name, unsigned len, char *ext ) {

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
        return( -1 );
    } else {
        return( DIGCliOpen( path, DIG_READ ) );
    }
}

/*
 * InitSymbols
 */
BOOL InitSymbols( void ) {

    int         rc;
    BOOL        diploaded;

    diploaded = FALSE;
    rc = DIPInit();
    if( rc & DS_ERR ) {
        RCMessageBox( NULL, STR_CANT_INIT_DIP, AppName,
                      MB_OK | MB_ICONEXCLAMATION );
        return( FALSE );
    }
    if( LoadTheDips() ) {
        dipIsLoaded = TRUE;
        return( TRUE );
    } else {
        return( FALSE );
    }
}

/*
 * FiniSymbols
 */
void FiniSymbols( void ) {
    if( dipIsLoaded ) {
        DIPFini();
        FiniDipMsgs();
    }
}

/*
 * LoadDbgInfo
 */
BOOL LoadDbgInfo( void ) {

    BOOL                err;
    unsigned            priority;

    DEBUGOUT( "Enter LoadDbgInfo" );
    err = TRUE;
    curProcess = DIPCreateProcess();
    curFileHdl = DIGCliOpen( DTModuleEntry.szExePath , DIG_READ );
    if( curFileHdl != -1 ) {
        DEBUGOUT( "File open OK" );
        priority = 0;
        for( ;; ) {
            priority = DIPPriority( priority );
            if( priority == 0 ) break;
            curModHdl = DIPLoadInfo( curFileHdl, 0, priority );
            if( curModHdl != NO_MOD ) break;
        }
        if( curModHdl != NO_MOD ) {
                DEBUGOUT( "debug info load OK" );
                DIPMapInfo( curModHdl, NULL );
                err = FALSE;
        } else {
            DEBUGOUT( "curModHdl == NO_MOD" );
        }
    }
    if( err ) {
        DEBUGOUT( "LoadDbgInfo Failed" );
        if( (int)curFileHdl != -1 ) {
            DIGCliClose( curFileHdl );
        }
        DIPDestroyProcess( curProcess );
        curProcess = NULL;
        curModHdl = NO_MOD;
        curFileHdl = -1;
        return( FALSE );
    }
    return( TRUE );
}

/*
 * doFindSymbol
 */
BOOL doFindSymbol( ADDRESS *addr, syminfo *si, int getsrcinfo ) {

    sym_handle          *symhdl;
    cue_handle          *cue;
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
    sr = AddrSym( NO_MOD, dipaddr, symhdl );
    switch( sr ) {
    case SR_CLOSEST:
        SymLocation( symhdl, NULL, &ll );
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
        SymName( symhdl, NULL, SN_OBJECT, si->name, MAX_SYM_NAME );
//      SymName( symhdl, NULL, SN_SOURCE, si->name, MAX_SYM_NAME );
        if( getsrcinfo ) {
            cue = MemAlloc( DIPHandleSize( HK_CUE ) );
            if( AddrCue( NO_MOD, dipaddr, cue ) == SR_NONE ) {
                MemFree( cue );
                ret = FALSE;
            } else {
                CueFile( cue, si->filename, MAX_FILE_NAME );
                si->linenum = CueLine( cue );
                MemFree( cue );
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
BOOL FindSymbol( ADDRESS *addr, syminfo *si ) {

    return( doFindSymbol( addr, si, FALSE ) );
}

/*
 * SymFileClose - close the current symfile
 */
void SymFileClose( void ) {

    if( curModHdl != NO_MOD ) {
        DIPUnloadInfo( curModHdl );
    }
    if( curProcess != NULL ) {
        DIPDestroyProcess( curProcess );
    }
    if( curFileHdl != -1 ) {
        DIGCliClose( curFileHdl );
    }
    curProcess = NULL;
    curModHdl = NO_MOD;
    curFileHdl = -1;
}


/*##########################################################################
  #
  # CLIENT routines for the DIP
  #
  ##########################################################################*/

/*
 * DIPCliImageUnload
 */
void DIGCLIENT DIPCliImageUnload( mod_handle hdl ) {
    hdl = hdl;
    DEBUGOUT( "ImageUnload" );
    //
    // do nothing - we don't have anything to clean up
    //
}

/*
 * DIPCliAlloc
 */
void *DIGCLIENT DIGCliAlloc( size_t size ) {
    void        *ret;

    DEBUGOUT( "alloc BEGIN" );
    ret = MemAlloc( size );
    DEBUGOUT( "alloc END" );
    return( ret );
}

/*
 * DIPCliRealloc
 */
void *DIGCLIENT DIGCliRealloc( void *ptr, size_t size ) {

    void        *ret;

    DEBUGOUT( "realloc BEGIN" );
    ret = MemReAlloc( ptr, size );
    DEBUGOUT( "realloc END" );
    return( ret );
}

/*
 * DIPCliFree
 */
void DIGCLIENT DIGCliFree( void *ptr ) {
    DEBUGOUT( "free BEGIN" );
    MemFree( ptr );
    DEBUGOUT( "free END" );
}

/*
 * horkyFindSegment - runs and tries to find a segment.  It does this by
 * finding the module entry in the global heap for this task.  The module
 * entry is a lot like an NE header, except that instead of segment numbers,
 * it has the selector values themselves.  For the format of the module entry
 * see p 319 of Undocumented Windows
 */
static WORD horkyFindSegment( HMODULE mod, WORD seg ) {

    WORD        sel;
    WORD        offset;
    GLOBALENTRY ge;

    if( !GlobalFirst( &ge, GLOBAL_ALL ) ) {
        return( 0 );
    }
    do {
        if( ge.hOwner == mod && ge.wType == GT_MODULE ) {
            ReadMem( (WORD)ge.hBlock, 0x22, &offset, sizeof( offset ) );
            offset += 8 + ( 10 * seg );
            ReadMem( (WORD)ge.hBlock, offset, &sel, sizeof( sel ) );
            return( sel );
        }
    } while( GlobalNext( &ge, GLOBAL_ALL ) );
    return( 0 );
}

/*
 * DIPCliMapAddr
 * Possibilites:
 *  1) We are mapping segments for a 32-bit extended app.  In this case,
 *     we return the segment:offset returned by CheckIsModuleWin32App
 *  2) We are mapping a segment for a 16-bit app that is NOT a load
 *     on call segment.  In this case, GlobalEntryModule works and
 *     we return the value we obtain from it
 *  3) We are mapping a segment for a 16-bit app that IS a load
 *     on call segment.  In this case, GlobalEntryModule FAILS (stupid
 *     f*cking Windows) and so we have to go find it ourselves using
 *     horkyFindSegment.
 */
void DIGCLIENT DIPCliMapAddr( addr_ptr *addr, void *info ) {

    GLOBALENTRY ge;
    LPVOID      ptr;
    WORD        sel;
    WORD        cs,ds;
    DWORD       off;

    DEBUGOUT( "mapaddr" );
    info = info;
    if( CheckIsModuleWin32App( DTModuleEntry.hModule, &ds, &cs, &off ) ) {
        addr->segment = cs;
        addr->offset = off;
    } else {
        ge.dwSize = sizeof( ge );
        if( !GlobalEntryModule( &ge, DTModuleEntry.hModule, addr->segment ) )
        {
            addr->segment = horkyFindSegment( DTModuleEntry.hModule,
                                              addr->segment );
        }
        ptr = GlobalLock( ge.hBlock );
        GlobalUnlock( ge.hBlock );
        sel = FP_SEG( ptr );
        if( sel == NULL ) {
            sel = (WORD)ge.hBlock + 1;
        }
        addr->segment = sel;
        addr->offset = 0;
    }
}

/*
 * DIPCliSymCreate
 */
sym_handle *DIGCLIENT DIPCliSymCreate( void *ptr ) {
    ptr = ptr;
    return( NULL );
}

/*
 * DIPCliSectLoaded
 */
dip_status DIGCLIENT DIPCliSectLoaded( unsigned sect ) {
    //
    // there are no overlays in Windows so just return TRUE
    //
    sect = sect;
    return( DS_OK );
}

/*
 * DIPCliItemLocation
 */
dip_status DIGCLIENT DIPCliItemLocation( location_context *context,
                                      context_item item, location_list *loc )
{
    context = context;
    item = item;
    loc = loc;
    return( DS_FAIL );
}

/*
 * DIPCliAssignLocation
 */
dip_status DIGCLIENT DIPCliAssignLocation( location_list *loc1,
                                    location_list *loc2, unsigned long item )
{
    loc1 = loc1;
    loc2 = loc2;
    item = item;
    return( DS_FAIL );
}

/*
 * DIPCliSameAddrSpace
 */
dip_status DIGCLIENT DIPCliSameAddrSpace( address a1, address a2 ) {
    if( a1.mach.segment == a2.mach.segment ) {
        return( DS_OK );
    } else {
        return( DS_FAIL );
    }
}

/*
 * DIPCliAddrSection
 */
void DIGCLIENT DIPCliAddrSection( address *addr ) {
    addr->sect_id = 0;
}


/*
 * DIPCliOpen
 */
dig_fhandle DIGCLIENT DIGCliOpen( const char *path, dig_open mode ) {

    dig_fhandle         ret;
    int                 flags;

    flags = O_BINARY;
    if( mode & DIG_READ )  flags |= O_RDONLY;
    if( mode & DIG_WRITE ) flags |= O_WRONLY;
    if( mode & DIG_TRUNC ) flags |= O_TRUNC;
    if( mode & DIG_CREATE ) {
        flags |= O_CREAT;
        ret = (dig_fhandle)sopen4( path, flags, SH_DENYWR, S_IRWXU | S_IRWXG | S_IRWXO );
    } else {
        ret = (dig_fhandle)sopen3( path, flags, SH_DENYWR );
    }
    return( ret );
}

/*
 * DIPCliSeek
 */
unsigned long DIGCLIENT DIGCliSeek( dig_fhandle hdl, unsigned long offset, dig_seek dipmode )
{
    int                 mode;
    unsigned long       ret;

    DEBUGOUT( "seek BEGIN" );
    switch( dipmode ) {
    case DIG_ORG:
        mode = SEEK_SET;
        break;
    case DIG_CUR:
        mode = SEEK_CUR;
        break;
    case DIG_END:
        mode = SEEK_END;
        break;
    }
    ret = lseek( (int)hdl, offset, mode );
    DEBUGOUT( "seek END" );
    return( ret );
}

/*
 * DIPCliRead
 */
unsigned DIGCLIENT DIGCliRead( dig_fhandle hdl, void *buf, unsigned size ) {

    DEBUGOUT( "reading" );
    return( read( (int)hdl, buf, size ) );
}

/*
 * DIPCliWrite
 */
unsigned DIGCLIENT DIGCliWrite( dig_fhandle hdl, const void *buf, unsigned size ) {

    return( write( (int)hdl, buf, size ) );
}

/*
 * DIPCliClose
 */
void DIGCLIENT DIGCliClose( dig_fhandle hdl ) {
    close( (int)hdl );
}

/*
 * DIPCliRemove
 */
void DIGCLIENT DIGCliRemove( const char *path, dig_open mode ) {
    mode = mode;
    remove( path );
}


/*
 * DIPCliStatus
 */
void DIGCLIENT DIPCliStatus( dip_status stat ) {
    stat = stat;
}

/*
 * DIPCliCurrMAD
 */
mad_handle DIGCLIENT DIPCliCurrMAD( void )
/****************************************/
{
    return( MAD_X86 );
}

/*
 * DIGCliMachineData
 */
unsigned DIGCLIENT DIGCliMachineData( address addr, unsigned info_type,
                        unsigned in_size,  const void *in,
                        unsigned out_size, void *out )
{
    addr = addr;
    info_type = info_type;
    in_size = in_size;
    in = in;
    out_size = out_size;
    out = out;
    return( 0 );
}
