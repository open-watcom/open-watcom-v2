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
   OVERLAYS -- overlay support routines which can't be put in an overlay.
*/

#include "linkstd.h"
#include "alloc.h"
#include "newmem.h"
#include "dbgall.h"
#include "dbgwat.h"
#include "objpass2.h"
#include "objcalc.h"
#include "distrib.h"
#include "overlays.h"

extern void ProcAllOvl( void (*rtn)( section * ) )
/************************************************/
{
    if( FmtData.type & MK_OVERLAYS ) {
        WalkAreas( Root->areas, rtn );
    }
}

extern void ParmWalkOvl( void (*rtn)( section *, void * ), void *parm )
/*********************************************************************/
{
    if( FmtData.type & MK_OVERLAYS ) {
        ParmWalkAreas( Root->areas, rtn, parm );
    }
}

static void NumASect( section *sect )
/***********************************/
{
    if( FmtData.u.dos.distribute ) {
        SectOvlTab[OvlNum] = sect;
    }
    sect->ovl_num = OvlNum++;
}

extern void NumberSections( void )
/********************************/
{
    if( FmtData.type & MK_OVERLAYS && FmtData.u.dos.distribute ) {
        _ChkAlloc( SectOvlTab, sizeof(section *) * (OvlNum + 1) );
        SectOvlTab[0] = Root;
    }
    OvlNum = 1;
    ProcAllOvl( &NumASect );
}

extern void FillOutFilePtrs( void )
/*********************************/
{
    ProcAllOvl( FillOutPtr );
}

extern void DBIOvlPass2( void )
/*****************************/
{
    ProcAllOvl( DBIP2Start );
}

extern void DBIOvlFini( void )
/****************************/
{
    ProcAllOvl( DBIFini );
    if( NonSect != NULL ) {
        DBIFini( NonSect );
    }
}

#if _LINKER != _WATFOR77
extern void WriteOvlSecs( void )
/******************************/
{
    ProcAllOvl( WriteDBISecs );
}
#endif

extern void DBIAddrOvlStart( void )
/*********************************/
{
    ProcAllOvl( DBIAddrSectStart );
}

extern void TryDefVector( symbol * sym )
/**************************************/
{
    if( FmtData.type & MK_OVERLAYS ) {
        if( sym->info & SYM_DISTRIB ) {
            DefDistribSym( sym );
        } else {
            OvlDefVector( sym );
        }
    }
}

extern void TryUseVector( symbol * sym, extnode *newnode )
/********************************************************/
{
    if( newnode != NULL ) {
        newnode->ovlref = 0;
        if( FmtData.type & MK_OVERLAYS ) {
            OvlUseVector( sym, newnode );
        }
    }
}

extern section * GetOvlSect( char *clname )
/*****************************************/
/* Pick the right section for this segment, based on its class */
{
    section             *sect;

    if( !(FmtData.type & MK_OVERLAYS) ) {
        sect = Root;
    } else {
        sect = CheckOvlSect( clname );
    }
    return( sect );
}

static void PSection( section *sec )
/***********************************/
{
    CurrSect = sec;
    PModList( sec->mods );
}

extern void OvlPass2( void )
/**************************/
{
    if( FmtData.type & MK_OVERLAYS ) {
        EmitOvlVectors();
        ProcAllOvl( PSection );
    }
}

#if 0           // NYI
extern void TryGetVector( extnode *ext, thread *targ )
/****************************************************/
{
    /* check if reference needs overlay vector */
    if( ext->ovlref != 0 ) {
        /* get address for overlay vector */
        GetVecAddr2( ext->entry->u.d.ovlref, targ );
        targ->flags |= TRD_DEFINED;
    } else {
        XSymAddr( ext->entry, targ );
    }
}
#endif
