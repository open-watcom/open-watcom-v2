/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2026      The Open Watcom Contributors. All Rights Reserved.
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
* Description:  routines for producing HLL debug information in the load file
*
****************************************************************************/

#include <string.h>
#include <stdint.h>
#include "linkstd.h"
#include "alloc.h"
#include "hll.h"
#include "virtmem.h"
#include "loadfile.h"
#include "objnode.h"
#include "objcalc.h"
#include "dbgcomm.h"
#include "ring.h"
#include "dbghll.h"
#include "specials.h"

// This is used to enable some extra debug checks and paddings.
#define HLL_DEBUG 1

// Undefine any of these to prevent those sections from being generated.
#define HLL_INC_LINE    1
#define HLL_INC_TYPE    1
#define HLL_INC_LOCAL   1
#define HLL_INC_PUBLIC  1


typedef struct hllmodinfo {
    unsigned_32 pubsize;
    /* used when writing segment info.
     * Initialized by HllAddModule. */
    virt_mem    segloc;
    /* Used for advancing from the first to the second seginfo
     * structure. The first seginfo structure is at the head
     * of the module record, while the rest follows the name. */
    virt_mem    next_segloc;
    unsigned_16 numlines;               /* number of lines */
    unsigned_16 numsegs;                /* number of segments */
    unsigned_16 modidx;                 /* the module index */
} hllmodinfo;


// global information needed to keep track of line number information

typedef struct {
    virt_mem            linestart;
    virt_mem            offbase;
    virt_mem            numbase;
    struct  {
        unsigned_32         start;
        unsigned_32         end;
    }                   range;
    unsigned_32         prevaddr;
    unsigned_16         seg;
    unsigned_8          needsort : 1;
} hll_lineinfo;

// split hll up into a number of different "sections" to keep track
// of where we write bits of the information.

enum {
    HLL_SECT_MODULE,
    HLL_SECT_MISC,
    HLL_SECT_DIRECTORY,
    NUM_HLL_SECTS
};


// The sizes of the sections hll we've defined above.
// Collected during the early link phases and used to calculate the
// actual size of the debug info and where to put things (SectOffsets).

static unsigned     SectSizes[NUM_HLL_SECTS];

// Where to write bits bits of information. This is as
// Calculated by HllAddrStart.

static virt_mem     SectOffsets[NUM_HLL_SECTS];

// The HLL information is just one big memory block. This is the start
// of it, and the length of it

static virt_mem     HllBase;
static unsigned     HllSize;
static hll_dirinfo  *HllDirHdr;

// The current module index.

static unsigned     ModIndex;

// something todo with line numbers?

static hll_lineinfo LineInfo;

static void     hllSrcModHeader( void );

extern void HllInit( void )
/************************/
// called just after command file parsing
{
    DEBUG(( DBG_HLL, "HllInit:" ));

    memset( SectSizes, 0, sizeof( SectSizes ) );
    memset( SectOffsets, 0, sizeof( SectOffsets ) );
    memset( &LineInfo, 0, sizeof( LineInfo ) );
    ModIndex = 0;
}

extern void HllInitModule( mod_entry *obj )
/****************************************/
// called before pass 1 is done on the module
{
    DEBUG(( DBG_HLL, "HllInitModule: obj=%p %s", obj, obj->name.u.ptr ));

    obj->u3.hll = _PermAlloc( sizeof( hllmodinfo ) );
    memset( obj->u3.hll, 0, sizeof( hllmodinfo ) );
}

static void hllDumpInfo( unsigned sect, void *data, unsigned len )
/****************************************************************/
{
    PutInfo( SectOffsets[sect], data, len );
    SectOffsets[sect] += len;
}

