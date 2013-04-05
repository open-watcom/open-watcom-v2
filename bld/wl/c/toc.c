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
* Description:  Keep track of Table of Contents (TOC) for NT PPC or of the
*               Global Offset Table (GOT) for OS/2 PPC.
*
****************************************************************************/


#include <string.h>
#include "linkstd.h"
#include "toc.h"
#include "hash.h"
#include "alloc.h"
#include "msg.h"
#include "wlnkmsg.h"
#include "virtmem.h"
#include "reloc.h"
#include "obj2supp.h"
#include "loadpe.h"
#include "loadfile.h"
#include "specials.h"
#include "objpass1.h"
#include "objstrip.h"

#define BOGUS  0xa5a5a5a5

static pHTable  Toc;
static char *   TocName;
static symbol * TocSym;

offset TocSize;
offset TocShift;

typedef struct {
    segdata *sdata;    // If sdata == NULL, use sym to get address
                       // else use off and sdata->addr get address
    union {
        offset off;  // relative to sdata->addr
        symbol *sym;
    } u;
} TocEntryId;

typedef struct {
    TocEntryId e;
    int pos;
} TocEntry;


void ResetToc( void )
/**************************/
{
    Toc = NULL;
    TocSize = 0;
    TocName = NULL;
    TocSym = NULL;
}

void CleanToc( void )
/**************************/
{
    ZapHTable( Toc, LFree );
}

static unsigned TocEntryHashFunc( void *_e, unsigned size )
/*********************************************************/
{
    TocEntry *e = _e;
    return( DataHashFunc( &e->e, sizeof( e->e ), size ) );
}

static int TocEntryCmp( const void *_e1, const void *_e2 )
/********************************************************/
{
    const TocEntry *e1 = _e1;
    const TocEntry *e2 = _e2;
    return( memcmp( &e1->e, &e2->e, sizeof( e1->e ) ) );
}

void InitToc( void )
/***********************/
{
    Toc = CreateHTable( 1024, TocEntryHashFunc, TocEntryCmp, ChkLAlloc, LFree );
    TocSize = 0;
    if( (LinkState & HAVE_PPC_CODE) && (FmtData.type & MK_PE) ) {
        TocName = TocSymName;
        TocShift = 0x8000;
    } else {
        TocName = GotSymName;
        TocShift = BOGUS;
            // It is an error to use TocShift until PrepareToc is called
    }
}

void CheckIfTocSym( symbol *sym )
/**************************************/
{
    if( TocName != NULL && TocSym == NULL ) {
        if( strcmp( sym->name, TocName ) == 0 ) {
            TocSym = sym;
        }
    }
}

bool IsTocSym( symbol *sym )
/*********************************/
{
    return( TocSym == sym );
}

static void AddToToc( TocEntryId *e )
/***********************************/
{
    TocEntry    searchEntry;
    TocEntry *  entry;

    searchEntry.e = *e;

    if( FindHTableElem( Toc, &searchEntry ) == NULL ) {
        entry = ChkLAlloc( sizeof( *entry ) );
        entry->e = *e;
        entry->pos = TocSize;
        TocSize += sizeof( offset );
        AddHTableElem( Toc, entry );
    }
}

void AddSymToToc( symbol *sym )
/************************************/
{
    TocEntryId e;

    e.sdata = NULL;
    e.u.sym = sym;
    AddToToc( &e );
}

void AddSdataOffToToc( segdata *sdata, offset off )
/********************************************************/
{
    TocEntryId e;

    DbgAssert( sdata != NULL );
    e.sdata = sdata;
    e.u.off = off;
    AddToToc( &e );
}


static void ConvertTocEntryId( TocEntryId *e )
/********************************************/
{
    if( e->sdata != NULL ) {
        // do nothing: already in sdata/offset form
    } else {
        symbol *sym = e->u.sym;
        segdata *seg = sym->p.seg;

        if( IS_SYM_IMPORTED( sym ) || seg == NULL ) {
            // do not convert; keep symbol around
        } else {
            e->sdata = seg;
            e->u.off = sym->addr.off - seg->a.delta -
                       seg->u.leader->seg_addr.off;
        }
    }
    return;
}

static void ConvertTocEntry( void *e )
/************************************/
{
    ConvertTocEntryId( &((TocEntry *)e)->e );
}

#define GOT_RESERVED_NEG_SIZE (2 * sizeof( long ))
#define GOT_RESERVED_POS0_SIZE (3 * sizeof( long ))
#define GOT_RESERVED_SIZE (GOT_RESERVED_NEG_SIZE + GOT_RESERVED_POS0_SIZE)

