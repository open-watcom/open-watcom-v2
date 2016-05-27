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
* Description:  routines for distributing libraries over overlays
*
****************************************************************************/

#include <string.h>
#include "linkstd.h"
#include "alloc.h"
#include "msg.h"
#include "pcobj.h"
#include "wlnkmsg.h"
#include "objpass1.h"
#include "objpass2.h"
#include "objfree.h"
#include "wcomdef.h"
#include "overlays.h"
#include "ring.h"
#include "distrib.h"
#include "specials.h"
#include "load16m.h"

static unsigned_16  CurrModThere;
static arcdata *    ArcBuffer;
static unsigned_32  ArcBufLen;
static mod_entry ** ModTable;
static unsigned_16  CurrModHandle;

section **          SectOvlTab;

/* forward declarations */

static bool NewRefVector( symbol *, unsigned_16, unsigned_16 );
static void ScanArcs( mod_entry *mod );

#define MOD_DEREF( x )  (ModTable[(x)])
#define INITIAL_MOD_ALLOC 32
#define INITIAL_ARC_ALLOC 32
#define MAX_NUM_MODULES   (8 * 1024)

void ResetDistrib( void )
/******************************/
{
    ArcBuffer = NULL;
    ModTable = NULL;
    SectOvlTab = NULL;
}

void InitModTable( void )
/******************************/
{
    CurrModThere = INITIAL_MOD_ALLOC;
    _ChkAlloc( ModTable, INITIAL_MOD_ALLOC * sizeof( mod_entry * ) );
    CurrModHandle = 0;
    ArcBufLen = INITIAL_ARC_ALLOC;
    _ChkAlloc( ArcBuffer, sizeof(arcdata)
                          + (INITIAL_ARC_ALLOC - 1) * sizeof(dist_arc));
    MakePass1Blocks();
}

void AddModTable( mod_entry * lp, unsigned_16 libspot )
/************************************************************/
/* add this module to the table, and make the arclist field point to a
 * scratch buffer */
// NYI: segdata changes have completely broken distributing libraries.
// fix this!
{
    mod_entry **    new;

    CurrModHandle++;
    if( CurrModHandle == CurrModThere ) {
        if( CurrModThere > MAX_NUM_MODULES ) {
            LnkMsg( FTL+MSG_TOO_MANY_LIB_MODS, NULL );
        }
        _ChkAlloc( new, sizeof( mod_entry * ) * CurrModThere * 2 );
        memcpy( new, ModTable, sizeof( mod_entry * ) * CurrModThere );
        _LnkFree( ModTable );
        ModTable = new;
        CurrModThere *= 2;
    }
    ModTable[ CurrModHandle ] = lp;
    lp->x.arclist = ArcBuffer;
    ArcBuffer->numarcs = 0;
    if( lp->modinfo & MOD_FIXED ) {
        ArcBuffer->ovlref = libspot;
    } else {
        ArcBuffer->ovlref = NO_ARCS_YET;
    }
}

void InitArcBuffer( mod_entry * mod )
/******************************************/
/* set up the mod_entry arcdata field for dead code elimination */
{
    if( !( ( FmtData.type & MK_OVERLAYS ) && FmtData.u.dos.distribute
                && ( LinkState & SEARCHING_LIBRARIES ) ) ) {
        _PermAlloc( mod->x.arclist, sizeof(arcdata) - DIST_ONLY_SIZE );
    }
}

static void MarkDead( void *_seg )
/********************************/
{
    segdata *seg = _seg;
        
    if( seg->isrefd )
        return;
    if( seg->isdead )
        return;

    if( seg->iscode ) {
        seg->isdead = true;
    } else {
        if( FmtData.type & MK_PE ) {
            char *segname = seg->u.leader->segname;
            if( ( strcmp( segname, CoffPDataSegName ) == 0 )
                || ( strcmp(segname, CoffReldataSegName) == 0 ) ) {
                seg->isdead = true;
            }
        }
    }
}

static void KillUnrefedSyms( void *_sym )
/***************************************/
{
    symbol  *sym = _sym;
    segdata *seg;

    seg = sym->p.seg;
    if( ( seg != NULL ) && !IS_SYM_IMPORTED(sym) && !IS_SYM_ALIAS(sym)
        && seg->isdead ) {
        if( seg->u.leader->combine == COMBINE_COMMON ) {
            seg = RingFirst( seg->u.leader->pieces );
            if( !seg->isdead ) {
                return;
            }
        }
        if( sym->e.def != NULL ) {
            WeldSyms( sym, sym->e.def );
        } else {
            sym->info |= SYM_DEAD;
        }
        if( LinkFlags & SHOW_DEAD ) {
            LnkMsg( MAP+MSG_SYMBOL_DEAD, "S", sym );
        }
    }
}