static segment hllGetSegment( seg_leader *seg )
/********************************************/
{
    segment         segm_index;
    group_entry     *group;

    if( ( seg != NULL )
      && ( seg->group != NULL ) ) {
        if( FmtData.type & (MK_REAL_MODE | MK_FLAT_OFFS | MK_ID_SPLIT) ) {
            segm_index = 1;
            for( group = Groups; group != NULL; group = group->next ) {
                if( group == seg->group )
                    return( segm_index );
                segm_index++;
            }
        } else {
            return( seg->seg_addr.seg );
        }
    }
    return( 0 );
}

static void hllAddSubSection( void )
/****************************************/
// Add a subsection to section directory.
{
    SectSizes[HLL_SECT_DIRECTORY] += sizeof( hll_dir_entry );
}

static void hllGenSubSection( hll_sst sect, unsigned size )
/*********************************************************/
// generate a subsection entry
{
    hll_dir_entry entry;

    entry.subsection = sect;
    entry.iMod = CurrMod ? CurrMod->u3.hll->modidx : 0;
    entry.cb = size;
    switch( sect ) {
    case hll_sstModules:
        entry.lfo = SectOffsets[HLL_SECT_MODULE] - HllBase;
        break;
    default:
        entry.lfo = SectOffsets[HLL_SECT_MISC] - HllBase;
        break;
    }
    DEBUG(( DBG_HLL, "hllGenSubSection: %h: sst=%h mod=%d cb=%h lfo=%h",
            SectOffsets[sect] - HllBase, sect, entry.iMod, size, entry.lfo ));
    hllDumpInfo( HLL_SECT_DIRECTORY, &entry, sizeof( entry ) );
}

extern void HllP1ModuleScanned( void )
/***********************************/
{
    DEBUG(( DBG_HLL, "HllP1ModuleScanned:" ));
}

#if 0 // FIXME: Implement support for non-HLL linenumbers.
static void hllAddLines( segdata * seg, void *line, unsigned size, bool is32bit )
/******************************************************************************/
// called during pass 1 linnum processing
{
    seg = seg;
    line = line;
    // CurrMod->u3.hll->numlines += CalcLineQty( size, is32bit );
    size = size;
    is32bit = is32bit;
}
#endif // FIXME: Implement support for non-HLL linenumbers.

extern void HllP1ModuleFinished( mod_entry *obj )
/************************************************
 * calculate size of the sstModule
 */
{
    byte    namelen;

    if( MOD_NOT_DEBUGGABLE( obj ) )
        return;
    CurrMod = obj;

    /* assign module index */
    obj->u3.hll->modidx = ++ModIndex;
    DEBUG(( DBG_HLL, "HllP1ModuleFinished: obj=%p %s modidx=%d offset=%h",
            obj, obj->name.u.ptr, ModIndex, SectSizes[HLL_SECT_MODULE] ));

    /* add the module subsection */
    namelen = strlen( obj->name.u.ptr );
    SectSizes[HLL_SECT_MODULE] += sizeof( hll_module ) + namelen;
    hllAddSubSection();

#ifdef HLL_INC_PUBLIC
    /* add public symbols subsection */
    Ring2Walk( obj->publist, DBIModGlobal );
    if( obj->u3.hll->pubsize > 0 ) {
        hllAddSubSection();
    }
#endif

#if 0 // FIXME: Implement support for non-HLL linenumbers.
    /* add linenumber subsection for old style linenumbers */
    if( obj->modinfo & DBI_LINE ) {
        DBILineWalk( obj->lines, hllAddLines );
    }

    if( obj->d.cv->numlines > 0 ) {
        unsigned    temp;

        hllAddSubSection();
        temp = sizeof( cheesy_module_header );
        temp += ROUND_UP( sizeof( cheesy_file_table ) + namelen, 4 );
        temp += sizeof( cheesy_mapping_table );
        temp += ROUND_UP( obj->d.cv->numlines
                    * ( sizeof( unsigned_32 ) + sizeof( unsigned_16 ) ), 4 );
        SectSizes[HLL_SECT_MISC] += temp;
    }
#endif
}

