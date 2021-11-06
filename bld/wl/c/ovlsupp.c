/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  OVLSUPP -- overlay support routines
*
****************************************************************************/

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
#include "ring.h"
#include "specials.h"

#include "clibext.h"


#ifdef _EXE

overlay_ref         OvlSectNum;         /* OvlSectNum value 0 is reserved for Root */
seg_leader          *OvlSeg;            /* pointer to seg_leader for overlaytab */
unsigned_16         OvlAreaSize;
list_of_names       *OvlClasses;        /* list of classes to be overlayed      */

static segdata      *OvlSegData;
static symbol       *OverlayTable;      /* symbol entry for overlay table */
static symbol       *OverlayTableEnd;   /* symbol entry for overlay table */
static symbol       *OvlVecStart;       /* symbol entry for overlay vector start */
static symbol       *OvlVecEnd;         /* symbol entry for overlay vector end */
static targ_addr    OvlvecAddr;         /* address of overlay vectors */
static targ_addr    Stash;
static group_entry  *OvlGroup;          /* pointer to group for overlay table   */
static targ_addr    OvltabAddr;         /* address of overlay tables            */
static unsigned     OvltabSize;         /* size of overlay tables               */
static vecnode      *OvlVectors;        /* point to overlay vector notes        */
static int          VecNum;             /* number of vectors so far             */

void ResetOverlaySupp( void )
/***************************/
{
    OvlAreaSize = 0xFFFF;
    OvlVectors = NULL;
    /* OvlSectNum value 0 is reserved for Root */
    /* Overlayed sections start at 1 */
    OvlSectNum = 1;
    OvlClasses = NULL;
    VecNum = 0;
    ResetDistribSupp();
}

static void ParmWalkSections( section *sect, void (*rtn)( section *, void * ), void *parm )
/*****************************************************************************************/
{
    for( ; sect != NULL; sect = sect->next_sect ) {
        rtn( sect, parm );
        ParmWalkAreas( sect->areas, rtn, parm );
    }
}

void ParmWalkAreas( OVL_AREA *ovl, void (*rtn)( section *, void * ), void *parm )
/*******************************************************************************/
{
    for( ; ovl != NULL; ovl = ovl->next_area ) {
        ParmWalkSections( ovl->sections, rtn, parm );
    }
}

static void WalkSections( section *sect, void (*rtn)( section * ) )
/*****************************************************************/
{
    for( ; sect != NULL; sect = sect->next_sect ) {
        rtn( sect );
        WalkAreas( sect->areas, rtn );
    }
}

void WalkAreas( OVL_AREA *ovl, void (*rtn)( section * ) )
/*******************************************************/
{
    for( ; ovl != NULL; ovl = ovl->next_area ) {
        WalkSections( ovl->sections, rtn );
    }
}

static void WriteVectors( void )
/******************************/
{
    vecnode             *vectnode;
    int                 n;
    targ_addr           addr;
    symbol              *sym;

    WriteMapNL( 2 );
    XReportSymAddr( OverlayTable );
    XReportSymAddr( OverlayTableEnd );
    XReportSymAddr( OvlVecStart );
    XReportSymAddr( OvlVecEnd );
    WriteOvlHead();
    n = 0;
    for( vectnode = OvlVectors; vectnode != NULL; vectnode = vectnode->next ) {
        OvlGetVecAddr( ++n, &addr );
        sym = vectnode->sym;
        WriteMap( "%a section %d : %S",
                  &addr, sym->p.seg->u.leader->class->section->ovlref, sym );
    }
}

static void DoSecPubs( section *sec )
/***********************************/
{
    WriteMapNL( 2 );
    WriteMap( "Overlay section %d address %a", sec->ovlref, &sec->sect_addr );
    WriteMap( "====================================" );
    WriteSegs( sec );
    StartMapSort();
    if( MapFlags & MAP_FLAG ) {
        WritePubHead();
    }
    ProcPubs( sec->mods, sec );
    OvlProcPubsSect( sec );
    FinishMapSort();
}

void OvlProcPubsSect( section *sec )
/**********************************/
{
    for( CurrMod = sec->u.dist_mods; CurrMod != NULL; CurrMod = CurrMod->x.next ) {
        DoPubs( sec );
    }
}

void OvlProcPubs( void )
/**********************/
{
    WriteVectors();
    WalkAreas( Root->areas, DoSecPubs );
}

static void         AllocAreas( OVL_AREA *area );