static void DefineOvlSegments( mod_entry *mod )
/*********************************************/
/* figure out which of the segments are live */
{
    Ring2Walk( mod->segs, MarkDead );
    Ring2Walk( mod->publist, KillUnrefedSyms );
}

void SetSegments( void )
/*****************************/
// now that we know where everything is, do all the processing that has been
// postponed until now.
{
    if( FmtData.type & MK_DOS16M ) {
        MakeDos16PM();
    }
    if( !( LinkFlags & STRIP_CODE ) )
        return;
    LinkState &= ~CAN_REMOVE_SEGMENTS;
    ObjFormat |= FMT_DEBUG_COMENT;
    if( ( FmtData.type & MK_OVERLAYS ) && FmtData.u.dos.distribute ) {
        _LnkFree( ArcBuffer );
        ArcBuffer = NULL;
    }
    if( LinkFlags & STRIP_CODE ) {
        WalkMods( DefineOvlSegments );
    }
#if 0           // NYI: distributing libraries completely broken.
    unsigned        index;
    mod_entry *     mod;
    unsigned_16     ovlref;
    mod_entry **    currmod;
    unsigned        num_segdefs;

    if( ( FmtData.type & MK_OVERLAYS ) && FmtData.u.dos.distribute ) {
        for( index = 1; index <= CurrModHandle; index++ ) {
            mod = ModTable[ index ];
            CurrMod = mod;
            ovlref = mod->x.arclist->ovlref;
            if( ovlref == NO_ARCS_YET ) {       // only data referenced
                CurrSect = Root;
            } else {
                CurrSect = SectOvlTab[ ovlref ];
            }
            DefModSegments( mod );
            mod->x.next = NULL;
            for( currmod = &CurrSect->u.dist_mods; *currmod != NULL; ) {
                currmod = &((*currmod)->x.next);
            }
            *currmod = mod;
            mod->n.sect = CurrSect;
        }
    }
    FixGroupProblems();
    FindRedefs();
    if( ( FmtData.type & MK_OVERLAYS ) && FmtData.u.dos.distribute ) {
        _LnkFree( SectOvlTab );
        SectOvlTab = NULL;
    }
#endif
    ReleasePass1();
}

void FreeDistStuff( void )
/*******************************/
{
    unsigned    index;

    for( index = 1; index <= CurrModHandle; index++ ) {
        FreeAMod( ModTable[ index ] );
    }
    _LnkFree( ModTable );
    _LnkFree( ArcBuffer );
    _LnkFree( SectOvlTab );
    ReleasePass1();
}

void ProcDistMods( void )
/******************************/
{
    unsigned_16 index;
    mod_entry * mod;

    for( index = 1; index <= CurrModHandle; index++ ) {
        mod = ModTable[ index ];
        CurrSect = mod->n.sect;
        PModule( mod );
    }
}

#define SECT_VISITED 0x8000

unsigned_16 LowestAncestor( unsigned_16 ovl1, section * sect )
/*******************************************************************/
/* find the lowest common ancestor of the two overlay values by marking all of
 * the ancestors of the first overlay, and then looking for marked ancestors
 * of the other overlay */
{
    section *   list;

    for( list = sect; list != NULL; list = list->parent ) {
        list->ovl_num |= SECT_VISITED;
    }
    for( list = SectOvlTab[ ovl1 ]; !(list->ovl_num & SECT_VISITED); ) {
        list = list->parent;
    }
    for( ; sect != NULL; sect = sect->parent ) {
        sect->ovl_num &= ~SECT_VISITED;
    }
    return( list->ovl_num );
}

