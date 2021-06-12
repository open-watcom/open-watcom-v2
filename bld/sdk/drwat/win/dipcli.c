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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "drwatcom.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dos.h>
#include <io.h>
#include "sopen.h"
#include "dip.h"
#include "dipimp.h"
#include "dipcli.h"
#if 0
#include "pathgrp2.h"

#include "clibext.h"
#endif


//#define DEBUGOUT( x ) LBPrintf( ListBox, x );
#define DEBUGOUT( x )

#if 0
FILE *PathOpen( char *name, unsigned len, const char *ext )
{
    pgroup2     pg;
    char        path[ _MAX_PATH ];
    char        *realname;
    char        *filename;

    /* unused parameters */ (void)len;

    if( ext == NULL || *ext == '\0' ) {
        realname = name;
    } else {
        realname = MemAlloc( _MAX_PATH );
        filename = MemAlloc( _MAX_FNAME );
        _splitpath2( name, pg.buffer, NULL, NULL, &pg.fname, NULL );
        _makepath( realname, NULL, NULL, pg.fname, ext );
        MemFree( realname );
        MemFree( filename );
    }
    _searchenv( realname, "PATH", path );
    if( *path == '\0' )
        return( NULL );
    return( DIGCli( Open )( path, DIG_READ ) );
}
#endif

/*##########################################################################
  #
  # CLIENT routines for the DIP
  #
  ##########################################################################*/

/*
 * DIPCliImageUnload
 */
void DIPCLIENTRY( ImageUnload )( mod_handle hdl )
{
    hdl = hdl;
    DEBUGOUT( "ImageUnload" );
    //
    // do nothing - we don't have anything to clean up
    //
}

/*
 * horkyFindSegment - runs and tries to find a segment.  It does this by
 * finding the module entry in the global heap for this task.  The module
 * entry is a lot like an NE header, except that instead of segment numbers,
 * it has the selector values themselves.  For the format of the module entry
 * see p 319 of Undocumented Windows
 */
static WORD horkyFindSegment( HMODULE mod, WORD seg )
{
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
void DIPCLIENTRY( MapAddr )( addr_ptr *addr, void *info )
{
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
        sel = _FP_SEG( ptr );
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
imp_sym_handle *DIPCLIENTRY( SymCreate )( imp_image_handle *iih, void *d )
{
    /* unused parameters */ (void)iih; (void)d;

    return( NULL );
}

#if 0
/*
 * DIPCliSectLoaded
 */
dip_status DIPCLIENTRY( SectLoaded )( unsigned sect )
{
    //
    // there are no overlays in Windows so just return TRUE
    //
    sect = sect;
    return( DS_OK );
}
#endif

/*
 * DIPCliItemLocation
 */
dip_status DIPCLIENTRY( ItemLocation )( location_context *context,
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
dip_status DIPCLIENTRY( AssignLocation )( location_list *loc1,
                            const location_list *loc2, unsigned long item )
{
    loc1 = loc1;
    loc2 = loc2;
    item = item;
    return( DS_FAIL );
}

/*
 * DIPCliSameAddrSpace
 */
dip_status DIPCLIENTRY( SameAddrSpace )( address a1, address a2 )
{
    if( a1.mach.segment == a2.mach.segment ) {
        return( DS_OK );
    } else {
        return( DS_FAIL );
    }
}

/*
 * DIPCliAddrSection
 */
void DIPCLIENTRY( AddrSection )( address *addr )
{
    addr->sect_id = 0;
}


/*
 * DIPCliStatus
 */
void DIPCLIENTRY( Status )( dip_status ds )
{
    /* unused parameters */ (void)ds;
}

/*
 * DIPCliCurrArch
 */
dig_arch DIPCLIENTRY( CurrArch )( void )
/**************************************/
{
    return( DIG_ARCH_X86 );
}
