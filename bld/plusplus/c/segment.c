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


#include "plusplus.h"

#include "compcfg.h"
#include "tgtenv.h"

#include "cgfront.h"
#include "cgback.h"
#include "cgdata.h"
#include "codegen.h"
#include "ring.h"
#include "errdefns.h"
#include "vbuf.h"
#include "memmgr.h"
#include "pragdefn.h"
#include "cginfo.h"
#include "pcheader.h"
#include "dbgsupp.h"
#include "initdefs.h"


typedef struct pc_segment PC_SEGMENT;
PCH_struct pc_segment {                 // PC_SEGMENT -- segment on PC
    PC_SEGMENT      *next;              // - next in ring
    PC_SEGMENT      *sibling;           // - related segment
    char            *class_name;        // - class name
    SYMBOL          label;              // - symbol for label in segment
    unsigned        attrs;              // - attributes
    fe_seg_id       seg_id;             // - id for segment
    target_offset_t align;              // - alignment
    target_size_t   offset;             // - offset within segment
#if _INTEL_CPU
    hw_reg_set      binding;            // - register bound to the segment
#endif
                                        // - segment:
    unsigned        dgroup : 1;         // - is part of DGROUP
    unsigned        lab_gened : 1;      // - label has been generated
    unsigned        used : 1;           // - has been used
    unsigned        fixed_alignment : 1;// - has a fixed alignment (no changes)
    unsigned        cg_defed : 1;       // - defined to code generator
    unsigned        module_prefix : 1;  // - name has ModuleName as a prefix
    unsigned        has_data : 1;       // - has data gened in segment
    unsigned        only_strings : 1;   // - only strings go in segment
    char            name[1];            // - name
};

typedef struct {                        // DEF_SEG -- code/data default segments
    PC_SEGMENT      *pcseg;             // - default pc segment
    unsigned        ctr;                // - # of segments allocated
    unsigned        ds_used : 1;        // - TRUE ==> has been used
} DEF_SEG;

static fe_seg_id seg_max;           // last segment # used
static fe_seg_id seg_import;        // next import segment #
#if _INTEL_CPU
static fe_seg_id seg_code_comdat;   // segment # for code comdat
#endif
static fe_seg_id seg_default_code;  // segment # for default code segment
static target_size_t dgroup_size;   // amount of DGROUP used so far
static PC_SEGMENT *seg_list;        // list of defined segments
static DEF_SEG code_def_seg;        // code segment -- default info.
static DEF_SEG data_def_seg;        // data segment -- default info.

static struct {
    unsigned in_back_end : 1;       // TRUE ==> now in CGBKMAIN
    unsigned use_def_seg : 1;       // TRUE ==> #pragma def_seg active
} flags;

enum                                // SEGMENT-ATTRIBUTE COMBINATIONS USED:
{   SGAT_CODE_BASED                 // - code: based
        = GIVEN_NAME | INIT | GLOBAL | EXEC
,   SGAT_CODE_GEN                   // - code: being generated
        = GIVEN_NAME | INIT | GLOBAL | EXEC
,   SGAT_DATA_BASED                 // - data: based
        = GIVEN_NAME | GLOBAL | INIT | PRIVATE
,   SGAT_STACK                      // - data: stack
        = GIVEN_NAME | GLOBAL
,   SGAT_DATA_CON1                  // - data: constant(1)
        = GIVEN_NAME | BACK | ROM | INIT
,   SGAT_DATA_CON2                  // - data: constant(2)
        = GIVEN_NAME | ROM | INIT
,   SGAT_DATA_RW                    // - data: read/write initialized
        = GIVEN_NAME | GLOBAL | INIT
,   SGAT_CPP                        // - used for common code
        = GIVEN_NAME | GLOBAL | INIT  | EXEC
,   SGAT_BSS                        // - data: read/write uninitialized
        = GIVEN_NAME | GLOBAL
,   SGAT_DATA_PRIVATE_RW            // - data: private read/write
        = GIVEN_NAME | PRIVATE | INIT
,   SGAT_DATA_PRIVATE_RO            // - data: private read-only
        = GIVEN_NAME | PRIVATE | INIT | ROM
,   SGAT_DATA_COMMON_INIT           // - data: common, init'ed, R/W
        = GIVEN_NAME | GLOBAL | INIT | COMMON | COMDAT
,   SGAT_DATA_COMMON_INIT_ROM       // - data: common, init'ed, R/O
        = GIVEN_NAME | GLOBAL | INIT | COMMON | COMDAT | ROM
,   SGAT_DATA_COMMON_ZERO           // - data: common, uninitialized
        = GIVEN_NAME | GLOBAL | COMMON | COMDAT
,   SGAT_TLS_LIMIT                  // - thread-local storage delimiter seg
        = GIVEN_NAME | ROM | INIT | THREAD_LOCAL
,   SGAT_TLS_DATA                   // - thread-local data
        = GIVEN_NAME | GLOBAL | INIT | THREAD_LOCAL
};

enum {
    SA_IN_DGROUP        = 0x01,
    SA_MODULE_PREFIX    = 0x02,
    SA_DEFINE_ANYTIME   = 0x04,
    SA_NULL             = 0x00
};

enum {
    ADS_MODULE_PREFIX   = 0x01,
    ADS_CONST_SEGMENT   = 0x02,
    ADS_STRING_SEGMENT  = 0x04,
    ADS_CODE_SEGMENT    = 0x08,
    ADS_ZM_SEGMENT      = 0x10,
    ADS_NULL            = 0x00
};

#define _markUsed( s, v ) \
        {   PC_SEGMENT *t = s, *p; \
            t->used = v; \
            for( p = t->sibling; p != t; p = p->sibling ) { \
                p->used = v; \
            } \
        }

static void addSibling( PC_SEGMENT *seg, PC_SEGMENT *sib )
{
    DbgAssert( sib->sibling == sib );
    sib->sibling = seg->sibling;
    seg->sibling = sib;
}

static SYMBOL segEmitLabel(         // EMIT SEGMENT LABEL
    PC_SEGMENT* seg )               // - current segment
{
    SYMBOL label;                   // - label in segment

    label = seg->label;
    if( label != NULL && ! seg->lab_gened ) {
        if( seg->seg_id == SEG_STACK ) {
            CGAutoDecl( (cg_sym_handle)label, TY_UINT_1 );
        } else {
            CgBackGenLabel( label );
        }
        seg->lab_gened = TRUE;
        _markUsed( seg, TRUE );
    }
    return label;
}


