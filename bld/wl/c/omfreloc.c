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
 *  OMFRELOC:  routines specific processing relocations in OMF
 *
*/

#include "linkstd.h"
#include "msg.h"
#include "wlnkmsg.h"
#include "alloc.h"
#include "newmem.h"
#include "virtmem.h"
#include "reloc.h"
#include <pcobj.h>
#include <string.h>
#include "obj2supp.h"
#include "objnode.h"
#include "objio.h"
#include "overlays.h"
#include "objstrip.h"
#include "objomf.h"
#include "objpass2.h"
#include "ring.h"
#include "omfreloc.h"

typedef struct bakpatlist {
    struct bakpatlist * next;
    virt_mem            addr;
    unsigned_16         len;
    byte                loctype;
    bool                is32bit;
    char                data[1];
} bakpat_list;

static bakpat_list *    BakPats;

#define MAX_THREADS 4

static frame_spec       FrameThreads[MAX_THREADS];
static frame_spec       TargThreads[MAX_THREADS];

static fix_type RelocTypeMap[] = {
    FIX_OFFSET_8,       // LOC_OFFSET_LO
    FIX_OFFSET_16,      // LOC_OFFSET
    FIX_BASE,           // LOC_BASE
    FIX_BASE_OFFSET_16, // LOC_BASE_OFFSET
    FIX_HIGH_OFFSET_8,  // LOC_OFFSET_HI
    FIX_OFFSET_32,      // LOC_OFFSET_32
    FIX_BASE_OFFSET_32, // LOC_BASE_OFFSET_32
    FIX_OFFSET_16 | FIX_LOADER_RES      // modified loader resolved off_16
};

extern void ResetOMFReloc( void )
/*******************************/
{
    BakPats = NULL;
}

extern void DoRelocs( void )
/**************************/
/* Process FIXUP records. */
{
    fix_type    fixtype;
    unsigned    typ;
    unsigned    omftype;
    offset      place_to_fix;
    unsigned    loc;
    signed_32   addend;
    frame_spec  fthread;
    frame_spec  tthread;

    if( ObjFormat & FMT_IGNORE_FIXUPP ) return;
    if( ObjFormat & FMT_IS_LIDATA ) {
        LnkMsg( LOC_REC+WRN+MSG_REL_IN_LIDATA, NULL );
        return;
    }
    do {
        typ = GET_U8_UN( ObjBuff );
        ++ObjBuff;
        omftype = (typ >> 2) & 7;
        if( (typ & 0x80) == 0 ) {/*  thread */
            if( typ & 0x40 ) {/*  frame */
                GetFrame( omftype, &FrameThreads[typ & 3] );
            } else {/*  target */
                GetTarget( omftype, &TargThreads[typ & 3] );
            }
        } else { /* fixup */
            if( typ & 0x20 ) {   // used in 32-bit microsoft fixups.
                switch( omftype ) {
                case LOC_OFFSET:
                case LOC_MS_LINK_OFFSET:
                    omftype = LOC_OFFSET_32;
                    break;
                case LOC_BASE_OFFSET:
                    omftype = LOC_BASE_OFFSET_32;
                    break;
                }
            } else if( omftype == LOC_MS_LINK_OFFSET
                        && !(ObjFormat & FMT_32BIT_REC) ) {
                omftype = LOC_BASE_OFFSET_32 + 1; // index of special table.
            }
            fixtype = RelocTypeMap[omftype];
            if( !(typ & 0x40) ) {
                fixtype |= FIX_REL;
            }
            place_to_fix = ((typ & 3) << 8) + GET_U8_UN( ObjBuff );
            ++ObjBuff;
            typ = *ObjBuff;
            ++ObjBuff;
            loc = typ >> 4 & 7;
            if( typ & 0x80 ) {
                fthread = FrameThreads[loc & 3];
            } else {
                GetFrame( loc, &fthread );
            }
            loc = typ & 7;
            if( typ & 8 ) {
                tthread = TargThreads[loc & 3];
            } else {
                GetTarget( loc, &tthread );
            }
            addend = 0;
            if( loc <= T_ABSWD ) {  /*  if( (loc & 4) == 0 )then */
                if( ObjFormat & FMT_32BIT_REC ) {
                    addend = *((signed_32 UNALIGN *)ObjBuff);
                    ObjBuff += sizeof( signed_32 );
                } else {
                    addend = GET_U16_UN(ObjBuff);
                    ObjBuff += sizeof( unsigned_16 );
                }
            }
            StoreFixup( place_to_fix, fixtype, &fthread, &tthread, addend );
        }
    } while( ObjBuff < EOObjRec );
}