extern void HllAddModule( mod_entry *obj, section *sect )
/******************************************************/
// called just before publics have been assigned addresses between p1 & p2
// We generate the module entry and some other bits now.
{
    hll_module      mod;
    unsigned        size;
    byte            namelen;

    DEBUG(( DBG_HLL, "HllAddModule: obj=%p %s sect=%p implib=%d pubsize=%h",
            obj, obj->name.u.ptr, sect, obj->modinfo & MOD_IMPORT_LIB,
            obj->u3.hll->pubsize ));

    CurrMod = obj; // paranoia

    /* generate the module entry */
    namelen = strlen( obj->name.u.ptr );
    size = sizeof( mod ) + namelen;
    if( obj->u3.hll->numsegs ) {
        size += ( obj->u3.hll->numsegs - 1 ) * sizeof( hll_seginfo );
    }
    hllGenSubSection( hll_sstModules, size );

    obj->u3.hll->segloc = SectOffsets[HLL_SECT_MODULE];
    mod.SegInfo.Seg     = 0;        /* corrected later if any code */
    mod.SegInfo.offset  = 0;
    mod.SegInfo.cbSeg   = 0;
    mod.ovlNumber       = 0;
    mod.iLib            = 0; /// @todo add library table and indexes into it. (not important)
    mod.cSeg            = obj->u3.hll->numsegs;
    switch( obj->omfdbg ) {
    case OMF_DBG_HLL_03:
        mod.Version = 0x300;
        mod.Style   = HLL_DEBUG_STYLE_HL;
        break;
    case OMF_DBG_HLL_04:
        mod.Version = 0x400;
        mod.Style   = HLL_DEBUG_STYLE_HL;
        break;
    case OMF_DBG_HLL_06:
        mod.Version = 0x600;
        mod.Style   = HLL_DEBUG_STYLE_HL;
        break;
    default:
        mod.Version = 0;
        mod.Style   = 0;
        break;
    }
    mod.name_len = namelen;
    hllDumpInfo( HLL_SECT_MODULE, &mod, sizeof( mod ) );
    hllDumpInfo( HLL_SECT_MODULE, obj->name.u.ptr, mod.name_len );

    /* next comes extra segment info structures. reserve space for them. */
    obj->u3.hll->next_segloc = SectOffsets[HLL_SECT_MODULE];
    if( obj->u3.hll->numsegs ) {
        SectSizes[HLL_SECT_MODULE] += ( obj->u3.hll->numsegs - 1 ) * sizeof( hll_seginfo );
    }

#ifdef HLL_INC_PUBLIC
    /* publics (the callers caller will define public afterwards). */
    if( obj->u3.hll->pubsize > 0 ) {
        hllGenSubSection( hll_sstPublics, obj->u3.hll->pubsize );
    }
#endif
    (void)sect; // komh: make compiler happy

}

#if 0 // FIXME: Implement support for non-HLL linenumbers.
static int hllRelocCompare( virt_mem a, virt_mem b )
/***********************************************/
{
    unsigned_32 a32;
    unsigned_32 b32;

    GET32INFO( a, a32 );
    GET32INFO( b, b32 );
    return( (int)a32 - (int)b32 );
}

static void hllSwapRelocs( virt_mem a, virt_mem b )
/**********************************************/
{
    unsigned    diffa;
    unsigned    diffb;
    unsigned_16 a16;
    unsigned_16 b16;
    unsigned_32 a32;
    unsigned_32 b32;

    GET32INFO( a, a32 );
    GET32INFO( b, b32 );
    PUT32INFO( a, b32 );
    PUT32INFO( b, a32 );
    diffa = ( a - LineInfo.offbase ) / sizeof( unsigned_32 );
    diffa *= sizeof( unsigned_16 );
    diffa += LineInfo.numbase;
    diffb = ( b - LineInfo.offbase ) / sizeof( unsigned_32 );
    diffb *= sizeof( unsigned_16 );
    diffb += LineInfo.numbase;
    GET16INFO( diffa, a16 );
    GET16INFO( diffb, b16 );
    PUT16INFO( diffa, b16 );
    PUT16INFO( diffb, a16 );
}

