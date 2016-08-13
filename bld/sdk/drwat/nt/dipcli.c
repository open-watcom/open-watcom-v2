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
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "drwatcom.h"
#include "dip.h"
#include "dipimp.h"
#include "dipcli.h"
#include "mem.h"

/*
 * mapAddress
 */
static void mapAddress( addr_ptr *addr, ModuleNode *mod )
{
    DWORD       seg;

    if( addr->segment == MAP_FLAT_CODE_SELECTOR ) {
        seg = 0;
        addr->segment = mod->procnode->SegCs;
    } else if( addr->segment == MAP_FLAT_DATA_SELECTOR ) {
        seg = 0;
        addr->segment = mod->procnode->SegDs;
    } else {
        seg = addr->segment - 1;
        if( seg < mod->syminfo->segcnt && mod->syminfo->seginfo[seg].code ) {
            addr->segment = mod->procnode->SegCs;
        } else {
            addr->segment = mod->procnode->SegDs;
        }
    }
    if( seg >= mod->syminfo->segcnt ) {
        addr->offset = -1;
    } else {
        addr->offset += mod->syminfo->seginfo[seg].segoff;
    }
}

/*
 * DIPCliImageUnload
 */
void DIGCLIENT DIPCliImageUnload( mod_handle hdl )
{
    hdl = hdl;
    //
    // do nothing - we don't have anything to clean up
    //
}

/*
 * DIGCliAlloc
 */
void *DIGCLIENT DIGCliAlloc( size_t size )
{
    return( MemAlloc( size ) );
}

/*
 * DIGCliRealloc
 */
void *DIGCLIENT DIGCliRealloc( void *ptr, size_t size )
{
    return( MemReAlloc( ptr, size ) );
}

/*
 * DIGCliFree
 */
void DIGCLIENT DIGCliFree( void *ptr )
{
    MemFree( ptr );
}


/*
 * DIPCliMapAddr
 */
void DIGCLIENT DIPCliMapAddr( addr_ptr *addr, void *ptr )
{
    mapAddress( addr, ptr );
}

/*
 * DIPCliSymCreate
 */
imp_sym_handle *DIGCLIENT DIPCliSymCreate( imp_image_handle *ih, void *d )
{
#ifdef DEBUG
    MessageBox( NULL, "symcreate called", "dipcli.c", MB_OK );
#endif
    ih=ih;
    d=d;
    return( NULL );
}

#if 0
/*
 * DIPCliSectLoaded
 */
dip_status DIGCLIENT DIPCliSectLoaded( unsigned sect )
{
    //
    // there are no overlays in NT so just return TRUE
    //
    sect = sect;
    return( DS_OK );
}
#endif

/*
 * DIPCliItemLocation
 */
dip_status DIGCLIENT DIPCliItemLocation( location_context *context,
                                      context_item item, location_list *loc )
{
#ifdef DEBUG
    MessageBox( NULL, "itemlocation called", "dipcli.c", MB_OK );
#endif
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
#ifdef DEBUG
    MessageBox( NULL, "assignlocation called", "dipcli.c", MB_OK );
#endif
    loc1 = loc1;
    loc2 = loc2;
    item = item;
    return( DS_FAIL );
}

/*
 * DIPCliSameAddrSpace
 */
dip_status DIGCLIENT DIPCliSameAddrSpace( address a1, address a2 )
{
//    return( a1.mach.segment == a2.mach.segment );
    a1 = a1;
    a2 = a2;
    return( DS_OK );
}

/*
 * DIPCliAddrSection
 */
void DIGCLIENT DIPCliAddrSection( address *addr )
{
    addr->sect_id = 0;
}


/*
 * DIGCliOpen
 */
dig_fhandle DIGCLIENT DIGCliOpen( const char *path, dig_open mode )
{
    HFILE               ret;
    int                 flags;
    OFSTRUCT            tmp;

    flags = 0;
    if( mode & DIG_READ )  flags |= OF_READ;
    if( mode & DIG_WRITE ) flags |= OF_WRITE;
    if( mode & DIG_TRUNC ) flags |= OF_CREATE;
    if( mode & DIG_CREATE ) flags |= OF_CREATE;
    //NYI: should check for DIG_SEARCH
    ret = OpenFile( path, &tmp, flags );
    if( ret == HFILE_ERROR )
        return( DIG_NIL_HANDLE );
    return( (dig_fhandle)ret );
}

/*
 * DIGCliSeek
 */
unsigned long DIGCLIENT DIGCliSeek( dig_fhandle dfh, unsigned long offset, dig_seek dipmode )
{
    int         mode;

    switch( dipmode ) {
    case DIG_ORG:
        mode = FILE_BEGIN;
        break;
    case DIG_CUR:
        mode = FILE_CURRENT;
        break;
    case DIG_END:
        mode = FILE_END;
        break;
    }
    return( SetFilePointer( (HANDLE)dfh, offset, 0, mode ) );
}

/*
 * DIPCliRead
 */
size_t DIGCLIENT DIGCliRead( dig_fhandle dfh, void *buf, size_t size )
{
    DWORD       bytesread;

    if( !ReadFile( (HANDLE)dfh, buf, size, &bytesread, NULL ) )
        return( DIG_RW_ERROR );
    return( bytesread );
}

/*
 * DIPCliWrite
 */
size_t DIGCLIENT DIGCliWrite( dig_fhandle dfh, const void *buf, size_t size )
{
    DWORD       byteswritten;

    if( !WriteFile( (HANDLE)dfh, buf, size, &byteswritten, NULL ) )
        return( DIG_RW_ERROR );
    return( byteswritten );
}

/*
 * DIGCliClose
 */
void DIGCLIENT DIGCliClose( dig_fhandle dfh )
{
    CloseHandle( (HANDLE)dfh );
}

/*
 * DIGCliRemove
 */
void DIGCLIENT DIGCliRemove( const char *path, dig_open mode )
{
    mode = mode;
    DeleteFile( path );
}

/*
 * DIPCliStatus
 */
void DIGCLIENT DIPCliStatus( dip_status stat )
{
    stat = stat;
}

/*
 * DIPCliCurrMAD
 */
dig_mad DIGCLIENT DIPCliCurrMAD( void )
/*************************************/
{
    return( SysConfig.mad );
}

