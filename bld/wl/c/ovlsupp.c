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
   OVLSUPP -- overlay support routines
*/

#include <string.h>
#include "linkstd.h"
#include "pcobj.h"
#include "newmem.h"
#include "msg.h"
#include "alloc.h"
#include "reloc.h"
#include "wlnkmsg.h"
#include "virtmem.h"
#include "fileio.h"
#include "mapio.h"
#include "procfile.h"
#include "objcalc.h"
#include "objfree.h"
#include "dbgall.h"
#include "objpass1.h"
#include "loadfile.h"
#include "loaddos.h"
#include "objnode.h"
#include "distrib.h"
#include "overlays.h"

static segdata *    OvlSegData;
static symbol *     OverlayTable;/* symbol entry for overlay table */
static symbol *     OverlayTableEnd;/* symbol entry for overlay table */
static symbol *     OvlVecStart; /* symbol entry for overlay vector start */
static symbol *     OvlVecEnd;   /* symbol entry for overlay vector end */
static targ_addr    OvlvecAddr;  /* address of overlay vectors */
static targ_addr    Stash;

unsigned_16     AreaSize;

extern void ResetOvlSupp( void )
/******************************/
{
    AreaSize = 0xFFFF;
}

static void ParmWalkSections( section *sect, void (*rtn)(section *,void *),
                              void *parm )
/*************************************************************************/
{
    for( ; sect != NULL; sect = sect->next_sect ) {
        rtn( sect, parm );
        ParmWalkAreas( sect->areas, rtn, parm );
    }
}

extern void ParmWalkAreas( OVL_AREA *ovl, void (*rtn)(section *,void *),
                           void *parm )
/**********************************************************************/
{
    for( ; ovl != NULL; ovl = ovl->next_area ) {
        ParmWalkSections( ovl->sections, rtn, parm );
    }
}

static void WalkSections( section *sect, void (*rtn)(section *) )
/***************************************************************/
{
    for( ; sect != NULL; sect = sect->next_sect ) {
        rtn( sect );
        WalkAreas( sect->areas, rtn );
    }
}

extern void WalkAreas( OVL_AREA *ovl, void (*rtn)(section *) )
/************************************************************/
{
    for( ; ovl != NULL; ovl = ovl->next_area ) {
        WalkSections( ovl->sections, rtn );
    }
}

static void WriteVectors( void )
/******************************/
{
    vecnode             *vec;
    int                 n;
    targ_addr           addr;
    symbol *            sym;

    WriteMapNL( 2 );
    XReportSymAddr( OverlayTable );
    XReportSymAddr( OverlayTableEnd );
    XReportSymAddr( OvlVecStart );
    XReportSymAddr( OvlVecEnd );
    WriteOvlHead();
    n = 0;
    vec = OvlVectors;
    while( vec != NULL ) {
        GetVecAddr( ++n, &addr );
        sym = vec->entry;
        WriteMap( "%a section %d : %S", &addr,
                   sym->p.seg->u.leader->class->section->ovl_num, sym );
        vec = vec->next;
    }
}

static void DoSecPubs( section *sec )
/***********************************/
{
    WriteMapNL( 2 );
    WriteMap( "Overlay section %d address %a", sec->ovl_num,
      &sec->sect_addr );
    WriteMap( "====================================" );
    WriteSegs( sec->classlist );
    StartMapSort();
    if( MapFlags & MAP_FLAG ) {
        WritePubHead();
    }
    ProcPubs( sec->mods, sec );
    ProcOvlSectPubs( sec );
    FinishMapSort();
}

extern void ProcOvlSectPubs( section *sec )
/*****************************************/
{
    mod_entry * next;

    if( sec->u.dist_mods != NULL ) {
        for( CurrMod = sec->u.dist_mods; CurrMod != NULL; CurrMod = next ) {
            next = CurrMod->x.next;
            DoPubs( sec );
        }
    }
}

extern void ProcOvlPubs( void )
/*****************************/
{
    WriteVectors();
    ProcAllOvl( DoSecPubs );
}