static void checkSegmentOverflow(   // CHECK FOR SEGMENTS OVERFLOW
    void )
{
    if( seg_max < 0 || seg_import >= 0 ) {
        CFatal( "Too many segments -- sub-divide source module" );
    }
}


static PC_SEGMENT *segIdLookup( // LOOKUP SEGMENT FOR ID
    fe_seg_id seg_id )          // - segment id
{
    PC_SEGMENT* curr;           // - current segment
    PC_SEGMENT* retn;           // - segment for id

    retn = NULL;
    RingIterBeg( seg_list, curr ) {
        if( curr->seg_id == seg_id ) {
            retn = curr;
            break;
        }
    } RingIterEnd( curr );
    return retn;
}


#if _CPU == _AXP || COMP_CFG_COFF == 1
static fe_seg_id markSegmentComdat(  // MARK SEGMENT AS COMDAT SEGMENT
    fe_seg_id seg_id )          // - segment id
{
    PC_SEGMENT* seg = segIdLookup( seg_id );
    DbgVerify( NULL != seg, "markSegmentComdat -- no segment" );
    seg->attrs |= COMDAT;
    return( seg_id );
}
#endif


static PC_SEGMENT *segNameLookup( // LOOKUP SEGMENT FOR NAME
    char *name )                // - segment name
{
    PC_SEGMENT* curr;           // - current segment
    PC_SEGMENT* retn;           // - segment for id

    retn = NULL;
    RingIterBeg( seg_list, curr ) {
        if( 0 == strcmp( curr->name, name ) ) {
            retn = curr;
            break;
        }
    } RingIterEnd( curr );
    return retn;
}


static void segmentCgDefine(    // DEFINE A SEGMENT
    PC_SEGMENT *segment )       // - current segment
{
    if( ! segment->cg_defed ) {
        BEDefSeg( segment->seg_id
                , segment->attrs
                , segment->name
                , segment->align );
        segment->cg_defed = TRUE;
    }
}


static PC_SEGMENT *segmentAlloc(    // SEGMENT: ALLOCATE NEW SEGMENT
    const char *seg_name,           // - segment name
    const char *class_name,         // - segment class name
    fe_seg_id seg_id,               // - segment id (if not SEG_NULL)
    unsigned attrs,                 // - segment attributes
    unsigned control )              // - control mask
{
    PC_SEGMENT *curr;               // - segment pointer
    size_t size;                    // - size of segment name

#if _INTEL_CPU && COMP_CFG_COFF == 0
//alpha permits segments defined anywhere and we need it for comdat data
//ditto for COFF
    DbgVerify( ! flags.in_back_end || ( attrs & PRIVATE ) || ( control & SA_DEFINE_ANYTIME )
             , "segmentAlloc -- defining in back end" );
#endif
    size = strlen( seg_name );
    curr = RingAlloc( &seg_list, sizeof( PC_SEGMENT ) + size );
    curr->sibling = curr;
    stvcpy( curr->name, seg_name, size );
    curr->offset = 0;
    curr->dgroup = (( control & SA_IN_DGROUP ) != 0 );
    curr->lab_gened = FALSE;
    if( class_name != NULL ) {
        curr->class_name = strpermsave( class_name );
    } else {
        curr->class_name = NULL;
    }
    curr->used = FALSE;
    curr->module_prefix = (( control & SA_MODULE_PREFIX ) != 0 );
    curr->fixed_alignment = FALSE;
    curr->cg_defed = FALSE;
    curr->has_data = FALSE;
    curr->only_strings = FALSE;
    curr->label = NULL;
    curr->attrs = attrs;
#if _INTEL_CPU
    HW_CAsgn( curr->binding, HW_EMPTY );
#endif
    if( seg_id == SEG_NULL ) {
        seg_id = ++seg_max;
        checkSegmentOverflow();
    } else {
        if( seg_id > seg_max ) {
            seg_max = seg_id;
            checkSegmentOverflow();
        }
    }
    curr->seg_id = seg_id;
    switch( seg_id ) {
    case SEG_PROF_BEG:
    case SEG_PROF_REF:
    case SEG_PROF_END:
        // we don't want padding introduced
        curr->align = TARGET_LONG;
        // we don't want alignment changed either
        curr->fixed_alignment = TRUE;
        break;
    case SEG_INIT_BEG:
    case SEG_INIT_REF:
    case SEG_INIT_END:
    case SEG_FINI_BEG:
    case SEG_FINI_REF:
    case SEG_FINI_END:
        // we don't want padding introduced
#if _INTEL_CPU
        curr->align = TARGET_SHORT;
#elif _CPU == _AXP
        curr->align = TARGET_POINTER;
#else
#error no alignment set
#endif
        // we don't want alignment changed either
        curr->fixed_alignment = TRUE;
        break;
    default:
        curr->align = TARGET_CHAR;
        if( flags.in_back_end ) {
            segmentCgDefine( curr );
        }
        break;
    }
    return( curr );
}


static void initDefSeg(         // SET NEW DEFAULT SEGMENT INFO
    DEF_SEG *def_seg,           // - default segment info.
    PC_SEGMENT *pcseg )         // - default segment
{
    if( def_seg->pcseg != pcseg ) {
        def_seg->pcseg = pcseg;
        def_seg->ctr = 0;
        def_seg->ds_used = FALSE;
    }
}


static void pruneDefSeg(        // PRUNE A DEFAULT SEGMENT IF NOT USED
    DEF_SEG *def_seg )          // - default segment info.
{
    PC_SEGMENT *pcseg;          // - pc segment for default

    if( ! def_seg->ds_used ) {
        pcseg = def_seg->pcseg;
        if( pcseg != NULL && ! pcseg->used ) {
            RingDealloc( &seg_list, pcseg );
        }
    }
}


