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
void DIPCLIENTRY( ImageUnload )( mod_handle hdl )
{
    hdl = hdl;
    //
    // do nothing - we don't have anything to clean up
    //
}

/*
 * DIPCliMapAddr
 */
void DIPCLIENTRY( MapAddr )( addr_ptr *addr, void *ptr )
{
    mapAddress( addr, ptr );
}

/*
 * DIPCliSymCreate
 */
imp_sym_handle *DIPCLIENTRY( SymCreate )( imp_image_handle *ih, void *d )
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
dip_status DIPCLIENTRY( SectLoaded )( unsigned sect )
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
dip_status DIPCLIENTRY( ItemLocation )( location_context *context,
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
dip_status DIPCLIENTRY( AssignLocation )( location_list *loc1,
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
dip_status DIPCLIENTRY( SameAddrSpace )( address a1, address a2 )
{
//    return( a1.mach.segment == a2.mach.segment );
    a1 = a1;
    a2 = a2;
    return( DS_OK );
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
void DIPCLIENTRY( Status )( dip_status stat )
{
    stat = stat;
}

/*
 * DIPCliCurrMAD
 */
dig_mad DIPCLIENTRY( CurrMAD )( void )
/************************************/
{
    return( SysConfig.mad );
}

