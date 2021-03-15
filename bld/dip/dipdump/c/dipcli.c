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
* Description:  Dummy DIP client routines.
*
****************************************************************************/


//#include <sys/types.h>
#include <stdio.h>
#include "bool.h"
#include "wio.h"
#include "dip.h"
#include "dipimp.h"

#if 0
# define dprintf(a)     do { printf a; } while( 0 )
#else
# define dprintf(a)     do {} while( 0 )
#endif


/*
 * DIPCliImageUnload
 */
void DIPCLIENTRY( ImageUnload )( mod_handle hdl )
{
    dprintf(( "DIPCliImageUnload: hld=%#x\n", hdl ));
    hdl = hdl;
}

/*
 * DIPCliMapAddr
 */
void DIPCLIENTRY( MapAddr )( addr_ptr *addr, void *ptr )
{
    dprintf(( "DIPCliMapAddr: addr=%p:{%04x:%08lx} ptr=%p\n",
              addr, addr->segment, (long)addr->offset, ptr ));
    addr = addr;
    ptr = ptr;
}

/*
 * DIPCliSymCreate
 */
imp_sym_handle *DIPCLIENTRY( SymCreate )( imp_image_handle *iih, void *d )
{
    /* unused parameters */ (void)iih; (void)d;

    dprintf(( "DIPCliSymCreate: iih=%p, ptr=%p\n", iih, d ));
    return( NULL );
}

#if 0
/*
 * DIPCliSectLoaded
 */
dip_status DIPCLIENTRY( SectLoaded )( unsigned sect )
{
    dprintf(( "DIPCliSectLoaded: ptr=%p\n", sect ));
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
    dprintf(( "DIPCliItemLocation:\n" ));
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
    dprintf(( "DIPCliAssignLocation:\n" ));
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
    dprintf(( "DIPCliSameAddrSpace: a1=%04x:%08lx a2=%04x:%08lx\n",
              a1.mach.segment, (long)a1.mach.offset,
              a2.mach.segment, (long)a2.mach.offset));
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
    dprintf(( "DIPCliAddrSection: addr=%p:{%04x:%08lx}\n",
              addr, addr->mach.segment, addr->mach.offset ));
    addr->sect_id = 0;
}

/*
 * DIPCliStatus
 */
void DIPCLIENTRY( Status )( dip_status ds )
{
    dprintf(( "DIPCliStatus: status=%d\n", ds ));
    /* unused parameters */ (void)ds;
}

/*
 * DIPCliCurrArch
 */
dig_arch DIPCLIENTRY( CurrArch )( void )
{
    //dprintf(( "DIPCliCurrArch\n" ));
    return( DIG_ARCH_X86 ); ///@todo option!
}
