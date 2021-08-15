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
* Description:  Routines specific processing relocations in OMF.
*
****************************************************************************/


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
    struct bakpatlist   *next;
    unsigned_16         len;
    segdata             *sdata;
    byte                loctype;
    bool                is32bit;
    char                data[1];
} bakpat_list;

static bakpat_list      *BakPats;

#define MAX_THREADS 4

static frame_spec       FrameThreads[MAX_THREADS];
static target_spec      TargThreads[MAX_THREADS];

void ResetOMFReloc( void )
/************************/
{
    BakPats = NULL;
}

static void GetFrame( unsigned method, frame_spec *frame )
/********************************************************/
/* Get frame for fixup. */
{
    extnode     *ext;
    grpnode     *group;
    segnode     *seg;

    switch( method ) {
    case FRAME_SEG:
        seg = (segnode *) FindNode( SegNodes, GetIdx() );
        frame->u.sdata = seg->entry;
        frame->type = FIX_FRAME_SEG;
        break;
    case FRAME_GRP:
        group = (grpnode *) FindNode( GrpNodes, GetIdx() );
        if( group->entry == NULL ) {
            frame->type = FIX_FRAME_FLAT;
        } else {
            frame->u.group = group->entry;
            frame->type = FIX_FRAME_GRP;
        }
        break;
    case FRAME_EXT:
        ext = (extnode *) FindNode( ExtNodes, GetIdx() );
        if( IS_SYM_IMPORTED( ext->entry ) ) {
            frame->type = FIX_FRAME_TARG;
        } else {
            frame->u.sym = ext->entry;
            frame->type = FIX_FRAME_EXT;
        }
        break;
    case FRAME_ABS:
        _TargU16toHost( _GetU16UN( ObjBuff ), frame->u.abs );
        ObjBuff += sizeof( unsigned_16 );
        frame->type = FIX_FRAME_ABS;
        break;
    case FRAME_LOC:
        frame->type = FIX_FRAME_LOC;
        break;
    case FRAME_TARG:
        frame->type = FIX_FRAME_TARG;
        break;
    default:
        BadObject();
    }
}

static void GetTarget( unsigned method, target_spec *target )
/***********************************************************/
{
    extnode             *ext;
    grpnode             *group;
    segnode             *seg;

    switch( method ) {
    case TARGET_SEG:
        seg = (segnode *) FindNode( SegNodes, GetIdx() );
        target->u.sdata = seg->entry;
        target->type = FIX_TARGET_SEG;
        break;
    case TARGET_GRP:
        group = (grpnode *) FindNode( GrpNodes, GetIdx() );
        target->u.group = group->entry;
        target->type = FIX_TARGET_GRP;
        break;
    case TARGET_EXT:
        ext = (extnode *) FindNode( ExtNodes, GetIdx() );
        target->u.sym = ext->entry;
        target->type = FIX_TARGET_EXT;
        break;
    case TARGET_ABS:
        _TargU16toHost( _GetU16UN( ObjBuff ), target->u.abs );
        ObjBuff += sizeof( unsigned_16 );
        target->type = FIX_TARGET_ABS;
        break;
    }
}

void DoRelocs( void )
/**************************/
/* Process FIXUP records. */
{
    fix_type    fixtype;
    unsigned    typ;
    offset      location;
    offset      addend;
    frame_spec  fthread;
    target_spec tthread;

    if( ObjFormat & FMT_IGNORE_FIXUPP )
        return;
    if( ObjFormat & FMT_IS_LIDATA ) {
        LnkMsg( LOC_REC+WRN+MSG_REL_IN_LIDATA, NULL );
        return;
    }
    do {
        typ = *ObjBuff++;
        if( typ & FIXUPP_FIXUP ) {
            /*
             * fixup
             */
            fixtype = 0;
            switch( (typ >> 2) & 0x0F ) {
            case LOC_OFFSET_LO:
                fixtype = FIX_OFFSET_8;
                break;
            case LOC_OFFSET:
                fixtype = FIX_OFFSET_16;
                break;
            case LOC_BASE:
                fixtype = FIX_BASE;
                break;
            case LOC_BASE_OFFSET:
                fixtype = FIX_BASE_OFFSET_16;
                break;
            case LOC_OFFSET_HI:
                fixtype = FIX_HIGH_OFFSET_8;
                break;
            case LOC_OFFSET_LOADER:
                if( ObjFormat & FMT_EASY_OMF ) {
                    fixtype = FIX_OFFSET_32;                    /* Pharlap only */
                } else {
                    fixtype = FIX_OFFSET_16 | FIX_LOADER_RES;   /* OMF standard */
                }
                break;
            case LOC_PHARLAP_BASE_OFFSET_32:
                if( ObjFormat & FMT_EASY_OMF ) {
                    fixtype = FIX_BASE_OFFSET_32;               /* Pharlap only */
                }
                break;
            case LOC_OFFSET_32:
                fixtype = FIX_OFFSET_32;
                break;
            case LOC_BASE_OFFSET_32:
                fixtype = FIX_BASE_OFFSET_32;
                break;
            case LOC_OFFSET_32_LOADER:
                fixtype = FIX_OFFSET_32; //| FIX_LOADER_RES;
                break;
            default:
                break;
            }
            if( fixtype && (typ & FIXUPP_MBIT) == 0 ) {
                fixtype |= FIX_REL;
            }
            location = ((typ & 3) << 8) + *ObjBuff++;
            typ = *ObjBuff++;
            /*
             * frame processing
             */
            if( typ & FIXDAT_FTHREAD ) {
                fthread = FrameThreads[( typ >> 4 ) & 3];
            } else {
                GetFrame( ( typ >> 4 ) & 7, &fthread );
            }
            /*
             * target processing
             */
            if( typ & FIXDAT_TTHREAD ) {
                tthread = TargThreads[typ & 3];
            } else {
                GetTarget( typ & 3, &tthread );
            }
            /*
             * target addend processing
             */
            addend = 0;
            if( (typ & FIXDAT_PBIT) == 0 ) {
                if( ObjFormat & FMT_32BIT_REC ) {
                    addend = GET_U32_UN( ObjBuff );
                    ObjBuff += sizeof( unsigned_32 );
                } else {
                    addend = GET_U16_UN( ObjBuff );
                    ObjBuff += sizeof( unsigned_16 );
                }
            }
            StoreFixup( location, fixtype, &fthread, &tthread, addend );
        } else {
            /*
             * thread
             */
            if( typ & 0x40 ) {
                /*  frame */
                GetFrame( (typ >> 2) & 7, &FrameThreads[typ & 3] );
            } else {
                /*  target */
                GetTarget( (typ >> 2) & 3, &TargThreads[typ & 3] );
            }
        }
    } while( ObjBuff < EOObjRec );
}