extern void FillOutPtr( section *sec )
/************************************/
{
    if( sec->outfile == NULL ) {
        if( sec->parent != NULL ) {
            sec->outfile = sec->parent->outfile;  //same file as ancestor.
        } else {
            sec->outfile = Root->outfile;
        }
    }
}

static void AllocSections( section *first_sect )
/**********************************************/
/* Allocate parallel overlay sections */
{
    targ_addr           save;
    targ_addr           max;
    section *           sect;
    unsigned long       result;
    unsigned            ovl_size;
    unsigned            min_size;
    bool                area_specified;

    CurrSect = first_sect;    /* NormalizeAddr() needs to know the section */
    MAlign( 4 );     /* as do some DBI routines. */
    NormalizeAddr();
    if( first_sect == NonSect ) {
        if( FmtData.u.dos.dynamic ) {
            ovl_size = CurrLoc.seg - Stash.seg;
            min_size = ovl_size + OvlNum + 1;  // need at least 1 para per sect. + 1
            if( min_size < 64 ) {
                min_size = 64;      // reserve 1 K for the stack
            }
            area_specified = TRUE;
            if( AreaSize == 0xFFFF ) {   // default is twice the memory size
                AreaSize = ovl_size * 2;
                area_specified = FALSE;
            } else if( AreaSize == 0 ) {
                area_specified = FALSE;
            }
            if( AreaSize < min_size ) {
                AreaSize = min_size;
                if( area_specified ) {
                    LnkMsg( WRN+MSG_AREA_TOO_SMALL, "l",
                                                 (unsigned long)min_size *16);
                }
            }
            result = CurrLoc.seg + (unsigned long) (AreaSize - ovl_size);
            if( result > 0xFFFF ) {
                LnkMsg( WRN+MSG_CANT_RESERVE_SPACE, "l",
                                                 (unsigned long)AreaSize << 4 );
            } else {
                CurrLoc.seg = result;
            }
        }
        AreaSize = CurrLoc.seg - Stash.seg;
    }
    save = CurrLoc;
    max.off = 0;
    max.seg = 0;
    for( sect = first_sect; sect != NULL; sect = sect->next_sect ) {
        CurrSect = sect;
        sect->sect_addr = save;
        AllocClasses( sect->classlist );
        if( sect->areas != NULL ) {
            AllocAreas( sect->areas );
        }
        NormalizeAddr();        /* avoid any overflow messages */
        AddSize( 2 );   /* reserve some space for the overlay manager */
        NormalizeAddr();        /*  get canonical form */
        if( CurrLoc.seg > max.seg
        || (CurrLoc.seg == max.seg && CurrLoc.off > max.off) ) {
            max = CurrLoc;
        }
        CurrLoc = save;
    }
    CurrLoc = max;
}

static void AllocAreas( OVL_AREA *area )
/**************************************/
{
    /* now CurrLoc is beginning of overlay area */
    DEBUG(( DBG_OLD, "Overlay area start %a", &CurrLoc ));
    for( ; area != NULL; area = area->next_area ) {
        AllocSections( area->sections );
    }
}