static void hllSortRelocs( void )
/****************************/
{
    LineInfo.offbase -= CurrMod->u3.hll->numlines * sizeof( unsigned_32 );
    LineInfo.numbase -= CurrMod->u3.hll->numlines * sizeof( unsigned_16 );
    VMemQSort( LineInfo.offbase, CurrMod->u3.hll->numlines, sizeof( unsigned_32 ),
               hllSwapRelocs, hllRelocCompare );
}
#endif // FIXME: Implement support for non-HLL linenumbers.

extern void HllGenModule( void )
/*****************************/
// generate an sstSrcModule
{
    DEBUG(( DBG_HLL, "HllGenModule: CurrMod=%p %s", CurrMod, CurrMod->name));
#if 0 // FIXME
    if( LineInfo.needsort ) {
        hllSortRelocs();
    }
#endif
    hllSrcModHeader();
}

extern void HllAddLocal( seg_leader *seg, offset length )
/*******************************************************/
// called during pass 1 final segment processing.
{
#if defined(HLL_INC_LINE) || defined(HLL_INC_LOCAL) || defined(HLL_INC_TYPE)
    switch( seg->dbgtype) {
#ifdef HLL_INC_TYPE
    case MS_TYPE:
#endif
#ifdef HLL_INC_LOCAL
    case MS_LOCAL:
#endif
#ifdef HLL_INC_LINE
    case HLL_LINE:
#endif
        DEBUG(( DBG_HLL, "HllAddLocal: seg=%h length=%d", seg, length));
        hllAddSubSection();
        break;
    }
#endif
    (void)length; // komh: make compiler happy
}

extern void HllAddGlobal( symbol *sym )
/************************************/
// called during pass 1 symbol definition
{
#ifdef HLL_INC_PUBLIC
    if( (sym->info & SYM_STATIC) == 0 ) {
        unsigned size;

        size = (byte)strlen( sym->name.u.ptr ) + sizeof( hll_public );
        CurrMod->u3.hll->pubsize += size;
        SectSizes[HLL_SECT_MISC] += size;
        DEBUG(( DBG_HLL, "HllAddGlobal: size=%h sym=%p %s",
                size, sym, sym->name ));
    }
#endif
    sym = sym;
}

extern void HllGenGlobal( symbol * sym, section *sect )
/****************************************************/
// called during symbol address calculation (between pass 1 & pass 2)
// also called by loadpe between passes
{
#ifdef HLL_INC_PUBLIC
    if( (sym->info & SYM_STATIC) == 0 ) {
        hll_public  pub;

        DEBUG(( DBG_HLL, "HllGenGlobal: %h: sym=%p %s sect=%p",
                SectOffsets[HLL_SECT_MISC] - HllBase, sym, sym->name, sect ));

        pub.offset  = sym->addr.off;
        pub.seg     = hllGetSegment( sym->p.seg->u.leader );
        pub.type    = 0; /** @todo fix the public type index! */
        pub.name_len = strlen( sym->name.u.ptr );
        hllDumpInfo( HLL_SECT_MISC, &pub, sizeof( pub ) );
        hllDumpInfo( HLL_SECT_MISC, sym->name.u.ptr, pub.name_len );
    }
#endif
    sym = sym;
    sect = sect;
}