static void GetFrame( unsigned frame, frame_spec *refframe )
/**********************************************************/
/* Get frame for fixup. */
{
    extnode *   ext;
    grpnode *   group;
    segnode *   seg;
    unsigned    index;

    if( frame < F_LOC ) {
        index = GetIdx();
    }
    refframe->type = frame;
    switch( frame ) {
    case F_SEG:
        seg = (segnode *) FindNode( SegNodes, index );
        refframe->u.sdata = seg->entry;
        break;
    case F_GRP:
        group = (grpnode *) FindNode( GrpNodes, index );
        if( group->entry == NULL ) {
            refframe->type = FIX_FRAME_FLAT;
        } else {
            refframe->u.group = group->entry;
        }
        break;
    case F_EXT:
        ext = (extnode *) FindNode( ExtNodes, index );
        if( IS_SYM_IMPORTED( ext->entry ) ) {
            refframe->type = FIX_FRAME_TARG;
        } else {
            refframe->u.sym = ext->entry;
        }
        break;
    case F_TARG:
    case F_LOC:
        break;
    default:
        BadObject();
    }
}

static void GetTarget( unsigned loc, frame_spec *targ )
/*****************************************************/
{
    extnode *           ext;
    grpnode *           group;
    segnode *           seg;

    targ->type = loc & 3;
    switch( loc ) {
    case T_SEGWD:
    case T_SEG:
        seg = (segnode *) FindNode( SegNodes, GetIdx() );
        targ->u.sdata = seg->entry;
        break;
    case T_GRPWD:
    case T_GRP:
        group = (grpnode *) FindNode( GrpNodes, GetIdx() );
        targ->u.group = group->entry;
        break;
    case T_EXTWD:
    case T_EXT:
        ext = (extnode *) FindNode( ExtNodes, GetIdx() );
        targ->u.sym = ext->entry;
        break;
    case T_ABSWD:
    case T_ABS:
        _TargU16toHost( _GetU16UN( ObjBuff ), targ->u.abs );
        ObjBuff += sizeof( unsigned_16 );
        break;
    }
}

static void StoreBakPat( segnode * seg, byte loctype )
/****************************************************/
/* store a bakpat record away for future processing. */
{
    unsigned            len;
    bakpat_list *       bkptr;

    len = EOObjRec - ObjBuff;
    _ChkAlloc( bkptr, sizeof(bakpat_list) + len - 1 );
    bkptr->len = len;
    bkptr->addr = seg->entry->data;
    bkptr->loctype = loctype;
    bkptr->is32bit = (ObjFormat & FMT_32BIT_REC) != 0;
    memcpy( bkptr->data, ObjBuff, len );
    LinkList( &BakPats, bkptr );
}

extern void ProcBakpat( void )
/****************************/
/* store the bakpat record away for future processing */
{
    segnode *           seg;
    byte                loctype;

    seg = (segnode *) FindNode( SegNodes, GetIdx() );
    if( seg->info & SEG_DEAD ) return;
    loctype = *ObjBuff++;
    StoreBakPat( seg, loctype );
}

extern void DoBakPats( void )
/***************************/
/* go through the list of stored bakpats and apply them all */
{
    char *              data;
    bakpat_list *       bkptr;
    bakpat_list *       next;
    offset              off;
    offset              value;
    unsigned_8          value8;
    unsigned_16         value16;
    unsigned_32         value32;
    virt_mem            vmemloc;

    bkptr = BakPats;
    while( bkptr != NULL ) {
        next = bkptr->next;
        data = bkptr->data;
        off = 0;
        value = 0;
        while( bkptr->len > 0 ) {
            if( bkptr->is32bit ) {
                _TargU32toHost( _GetU32( data ), off );
                data += sizeof(unsigned_32);
                _TargU32toHost( _GetU32( data ), value );
                data += sizeof(unsigned_32);
                bkptr->len -= 2 * sizeof(unsigned_32);
            } else {
                _TargU16toHost( _GetU16( data ), off );
                data += sizeof(unsigned_16);
                _TargU16toHost( _GetU16( data ), value );
                data += sizeof(unsigned_16);
                bkptr->len -= 2 * sizeof(unsigned_16);
            }
            vmemloc = bkptr->addr + off;
            switch( bkptr->loctype ) {
            case 0:
                ReadInfo( vmemloc, &value8, sizeof(unsigned_8) );
                value8 += value;
                PutInfo( vmemloc, &value8, sizeof(unsigned_8) );
                break;
            case 1:
                ReadInfo( vmemloc, &value16, sizeof(unsigned_16) );
                value16 += value;
                PutInfo( vmemloc, &value16, sizeof(unsigned_16) );
                break;
            case 2:
                ReadInfo( vmemloc, &value32, sizeof(unsigned_32) );
                value32 += value;
                PutInfo( vmemloc, &value32, sizeof(unsigned_32) );
                break;
            }
        }
        _LnkFree( bkptr );
        bkptr = next;
    }
    BakPats = NULL;
}

extern void ProcNbkpat( void )
/****************************/
/* process a named bakpat record */
{
    list_of_names *     symname;
    symbol *            sym;
    segnode             seg;
    byte                loctype;

    loctype = *ObjBuff++;
    symname = FindName( GetIdx() );
    sym = RefISymbol( symname->name );
    if( !IS_SYM_COMDAT(sym) ) return;   /* can't handle these otherwise */
    if( sym->info & SYM_DEAD ) return;
    seg.entry = sym->p.seg;
    StoreBakPat( &seg, loctype );
}