static void AllocSections( section *first_sect )
/**********************************************/
/* Allocate parallel overlay sections */
{
    targ_addr           save;
    targ_addr           max;
    section             *sect;
    unsigned long       result;
    unsigned            ovl_size;
    unsigned            min_size;
    bool                area_specified;

    CurrSect = first_sect;    /* NormalizeAddr() needs to know the section */
    MAlign( FmtData.SegShift );     /* as do some DBI routines. */
    NormalizeAddr();
    if( first_sect == NonSect ) {
        if( FmtData.u.dos.dynamic ) {
            ovl_size = CurrLoc.seg - Stash.seg;
            /* OvlSectNum value 0 is reserved for Root */
            min_size = ovl_size + OvlSectNum + 1;  // need at least 1 para per sect. + 1
            if( min_size < 64 ) {
                min_size = 64;      // reserve 1 K for the stack
            }
            area_specified = true;
            if( OvlAreaSize == 0xFFFF ) {   // default is twice the memory size
                OvlAreaSize = ovl_size * 2;
                area_specified = false;
            } else if( OvlAreaSize == 0 ) {
                area_specified = false;
            }
            if( OvlAreaSize < min_size ) {
                OvlAreaSize = min_size;
                if( area_specified ) {
                    LnkMsg( WRN+MSG_AREA_TOO_SMALL, "l", (unsigned long)min_size << FmtData.SegShift );
                }
            }
            result = CurrLoc.seg + (unsigned long)( OvlAreaSize - ovl_size );
            if( result > 0xFFFF ) {
                LnkMsg( WRN+MSG_CANT_RESERVE_SPACE, "l", (unsigned long)OvlAreaSize << FmtData.SegShift );
            } else {
                CurrLoc.seg = (segment)result;
            }
        }
        OvlAreaSize = CurrLoc.seg - Stash.seg;
    }
    save = CurrLoc;
    max.off = 0;
    max.seg = 0;
    for( sect = first_sect; sect != NULL; sect = sect->next_sect ) {
        CurrSect = sect;
        sect->sect_addr = save;
        AllocClasses( sect );
        if( sect->areas != NULL ) {
            AllocAreas( sect->areas );
        }
        NormalizeAddr();        /* avoid any overflow messages */
        AddSize( 2 );   /* reserve some space for the overlay manager */
        NormalizeAddr();        /*  get canonical form */
        if( ( CurrLoc.seg > max.seg ) || ( CurrLoc.seg == max.seg ) && ( CurrLoc.off > max.off ) ) {
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

void OvlCalc( void )
/******************/
{
    unsigned        temp;
    outfilelist     *fnode;

    CurrSect = Root;
    /* record starting address of overlay table */
    Align( 2 ); // for overlay table speed.
    SET_SYM_ADDR( OverlayTable, CurrLoc.off, CurrLoc.seg );
    DEBUG(( DBG_OLD, "Overlay table address %a", &CurrLoc ));
    OvltabAddr = CurrLoc;
    /* calculate size of overlay table proper */
    /* OvlSectNum value 0 is reserved for Root, Root entry is not emited */
    temp = sizeof( ovltab_prolog ) + ( OvlSectNum - 1 ) * sizeof( ovltab_entry );
    SET_SYM_ADDR( OverlayTableEnd, CurrLoc.off + temp, CurrLoc.seg );
    temp += sizeof( unsigned_16 );      /* add Overlays table terminator */
    for( fnode = OutFiles; fnode != NULL; fnode = fnode->next ) {
        fnode->ovlfnoff = temp;
        temp += strlen( fnode->fname ) + 1;
    }
    Root->outfile->ovlfnoff |= OVE_EXE_FILENAME;    // indicate the .exe file.
    /* calculate starting address of overlay vectors, record */
    OvltabSize = temp;   /*  incl. NULLCHAR */
    CurrentSeg = NULL;
    AddSize( OvltabSize );
    SET_SYM_ADDR( OvlVecStart, CurrLoc.off, CurrLoc.seg );
    DEBUG(( DBG_OLD, "Overlay vector start %a", &CurrLoc ));
    OvlvecAddr = CurrLoc;
    /* calculate start of overlay area */
    if( FmtData.u.dos.ovl_short ) {
        temp = VecNum * sizeof( svector );
    } else {
        temp = VecNum * sizeof( lvector );
    }
    AddSize( temp );
    OvltabSize += temp;
    SET_SYM_ADDR( OvlVecEnd, CurrLoc.off, CurrLoc.seg );
    AddSize( 2 );       // reserve some space for the ovl. manager.
    Align( 4 );
    NormalizeAddr();
    Stash = CurrLoc;
    AllocAreas( Root->areas );
    MAlign( 4 );          /*    don't add to section size */
    NormalizeAddr();        /*  now CurrLoc.seg points to next free para */

    OvlGroup = AllocGroup( AutoGrpName, &Groups );
    OvlGroup->grp_addr = OvltabAddr;
    OvlGroup->section = Root;
    OvlGroup->size = OvltabSize;
    OvlGroup->totalsize = OvltabSize;
    OvlGroup->num = 0;
    OvlGroup->linear = 0;
    Ring2Append( &OvlGroup->leaders, OvlSeg );

    OvlSeg->group = OvlGroup;
    OvlSeg->seg_addr = OvltabAddr;
    OvlSeg->size = OvltabSize;

    OvlSegData->u1.vm_ptr = AllocStg( OvltabSize );
    OvlSegData->length = OvltabSize;
    RingAppend( &OvlSegData->u.leader->pieces, OvlSegData );
}

void FreeOverlaySupp( void )
/**************************/
{
    OvlClasses = NULL;
    OvlVectors = NULL;
    if( OvlSeg != NULL ) {
        FreeLeader( OvlSeg );
    }
    FreeDistribSupp();
}

static bool IsAncestor( overlay_ref elder_ovlref, section *ceorl )
/****************************************************************/
/* Is overlay section # "elder" an ancestor of section "ceorl" */
{
    for( ; ; ) {
        if( ceorl->ovlref == elder_ovlref )
            return( true );
        if( ceorl->parent == NULL )
            return( false );
        ceorl = ceorl->parent;
    }
}

#define NO_VECTOR( sym ) ( ( IS_SYM_COMMUNAL( sym ) ) \
                            || ( (sym)->u.d.ovlstate & OVL_FORCE ) )

void OvlDefVector( symbol *sym )
/******************************/
{
    segdata     *sdata;
    overlay_ref ovlref;

    if( NO_VECTOR( sym ) )
        return;
    sdata = sym->p.seg;
    if( sdata == NULL ) {
        sym->u.d.ovlstate |= (OVL_FORCE | OVL_NO_VECTOR);
        return;         /* NOTE: <--- premature return <----------- */
    }
    ovlref = sdata->u.leader->class->section->ovlref;
    if( !sdata->iscode || ( ovlref == 0 ) ) {      // not code or in root
        sym->u.d.ovlstate |= (OVL_FORCE | OVL_NO_VECTOR);
    } else {
        if( sym->info & SYM_REFERENCED ) {
            if( sym->u.d.ovlref != ovlref ) {
                /* first reference must have been a proper ancestor or cousin */
                OvlVectorize( sym );
            }
        } else {
            sym->u.d.ovlstate |= OVL_REF;
            sym->u.d.ovlref = ovlref;
        }
    }
}

void OvlVectorize( symbol *sym )
/******************************/
/* allocate an overlay vector for a symbol */
{
    vecnode         *vectnode;

    sym->u.d.ovlref = ++VecNum;
    sym->u.d.ovlstate &= ~OVL_NO_VECTOR;
    sym->u.d.ovlstate |= OVL_FORCE;
    _PermAlloc( vectnode, sizeof( vecnode ) );
    vectnode->sym = sym;
    LinkList( &OvlVectors, vectnode );
    DEBUG(( DBG_OLD, "Vectorize %d %S", VecNum, sym ));
}

static void OvlRefVector( symbol *sym )
/*************************************/
{
    overlay_ref     ovlref;

    if( IS_SYM_COMMUNAL( sym ) )
        return;
    if( (sym->u.d.ovlstate & OVL_VEC_MASK) != OVL_UNDECIDED )
        return;
    if( (sym->info & SYM_DEFINED) == 0 ) {
        if( (sym->u.d.ovlstate & OVL_REF) == 0 ) {
            sym->u.d.ovlref = CurrSect->ovlref;
            sym->u.d.ovlstate |= OVL_REF;
        } else if( FmtData.u.dos.distribute ) {
            sym->u.d.ovlref = LowestAncestor( sym->u.d.ovlref, CurrSect );
        }
    } else {
        if( sym->p.seg == NULL )
            return;
        /* at this point, we know it has already been defined, but does */
        /* not have an overlay vector, and is not data */
        ovlref = sym->p.seg->u.leader->class->section->ovlref;
        if( IsAncestor( ovlref, CurrSect ) )
            return;
        /* overlay vector necessary */
        OvlVectorize( sym );
    }
}

void OvlTryRefVector( symbol *sym )
/*********************************/
{
    if( FmtData.u.dos.distribute && (LinkState & LS_SEARCHING_LIBRARIES) ) {
        RefDistribSym( sym );
    } else {
        OvlRefVector( sym );
    }
}

void OvlUseVector( symbol *sym, extnode *newnode )
/************************************************/
{
    if( IS_SYM_COMMUNAL( sym ) )
        return;
    if( (sym->u.d.ovlstate & OVL_VEC_MASK) != OVL_MAKE_VECTOR )
        return;
    if( IsAncestor( sym->p.seg->u.leader->class->section->ovlref, CurrSect ) ) {
        return;
    }
    /* use overlay vector for all calls */
    newnode->ovlref = sym->u.d.ovlref;
}

void OvlIndirectCall( symbol *sym )
/*********************************/
{
    overlay_ref ovlref;

    if( NO_VECTOR( sym ) )
        return;
    if( sym->info & SYM_DEFINED ) {
        if( sym->info & SYM_DISTRIB ) {
            DistribIndirectCall( sym );
        } else if( sym->p.seg != NULL ) {
            ovlref = sym->p.seg->u.leader->class->section->ovlref;
            if( ( ovlref != 0 ) && sym->p.seg->iscode ) {
                OvlVectorize( sym );
            }
        }
    } else {
        // indicate a reference from the root for purposes of vector generation
        sym->u.d.ovlstate |= OVL_REF;
        sym->u.d.ovlref = 0;
    }
}

void OvlGetVecAddr( int vecnum, targ_addr *addr )
/***********************************************/
/* return address of overlay vector in canonical form */
{
    *addr = OvlvecAddr;
    if( FmtData.u.dos.ovl_short ) {
        /* short address -- fits into 16 bits */
        addr->off += ( vecnum - 1 ) * sizeof( svector ) + ( addr->seg << FmtData.SegShift );
        addr->seg = 0;
    } else {
        addr->off += ( vecnum - 1 ) * sizeof( lvector );
    }
}

bool OvlCheckOvlClass( const char *clname, bool *isovlclass )
/***********************************************************/
/* check if among overlay classes, and return true if it is code. */
{
    list_of_names       *cnamelist;
    bool                retval;

    retval = IsCodeClass( clname, strlen( clname ) );
    cnamelist = OvlClasses;
    if( cnamelist == NULL ) {   /* check for anything ending in code */
        *isovlclass = retval;
        return( retval );
    } else {
        do {
            if( stricmp( clname, cnamelist->name ) == 0 ) {
                *isovlclass = true;
                return( retval );
            }
            cnamelist = cnamelist->next_name;
        } while( cnamelist != NULL );
    }
    *isovlclass = false;
    return( false );
}

section *OvlCheckOvlSection( const char *clname )
/***********************************************/
{
    section     *sect;
    bool        dummy;

    sect = DBIGetSect( clname );
    if( sect == NULL ) {
        if( OvlCheckOvlClass( clname, &dummy ) ) {
            sect = CurrSect;
        } else {
            sect = NonSect;
        }
    }
    return( sect );
}

static void PutOvlInfo( unsigned off, void *src, unsigned len )
/*************************************************************/
{
    PutInfo( OvlSegData->u1.vm_ptr + off - OvlGroup->grp_addr.off, src, len );
}

static void ShortVectors( symbol *loadsym )
/*****************************************/
{
    vecnode             *vectnode;
    unsigned            vectoff;
    svector             vectdata;
    unsigned_32         loadval;
    unsigned_32         temp;
    signed_32           diff;
    int                 vecnum;

    vectoff = OvlvecAddr.off;
    temp = MK_REAL_ADDR( OvlvecAddr.seg, OvlvecAddr.off );
    /* get loader address */
    loadval = MK_REAL_ADDR( loadsym->addr.seg, loadsym->addr.off );
    /* fill in overlay vector */
    vectdata.call_op = 0xe8;
    vectdata.jmp_op = 0xe9;     // near jmp
    vecnum = 1;
    for( vectnode = OvlVectors; vectnode != NULL; vectnode = vectnode->next ) {
        temp = vectoff + offsetof( svector, ldr_addr ) + sizeof( unsigned_16 );
        diff = loadval - temp;
        if( ( diff < -32768 ) || ( diff > 32767 ) ) {
            LnkMsg( ERR+MSG_VECT_RANGE, "sd", "short (1)", vecnum );
        }
        _HostU16toTarg( diff, vectdata.ldr_addr );
        loadsym = vectnode->sym;
        _HostU16toTarg( loadsym->p.seg->u.leader->class->section->ovlref, vectdata.sec_num );
        temp = vectoff + offsetof( svector, target ) + sizeof( unsigned_16 );
        diff = MK_REAL_ADDR( loadsym->addr.seg, loadsym->addr.off ) - temp;
        if( ( diff < -32768 ) || ( diff > 32767 ) ) {
            LnkMsg( ERR+MSG_VECT_RANGE, "sd", "short (2)", vecnum );
        }
        _HostU16toTarg( diff, vectdata.target );
        PutOvlInfo( vectoff, &vectdata, sizeof( vectdata ) );
        vectoff += sizeof( vectdata );
        ++vecnum;
    }
}

static void LongVectors( symbol *loadsym )
/****************************************/
{
    vecnode             *vectnode;
    unsigned            vectoff;
    lvector             vectdata;
    dos_addr            addr;
    signed_32           diff;
    unsigned_32         loadval;
    unsigned_32         temp;
    int                 vecnum;

    vectoff = OvlvecAddr.off;
    /* fill in overlay vector */
    vectdata.u.v.call_op = 0xe8;
    vectdata.jmp_op = 0xea;     // far jmp
    loadval = loadsym->addr.off;
    addr.seg = OvlGroup->grp_addr.seg;
    vecnum = 1;
    for( vectnode = OvlVectors; vectnode != NULL; vectnode = vectnode->next ) {
        temp = vectoff + offsetof( lvector, u.v.ldr_addr ) + sizeof( unsigned_16 );
        diff = loadval - temp;
        if( ( diff < -32768 ) || ( diff > 32767 ) ) {
            LnkMsg( ERR+MSG_VECT_RANGE, "sd", "long", vecnum );
        }
        _HostU16toTarg( diff, vectdata.u.v.ldr_addr );
        loadsym = vectnode->sym;
        _HostU16toTarg( loadsym->p.seg->u.leader->class->section->ovlref, vectdata.u.v.sec_num );
        _HostU16toTarg( loadsym->addr.off, vectdata.target.off );
        if( FmtData.u.dos.dynamic ) {
            _HostU16toTarg( loadsym->addr.seg - loadsym->p.seg->u.leader->group->grp_addr.seg, vectdata.target.seg );
        } else {
            _HostU16toTarg( loadsym->addr.seg, vectdata.target.seg );
            addr.off = vectoff + offsetof( lvector, target.seg );
            WriteReloc( OvlGroup, addr.off, &addr, sizeof( dos_addr ) );
        }
        PutOvlInfo( vectoff, &vectdata, sizeof( vectdata ) );
        vectoff += sizeof( vectdata );
        vecnum++;
    }
}

void OvlEmitVectors( void )
/*************************/
{
    symbol      *symptr;
    dos_addr    addr;
    const char  *loader_name;
    bool        isshort;

    /* output relocation items for overlay table */
    addr.seg = OvlGroup->grp_addr.seg;
    addr.off = OvltabAddr.off + offsetof( ovltab_prolog, start.seg );
    WriteReloc( OvlGroup, addr.off, &addr, sizeof( dos_addr ) );
    addr.off = OvltabAddr.off + offsetof( ovltab_prolog, delta );
    WriteReloc( OvlGroup, addr.off, &addr, sizeof( dos_addr ) );

    isshort = false;
    if( FmtData.u.dos.dynamic ) {
        loader_name = _DynamicOvlldr;
    } else if( FmtData.u.dos.ovl_short ) {
        loader_name = _ShortOvlldr;
        isshort = true;
    } else {
        loader_name = _LongOvlldr;
    }
    symptr = RefISymbol( loader_name );
    if( (symptr->info & SYM_DEFINED) == 0 ) {
        LnkMsg( ERR+MSG_NO_OVERLAY_LOADER, NULL );
    } else if( isshort ) {
        ShortVectors( symptr );
    } else {
        LongVectors( symptr );
    }
}

void OvlSetStartAddr( void )
/**************************/
{
    symbol      *sym;

    /* stuff overlay init routine address in header */
    Stash = StartInfo.addr;

    if( FmtData.u.dos.dynamic ) {
        sym = FindISymbol( _DynamicInitRtn );
    } else if( FmtData.u.dos.ovl_short ) {
        sym = FindISymbol( _ShortOvlInitRtn );
    } else {
        sym = FindISymbol( _LongOvlInitRtn );
    }
    if( sym == NULL )
        return;
    if( sym->info & SYM_DEFINED ) {
        StartInfo.addr = sym->addr;
    }
}

void OvlPass1( void )
/*******************/
{
    symbol      *sym;

    WalkAreas( Root->areas, LoadObjFiles );

    /* define symbols for overlay table */
    OverlayTable = DefISymbol( _OvltabName );
    OverlayTableEnd = DefISymbol( _OvltabEndName );
    OvlVecStart = DefISymbol( _OvlVecStartName );
    OvlVecEnd = DefISymbol( _OvlVecEndName );

    OvlSeg = InitLeader( "" );
    OvlSeg->class = FindClass( Root, OvlMgrClass, false, true );
    OvlSeg->combine = COMBINE_INVALID;
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

static void EmitOvlEntry( section *sect, void *_off )
/***************************************************/
{
    ovltab_entry        entry;
    offset              len;
    unsigned_16         flags_anc;
    unsigned_16         start_para;
    unsigned            *off = _off;

    _HostU16toTarg( 0, entry.code_handle );
    flags_anc = sect->parent->ovlref;
    if( sect == NonSect ) {
        flags_anc |= OVE_FLAG_PRELOAD;/*  pre-load the data area */
    }
    _HostU16toTarg( sect->outfile->ovlfnoff, entry.fname );
    _HostU16toTarg( sect->relocs, entry.relocs );
    _HostU16toTarg( flags_anc, entry.flags_anc );
    start_para = sect->sect_addr.seg + ( sect->sect_addr.off >> FmtData.SegShift );
    _HostU16toTarg( start_para, entry.start_para );
    len = ( sect->size + 15 ) >> 4;
    _HostU16toTarg( len, entry.num_paras );
    _HostU32toTarg( sect->u.file_loc, entry.disk_addr );
    PutOvlInfo( *off, &entry, sizeof( entry ) );
    *off += sizeof( ovltab_entry );
}

void OvlEmitTable( void )
/***********************/
/* generate overlay table */
{
    unsigned            off;
    ovltab_prolog       prolog;
    unsigned_16         u16;
    unsigned            len;
    outfilelist         *fnode;

    off = OvltabAddr.off;
    /*
     *  Generate prolog :
     */
    prolog.major = OVL_MAJOR_VERSION;
    prolog.minor = OVL_MINOR_VERSION;
    /*
     *  output start address for program
     *  reloc for this was emitted by OvlEmitVectors
     */
    _HostU16toTarg( Stash.off, prolog.start.off );
    _HostU16toTarg( Stash.seg, prolog.start.seg );
    /*
     *  this should give us the paragraph of the load module start
     *  reloc for this was emitted by OvlEmitVectors
     */
    _HostU16toTarg( 0, prolog.delta );
    _HostU16toTarg( OvlAreaSize, prolog.ovl_size );

    PutOvlInfo( off, &prolog, sizeof( prolog ) );
    /*
     *  Generate entries :
     */
    off += sizeof( prolog );
    ParmWalkAreas( Root->areas, EmitOvlEntry, &off );
    /*
     *  Generate epilog :
     */
    u16 = OVLTAB_TERMINATOR;
    PutOvlInfo( off, &u16, sizeof( unsigned_16 ) );
    off += sizeof( unsigned_16 );
    /* generate overlay filenames, including NULLCHARS */
    for( fnode = OutFiles; fnode != NULL; fnode = fnode->next ) {
        len = strlen( fnode->fname ) + 1;
        PutOvlInfo( off, fnode->fname, len );
        off += len;
    }
}

void OvlPadOvlFiles( void )
/*************************/
// The overlay files must contain a complete paragraph at the end of the file
// for the overlay loader to be able to correctly read it.
{
    outfilelist     *fnode;
    size_t          pad;

    for( fnode = OutFiles; fnode != NULL; fnode = fnode->next ) {
        pad = MAKE_PARA( fnode->file_loc ) - fnode->file_loc;
        if( pad ) {
            PadBuffFile( fnode, pad );
        }
    }
}

void OvlSetTableLoc( group_entry *group, unsigned long loc )
/**********************************************************/
{
    if( group == OvlGroup ) {
        OvlTabOffset = loc;
    }
}

#endif