extern void CalcOvl( void )
/*************************/
{
    unsigned        temp;
    outfilelist *   fnode;

    CurrSect = Root;
    /* record starting address of overlay table */
    Align( 2 ); // for overlay table speed.
    XDefSymAddr( OverlayTable, CurrLoc.off, CurrLoc.seg );
    DEBUG(( DBG_OLD, "Overlay table address %a", &CurrLoc ));
    OvltabAddr = CurrLoc;
    /* calculate size of overlay table proper */
    temp = sizeof(ovl_null_table) + ( OvlNum - 1 ) * sizeof(ovltab_entry);
    XDefSymAddr( OverlayTableEnd, CurrLoc.off + temp - sizeof(unsigned_16),
                                                                 CurrLoc.seg);
    fnode = OutFiles;
    while( fnode != NULL ) {
        fnode->ovlfnoff = temp;
        temp += strlen( fnode->fname ) + 1;
        fnode = fnode->next;
    }
    Root->outfile->ovlfnoff |= EXE_FILENAME;    // indicate the .exe file.
    /* calculate starting address of overlay vectors, record */
    OvltabSize = temp;   /*  incl. NULLCHAR */
    CurrentSeg = NULL;
    AddSize( OvltabSize );
    XDefSymAddr( OvlVecStart, CurrLoc.off, CurrLoc.seg );
    DEBUG(( DBG_OLD, "Overlay vector start %a", &CurrLoc ));
    OvlvecAddr = CurrLoc;
    /* calculate start of overlay area */
    if( FmtData.u.dos.ovl_short ) {
        temp = VecNum * sizeof( svec );
    } else {
        temp = VecNum * sizeof( lvec );
    }
    AddSize( temp );
    OvltabSize += temp;
    XDefSymAddr( OvlVecEnd, CurrLoc.off, CurrLoc.seg );
    AddSize( 2 );       // reserve some space for the ovl. manager.
    Align( 4 );
    NormalizeAddr();
    Stash = CurrLoc;
    AllocAreas( Root->areas );
    MAlign( 4 );          /*    don't add to section size */
    NormalizeAddr();        /*  now CurrLoc.seg points to next free para */
}

extern void FreeOvlStruct( void )
/*******************************/
{
    OvlClasses = NULL;
/*    FreeList( OvlVectors );  vectors permalloc'd now. */
    OvlVectors = NULL;
    if( OvlSeg != NULL ) {
        FreeLeader( OvlSeg );
    }
    if( OvlSegData != NULL ) {
        FreeSegData( OvlSegData );
    }
    FreeDistStuff();
}

static bool IsAncestor( int elder, section *ceorl )
/*************************************************/
/* Is overlay section # "elder" an ancestor of section "ceorl" */
{
    for(;;) {
        if( ceorl->ovl_num == elder ) return( TRUE );
        if( ceorl->parent == NULL ) return( FALSE );
        ceorl = ceorl->parent;
    }
}

#define NO_VECTOR( sym ) ((IS_SYM_COMMUNAL(sym)) \
                            || ((sym)->u.d.ovlstate & OVL_FORCE))

extern void OvlDefVector( symbol * sym )
/**************************************/
{
    segdata *   sdata;
    unsigned_16 ovl_num;

    if( NO_VECTOR( sym ) ) return;
    sdata = sym->p.seg;
    if( sdata == NULL ) {
        sym->u.d.ovlstate |= (OVL_FORCE | OVL_NO_VECTOR);
        return;         /* NOTE: <--- premature return <----------- */
    } else {
        ovl_num = sdata->u.leader->class->section->ovl_num;
    }
    if( !sdata->iscode || ovl_num == 0 ) {      // not code or in root
        sym->u.d.ovlstate |= (OVL_FORCE | OVL_NO_VECTOR);
    } else {
        if( sym->info & SYM_REFERENCED ) {
            if( sym->u.d.ovlref != ovl_num ) {
                /* first reference must have been a proper ancestor or cousin */
                Vectorize( sym );
            }
        } else {
            sym->u.d.ovlstate |= OVL_REF;
            sym->u.d.ovlref = ovl_num;
        }
    }
}

extern void Vectorize( symbol * sym )
/***********************************/
/* allocate an overlay vector for a symbol */
{
    vecnode             *vec;

    sym->u.d.ovlref = ++VecNum;
    sym->u.d.ovlstate &= ~OVL_NO_VECTOR;
    sym->u.d.ovlstate |= OVL_FORCE;
    _PermAlloc( vec, sizeof( vecnode ) );
    vec->entry = sym;
    LinkList( &OvlVectors, vec );
    DEBUG(( DBG_OLD, "Vectorize %d %S", VecNum, sym ));
}

