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
#include <sys\types.h>
#include <sys\stat.h>
#include <fcntl.h>
#include <windows.h>
#include "dip.h"
#include "dipcli.h"
#include "mem.h"
#include "procctl.h"

/*
 * mapAddress
 */
static void mapAddress( addr_ptr *addr, ModuleNode *mod ) {

    DWORD       seg;

    seg = addr->segment - 1;
    if( seg >= mod->syminfo->segcnt ) {
        addr->offset = -1;
    } else {
        addr->offset += mod->syminfo->segoff[seg];
    }
}

/*
 * DIPCliImageUnload
 */
void DIPCLIENT DIPCliImageUnload( mod_handle hdl ) {
    hdl = hdl;
    //
    // do nothing - we don't have anything to clean up
    //
}

/*
 * DIPCliAlloc
 */
void *DIPCLIENT DIPCliAlloc( unsigned size ) {
    return( MemAlloc( size ) );
}

/*
 * DIPCliRealloc
 */
void *DIPCLIENT DIPCliRealloc( void *ptr, unsigned size ) {
    return( MemReAlloc( ptr, size ) );
}

/*
 * DIPCliFree
 */
void DIPCLIENT DIPCliFree( void *ptr ) {
    MemFree( ptr );
}


/*
 * DIPCliMapAddr
 */
void DIPCLIENT DIPCliMapAddr( addr_ptr *addr, void *ptr ) {

    mapAddress( addr, ptr );
}

/*
 * DIPCliSymCreate
 */
sym_handle *DIPCLIENT DIPCliSymCreate( void *ptr ) {
#ifdef DEBUG
    MessageBox( NULL, "symcreate called", "dipcli.c", MB_OK );
#endif
    ptr = ptr;
    return( NULL );
}

/*
 * DIPCliSectLoaded
 */
dip_status DIPCLIENT DIPCliSectLoaded( unsigned sect ) {
    //
    // there are no overlays in NT so just return TRUE
    //
    sect = sect;
    return( DS_OK );
}

/*
 * DIPCliItemLocation
 */
dip_status DIPCLIENT DIPCliItemLocation( location_context *context,
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
dip_status DIPCLIENT DIPCliAssignLocation( location_list *loc1,
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
dip_status DIPCLIENT DIPCliSameAddrSpace( address a1, address a2 ) {
//    return( a1.mach.segment == a2.mach.segment );
    a1 = a1;
    a2 = a2;
    return( DS_OK );
}

/*
 * DIPCliAddrSection
 */
void DIPCLIENT DIPCliAddrSection( address *addr ) {
    addr->sect_id = 0;
}


/*
 * DIPCliOpen
 */
dip_fhandle DIPCLIENT DIPCliOpen( char *path, dip_open mode ) {

    dip_fhandle         ret;
    int                 flags;
    OFSTRUCT            tmp;

    flags = 0;
    if( mode & DIP_READ )  flags |= OF_READ;
    if( mode & DIP_WRITE ) flags |= OF_WRITE;
    if( mode & DIP_TRUNC ) flags |= OF_CREATE;
    if( mode & DIP_CREATE ) flags |= OF_CREATE;
    ret = (dip_fhandle)OpenFile( path, &tmp, flags );
    return( ret );
}

/*
 * DIPCliSeek
 */
unsigned long DIPCLIENT DIPCliSeek( dip_fhandle hdl, unsigned long offset,
                                   dip_seek dipmode )
{
    int         mode;

    switch( dipmode ) {
    case DIP_ORG:
        mode = FILE_BEGIN;
        break;
    case DIP_CUR:
        mode = FILE_CURRENT;
        break;
    case DIP_END:
        mode = FILE_END;
        break;
    }
    return( SetFilePointer( (HANDLE)hdl, offset, 0, mode ) );
}

/*
 * DIPCliRead
 */
unsigned DIPCLIENT DIPCliRead( dip_fhandle hdl, void *buf, unsigned size ) {

    DWORD       bytesread;

    if( !ReadFile( (HANDLE)hdl, buf, size, &bytesread, NULL ) ) return( 0 );
    return( bytesread );
}

/*
 * DIPCliWrite
 */
unsigned DIPCLIENT DIPCliWrite( dip_fhandle hdl, void *buf, unsigned size ) {
    DWORD       byteswritten;

    if( !WriteFile( (HANDLE)hdl, buf, size, &byteswritten, NULL ) ) {
        return( 0 );
    }
    return( byteswritten );
}

/*
 * DIPCliClose
 */
void DIPCLIENT DIPCliClose( dip_fhandle hdl ) {
    CloseHandle( (HANDLE)hdl );
}

/*
 * DIPCliRemove
 */
void DIPCLIENT DIPCliRemove( char *path, dip_open mode ) {
    mode = mode;
    DeleteFile( path );
}

/*
 * DIPCliStatus
 */
void DIPCLIENT DIPCliStatus( dip_status stat ) {
    stat = stat;
}