static void hllSrcModHeader( void )
/*********************************/
// emit header for line number information now that we know where everything
// is.
{
#if 0 // FIXME: Implement support for non-HLL linenumbers.
    cheesy_module_header    mod_hdr;
    cheesy_file_table       file_tbl;
    cheesy_mapping_table    map_tbl;
    unsigned                adjust;
    unsigned                buff;

    if( LineInfo.linestart == 0 )
        return;
    memset( &mod_hdr, 0, sizeof( mod_hdr ) );
    mod_hdr.cFile = 1;
    mod_hdr.cSeg = 1;
    mod_hdr.range[0] = LineInfo.range;
    mod_hdr.baseSrcFile[0] = sizeof( mod_hdr );
    mod_hdr.seg[0] = LineInfo.seg;
    mod_hdr.pad = 0;
    PutInfo( LineInfo.linestart, &mod_hdr, sizeof( mod_hdr ) );
    LineInfo.linestart += sizeof( mod_hdr );
    file_tbl.cSeg = 1;
    file_tbl.pad = 0;
    file_tbl.range[0] = LineInfo.range;
    file_tbl.name[0] = strlen( CurrMod->name );
    file_tbl.baseSrcLn[0] = sizeof( mod_hdr ) +
                    ROUND_UP( sizeof( file_tbl ) + file_tbl.name[0], 4 );
    PutInfo( LineInfo.linestart, &file_tbl, sizeof( file_tbl ) );
    LineInfo.linestart += sizeof( file_tbl );
    PutInfo( LineInfo.linestart, CurrMod->name, file_tbl.name[0] );
    LineInfo.linestart += file_tbl.name[0];
    adjust = file_tbl.baseSrcLn[0] - sizeof( mod_hdr ) - sizeof( file_tbl )
                - file_tbl.name[0];
    if( adjust != 0 ) {
        buff = 0;
        PutInfo( LineInfo.linestart, &buff, adjust );
        LineInfo.linestart += adjust;
    }
    map_tbl.Seg = mod_hdr.seg[0];
    map_tbl.cPair = CurrMod->u3.hll->numlines;
    PutInfo( LineInfo.linestart, &map_tbl, sizeof( map_tbl ) );
    memset( &LineInfo, 0, sizeof( LineInfo ) );
#endif // FIXME: Implement support for non-HLL linenumbers.
}

extern void HllGenLines( lineinfo *info )
/*******************************************************************************/
// called during pass 2 linnum processing
{
    // FIXME: Implement support for non-HLL linenumbers.
    segdata             *seg;
    bool                is32bit;
    unsigned            size;

    seg = info->seg;
    // lines = lines;
    size = info->size;
    is32bit = seg->bits == BITS_32;
    DEBUG(( DBG_HLL, "HllGenLines: info=%p seg=%p size=%d is32bit %d",
            info, seg, size, is32bit ));
    (void)is32bit;  // komh: make compiler happy
    (void)size;     // komh: make compiler happy
}

static void hllAddAddrInit( segdata *sdata, void *cookie )
/*******************************************************/
{
    sdata = sdata;
    cookie = cookie;
}

static void hllAddAddrAdd( segdata *sdata, offset delta, offset size,
                          void *cookie, bool isnewmod )
/******************************************************************/
{
    delta = delta;
    size = size;
    cookie = cookie;
    if( !isnewmod )
        return;

    sdata->o.mod->u3.hll->numsegs++;
}

extern void HllAddAddrInfo( seg_leader *seg )
/******************************************/
// We take this opportunity to count seginfo structures.
{
    if( (seg->info & SEGINF_CODE) == 0 )
        return;
    DEBUG(( DBG_HLL, "HllAddAddrInfo: seg=%p %s", seg, seg->segname ));
    DBIAddrInfoScan( seg, hllAddAddrInit, hllAddAddrAdd, NULL );
}

static void hllGenAddrInit( segdata *sdata, void *_info )
/******************************************************/
{
    hll_seginfo *info = _info;
    info->Seg = hllGetSegment( sdata->u.leader );
    info->offset = sdata->u.leader->seg_addr.off + sdata->a.delta;
}

static void hllGenAddrAdd( segdata *sdata, offset delta, offset size,
                          void *_info, bool isnewmod )
/******************************************************************/
{
    hll_seginfo *info = _info;
    if( !isnewmod )
        return;
    info->cbSeg = size;
    PutInfo( sdata->o.mod->u3.hll->segloc, info, sizeof( hll_seginfo ) );

    sdata->o.mod->u3.hll->segloc += sizeof( hll_seginfo );
    info->offset = sdata->u.leader->seg_addr.off + delta;
}