void DefDistribSym( symbol * sym )
/***************************************/
/* move current module based on where this symbol has been referenced from,
 * and make the symbol point to the current module. All symbols which get
 * passed to this routine are in an overlay class. */
{
    arcdata *   arcs;
    segdata *   seg;

    if( sym->info & SYM_REFERENCED ) {
        arcs = CurrMod->x.arclist;
        if( CurrMod->modinfo & MOD_FIXED ) {
            seg = sym->p.seg;
            if( seg->iscode ) {      // if code..
                NewRefVector( sym, sym->u.d.ovlref, arcs->ovlref );
            } else if( !( sym->u.d.ovlstate & OVL_FORCE ) ) {
                // don't generate a vector.
                sym->u.d.ovlstate |= OVL_FORCE | OVL_NO_VECTOR;
            }
        } else {
            if( arcs->ovlref == NO_ARCS_YET ) {
                arcs->ovlref = sym->u.d.ovlref;
            } else {
                arcs->ovlref = LowestAncestor( arcs->ovlref,
                                               SectOvlTab[sym->u.d.ovlref] );
            }
        }
    }
    sym->u.d.modnum = CurrModHandle;
}

static void AddArc( dist_arc arc )
/********************************/
/* add an arc to the arclist for the current module */
{
    arcdata *   arclist;

    arclist = CurrMod->x.arclist;
    if( arclist->numarcs >= ArcBufLen ) {
        _ChkAlloc( arclist, sizeof(arcdata)
                             + (ArcBufLen * 2 - 1) * sizeof(dist_arc) );
        memcpy( arclist, ArcBuffer, sizeof(arcdata)
                                       + (ArcBufLen-1) * sizeof(dist_arc));
        _LnkFree( ArcBuffer );
        CurrMod->x.arclist = arclist;
        ArcBuffer = arclist;
        ArcBufLen *= 2;
    }
    arclist->arcs[arclist->numarcs] = arc;
    arclist->numarcs++;
}

static bool NotAnArc( dist_arc arc )
/**********************************/
/* return true if this is not an arc in the current module */
{
    unsigned    index;
    arcdata *   arclist;

    arclist = CurrMod->x.arclist;
    for( index = arclist->numarcs; index-- > 0; ) {
        if( arclist->arcs[index].test == arc.test ) {
            return( false );
        }
    }
    return( true );
}

void RefDistribSym( symbol * sym )
/***************************************/
/* add an arc to the reference graph if it is not already in the graph */
{
    mod_entry * mod;
    segdata *   seg;
    dist_arc    arc;

    arc.sym = sym;
    if( sym->info & SYM_DEFINED ) {
        if( sym->info & SYM_DISTRIB ) {
            mod = ModTable[ sym->u.d.modnum ];
            if( mod->modinfo & MOD_FIXED ) {        // add reference, as long
                seg = sym->p.seg;                   // as it is a code ref.
                if( seg->iscode ) {
                    AddArc( arc );
                }
            } else {
                arc.test = sym->u.d.modnum;
                if( NotAnArc( arc ) && ( sym->u.d.modnum != CurrModHandle ) ) {
                    AddArc( arc );
                }
            }
        } else if( ( sym->u.d.ovlstate & OVL_VEC_MASK ) == OVL_UNDECIDED ) {
            if( NotAnArc( arc ) ) {
                AddArc( arc );
            }
        }
    } else {   // just a reference, so it has to be added to the call graph.
        AddArc( arc );
    }
}

static bool NewRefVector( symbol *sym, unsigned_16 ovlnum,
                                                   unsigned_16 sym_ovlnum )
/*************************************************************************/
/* sometimes there can be an overlay vector generated to a routine specified
 * in an .OBJ file caused by a call from a library routine. this checks for
 * this case.*/
{
    if( ( sym->p.seg == NULL )
        || ( ( sym->u.d.ovlstate & OVL_VEC_MASK ) != OVL_UNDECIDED ) ) {
        return( true );
    }
/*
 * at this point, we know it has already been defined, but does not have an
 * overlay vector, and is not data
*/
    if( LowestAncestor( sym_ovlnum, SectOvlTab[ ovlnum ] ) != sym_ovlnum ) {
        Vectorize( sym );
        return( true );
    }
    return( false );
}

static void DoRefGraph( unsigned_16 ovlnum, mod_entry * mod )
/***********************************************************/
/* checks to see if the mod has changed position, and if it has, check all
 * of the routines that mod references */
{
    arcdata *   arcs;
    unsigned_16 ancestor;

    arcs = mod->x.arclist;
/*
 * this next line is necessary to break cycles in the graph.
*/
    if( ( mod->modinfo & MOD_VISITED ) && ( ovlnum == arcs->ovlref )
        || ( mod->modinfo & MOD_FIXED ) )
        return;
    if( arcs->ovlref == NO_ARCS_YET ) {
        arcs->ovlref = 0;
        ancestor = 0;
        ScanArcs( mod );
    } else {
        ancestor = LowestAncestor( ovlnum, SectOvlTab[arcs->ovlref] );
        if( ancestor != arcs->ovlref ) {
            arcs->ovlref = ancestor;
            ScanArcs( mod );
        }
    }
    if( ancestor == 0 ) {   // it's at the root, so pull it out of the graph
        arcs->numarcs = 0;
    }
}