static void OvlRefVector( symbol * sym )
/**************************************/
{
    unsigned_16 ovl_num;

    if( IS_SYM_COMMUNAL(sym) ) return;
    if( (sym->u.d.ovlstate & OVL_VEC_MASK) != OVL_UNDECIDED ) return;
    if( !(sym->info & SYM_DEFINED) ) {
        if( !(sym->u.d.ovlstate & OVL_REF) ) {
            sym->u.d.ovlref = CurrSect->ovl_num;
            sym->u.d.ovlstate |= OVL_REF;
        } else if( FmtData.u.dos.distribute ) {
            sym->u.d.ovlref = LowestAncestor( sym->u.d.ovlref, CurrSect );
        }
    } else {
        if( sym->p.seg == NULL ) return;
        /* at this point, we know it has already been defined, but does */
        /* not have an overlay vector, and is not data */
        ovl_num = sym->p.seg->u.leader->class->section->ovl_num;
        if( IsAncestor( ovl_num, CurrSect ) ) return;
        /* overlay vector necessary */
        Vectorize( sym );
    }
}

extern void TryRefVector( symbol * sym )
/**************************************/
{
    if( !(FmtData.type & MK_OVERLAYS) ) return;
    if( (LinkState & SEARCHING_LIBRARIES) && FmtData.u.dos.distribute ) {
        RefDistribSym( sym );
    } else {
        OvlRefVector( sym );
    }
}

extern void OvlUseVector( symbol * sym, extnode *newnode )
/********************************************************/
{
    if( !(FmtData.type & MK_OVERLAYS) ) return;
    if( IS_SYM_COMMUNAL(sym) ) return;
    if( (sym->u.d.ovlstate & OVL_VEC_MASK) != OVL_MAKE_VECTOR ) return;
    if( IsAncestor( sym->p.seg->u.leader->class->section->ovl_num, CurrSect )) {
         return;
    }
    /* use overlay vector for all calls */
    newnode->ovlref = sym->u.d.ovlref;
}

extern void IndirectCall( symbol *sym )
/*************************************/
{
    unsigned_16 ovl_num;

    if( !(FmtData.type & MK_OVERLAYS) ) return;
    if( NO_VECTOR( sym ) ) return;
    if( sym->info & SYM_DEFINED ) {
        if( sym->info & SYM_DISTRIB ) {
            DistIndCall( sym );
        } else if( sym->p.seg != NULL ) {
            ovl_num = sym->p.seg->u.leader->class->section->ovl_num;
            if( ovl_num != 0 && sym->p.seg->iscode ){
                Vectorize( sym );
            }
        }
    } else {
        // indicate a reference from the root for purposes of vector generation
        sym->u.d.ovlstate |= OVL_REF;
        sym->u.d.ovlref = 0;
    }
}

static void GetVecAddr( int vecnum, targ_addr *addr )
/***************************************************/
/* return address of overlay vector in canonical form */
{
    *addr = OvlvecAddr;
    if( FmtData.u.dos.ovl_short ) {
        /* short address -- fits into 16 bits */
        addr->off += (vecnum - 1) * sizeof( svec ) + (addr->seg << 4);
        addr->seg = 0;
    } else {
        addr->off += (vecnum - 1) * sizeof( lvec );
    }
}

#if 0
// NYI  broken from removal of thread
extern void OvlForceVect( thread *thd, bool indirect )
/****************************************************/
{
    symbol  *sym;

    if( !(FmtData.type & MK_OVERLAYS) ) return;
    if( !(thd->flags & TRD_SYMBOL) ) return;
    sym = thd->s.sym;
    if( IS_SYM_COMMUNAL(sym) ) return;
    if( (sym->u.d.ovlstate & OVL_VEC_MASK) != OVL_MAKE_VECTOR ) return;
    if( !(indirect || (sym->u.d.ovlstate & OVL_ALWAYS)) ) return;
    /* taking the address of a function -- must use vector */
    GetVecAddr( sym->u.d.ovlref, &thd->addr );
}

extern void GetVecAddr2( int vecnum, thread *targ )
/*************************************************/
/* return address of overlay vector in canonical form */
{
    unsigned int        temp;

    targ->addr = OvlvecAddr;
    if( FmtData.u.dos.ovl_short ) {
        /* short address -- fits into 16 bits */
        temp = (vecnum - 1) * sizeof( svec ) + offsetof( svec, call );
    } else {
        /* long address -- put in canonical form */
        temp = (vecnum - 1) * sizeof( lvec ) + offsetof( lvec, call );
    }
    targ->addr.off += temp;
}
#endif