static PC_SEGMENT *addDefSeg(   // ADD A DEFAULT PC SEGMENT
    DEF_SEG *def_seg,           // - default segment info.
    unsigned ads_control )      // - control mask
{
    unsigned attrs;             // - attributes for segment
    PC_SEGMENT *curr;           // - segment pointer
    VBUF seg_name;              // - virtual buffer for name
    unsigned sa_control;        // - segmentAlloc control mask

    VbufInit( &seg_name );
    ++def_seg->ctr;
    sa_control = SA_NULL;
    if( ads_control & ADS_MODULE_PREFIX ) {
        if(( ads_control & ADS_CODE_SEGMENT ) == 0 && DataSegName[0] != '\0' ) {
            VbufConcStr( &seg_name, DataSegName );
        } else {
            VbufConcStr( &seg_name, ModuleName );
        }
        sa_control |= SA_MODULE_PREFIX;
    }
    if( ads_control & ADS_STRING_SEGMENT ) {
        sa_control |= SA_DEFINE_ANYTIME;
    }
    VbufConcStr( &seg_name, def_seg->pcseg->name );
    if( ads_control & ADS_ZM_SEGMENT ) {
        VbufConcDecimal( &seg_name, def_seg->ctr );
    }
    if( def_seg == &code_def_seg ) {
        attrs = SGAT_CODE_GEN;
    } else {
        if( ads_control & ADS_CONST_SEGMENT ) {
            attrs = SGAT_DATA_PRIVATE_RO;
        } else {
            attrs = SGAT_DATA_PRIVATE_RW;
        }
        VbufConcDecimal( &seg_name, def_seg->ctr );
    }
    curr = segmentAlloc( VbufString( &seg_name ), NULL, SEG_NULL, attrs, sa_control );
    if( 0 == ( attrs & EXEC ) ) {
        _markUsed( curr, TRUE );
    }
    if( ads_control & ADS_STRING_SEGMENT ) {
        curr->only_strings = TRUE;
    }
    VbufFree( &seg_name );
    return( curr );
}


static
target_size_t segmentTypeSize(  // SEGMENT: SIZE OF TYPE
    TYPE type )                 // - type to be sized
{
    target_size_t size;

    size = CgMemorySize( type );
    if( size == 0 ) {
        size = TARGET_CHAR;
    }
    return( size );
}