#if 0 // OS/2 PPC development temporarly on hold
static void AdjustGotEntry( TocEntry *e, offset *middle ) {
    if( e->pos < *middle ) {
        e->pos -= GOT_RESERVED_NEG_SIZE;
    } else {
        e->pos += GOT_RESERVED_POS0_SIZE;
    }
}
#endif

void PrepareToc( void )
/****************************/
{
    if( Toc == NULL )
        return;
    WalkHTable( Toc, ConvertTocEntry );
    RehashHTable( Toc );
#if 0
    if( (LinkState & HAVE_PPC_CODE) && (FmtData.type & MK_OS2) ) {
        // Development temporarly on hold
        offset middle = ( TocSize / 2 ) & ~0x3;
        WalkHTableCookie( Toc, AdjustGotEntry, &middle );
        TocShift = middle + GOT_RESERVED_NEG_SIZE;
        TocSize += GOT_RESERVED_SIZE;
    }
#endif
    if( TocSym != NULL )  {
        TocSym->info |= SYM_DCE_REF | SYM_DEFINED;
        SetAddPubSym( TocSym, SYM_REGULAR, FakeModule, 0, 0 );
        if( LinkFlags & STRIP_CODE ) {
            CleanStripInfo( TocSym );
        }
    }
}

void SetTocAddr( offset off, group_entry *group )
/******************************************************/
{
    if( Toc == NULL || TocSym == NULL )
        return;
    XDefSymAddr( TocSym, off + TocShift, group->grp_addr.seg );
}

static offset OffFromToc( offset off )
/***************************************/
{
    offset toff;

    toff = off - TocShift;
    if( (signed_16)toff != (signed_32)toff ) {
        LnkMsg( ERR+MSG_TOC_TOO_BIG, NULL );
    }
    return( toff );
}

offset FindEntryPosInToc( TocEntryId *e )
/*************************************************/
{
    TocEntry    searchEntry;
    TocEntry    *entry;

    searchEntry.e = *e;
    entry = FindHTableElem( Toc, &searchEntry );
    if( entry != NULL ) {
        return( OffFromToc( entry->pos ) );
    } else {
        return( BOGUS ); // return bogus position; likely to cause alignment exception
    }

}

offset FindSdataOffPosInToc( segdata *sdata, offset off )
/*****************************************************************/
{
    TocEntryId e;

    DbgAssert( sdata != NULL );
    e.sdata = sdata;
    e.u.off = off;
    return( FindEntryPosInToc( &e ) );
}

offset FindSymPosInToc( symbol * sym )
/**********************************************/
{
    TocEntryId e;

    e.sdata = NULL;
    e.u.sym = sym;
    ConvertTocEntryId( &e );
    return( FindEntryPosInToc( &e ) );
}

static void WriteOutTokElem( void *_elem, void *buf )
/***************************************************/
{
    TocEntry   *elem = _elem;
    offset      addr;
    segdata *   sdata;
    seg_leader *leader;

    if( elem->e.sdata ) {
        sdata = elem->e.sdata;
        leader = sdata->u.leader;
        addr = elem->e.u.off + sdata->a.delta + leader->group->linear
                        + leader->seg_addr.off +  FmtData.base;
    } else {
        addr = SymbolAbsAddr( elem->e.u.sym );
    }
    DbgAssert( elem->pos >= 0 );
    PutInfo( (*((virt_mem *)buf)) + elem->pos, &addr, sizeof( addr ) );
}

void WriteToc( virt_mem buf )
/**********************************/
{
    if( Toc == NULL )
        return;
    WalkHTableCookie( Toc, WriteOutTokElem, &buf );
#if 0
    if( (LinkState & HAVE_PPC_CODE) && (FmtData.type & MK_OS2) ) {
        // Development temporarly on hold
        offset res[GOT_RESERVED_SIZE / sizeof( offset )] = { 0 };
        enum { zero = GOT_RESERVED_NEG_SIZE / sizeof( offset ) };
        enum { blrl_opcode = 0x4E800021 };

        DbgAssert( TocShift >= GOT_RESERVED_NEG_SIZE );

        res[zero - 1] = blrl_opcode;
        res[zero] = IDataGroup->linear + FmtData.base;
        PutInfo( buf + TocShift - GOT_RESERVED_NEG_SIZE, res, GOT_RESERVED_SIZE );
    }
#endif
}