extern bool CheckOvlClass( char *clname, bool *isovlclass )
/*********************************************************/
/* check if among overlay classes, and return TRUE if it is code. */
{
    list_of_names *     cnamelist;
    bool                retval;

    retval = IsCodeClass( clname, strlen(clname) );
    cnamelist = OvlClasses;
    if( cnamelist == NULL ) {   /* check for anything ending in code */
        *isovlclass = retval;
        return retval;
    } else {
        do {
            if( stricmp( clname, cnamelist->name ) == 0 ) {
                *isovlclass = TRUE;
                return retval;
            }
            cnamelist = cnamelist->next_name;
        } while( cnamelist != NULL );
    }
    *isovlclass = FALSE;
    return( FALSE );
}

extern section * CheckOvlSect( char *clname )
/*******************************************/
{
    section *   sect;
    bool        dummy;

    sect = DBIGetSect( clname );
    if( sect == NULL ) {
        if( CheckOvlClass( clname, &dummy ) ) {
            sect = CurrSect;
        } else {
            sect = NonSect;
        }
    }
    return sect;
}

extern void EmitOvlVectors( void )
/********************************/
{
    symbol *    symptr;
    dos_addr    addr;
    char *      loader_name;
    bool        isshort;

    /* output relocation items for overlay table */
    addr.seg = OvlGroup->grp_addr.seg;
    addr.off = OvltabAddr.off + offsetof( ovltab_prolog, start.seg );
    WriteReloc( OvlGroup, addr.off, &addr, sizeof( dos_addr ) );
    addr.off = OvltabAddr.off + offsetof( ovltab_prolog, delta );
    WriteReloc( OvlGroup, addr.off, &addr, sizeof( dos_addr ) );

    isshort = FALSE;
    if( FmtData.u.dos.dynamic ) {
        loader_name = _DynamicOvlldr;
    } else if( FmtData.u.dos.ovl_short ) {
        loader_name = _ShortOvlldr;
        isshort = TRUE;
    } else {
        loader_name = _LongOvlldr;
    }
    symptr = RefISymbol( loader_name );
    if( !(symptr->info & SYM_DEFINED) ) {
        LnkMsg( ERR+MSG_NO_OVERLAY_LOADER, NULL );
    } else if( isshort ) {
        ShortVectors( symptr );
    } else {
        LongVectors( symptr );
    }
}

static void ShortVectors( symbol *loadsym )
/*****************************************/
{
    vecnode             *vec;
    unsigned            vect_off;
    unsigned_32         loadval;
    unsigned_32         temp;
    signed_32           diff;
    int                 vecnum;
    svec                template;

    vect_off = OvlvecAddr.off;
    temp = MK_REAL_ADDR( OvlvecAddr.seg, OvlvecAddr.off );
    /* get loader address */
    loadval = MK_REAL_ADDR( loadsym->addr.seg, loadsym->addr.off );
    /* fill in overlay vector template */
    template.call = 0xe8;
    template.jmpto = 0xe9;
    vecnum = 1;
    for( vec = OvlVectors; vec != NULL; vec = vec->next ) {
        temp = vect_off + offsetof( svec, loader ) + sizeof( unsigned_16 );
        diff = loadval - temp;
        if( diff < -32768 || diff > 32767 ) {
            LnkMsg( ERR+MSG_SHORT_VECT_RANGE, "d", vecnum );
        }
        _HostU16toTarg( diff, template.loader );
        loadsym = vec->entry;
        _HostU16toTarg( loadsym->p.seg->u.leader->class->section->ovl_num,
                        template.sec_num );
        temp = vect_off + offsetof( svec, target ) + sizeof( unsigned_16 );
        diff = MK_REAL_ADDR( loadsym->addr.seg, loadsym->addr.off ) - temp;
        if( diff < -32768 || diff > 32767 ) {
            LnkMsg( ERR+MSG_SHORT_VECT_RANGE, "d", vecnum );
        }
        _HostU16toTarg( diff, template.target );
        PutOvlInfo( vect_off, &template, sizeof( svec ) );
        vect_off += sizeof( svec );
        ++vecnum;
    }
}