target_offset_t SegmentAlignment(   // SEGMENT: ALIGNMENT FOR SYMBOL
    SYMBOL sym )                    // - symbol to align
{
    target_offset_t align;
    TYPE type;
    TYPE align_type;

    if( CompFlags.dont_align_segs ) {
        return( TARGET_CHAR );
    }
#if _CPU == _AXP
    if( PackAmount != TARGET_CHAR ) {
#else
    if( OptSize <= 50 || PackAmount != TARGET_CHAR ) {
#endif
        type = sym->sym_type;
        align_type = AlignmentType( type );
        align = segmentTypeSize( align_type );
        if( align == TARGET_CHAR ) {
            // no alignment; let PackAlignment know the real size
            align = segmentTypeSize( type );
        }
        align = PackAlignment( TARGET_MAX_PACKING, align );
    } else {
        align = TARGET_CHAR;
    }
    return( align );
}


target_offset_t SegmentAdjust(  // SEGMENT: ADJUST OFFSET TO ALIGN
    fe_seg_id segid,            // - segment identifier
    target_size_t offset,       // - current offset
    target_offset_t align )     // - required aligment
{
    target_size_t   calc_offset;
    target_offset_t adjust;

    switch( segid ) {
    case SEG_INIT_BEG:
    case SEG_INIT_REF:
    case SEG_INIT_END:
    case SEG_FINI_BEG:
    case SEG_FINI_REF:
    case SEG_FINI_END:
        /* no padding in these segments */
        return( 0 );
    }
    calc_offset = offset;
    calc_offset += align - 1;
    calc_offset &= ~(((target_size_t) align ) - 1 );
    adjust = calc_offset - offset;
    _CHECK_ADJUST( adjust, calc_offset, offset );
    return( adjust );
}


struct seg_look {                           // used to lookup segments
    const char      *seg_name;              // - segment name
    const char      *class_name;            // - segment class name
    unsigned        attrs;                  // - attributes
    fe_seg_id       seg_id;                 // - id for segment
    target_offset_t align;                  // - segment alignment
    target_size_t   sym_size;               // - space needed for symbol
    target_size_t   sym_align;              // - alignment needed for symbol
    unsigned        use_seg_id : 1;         // - for lookup
    unsigned        use_attrs : 1;          // - for lookup
    unsigned        use_align : 1;          // - for lookup
    unsigned        use_sym_size_align : 1; // - for lookup
    unsigned        use_name : 1;           // - for lookup
    unsigned        use_only_strings : 1;   // - for lookup
};


static boolean same_segment(    // DETERMINE IF SAME SEGMENT
    void * _curr,           // - current segment
    const void * _lk )  // - segment lookup structure
{
    PC_SEGMENT *curr = _curr;
    const struct seg_look* lk = _lk;

    target_offset_t     align_adjust;
    target_size_t       new_offset;

    if( lk->use_seg_id && lk->seg_id != SEG_NULL && curr->seg_id != lk->seg_id ) {
        return( FALSE );
    }
    if( lk->use_attrs && curr->attrs != lk->attrs ) {
        return( FALSE );
    }
    if( lk->use_align && curr->align != lk->align ) {
        return( FALSE );
    }
    if( lk->use_sym_size_align ) {
        align_adjust = SegmentAdjust( curr->seg_id, curr->offset, lk->sym_align );
        new_offset = curr->offset + align_adjust + lk->sym_size;
        _CHECK_ADJUST( new_offset, new_offset, curr->offset );
        if( new_offset == 0 ) {
            return( FALSE );
        }
    }
    if( lk->use_name ) {
        if( strcmp( curr->name, lk->seg_name ) != 0 ) {
            return( FALSE );
        }
        if( curr->class_name != NULL ) {
            if( lk->class_name == NULL ) {
                return( FALSE );
            }
            if( strcmp( curr->class_name, lk->class_name ) != 0 ) {
                return( FALSE );
            }
        } else {
            if( lk->class_name != NULL ) {
                return( FALSE );
            }
        }
    }
    if( lk->use_only_strings && !curr->only_strings ) {
        return( FALSE );
    }
    return( TRUE );
}


static PC_SEGMENT *segmentDefine(// SEGMENT: DEFINE IF REQUIRED
    const char *seg_name,       // - segment name
    const char *class_name,     // - segment class name
    fe_seg_id seg_id,           // - segment id (if not SEG_NULL)
    unsigned attrs,             // - segment attributes
    unsigned control )          // - segmentAlloc control mask
{
    PC_SEGMENT *curr;           // - current segment
    struct seg_look lk;         // - look-up structure
#if _INTEL_CPU
    const char* pc_reg;         // - scans register bound to segment
    char pc_reg_name[8];        // - name of pc register

    for( pc_reg = seg_name; ; ++pc_reg ) {
        if( *pc_reg == '\0' ) {
            pc_reg_name[0] = '\0';
            break;
        }
        if( *pc_reg == ':' ) {
            stvcpy( pc_reg_name, seg_name, pc_reg - seg_name );
            seg_name = pc_reg + 1;
            break;
        }
    }
#endif
    lk.seg_id = seg_id;
    lk.use_seg_id = TRUE;
    lk.attrs = attrs;
    lk.use_attrs = TRUE;
    lk.use_align = FALSE;
    lk.use_sym_size_align = FALSE;
    lk.seg_name = seg_name;
    lk.class_name = class_name;
    lk.use_name = TRUE;
    lk.use_only_strings = FALSE;
    curr = RingLookup( seg_list, &same_segment, &lk );
    if( curr == NULL ) {
        curr = segmentAlloc( lk.seg_name, lk.class_name, lk.seg_id, lk.attrs, control );
#if _INTEL_CPU
        if( pc_reg_name[0] != '\0' ) {
            curr->binding = PragRegName( pc_reg_name );
        }
#endif
    }
    return curr;
}


#if _CPU == 8086
static fe_seg_id createHugeSegment( target_size_t size, unsigned ads_control )
{
    PC_SEGMENT *curr;
    fe_seg_id id;
    target_size_t used;

    id = SEG_NULL;
    while( size > 0 ) {
        curr = addDefSeg( &data_def_seg, ads_control );
        if( id == SEG_NULL ) {
            id = curr->seg_id;
        }
        used = min( size, TARGET_UINT_MAX+1 );
        curr->offset = used;
        curr->align = 16;
        _markUsed( curr, TRUE );
        curr->has_data = TRUE;
        size -= used;
    }
    return( id );
}


fe_seg_id SegmentAddHuge(       // SEGMENT: ADD SYMBOL TO HUGE SEGMENT
    target_size_t size )        // - size of symbol
{
    return( createHugeSegment( size, ADS_MODULE_PREFIX ) );
}


fe_seg_id SegmentAddConstHuge(  // SEGMENT: ADD CONST SYMBOL TO HUGE SEGMENT
    target_size_t size )        // - size of symbol
{
    return( createHugeSegment( size, ADS_MODULE_PREFIX | ADS_CONST_SEGMENT ) );
}
#else
fe_seg_id SegmentAddHuge(       // SEGMENT: ADD SYMBOL TO HUGE SEGMENT
    target_size_t size )        // - size of symbol
{
    return( SegmentAddFar( size, 16 ) );
}


fe_seg_id SegmentAddConstHuge(  // SEGMENT: ADD CONST SYMBOL TO HUGE SEGMENT
    target_size_t size )        // - size of symbol
{
    return( SegmentAddConstFar( size, 16 ) );
}
#endif


static void accumAlignment( PC_SEGMENT *curr, target_offset_t align )
{
    if( ! curr->fixed_alignment ) {
        curr->align = max( curr->align, align );
    }
}

static fe_seg_id findFarSegment(// SEGMENT: ADD SYMBOL TO FAR SEGMENT
    target_size_t size,         // - size of symbol
    target_offset_t align,      // - alignment of symbol
    unsigned ads_control )      // - addDefSeg control word
{
    PC_SEGMENT *curr;           // - new segment
    struct seg_look lk;         // - look-up structure

    lk.use_seg_id = FALSE;
    if( ads_control & ADS_CODE_SEGMENT ) {
        lk.attrs = SGAT_CODE_BASED;
    } else if( ads_control & ADS_CONST_SEGMENT ) {
        lk.attrs = SGAT_DATA_PRIVATE_RO;
    } else {
        lk.attrs = SGAT_DATA_PRIVATE_RW;
    }
    lk.use_attrs = TRUE;
    lk.use_align = FALSE;
    lk.sym_size = size;
    lk.sym_align = align;
    lk.use_sym_size_align = TRUE;
    lk.use_name = FALSE;
    lk.use_only_strings = FALSE;
    if( ads_control & ADS_STRING_SEGMENT ) {
        lk.use_only_strings = TRUE;
    }
    curr = RingLookup( seg_list, &same_segment, &lk );
    if( curr == NULL ) {
        if( ads_control & ADS_CODE_SEGMENT ) {
            curr = addDefSeg( &code_def_seg, ads_control );
            code_def_seg.ds_used = TRUE;
        } else {
            curr = addDefSeg( &data_def_seg, ads_control );
            data_def_seg.ds_used = TRUE;
        }
    }
    curr->offset += SegmentAdjust( curr->seg_id, curr->offset, align );
    curr->offset += size;
    accumAlignment( curr, align );
    _markUsed( curr, TRUE );
    curr->has_data = TRUE;
    return( curr->seg_id );
}

fe_seg_id SegmentAddFar(        // SEGMENT: ADD SYMBOL TO FAR SEGMENT
    target_size_t size,         // - size of symbol
    target_offset_t align )     // - alignment of symbol
{
    return( findFarSegment( size, align, ADS_MODULE_PREFIX ) );
}


fe_seg_id SegmentAddConstFar(   // SEGMENT: ADD CONST SYMBOL TO FAR SEGMENT
    target_size_t size,         // - size of symbol
    target_offset_t align )     // - alignment of symbol
{
    return( findFarSegment( size, align, ADS_MODULE_PREFIX | ADS_CONST_SEGMENT ) );
}


fe_seg_id SegmentAddStringConstFar(// SEGMENT: ADD CONST STRING TO FAR SEGMENT
    target_size_t size,         // - size of symbol
    target_offset_t align )     // - alignment of symbol
{
    return( findFarSegment( size, align,
                            ADS_MODULE_PREFIX |
                            ADS_CONST_SEGMENT |
                            ADS_STRING_SEGMENT ) );
}


fe_seg_id SegmentAddStringCodeFar(// SEGMENT: ADD CONST STRING TO CODE SEGMENT
    target_size_t size,         // - size of symbol
    target_offset_t align )     // - alignment of symbol
{
    return( findFarSegment( size, align,
                            ADS_CODE_SEGMENT |
                            ADS_STRING_SEGMENT ) );
}


fe_seg_id SegmentAddSym(        // SEGMENT: ADD SYMBOL TO SPECIFIED SEGMENT
    SYMBOL sym,                 // - sym to add
    fe_seg_id id,               // - id of segment to use
    target_size_t size,         // - size of sym
    target_offset_t align )     // - alignment for sym
{
    PC_SEGMENT *curr;           // - new segment
    target_size_t aligned_offset;
    target_size_t calc_offset;
    target_size_t total_size;

    if( id == SEG_DATA || ( id == SEG_BSS && flags.use_def_seg ) ) {
        curr = data_def_seg.pcseg;
        id = curr->seg_id;
    } else if( id == SEG_CODE ) {
        curr = code_def_seg.pcseg;
        id = curr->seg_id;
    } else {
        curr = segIdLookup( id );
    }
    if( curr == NULL ) {
        CFatal( "segment: cannot find default segment" );
    } else {
        accumAlignment( curr, align );
        if( ( ! SymIsInitialized( sym ) ) && SymIsExtern( sym ) ) {
            id = curr->seg_id;
            _markUsed( curr, TRUE );
            curr->has_data = TRUE;
            data_def_seg.ds_used = TRUE;
        } else {
            aligned_offset = SegmentAdjust( curr->seg_id, curr->offset, align );
            calc_offset = curr->offset + aligned_offset + size;
            _CHECK_ADJUST( calc_offset, calc_offset, curr->offset );
            if( calc_offset == 0 ) {
                if( size != 0 ) {
                    CErr( ERR_MAX_SEGMENT_EXCEEDED, curr->name, sym );
                }
                id = SEG_NULL;
            } else if( curr->dgroup ) {
                total_size = dgroup_size + size + aligned_offset;
                _CHECK_ADJUST( calc_offset, total_size, dgroup_size );
                if( calc_offset == 0 ) {
                    if( size != 0 ) {
                        CErr( ERR_MAX_DGROUP_EXCEEDED, sym, curr->name );
                    }
                    id = SEG_NULL;
                } else {
                    dgroup_size += size + aligned_offset;
                    curr->offset = calc_offset;
                    _markUsed( curr, TRUE );
                    curr->has_data = TRUE;
                    id = curr->seg_id;
                    data_def_seg.ds_used = TRUE;
                }
            } else {
                curr->offset = calc_offset;
                _markUsed( curr, TRUE );
                curr->has_data = TRUE;
                id = curr->seg_id;
                data_def_seg.ds_used = TRUE;
            }
        }
    }
    return id;
}


#if _CPU == _AXP || COMP_CFG_COFF == 1
fe_seg_id SegmentAddComdatData( // ADD SEGMENT FOR A COMDAT DATA SYMBOL
    SYMBOL sym,                 // - the symbol
    SEGID_CONTROL control )     // - segment control
{
    unsigned attrs;             // - attributes
    char const * name;          // - segment name
    PC_SEGMENT* seg;            // - allocated segment

    sym = sym;
    if( control & SI_ALL_ZERO ) {
        attrs = SGAT_DATA_COMMON_ZERO;
        name = TS_SEG_BSS;
    } else {
        type_flag flags;
        TypeModFlags( sym->sym_type, &flags );
        if( flags & TF1_CONST ) {
            attrs = SGAT_DATA_COMMON_INIT_ROM;
            name = TS_SEG_DATA;
        } else {
            attrs = SGAT_DATA_COMMON_INIT;
            name = TS_SEG_CONST2;
        }
    }
    seg = segmentAlloc( name, NULL, SEG_NULL, attrs, SA_IN_DGROUP );
    return seg->seg_id;
}
#endif


fe_seg_id SegmentImport(        // GET NEXT IMPORT SEGMENT #
    void )
{
    --seg_import;
    checkSegmentOverflow();
    return( seg_import );
}


static SYMBOL segDefineLabel(   // DEFINE LABEL FOR SEGMENT, IF REQ'D
    PC_SEGMENT *seg )           // - current segment
{
    SYMBOL label;               // - reference symbol
    char *name;                 // - label's name
    SYMBOL func;                // - function being compiled

    label = seg->label;
    if( label == NULL ) {
        func = ScopeFunctionInProgress();
        if( ( func != NULL ) && ( func->segid == seg->seg_id ) ) {
            label = func;
        }
    }
    if( label == NULL ) {
        label = SymMakeDummy( GetBasicType( TYP_CHAR ), &name );
        label->segid = seg->seg_id;
        if( label->segid == SEG_STACK ) {
            label->id = SC_AUTO;
        } else {
            label->id = SC_STATIC;
            InsertSymbol( GetFileScope(), label, name );
        }
        seg->label = label;
        _markUsed( seg, TRUE );
    }
    return label;
}


SYMBOL SegmentLabelSym(         // GET LABEL IN SEGMENT
    fe_seg_id seg_id )          // - segment id
{
    return segDefineLabel( segIdLookup( seg_id ) );
}


SYMBOL SegmentLabelGen(         // GENERATE SEGMENT LABEL IF REQ'D
    fe_seg_id seg_id )          // - segment id
{
    PC_SEGMENT *seg;            // - current segment
    SYMBOL label;               // - reference symbol

    seg = segIdLookup( seg_id );
    segmentCgDefine( seg );
    segDefineLabel( seg );
    label = segEmitLabel( seg );
    return label;
}


SYMBOL SegmentLabelStackReset(  // RESET STACK-SEGMENT LABEL
    void )
{
    SYMBOL lab;                 // - label used for stack
    PC_SEGMENT *stk;            // - segment for stack;

    stk = segIdLookup( SEG_STACK );
    lab = stk->label;
    stk->label = NULL;
    stk->lab_gened = FALSE;
    return lab;
}

#if _INTEL_CPU
    #define CODE_ENDING "TEXT"
#elif _CPU == _AXP
    #define CODE_ENDING "text"
#else
    #error Invalid machine
#endif
#define ENDING_SIZE (sizeof(CODE_ENDING)-1)

static boolean isCodeSegmentName( char *segname )
{
    size_t len;

    len = strlen( segname );
    if( len >= 4 ) {
        if( stricmp( segname + len - ENDING_SIZE, CODE_ENDING ) == 0 ) {
            return( TRUE );
        }
    }
    return( FALSE );

}

static boolean segmentIsCode(
    fe_seg_id segid )           // - function symbol
{
    PC_SEGMENT *seg;

    seg = segIdLookup( segid );
    if( ( seg->attrs & EXEC ) == 0 ) {
        CErr( ERR_CODE_IN_NONCODE_SEG, seg->name );
        InfMsgPtr( INF_CODE_SEGMENT_SUFFIX, CODE_ENDING );
        return( FALSE );
    }
    return( TRUE );
}

#undef CODE_ENDING
#undef ENDING_SIZE

fe_seg_id SegmentFindNamed(     // FIND SEGMENT ENTRY FOR NAME
    char* segname )             // - segment name
{
    PC_SEGMENT *segmt;          // - defined segment

    if( 0 == strcmp( segname, "_STACK" ) ) {
        return SEG_STACK;
    }
    if( 0 == strcmp( segname, "_CODE" ) ) {
        return SEG_CODE;
    }
    segmt = segNameLookup( segname );
    if( segmt == NULL ) {
        if( isCodeSegmentName( segname ) ) {
            segmt = segmentDefine( segname
                                 , NULL
                                 , SEG_NULL
                                 , SGAT_CODE_BASED
                                 , SA_NULL );
        } else {
            segmt = segmentDefine( segname
                                 , NULL
                                 , SEG_NULL
                                 , SGAT_DATA_BASED
                                 , SA_NULL );
        }
    }
    _markUsed( segmt, TRUE );
    return segmt->seg_id;
}


fe_seg_id SegmentFindBased(     // FIND SEGMENT ID FOR TF1_BASED_STRING
    TYPE expr_type )            // - type being based
{
    TYPE base_mod;              // - __based modifier
    STRING_CONSTANT string;     // - constant for segment name
    char *segname;              // - segment name

    base_mod = BasedType( expr_type );
    string = base_mod->u.m.base;
    segname = string->string;
    return SegmentFindNamed( segname );
}


static PC_SEGMENT *segmentAllocRom(    // ALLOCATE R/O DATA SEGMENT
    char *name,                 // - name
    fe_seg_id segid )           // - segment id
{
    return segmentAlloc( name, NULL, segid, SGAT_DATA_CON2, SA_IN_DGROUP );
}

static PC_SEGMENT *segmentAllocRW(    // ALLOCATE R/W DATA SEGMENT
    char *name,                 // - name
    fe_seg_id segid )           // - segment id
{
    return segmentAlloc( name, NULL, segid, SGAT_DATA_RW, SA_IN_DGROUP );
}


void SegmentData(               // SET DEFAULT DATA SEGMENT
    char *segname,              // - segment name
    char *segclass )            // - segment class name
{
    PC_SEGMENT *seg;            // - new default segment
    unsigned control;           // - segmentAlloc control mask

    if( segname == NULL ) {
        segname = TS_SEG_DATA;
        flags.use_def_seg = FALSE;
    } else {
        flags.use_def_seg = TRUE;
    }
    pruneDefSeg( &data_def_seg );
    control = SA_IN_DGROUP;
    if( strpref( ModuleName, segname ) ) {
        control |= SA_MODULE_PREFIX;
    }
    seg = segmentDefine( segname, segclass, SEG_NULL, SGAT_DATA_RW, control );
    initDefSeg( &data_def_seg, seg );
}


void SegmentCode(               // SET DEFAULT CODE SEGMENT
    char *segname,              // - segment name
    char *segclass )            // - segment class name
{
    PC_SEGMENT *seg;            // - new default segment
    unsigned control;           // - segmentAlloc control mask

    if( segname == NULL ) {
        SegmentCode( TextSegName, NULL );
        return;
    }
    pruneDefSeg( &code_def_seg );
    control = SA_NULL;
    if( strpref( ModuleName, segname ) ) {
        control |= SA_MODULE_PREFIX;
    }
    seg = segmentDefine( segname, segclass, SEG_NULL, SGAT_CODE_GEN, control );
    initDefSeg( &code_def_seg, seg );
}

static void initDefaultCodeSeg( char *code_seg_name )
{
    PC_SEGMENT *pcseg;

    SegmentCode( code_seg_name, NULL );
    code_def_seg.ds_used = TRUE;
    pcseg = code_def_seg.pcseg;
    _markUsed( pcseg, TRUE );
    seg_default_code = pcseg->seg_id;
}

static void initDefaultDataSeg( void )
{
    PC_SEGMENT *pcseg;

    SegmentData( NULL, NULL );
    data_def_seg.ds_used = TRUE;        // used for data externs
    pcseg = data_def_seg.pcseg;
    _markUsed( pcseg, TRUE );
}

#if _CPU == 386
static void initP5TimingSegs( void )
{
    PC_SEGMENT *seg;
    PC_SEGMENT *sib;

    if( TargetSwitches & (P5_PROFILING|NEW_P5_PROFILING) ) {
        seg = segmentAllocRom( TS_SEG_TIB, SEG_PROF_BEG );
        sib = segmentAllocRW( TS_SEG_TI, SEG_PROF_REF );
        addSibling( seg, sib );
        sib = segmentAllocRom( TS_SEG_TIE, SEG_PROF_END );
        addSibling( seg, sib );
        _markUsed( seg, TRUE );
    }
}
#else
#define initP5TimingSegs()
#endif

void SegmentInit(               // SEGMENT: INITIALIZATION
    char *code_seg_name )       // - name of data segment
{
    PC_SEGMENT *seg;
    PC_SEGMENT *sib;

    if( CompFlags.dll_subsequent ) {
        flags.in_back_end = FALSE;
        seg_list = NULL;
        dgroup_size = 0;
        memset( &code_def_seg, 0, sizeof( code_def_seg ) );
        memset( &data_def_seg, 0, sizeof( data_def_seg ) );
        seg_max = SEG_NULL;
#if _INTEL_CPU
        seg_code_comdat = SEG_NULL;
#endif
    }
    seg_import = -1;
    // code seg
    initDefaultCodeSeg( code_seg_name );
    // string literal data seg
    segmentAlloc( TS_SEG_CONST, NULL, SEG_CONST, SGAT_DATA_CON1, SA_IN_DGROUP );
    // other R/O data seg
    segmentAllocRom( TS_SEG_CONST2, SEG_CONST2 );
    // r/w data seg
    initDefaultDataSeg();
    // thread-local storage
    seg = segmentAlloc( TS_SEG_TLSB, TS_SEG_TLS_CLASS, SEG_TLS_BEG, SGAT_TLS_LIMIT, SA_IN_DGROUP );
    sib = segmentAlloc( TS_SEG_TLS, TS_SEG_TLS_CLASS, SEG_TLS, SGAT_TLS_DATA, SA_IN_DGROUP );
    addSibling( seg, sib );
    sib = segmentAlloc( TS_SEG_TLSE, TS_SEG_TLS_CLASS, SEG_TLS_END, SGAT_TLS_LIMIT, SA_IN_DGROUP );
    addSibling( seg, sib );
    // bss
    segmentAlloc( TS_SEG_BSS, NULL, SEG_BSS, SGAT_BSS, SA_IN_DGROUP );
    // profiling data
    initP5TimingSegs();
    // init data
    seg = segmentAllocRom( TS_SEG_XIB, SEG_INIT_BEG );
    sib = segmentAllocRW( TS_SEG_XI, SEG_INIT_REF );
    addSibling( seg, sib );
    sib = segmentAllocRom( TS_SEG_XIE, SEG_INIT_END );
    addSibling( seg, sib );
    // fini data
    seg = segmentAllocRom( TS_SEG_YIB, SEG_FINI_BEG );
    sib = segmentAllocRW( TS_SEG_YI, SEG_FINI_REF );
    addSibling( seg, sib );
    sib = segmentAllocRom( TS_SEG_YIE, SEG_FINI_END );
    addSibling( seg, sib );
    // stack
    segmentAlloc( TS_SEG_STACK, NULL, SEG_STACK, SGAT_STACK, SA_IN_DGROUP );
}


void SegmentFini(               // SEGMENT: COMPLETION
    void )
{
    RingFree( &seg_list );
}


static fe_seg_id nextZmSegment( // GET NEXT CODE SEGMENT FOR -zm
    void )
{
    fe_seg_id segid;            // - segment id
    unsigned ads_control;       // - def seg control

    if( code_def_seg.ds_used ) {
        ads_control = ADS_NULL;
        if( IsBigCode() && CompFlags.zm_switch_used ) {
            if( !CompFlags.zmf_switch_used ) {
                ads_control = ADS_ZM_SEGMENT;
            }
        }
        segid = addDefSeg( &code_def_seg, ads_control )->seg_id;
    } else {
        code_def_seg.ds_used = TRUE;
        segid = code_def_seg.pcseg->seg_id;
        SegmentMarkUsed( segid );
    }
    return segid;
}

boolean SegmentIfBasedOK( SYMBOL func )
{
    fe_seg_id segid;

    segid = func->segid;
    if( segid == SEG_NULL ) {
        if( NULL != BasedType( func->sym_type ) ) {
            segid = SegmentFindBased( func->sym_type );
            return( segmentIsCode( segid ) );
        }
    }
    return( TRUE );
}

fe_seg_id SegmentForDefinedFunc(// GET SEGMENT FOR A DEFINED FUNCTION
    SYMBOL func )               // - function
{
    fe_seg_id segid;

    segid = func->segid;
    if( segid == SEG_NULL ) {
        if( NULL != BasedType( func->sym_type ) ) {
            segid = SegmentFindBased( func->sym_type );
        } else if( CompFlags.zm_switch_used ) {
#if _INTEL_CPU
            if( SymIsGennedComdatFun( func ) ) {
                segid = seg_code_comdat;
                if( SEG_NULL == segid ) {
                    segid = nextZmSegment();
                    seg_code_comdat = segid;
                }
            } else {
                segid = nextZmSegment();
            }
#elif _CPU == _AXP || COMP_CFG_COFF == 1
            segid = nextZmSegment();
            if( SymIsGennedComdatFun( func ) ) {
                segid = markSegmentComdat( segid );
            }
#else
            #error Bad Machine Type
#endif
#if _CPU == _AXP || COMP_CFG_COFF == 1
        } else if( SymIsGennedComdatFun( func ) ) {
            segid = nextZmSegment();
            segid = markSegmentComdat( segid );
#endif
        } else {
            segid = code_def_seg.pcseg->seg_id;
            SegmentMarkUsed( segid );
            code_def_seg.ds_used = TRUE;
        }
    }
    return segid;
}


fe_seg_id SegmentDefaultCode(   // GET CURRENT DEFAULT CODE SEGMENT
    void )
{
    return( seg_default_code );
}


void SegmentMarkRoBlock(        // INDICATE R/O BLOCK TO BE GENERATED
    void )
{
    // NYI: should never be used (use SegmentMarkUsed with the symbol's segid)
    SegmentMarkUsed( SEG_CONST2 );
}


void SegmentMarkUsed(           // MARK SEGMENT AS BEING USED
    fe_seg_id segid )           // - XI, YI segment id
{
    PC_SEGMENT *seg;            // - the segment

    if( segid != SEG_NULL ) {
        seg = segIdLookup( segid );
        // extern segments are not SEG_NULL, but will not be found
        if( seg != NULL ) {
            _markUsed( seg, TRUE );
        }
    }
}


void SegmentCgDefineCode(       // DEFINE CODE SEGMENT IF REQ'D
    fe_seg_id segid )           // - segment id
{
#ifndef NDEBUG
    PC_SEGMENT *seg;

    seg = segIdLookup( segid );
    DbgAssert( seg->attrs & EXEC );
    segmentCgDefine( seg );
#else
    segmentCgDefine( segIdLookup( segid ) );
#endif
}


void SegmentCodeCgInit(         // TURN OFF USED BIT FOR ALL CODE SEGMENTS
    void )
{
    PC_SEGMENT *segment;        // - current segment

    // reset any #pragma data_seg/code_seg changes back to defaults
    SegmentData( NULL, NULL );
    SegmentCode( NULL, NULL );
    RingIterBeg( seg_list, segment ) {
        if( ( segment->attrs & EXEC ) && ! segment->has_data ) {
            _markUsed( segment, FALSE );
        }
    } RingIterEnd( segment )
    // call out for non-call graph code segment uses
    DbgSuppSegRequest();
    // used for default code segment externs
    SegmentMarkUsed( SEG_CODE );
}


void SegmentCgInit(             // INITIALIZE SEGMENTS FOR CODE-GENERATION
    void )
{
    PC_SEGMENT *segment;        // - current segment

    RingIterBeg( seg_list, segment ) {
        switch( segment->seg_id ) {
          case SEG_STACK :
            // Cg already "knows" about this segment
            segment->cg_defed = TRUE;
            break;
          default :
            if( segment->attrs & BACK ) {
                segmentCgDefine( segment );
                break;
            }
            if( ! segment->used ) break;
            // drops thru
          case SEG_DATA :
          case SEG_CONST :
          case SEG_CONST2 :
          case SEG_BSS :
            // These could be defined only if they are used but unfortunately
            // the CG doesn't like DGROUP segments being defined on the fly.
            // Based pointers would cause these segments to be defined on the
            // fly so if we get rid of based pointer support, we could selectively
            // define them but then again, it's only four segments...
            segmentCgDefine( segment );
            break;
        }
    } RingIterEnd( segment )
    flags.in_back_end = TRUE;
}


char *SegmentClassName(         // GET CLASS NAME OF SEGMENT (IF ANY)
    fe_seg_id id )              // - id of segment
{
    PC_SEGMENT *pc_seg;
    char *class_name;

    pc_seg = segIdLookup( id );
    if( pc_seg == NULL ) {
        return( NULL );
    }
    class_name = pc_seg->class_name;
    if( class_name == NULL && DataSegName[0] != '\0' ) {
        class_name = DataSegName;
    }
    return( class_name );
}


#if _INTEL_CPU
void* SegmentBoundReg(          // GET REGISTER BOUND TO SEGMENT
    fe_seg_id seg_id )          // - segment id
{
    void* retn;                 // - NULL or hardware reg set
    PC_SEGMENT *seg;            // - segment for id

    seg = segIdLookup( seg_id );
    if( seg == NULL ) {
        retn = NULL;
    } else {
        retn = &seg->binding;
    }
    return retn;
}
#endif

pch_status PCHReadSegments( void )
{
    char *src_suffix;
    char *dest_suffix;
    char *class_name;
    size_t class_name_len;
    size_t suffix_len;
    size_t len;
    size_t old_module_len;
    size_t curr_module_len;
    size_t extra;
    fe_seg_id sib_id;
    PC_SEGMENT *curr;

    curr_module_len = strlen( ModuleName );
    old_module_len = PCHReadUInt();
    extra = 0;
    if( curr_module_len > old_module_len ) {
        extra = curr_module_len - old_module_len;
    }
    PCHRead( &seg_max, sizeof( seg_max ) );
    PCHRead( &dgroup_size, sizeof( dgroup_size ) );
    RingFree( &seg_list );
    for(;;) {
        len = PCHReadUInt();
        if( len == 0 ) break;
        curr = CMemAlloc( len + extra );
        PCHRead( curr, len );
        if( curr->module_prefix ) {
            if( old_module_len != curr_module_len ) {
                suffix_len = len;
                suffix_len -= sizeof( PC_SEGMENT ) - sizeof( char );
                suffix_len -= old_module_len;
                src_suffix = &(curr->name[old_module_len]);
                dest_suffix = &(curr->name[curr_module_len]);
                memmove( dest_suffix, src_suffix, suffix_len );
            }
            memcpy( curr->name, ModuleName, curr_module_len );
        }
        curr->label = SymbolMapIndex( curr->label );
        class_name_len = (size_t) curr->class_name;
        if( class_name_len != 0 ) {
            class_name = CPermAlloc( class_name_len );
            PCHRead( class_name, class_name_len );
        } else {
            class_name = NULL;
        }
        curr->class_name = class_name;
        RingAppend( &seg_list, curr );
    }
    RingIterBeg( seg_list, curr ) {
        sib_id = (fe_seg_id) curr->sibling;
        if( sib_id != curr->seg_id ) {
            curr->sibling = segIdLookup( sib_id );
        } else {
            curr->sibling = curr;
        }
    } RingIterEnd( curr )
    code_def_seg.pcseg = segIdLookup( PCHReadUInt() );
    data_def_seg.pcseg = segIdLookup( PCHReadUInt() );
    return( PCHCB_OK );
}

pch_status PCHWriteSegments( void )
{
    size_t prefix_len;
    size_t len;
    size_t class_name_len;
    SYMBOL save_label;
    char *save_class_name;
    PC_SEGMENT *save_sibling;
    PC_SEGMENT *curr;

    prefix_len = strlen( ModuleName );
    PCHWriteUInt( prefix_len );
    PCHWrite( &seg_max, sizeof( seg_max ) );
    PCHWrite( &dgroup_size, sizeof( dgroup_size ) );
    RingIterBeg( seg_list, curr ) {
        class_name_len = 0;
        if( curr->class_name != NULL ) {
            class_name_len = strlen( curr->class_name ) + 1;
        }
        save_sibling = curr->sibling;
        curr->sibling = (PC_SEGMENT*) save_sibling->seg_id;
        save_label = curr->label;
        curr->label = SymbolGetIndex( save_label );
        save_class_name = curr->class_name;
        curr->class_name = (void*) class_name_len;
        len = sizeof( PC_SEGMENT ) + strlen( curr->name );
        PCHWriteUInt( len );
        PCHWrite( curr, len );
        curr->sibling = save_sibling;
        curr->label = save_label;
        curr->class_name = save_class_name;
        if( class_name_len != 0 ) {
            PCHWrite( save_class_name, class_name_len );
        }
    } RingIterEnd( curr )
    len = 0;
    PCHWriteUInt( len );
    PCHWriteUInt( code_def_seg.pcseg->seg_id );
    PCHWriteUInt( data_def_seg.pcseg->seg_id );
    return( PCHCB_OK );
}

pch_status PCHInitSegments( boolean writing )
{
    writing = writing;
    return( PCHCB_OK );
}

pch_status PCHFiniSegments( boolean writing )
{
    writing = writing;
    return( PCHCB_OK );
}

static void fini(               // FINALIZATION
    INITFINI* defn )            // - definition
{
    defn = defn;
    SegmentFini();
}

INITDEFN( segment, InitFiniStub, fini );