extern void HllGenAddrInfo( seg_leader *seg )
/******************************************/
// called for each section just before pass 2 starts
// Fill in seginfo.
{
    hll_seginfo info;

    if( (seg->info & SEGINF_CODE) == 0 )
        return;
    DEBUG(( DBG_HLL, "HllGenAddrInfo: seg=%p %s", seg, seg->segname ));
    DBIAddrInfoScan( seg, hllGenAddrInit, hllGenAddrAdd, &info );
}

extern void HllDefClass( class_entry *class, unsigned size )
/***********************************************************
 * called during address calculation for the purpose of catching
 * debug info segment classes.
 */
{
#if defined(HLL_INC_LINE) || defined(HLL_INC_LOCAL) || defined(HLL_INC_TYPE)
    group_entry *group;
    DEBUG(( DBG_HLL, "HllDefClass: class=%p flags=%h size=%h %s", class, class->flags, size, class->name ));

    switch( class->flags & CLASS_DEBUG_INFO ) {
#ifdef HLL_INC_TYPE
    case CLASS_MS_TYPE:
#endif
#ifdef HLL_INC_LOCAL
    case CLASS_MS_LOCAL:
#endif
#ifdef HLL_INC_LINE
    case CLASS_HLL_LINE:
#endif
        /* add the class to the DBI groups (we'll traverse them later) */
        group = AllocGroup( AutoGrpName, &DBIGroups );
        group->g.class = class;
        group->addr.seg = 0;

        DEBUG(( DBG_HLL, "HllDefClass: class=%p %s group=%p flags=%h "
                "offset=%h size=%h", class, class->name, group,
                class->flags, SectSizes[HLL_SECT_MISC], size ));
        SectSizes[HLL_SECT_MISC] += size;
        break;
    }
#endif
}

static void hllDefSegData( void *_sdata )
/**********************************/
// Creates a Type or Symbol subsection for a module and copies the data into it.
/* NOTE: this assumes that codeview segments are byte aligned! */
{
    segdata    *sdata = _sdata;
    seg_leader *leader = sdata->u.leader;

    DEBUG(( DBG_HLL, "hllDefSegData: iMod=%d length=%h align=2**%d isdead=%d dbgtype=%h",
            sdata->o.mod->u3.hll->modidx, sdata->length, sdata->align, sdata->isdead, leader->dbgtype ));

    if( !sdata->isdead ) {
        hll_sst sect;

        switch ( leader->dbgtype ) {
#ifdef HLL_INC_TYPE
        case MS_TYPE:   sect = hll_sstTypes;   break;
#endif
#ifdef HLL_INC_LOCAL
        case MS_LOCAL:  sect = hll_sstSymbols; break;
#endif
#ifdef HLL_INC_LINE
        case HLL_LINE:  sect = hll_sstHLLSrc;  break;
#endif
        default:
            return;
        }
#if defined(HLL_INC_LINE) || defined(HLL_INC_LOCAL) || defined(HLL_INC_TYPE)
        CurrMod = sdata->o.mod;
        DEBUG(( DBG_HLL, "hllDefSegData: %h: iMod=%d sect=%d length=%h align=2**%d %s",
                SectOffsets[HLL_SECT_MISC] - HllBase, CurrMod->u3.hll->modidx, sect, sdata->length,
                sdata->align, CurrMod->name ));

        hllGenSubSection( sect, sdata->length );
        CopyInfo( SectOffsets[HLL_SECT_MISC], sdata->u1.vm_ptr, sdata->length );
        sdata->u1.vm_ptr = SectOffsets[HLL_SECT_MISC];   // FIXME: inefficient
        SectOffsets[HLL_SECT_MISC] += sdata->length;
#endif
    }
}