static void LongVectors( symbol *loadsym )
/****************************************/
{
    vecnode             *vec;
    unsigned            vect_off;
    lvec                template;
    dos_addr            addr;
    signed_32           diff;
    unsigned_32         loadval;
    unsigned_32         temp;
    int                 vecnum;

    vect_off = OvlvecAddr.off;
    /* fill in overlay vector template */
    template.call = 0xe8;
    template.jmpto = 0xea;
    loadval = loadsym->addr.off;
    addr.seg = OvlGroup->grp_addr.seg;
    vecnum = 1;
    for( vec = OvlVectors; vec != NULL; vec = vec->next ) {
        temp = vect_off + offsetof( lvec, loader ) + sizeof( unsigned_16 );
        diff = loadval - temp;
        if( diff < -32768 || diff > 32767 ) {
            LnkMsg( ERR+MSG_SHORT_VECT_RANGE, "d", vecnum );
        }
        _HostU16toTarg( diff, template.loader );
        loadsym = vec->entry;
        _HostU16toTarg( loadsym->p.seg->u.leader->class->section->ovl_num,
                        template.sec_num );
        _HostU16toTarg( loadsym->addr.off, template.target.off );
        if( FmtData.u.dos.dynamic ) {
            _HostU16toTarg( loadsym->addr.seg
                            - loadsym->p.seg->u.leader->group->grp_addr.seg,
                                                   template.target.seg );
        } else {
            _HostU16toTarg( loadsym->addr.seg, template.target.seg );
            addr.off = vect_off + offsetof( lvec, target.seg );
            WriteReloc( OvlGroup, addr.off, &addr, sizeof( dos_addr ) );
        }
        PutOvlInfo( vect_off, &template, sizeof( lvec ) );
        vect_off += sizeof( lvec );
        vecnum++;
    }
}


extern void SetOvlStartAddr( void )
/*********************************/
{
    symbol *    sym;

    /* stuff overlay init routine address in header */
    Stash = StartInfo.addr;

    if( FmtData.u.dos.dynamic ) {
        sym = FindISymbol( _DynamicInitRtn );
    } else if( FmtData.u.dos.ovl_short ) {
        sym = FindISymbol( _ShortOvlInitRtn );
    } else {
        sym = FindISymbol( _LongOvlInitRtn );
    }
    if( sym == NULL ) return;
    if( sym->info & SYM_DEFINED ) {
        StartInfo.addr = sym->addr;
    }
}

extern void OvlPass1( void )
/**************************/
{
    symbol *    sym;

    ProcAllOvl( LoadObjFiles );
    /* define symbols for overlay table */
    OverlayTable = DefISymbol( _OvltabName );
    OverlayTableEnd = DefISymbol( _OvltabEndName );
    OvlVecStart = DefISymbol( _OvlVecStartName );
    OvlVecEnd = DefISymbol( _OvlVecEndName );
    OvlSeg = InitLeader( "", 0 );
    OvlSeg->align = 0;
    OvlSeg->combine = COMBINE_INVALID;
    OvlSeg->dbgtype = NOT_DEBUGGING_INFO;
    OvlSegData = AllocSegData();
    OvlSegData->u.leader = OvlSeg;
    OverlayTable->p.seg = OvlSegData;
    OverlayTableEnd->p.seg = OvlSegData;
    OvlVecStart->p.seg = OvlSegData;
    OvlVecEnd->p.seg = OvlSegData;
    /* generate reference for overlay loader */
    if( FmtData.u.dos.dynamic ) {
        RefISymbol( _DynamicInitRtn );
        sym = RefISymbol( _DynamicOvlldr );
    } else if( FmtData.u.dos.ovl_short ) {
        RefISymbol( _ShortOvlInitRtn );
        sym = RefISymbol( _ShortOvlldr );
    } else {
        RefISymbol( _LongOvlInitRtn );
        sym = RefISymbol( _LongOvlldr );
    }
    sym->u.d.ovlref = 0;
    sym->u.d.ovlstate |= OVL_REF;
}