static void DeleteArc( arcdata *arc, unsigned_16 index )
/******************************************************/
/* delete an arc from the specified arc list */
{
    arc->numarcs--;
    if( arc->numarcs > 0 ) {
        arc->arcs[index] = arc->arcs[arc->numarcs];
    }
}

static void ScanArcs( mod_entry *mod )
/************************************/
/* go through all modules referenced by mod, and see if they need to change
 * position because of the position of mod */
{
    arcdata *   arcs;
    symbol *    sym;
    mod_entry * refmod;
    unsigned_16 index;
    unsigned_16 ovlnum;
    dist_arc    currarc;

    mod->modinfo |= MOD_VISITED;
    arcs = mod->x.arclist;
    ovlnum = arcs->ovlref;
    if( ovlnum != NO_ARCS_YET ) {
        for( index = arcs->numarcs; index-- > 0; ) {
            currarc = arcs->arcs[index];
            if( currarc.test <= MAX_NUM_MODULES ) {     // GIANT KLUDGE!
                DoRefGraph( ovlnum, MOD_DEREF( currarc.mod ) );
            } else {
                sym = currarc.sym;
                if( sym->info & SYM_DEFINED ) {
                    if( sym->info & SYM_DISTRIB ) {
                        currarc.test = sym->u.d.modnum;
                        refmod = MOD_DEREF( currarc.mod );
                        if( refmod->modinfo & MOD_FIXED ) {
                            if( NewRefVector( sym, ovlnum,
                                              refmod->x.arclist->ovlref ) ) {
                                DeleteArc( arcs, index );
                            }
                        } else {
                            DoRefGraph( ovlnum, refmod );
                            if( !NotAnArc( currarc ) ) {
                                DeleteArc( arcs, index );
                            } else {
                                arcs->arcs[index] = currarc;
                            }
                        }
                    } else {
                        if( ( sym->p.seg == NULL )
                            || NewRefVector( sym, ovlnum,
                               sym->p.seg->u.leader->class->section->ovl_num ) ) {
                            DeleteArc( arcs, index );
                        }
                    }
                } else {
                    if( !( sym->u.d.ovlstate & OVL_REF ) ) {
                        sym->u.d.ovlref = ovlnum;
                        sym->u.d.ovlstate |= OVL_REF;
                    } else {
                        sym->u.d.ovlref = LowestAncestor( ovlnum,
                                                 SectOvlTab[sym->u.d.ovlref] );
                    }
                }
            } /* if (a module) */
        } /* for (arcs left) */
    } /* if (an ovlnum defined) */
    mod->modinfo &= ~MOD_VISITED;
}

void FinishArcs( mod_entry *mod )
/**************************************/
/* check the position of the modules referenced by mod, and then make a
 * more permanent copy of the arclist for this module. */
{
    arcdata *   newarcs;
    unsigned    allocsize;

    ScanArcs( mod );
    if( mod->modinfo & MOD_FIXED ) {    // no need to scan a fixed module
        mod->x.arclist->numarcs = 0;        // more than once
    }
    allocsize = mod->x.arclist->numarcs * sizeof(dist_arc)
                                 + sizeof(arcdata) - sizeof(dist_arc);
    _Pass1Alloc( newarcs, allocsize );
    memcpy( newarcs, mod->x.arclist, allocsize );
    mod->x.arclist = newarcs;
}

void DistIndCall( symbol *sym )
/************************************/
// handle indirect calls and their effect on distributed libs.
{
    arcdata *   arcs;
    unsigned_16 ovlsave;

    arcs = CurrMod->x.arclist;
    ovlsave = arcs->ovlref;
    arcs->ovlref = 0;           // make sure current module isn't
    CurrMod->modinfo |= MOD_VISITED;        // visited
    DoRefGraph( 0, MOD_DEREF( sym->u.d.modnum ) );
    CurrMod->modinfo &= ~MOD_VISITED;
    arcs->ovlref = ovlsave;
    sym->u.d.ovlstate |= OVL_REF;
    sym->u.d.ovlref = 0;        // make sure current symbol put in root.
}