static bool hllDefLeader( void *_leader, void *group )
/*************************************************/
{
    seg_leader *leader = _leader;
    DEBUG((DBG_HLL, "hllDefLeader: leader=%p %s", leader, leader->segname ));

    leader->group = group;
    RingWalk( leader->pieces, hllDefSegData );
    return( false );
}

extern void HllAddrStart( void )
/*****************************/
// called after address calculation is done.
{
    hll_dirinfo         dirhdr;
    hll_trailer         start;
    virt_mem            currpos;
    int                 index;
    group_entry        *group;
    DEBUG((DBG_HLL, "HllAddrStart" ));

    /* calc the debug info size */
    SectSizes[HLL_SECT_DIRECTORY] += sizeof( dirhdr );
    HllSize = 2 * sizeof( start );
    for( index = 0; index < NUM_HLL_SECTS; index++ ) {
        HllSize += SectSizes[index];
    }

    /* Allocate memory for the debug info. */
    HllBase = DBIAlloc( HllSize );

    /* calc the position of the section. */
    currpos = HllBase + sizeof( start );
    for( index = 0; index < NUM_HLL_SECTS; index++ ) {
        DEBUG (( DBG_HLL, "HllAddrStart: %d: off=%h size=%h",
                 index, currpos - HllBase, SectSizes[index] ));
        SectOffsets[index] = currpos;
        currpos += SectSizes[index];
#ifdef HLL_DEBUG
        memset( (void *)SectOffsets[index], '0' + index, SectSizes[index] ); /* fill */
#endif
    }

    /* write the NB04 signatures */
    memcpy( start.sig, HLL_NB04, sizeof( start.sig ) );
    start.offset = SectOffsets[HLL_SECT_DIRECTORY] - HllBase;
    PutInfo( HllBase, &start, sizeof( start ) );
    start.offset = HllSize;
    PutInfo( HllBase + HllSize - sizeof( start ), &start, sizeof( start ) );

    /* write the subsection directory header (also called directory info) */
    HllDirHdr = (hll_dirinfo *)SectOffsets[HLL_SECT_DIRECTORY];
    dirhdr.cbDirHeader = sizeof( dirhdr );
    dirhdr.cbDirEntry = sizeof( hll_dir_entry );
    dirhdr.cDir = (SectSizes[HLL_SECT_DIRECTORY] - sizeof( dirhdr )) / sizeof( hll_dir_entry );
    hllDumpInfo( HLL_SECT_DIRECTORY, &dirhdr, sizeof( dirhdr ) );

    /* Walk the debug info groups and dump the data in those segments. */
    for( group = DBIGroups; group != NULL; group = group->next ) {
        RingLookup( group->g.class->segs, hllDefLeader, group );
        group->g.class = NULL;
    }
}

extern void HllFini( section *sect )
/*********************************/
// called after pass 2 is finished, but before load file generation
{
    DEBUG(( DBG_HLL, "HllFini: sect=%p", sect ));
    (void)sect; // komh: make compiler happy
}

extern void HllWrite( void )
/****************************/
// called during load file generation.  It is assumed that the loadfile is
// positioned to the right spot.
{
    unsigned        i, j;
    hll_dir_entry  *dir_entries;

    DEBUG(( DBG_HLL, "HllWrite: HllBase=%h HllSize=%h", HllBase, HllSize ));

    /* before writing it sort the directory by module. */
    dir_entries = (hll_dir_entry *)(HllDirHdr + 1);
    for( i = 0; i < HllDirHdr->cDir; i++ ) {
        for( j = i + 1; j < HllDirHdr->cDir; j++ ) {
            if( dir_entries[j].iMod < dir_entries[i].iMod
              || ( dir_entries[j].iMod == dir_entries[i].iMod
              && dir_entries[j].subsection < dir_entries[i].subsection ) ) {
                hll_dir_entry tmp = dir_entries[j];
                dir_entries[j] = dir_entries[i];
                dir_entries[i] = tmp;
            }
        }
    }

    /* write the debug section */
    WriteInfoLoad( HllBase, HllSize );
}