static unsigned EmitOvlEntry( unsigned off, section *sect )
/*********************************************************/
{
    static unsigned EmitOvlAreaEntry( unsigned off, OVL_AREA *area );
    ovltab_entry        entry;
    offset              len;
    unsigned_16         flags_anc;
    unsigned_16         start_para;

    _HostU16toTarg( 0, entry.code_handle );
    for( ; sect != NULL; sect = sect->next_sect ) {/* write out table entry */
        flags_anc = sect->parent->ovl_num;
        if( sect == NonSect ) {
            flags_anc |= FLAG_PRELOAD;/*  pre-load the data area */
        }
        _HostU16toTarg( sect->outfile->ovlfnoff, entry.fname );
        _HostU16toTarg( sect->relocs, entry.relocs );
        _HostU16toTarg( flags_anc, entry.flags_anc );
        start_para = sect->sect_addr.seg + ( sect->sect_addr.off >> 4 );
        _HostU16toTarg( start_para, entry.start_para );
        len = sect->size + 15 >> 4;
        _HostU16toTarg( len, entry.num_paras );
        _HostU32toTarg( sect->u.file_loc, entry.disk_addr );
        PutOvlInfo( off, &entry, sizeof( entry ) );
        off += sizeof( ovltab_entry );
        off = EmitOvlAreaEntry( off, sect->areas );
    }
    return( off );
}

static unsigned EmitOvlAreaEntry( unsigned off, OVL_AREA *area )
{
    for( ; area != NULL; area = area->next_area ) {
        off = EmitOvlEntry( off, area->sections );
    }
    return( off );
}

extern void EmitOvlTable( void )
/******************************/
/* generate overlay table */
{
    unsigned            off;
    ovltab_prolog       template;
    unsigned_16         u16;
    int                 len;
    outfilelist *       fnode;

    off = OvltabAddr.off;
/*
    Generate prolog :
*/
    template.major = OVL_MAJOR_VERSION;
    template.minor = OVL_MINOR_VERSION;
/*
    output start address for program
    reloc for this was emitted by EmitOvlVectors
*/
    _HostU16toTarg( Stash.off, template.start.off );
    _HostU16toTarg( Stash.seg, template.start.seg );
/*
    this should give us the paragraph of the load module start
    reloc for this was emitted by EmitOvlVectors
*/
    _HostU16toTarg( 0, template.delta );
    _HostU16toTarg( AreaSize, template.ovl_size );

    PutOvlInfo( off, &template, sizeof( template ) );
/*
    Generate entries :
*/
    off += sizeof( template );
    off = EmitOvlAreaEntry( off, Root->areas );
/*
    Generate epilog :
*/
    u16 = OVLTAB_TERMINATOR;
    PutOvlInfo( off, &u16, sizeof( unsigned_16 ) );
    off += sizeof( unsigned_16 );
    /* generate overlay filenames, including NULLCHARS*/
    fnode = OutFiles;
    while( fnode != NULL ) {
        len = strlen( fnode->fname ) + 1;
        PutOvlInfo( off, fnode->fname, len );
        off += len;
        fnode = fnode->next;
    }
}

extern void PadOvlFiles( void )
/*****************************/
// The overlay files must contain a complete paragraph at the end of the file
// for the overlay loader to be able to correctly read it.
{
    outfilelist *   fnode;
    unsigned        pad;

    fnode = OutFiles;
    for( fnode = OutFiles; fnode != NULL; fnode = fnode->next ) {
        pad = 16 - (fnode->file_loc & 0xF);
        if( pad != 16 ) {
            if( fnode->handle == NIL_HANDLE ) {
                OpenOvlFile( fnode );
            }
            WriteNulls( fnode->handle, pad, fnode->fname );
            fnode->file_loc += pad;
        }
    }
}

static void PutOvlInfo( unsigned off, void *src, unsigned len )
/*************************************************************/
{
#if 0   // NYI - group->memaddr has been removed
   PutInfo( OvlGroup->memaddr + off - OvlGroup->grp_addr.off, src, len );
#else
   off = off;
   src = src;
   len = len;
#endif
}
