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


/*
 *  OBJPASS2  : pass 2 of WATFOR-77 8086 linker
 *
*/

#include <string.h>
#include "linkstd.h"
#include "msg.h"
#include "newmem.h"
#include "wlnkmsg.h"
#include "virtmem.h"
#include "obj2supp.h"
#include "dbgall.h"
#include "mapio.h"
#include "overlays.h"
#include "objfree.h"
#include "distrib.h"
#include "strtab.h"
#include "ring.h"
#include "carve.h"
#include "permdata.h"
#include "objpass2.h"

lobject_data            CurrRec;

extern void ObjPass2( void )
/**************************/
/* Pass 2 of 8086 linker. */
{
    DEBUG(( DBG_BASE, "ObjPass2()" ));
    IncP2Start();
    DBIP2Start( Root );
    DBIOvlPass2();
    CurrSect = Root;/*  TAI */
    PModList( Root->mods );
    OvlPass2();
    if( FmtData.type & MK_OVERLAYS && FmtData.u.dos.distribute ) {
        ProcDistMods();
    } else {
        CurrSect = Root;
        PModList( LibModules );
    }
    WriteUndefined();
    if( FmtData.type & MK_OVERLAYS ) {
        SetOvlStartAddr();
    }
    DBIFini( Root );
    DBIOvlFini();
}

extern void PModList( mod_entry *head )
/*************************************/
{
    mod_entry           *obj;

    for( obj = head; obj != NULL; obj = obj->n.next_mod ) {
        PModule( obj );
    }
}

extern void PModule( mod_entry *obj )
/***********************************/
{
    if( !(obj->modinfo & MOD_NEED_PASS_2) ) return;
    DEBUG(( DBG_BASE, "2 : processing module %s", obj->name ));
    CurrMod = obj;
    IterateModRelocs( CurrMod->relocs, CurrMod->sizerelocs, IncExecRelocs );
    DBIGenModule();
    CheckStop();
}

extern bool LoadObj( segdata *seg )
/*********************************/
{
    seg_leader *leader;

    leader = seg->u.leader;
    if( leader == NULL || DBISkip( leader->dbgtype ) ) return FALSE;
    CurrRec.seg = seg;
    if( leader->group == NULL ) {
        CurrRec.addr = leader->seg_addr;
#if _DEVELOPMENT == _ON
        LnkMsg( WRN+MSG_INTERNAL, "s", "null leader group found" );
#endif
    } else {
        CurrRec.addr = leader->group->grp_addr;
        CurrRec.addr.off += SUB_ADDR(leader->seg_addr, leader->group->grp_addr);
    }
    CurrRec.addr.off += seg->a.delta;
    return TRUE;
}