static void StoreBakPat( segdata *sdata, byte loctype )
/*****************************************************/
/* store a bakpat record away for future processing. */
{
    unsigned            len;
    bakpat_list         *bkptr;

    len = EOObjRec - ObjBuff;
    _ChkAlloc( bkptr, sizeof( bakpat_list ) + len - 1 );
    bkptr->len = len;
    bkptr->loctype = loctype;
    bkptr->sdata = sdata;   /* We don't know the data offset yet. */
    bkptr->is32bit = (ObjFormat & FMT_32BIT_REC) != 0;
    memcpy( bkptr->data, ObjBuff, len );
    LinkList( &BakPats, bkptr );
}

void ProcBakpat( void )
/****************************/
/* store the bakpat record away for future processing */
{
    segnode             *seg;
    byte                loctype;

    seg = (segnode *) FindNode( SegNodes, GetIdx() );
    if( seg->info & SEG_DEAD )
        return;
    loctype = *ObjBuff++;
    StoreBakPat( seg->entry, loctype );
}

void DoBakPats( void )
/***************************/
/* go through the list of stored bakpats and apply them all */
{
    char                *data;
    bakpat_list         *bkptr;
    bakpat_list         *next;
    offset              off;
    offset              value;
    unsigned_8          value8;
    unsigned_16         value16;
    unsigned_32         value32;
    virt_mem            vmemloc;

    for( bkptr = BakPats; bkptr != NULL; bkptr = next ) {
        next = bkptr->next;
        data = bkptr->data;
        off = 0;
        value = 0;
        while( bkptr->len > 0 ) {
            if( bkptr->is32bit ) {
                _TargU32toHost( _GetU32( data ), off );
                data += sizeof( unsigned_32 );
                _TargU32toHost( _GetU32( data ), value );
                data += sizeof( unsigned_32 );
                bkptr->len -= 2 * sizeof( unsigned_32 );
            } else {
                _TargU16toHost( _GetU16( data ), off );
                data += sizeof( unsigned_16 );
                _TargU16toHost( _GetU16( data ), value );
                data += sizeof( unsigned_16 );
                bkptr->len -= 2 * sizeof( unsigned_16 );
            }
            /* Now the sdata->data pointer should be valid. */
            vmemloc = bkptr->sdata->u1.vm_ptr + off;
            switch( bkptr->loctype ) {
            case 0:
                ReadInfo( vmemloc, &value8, sizeof( unsigned_8 ) );
                value8 += value;
                PutInfo( vmemloc, &value8, sizeof( unsigned_8 ) );
                break;
            case 1:
                ReadInfo( vmemloc, &value16, sizeof( unsigned_16 ) );
                value16 += value;
                PutInfo( vmemloc, &value16, sizeof( unsigned_16 ) );
                break;
            case 2:
                ReadInfo( vmemloc, &value32, sizeof( unsigned_32 ) );
                value32 += value;
                PutInfo( vmemloc, &value32, sizeof( unsigned_32 ) );
                break;
            }
        }
        _LnkFree( bkptr );
    }
    BakPats = NULL;
}

void ProcNbkpat( void )
/****************************/
/* process a named bakpat record */
{
    list_of_names       *symname;
    symbol              *sym;
    byte                loctype;

    loctype = *ObjBuff++;
    symname = FindName( GetIdx() );
    sym = RefISymbol( symname->name );
    if( !IS_SYM_COMDAT( sym ) )         /* can't handle these otherwise */
        return;
    if( sym->info & SYM_DEAD )
        return;
    StoreBakPat( sym->p.seg, loctype );
}
