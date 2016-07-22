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
* Description:  Emit OMF object records.
*
****************************************************************************/


#include "cgstd.h"
#include <setjmp.h>
#include "coderep.h"
#include "cgdefs.h"
#include "cgauxinf.h"
#include "system.h"
#include "import.h"
#include "fppatch.h"
#include "cgmem.h"
#include "zoiks.h"
#include "data.h"
#include "rtrtn.h"
#include "i86obj.h"
#include "utils.h"
#include "objout.h"
#include "dbsyms.h"
#include "cvsyms.h"
#include "dw.h"
#include "dfsyms.h"
#include "wvsyms.h"
#include "wvtypes.h"
#include "objio.h"
#include "onexit.h"
#include "ocentry.h"
#include "optmain.h"
#include "intrface.h"
#include "feprotos.h"

#ifdef _PHAR_LAP /* This is a misnomer. Let's rename it */
    #define _OMF_32
#endif

#define _NIDX_NULL      1   // lname ""
#define _NIDX_CODE      2   // lname "CODE"
#define _NIDX_DATA      3   // lname "DATA"
#define _NIDX_BSS       4   // lname "BSS"
#define _NIDX_TLS       5   // lname "TLS"

#define MODEST_HDR      50
#define INCREMENT_HDR   50

#define MODEST_INFO     5
#define INCREMENT_INFO  5

#define MODEST_IMP      BUFFSIZE
#define INCREMENT_IMP   50

#define MODEST_LINE     BUFFSIZE
#define INCREMENT_LINE  200

#define MODEST_OBJ      BUFFSIZE
#define INCREMENT_OBJ   256

#define NOMINAL_FIX     20
#define INCREMENT_FIX   50

#define MODEST_EXP      BUFFSIZE
#define INCREMENT_EXP   50

#define MODEST_PAT      10
#define INCREMENT_PAT   10


#define _CopyTrans( src, dst, len )        Copy( src, dst, len )

#define _ARRAY( what, type )    (*(type *)((char *)(what)->array + (what)->used))
#define _ARRAYOF( what, type )  ((type *)(what)->array)
#define _CHGTYPE( what, type )  (*(type *)&(what))


typedef enum {
    OFC_LOBYTE              = 0,    /* not used */
    OFC_OFFSET              = 1,
    OFC_BASE                = 2,
    OFC_PTR                 = 3,
    OFC_HIBYTE              = 4,    /* not used */
    OFC_PHAR_OFFSET         = 5,
    OFC_LDR_OFFSET          = 5,
    OFC_PHAR_PTR            = 6,
    OFC_MS_OFFSET_32        = 9,
    OFC_MS_PTR              = 11,
    OFC_MS_LDR_OFFSET_32    = 13,
} omf_fix_class;

#include "pushpck1.h"
typedef struct line_num_entry {
    unsigned_16     line;
    offset          off;
} line_num_entry;
#include "poppck.h"

typedef struct lname_cache {
    struct lname_cache  *next;
    omf_idx             idx;
    unsigned_8          name[1];        /* var sized, first byte is length */
} lname_cache;

typedef struct virt_func_ref_list {
    struct virt_func_ref_list   *next;
    void                        *cookie;
} virt_func_ref_list;

typedef struct dbg_seg_info {
    segment_id  *id;
    char        *seg_name;
    char        *class_name;
} dbg_seg_info;


extern  void            TellDonePatch(label_handle);
extern  void            TellAddress(label_handle,offset);
extern  void            TellCommonLabel(label_handle,import_handle);
extern  void            TellUnreachLabels(void);
extern  void            KillLblRedirects( void );
extern  void            DoOutObjectName(cg_sym_handle,void(*)(const char *,void *),void *,import_type);

extern  bool            Used87;

/* Forward ref's */
static  void            DumpImportResolve( cg_sym_handle sym, omf_idx idx );

static  bool            GenStaticImports;
static  omf_idx         ImportHdl;
static  array_control   *Imports;
static  array_control   *SegInfo;
static  abspatch        *AbsPatches;
static  segment_id      CodeSeg = BACKSEGS;
static  segment_id      BackSeg;
static  segdef          *SegDefs;
static  long_offset     CodeSize;
static  long_offset     DataSize;
static  long_offset     DbgTypeSize;
static  index_rec       *CurrSeg;
static  omf_idx         GroupIndex;
static  omf_idx         DGroupIndex;
static  omf_idx         SegmentIndex;
static  omf_idx         PrivateIndexRW;
static  omf_idx         PrivateIndexRO;
static  omf_idx         CodeGroupGIdx;
static  omf_idx         CodeGroupNIdx;
static  char            CodeGroup[80];
static  char            DataGroup[80];
static  offset          SelStart;
static  omf_idx         selIdx;
static  segment_id      BackSegIdx = BACKSEGS;
static  omf_idx         FPPatchImp[FPP_NUMBER_OF_TYPES];
static  segment_id      SegsDefd;
static  bool            NoDGroup;
static  short           CurrFNo;
#ifdef _OMF_32
static  omf_idx         FlatGIndex;
static  omf_idx         FlatNIndex;
#endif
static  omf_idx         TLSGIndex;

static  omf_idx         NameIndex;
static  lname_cache     *NameCache;
static  lname_cache     *NameCacheDumped;

static char *FPPatchName[] = {
#define pick_fp(enum,name,alt_name) name,
#include "fppatche.h"
#undef pick_fp
};

static char *FPPatchAltName[] = {
#define pick_fp(enum,name,alt_name) alt_name,
#include "fppatche.h"
#undef pick_fp
};

static struct dbg_seg_info DbgSegs[] = {
    { &DbgLocals, "$$SYMBOLS", "DEBSYM" },
    { &DbgTypes,  "$$TYPES",   "DEBTYP" },
};


extern  void    InitSegDefs( void )
/*********************************/
{
    SegDefs = NULL;
    NameCache = NULL;
    NameCacheDumped = NULL;
    NameIndex = 0;
    GroupIndex = 0;
    DGroupIndex = 0;
    TLSGIndex = 0;
#ifdef _OMF_32
    FlatGIndex = 0;
#endif
    SegsDefd = 0;
    CodeSeg = BACKSEGS; /* just so it doesn't match a FE seg_id */
    BackSegIdx = BACKSEGS;
}

static omf_idx GetNameIdx( const char *name, const char *suff, bool alloc )
/*************************************************************************/
{
    lname_cache         **owner;
    lname_cache         *curr;
    unsigned            name_len;
    unsigned            suff_len;

    name_len = Length( name );
    suff_len = Length( suff );
    owner = &NameCache;
    for( ;; ) {
        curr = *owner;
        if( curr == NULL )
            break;
        if( (name_len + suff_len) == curr->name[0]
          && memcmp( name, &curr->name[1], name_len ) == 0
          && memcmp( suff, &curr->name[name_len+1], suff_len ) == 0 ) {
            return( curr->idx );
        }
        owner = &curr->next;
    }
    if( !alloc )
        return( 0 );
    curr = CGAlloc( sizeof( *curr ) + name_len + suff_len );
    *owner = curr;
    curr->next = NULL;
    curr->idx = ++NameIndex;
    assert(( name_len + suff_len ) < 256 );
    curr->name[0] = name_len + suff_len;
    memcpy( &curr->name[1], name, name_len );
    memcpy( &curr->name[name_len+1], suff, suff_len );
    return( NameIndex );
}

static void FlushNames( void )
/****************************/
{
    /*
        don't want to allocate memory because we might be in a low memory
        situation
    */
    unsigned_8          buff[512];
    unsigned            i;
    lname_cache         *dmp;

    i = 0;
    dmp = NameCacheDumped != NULL ? NameCacheDumped->next : NameCache;
    for( ; dmp != NULL; dmp = dmp->next ) {
        if( (i + dmp->name[0]) > (sizeof( buff ) - 1) ) {
            PutObjOMFRec( CMD_LNAMES, buff, i );
            i = 0;
        }
        buff[i++] = dmp->name[0];
        _CopyTrans( &dmp->name[1], &buff[i], dmp->name[0] );
        i += dmp->name[0];
        NameCacheDumped = dmp;
    }
    if( i > 0 ) {
        PutObjOMFRec( CMD_LNAMES, buff, i );
    }
}

bool FreeObjCache( void )
/***********************/
{
    lname_cache         *tmp;

    if( NameCache == NULL )
        return( false );
    FlushNames();
    while( NameCache != NULL ) {
        tmp = NameCache->next;
        CGFree( NameCache );
        NameCache = tmp;
    }
    NameCacheDumped = NULL;
    return( true );
}


static  index_rec   *AskSegIndex( segment_id seg )
/************************************************/
{
    index_rec   *rec;
    unsigned    i;

    rec = SegInfo->array;
    for( i = 0; i < SegInfo->used; ++i ) {
        if( rec->seg == seg ) {
            return( rec );
        }
        ++rec;
    }
    return( NULL );
}


static  void    ReallocArray( array_control *arr, unsigned need )
/***************************************************************/
{
    byte        *p;
    unsigned    new;

    new = arr->alloc;
    for( ;; ) {
        new += arr->inc;
        if( new >= need ) {
            break;
        }
    }
    p = CGAlloc( arr->entry * new );
    Copy( arr->array, p, arr->entry * arr->used );
    CGFree( arr->array );
    arr->array = p;
    arr->alloc = new;
}

static  byte    SegmentAttr( byte align, seg_attr tipe, bool use_16 )
/*******************************************************************/
{
    byte        attr;

    use_16 = use_16;
    if( align <= 1 ) {
        attr = SEG_ALGN_BYTE;
    } else if( align <= 2 ) {
        attr = SEG_ALGN_WORD;
#ifdef _OMF_32
    } else if( align <= 4 ) {
        attr = SEG_ALGN_DWORD;
#endif
    } else if( align <= 16 ) {
        attr = SEG_ALGN_PARA;
    } else {
        attr = SEG_ALGN_PAGE;
#if 0
    // align is a byte - how can it be bigger than 4k - BBB
  #ifdef _OMF_32
        if( _IsTargetModel( EZ_OMF ) ) {
            if( align > 256 ) {
                attr = SEG_ALGN_4K;
            }
        }
  #endif
#endif
    }
    if( tipe & COMMON ) {
        attr |= SEG_COMB_COMMON;
    } else if( ( tipe & PRIVATE ) != 0 && ( tipe & GLOBAL ) == 0 ) {
        attr |= SEG_COMB_PRIVATE;
    } else { /* normal or a kludge for wsl front end ( PRIVATE | GLOBAL )*/
        attr |= SEG_COMB_NORMAL;
    }
#ifdef _OMF_32
    if( _IsntTargetModel( EZ_OMF ) ) {
        if( _IsTargetModel( USE_32 ) ) {
            if( use_16 == false ) {
                attr |= SEG_USE_32;
            }
        }
    }
#endif
    return( attr );
}

static  void    SegmentClass( index_rec *rec )
/********************************************/
{
    char        *class_name;

    class_name = FEAuxInfo( (pointer)(pointer_int)rec->seg, CLASS_NAME );
    if( class_name != NULL ) {
        rec->cidx = GetNameIdx( class_name, "", true );
    }
}

/* Array Control Routines*/

static void FillArray( array_control *res, unsigned size, unsigned starting, unsigned increment )
/***********************************************************************************************/
{
    res->array = CGAlloc( starting * size );
    res->alloc = starting;
    res->used = 0;
    res->entry = size;
    res->inc = increment;
}

/* DO NOT call InitArray with size or starting value zero*/

static array_control *InitArray( unsigned size, unsigned starting, unsigned increment )
/*************************************************************************************/
{
    array_control       *res;

    res = CGAlloc( sizeof( array_control ) );
    FillArray( res, size, starting, increment );
    return( res );
}


static  void    OutByte( byte value, array_control *dest )
/********************************************************/
{
    unsigned    need;

    need = dest->used + 1;
    if( need > dest->alloc ) {
        ReallocArray( dest, need );
    }
    _ARRAY( dest, byte ) = value;
    dest->used = need;
}

static  void    OutShort( unsigned_16 value, array_control *dest )
/****************************************************************/
{
    unsigned    need;

    need = dest->used + sizeof( unsigned_16 );
    if( need > dest->alloc ) {
        ReallocArray( dest, need );
    }
    _ARRAY( dest, unsigned_16 ) = _TargetShort( value );
    dest->used = need;
}

static  void    OutLongInt( unsigned_32 value, array_control *dest )
/******************************************************************/
{
    unsigned    need;

    need = dest->used + sizeof( unsigned_32 );
    if( need > dest->alloc ) {
        ReallocArray( dest, need );
    }
    _ARRAY( dest, unsigned_32 ) = _TargetLongInt( value );
    dest->used = need;
}

static  void    OutOffset( offset value, array_control *dest )
/************************************************************/
{
    unsigned    need;

    need = dest->used + sizeof( offset );
    if( need > dest->alloc ) {
        ReallocArray( dest, need );
    }
    _ARRAY( dest, offset ) = _TargetOffset( value );
    dest->used = need;
}

#ifndef _OMF_32  // 32bit debug seg support dwarf,codeview
static  void    OutLongOffset( long_offset value, array_control *dest )
/*********************************************************************/
{
    unsigned    need;

    need = dest->used + sizeof( long_offset );
    if( need > dest->alloc ) {
        ReallocArray( dest, need );
    }
    _ARRAY( dest, long_offset ) = _TargetLongInt( value );
    dest->used = need;
}
#endif

static  void    OutIdx( omf_idx value, array_control *dest )
/**********************************************************/
{
    if( value >= 128 ) {
        OutByte( (value >> 8) | 0x80, dest );
    }
    OutByte( value, dest );
}

static  void    OutBuffer( const void *name, unsigned len, array_control *dest )
/******************************************************************************/
{
    unsigned    need;

    need = dest->used + len;
    if( need > dest->alloc ) {
        ReallocArray( dest, need );
    }
    _CopyTrans( name, &_ARRAY( dest, byte ), len );
    dest->used = need;
}

static  cmd_omf    PickOMF( cmd_omf cmd )
/***************************************/
{
#ifdef _OMF_32
    if( _IsntTargetModel( EZ_OMF ) )
        ++cmd;
#endif
    return( cmd );
}

static  void    DoASegDef( index_rec *rec, bool use_16 )
/******************************************************/
{
    object      *obj;
    cmd_omf     cmd;

    use_16 = use_16;
    obj = CGAlloc( sizeof( object ) );
    rec->obj = obj;
    obj->index = rec->sidx;
    obj->start = rec->location;
    obj->patches = NULL;
    obj->gen_static_exports = false;
    obj->pending_line_number = 0;
    FillArray( &obj->data, sizeof( byte ), MODEST_OBJ, INCREMENT_OBJ );
    FillArray( &obj->fixes, sizeof( byte ), NOMINAL_FIX, INCREMENT_FIX );
    if( rec->exec ) { /* try to bag all the memory we'll need now*/
        obj->exports = InitArray( sizeof( byte ), MODEST_EXP, INCREMENT_EXP );
    } else {
        obj->exports = NULL;
    }
    obj->lines = NULL;
    obj->line_info = false;
    OutByte( rec->attr, &obj->data );
#ifdef _OMF_32
    OutOffset( 0, &obj->data );         /* segment size (for now) */
#else  //SEG32DBG dwarf, codeview
    if( rec->attr & SEG_USE_32 ) {
        OutLongOffset( 0, &obj->data ); /* segment size (for now) */
    } else {
        OutOffset( 0, &obj->data );     /* segment size (for now) */
    }
#endif
    OutIdx( rec->nidx, &obj->data );    /* segment name index */
    OutIdx( rec->cidx, &obj->data );    /* class name index */
    OutIdx( _NIDX_NULL, &obj->data );   /* overlay name index */
#ifdef _OMF_32
    if( _IsTargetModel( EZ_OMF ) ) {
        if( _IsntTargetModel( USE_32 ) || use_16 ) {
            OutByte( 2, &obj->data );   /* to indicate USE16 EXECUTE/READ */
        }
    }
#endif
    FlushNames();
    obj->segfix = AskObjHandle();
    if( ++SegsDefd > 32000 ) {
        FEMessage( MSG_FATAL, "too many segments" );
    }
#ifdef _OMF_32
    cmd = PickOMF( CMD_SEGDEF );
#else //SEG32DBG dwarf, codeview
    if(( rec->attr & SEG_USE_32 ) && ( _IsntTargetModel( EZ_OMF ))) {
        cmd = CMD_SEGDEF32;
    } else {
        cmd = CMD_SEGDEF;
    }
#endif
    PutObjOMFRec( cmd, obj->data.array, obj->data.used );
    if( rec->exec ) {
        obj->data.used = 0;
        OutShort( LINKER_COMMENT, &obj->data );
        OutByte( LDIR_OPT_FAR_CALLS, &obj->data );
        OutIdx( rec->sidx, &obj->data );
        PutObjOMFRec( CMD_COMENT, obj->data.array, obj->data.used );
    }
    obj->data.used = 0;
    rec->location = 0;
}


static  void    OutCGroup( omf_idx sidx, array_control *dest )
/************************************************************/
{
    FlushNames();
    OutIdx( CodeGroupNIdx, dest );
    OutByte( GRP_SEGIDX, dest );
    OutIdx( sidx, dest );
    PutObjOMFRec( CMD_GRPDEF, dest->array, dest->used );
    dest->used = 0;
}

static void OutGroup( omf_idx sidx, array_control *group_def, omf_idx *index_p )
/******************************************************************************/
{
    if( *index_p == 0 ) {
        *index_p = ++GroupIndex;
    }
    OutByte( GRP_SEGIDX, group_def );
    OutIdx( sidx, group_def );
}

static  index_rec   *AllocNewSegRec( void )
/*****************************************/
{
    index_rec   *rec;
    segment_id  old = 0;
    unsigned    need;

    if( CurrSeg != NULL ) {
        old = CurrSeg->seg;
    }
    need = SegInfo->used + 1;
    if( need > SegInfo->alloc ) {
        ReallocArray( SegInfo, need );
    }
    rec = &_ARRAYOF( SegInfo, index_rec )[SegInfo->used++];
    if( CurrSeg != NULL ) {
        // CurrSeg might have moved on us
        CurrSeg = AskSegIndex( old );
    }
    return( rec );
}

static void DoSegment( segdef *seg, array_control *dgroup_def, array_control *tgroup_def, bool use_16 )
/*****************************************************************************************************/
{
    index_rec   *rec;

    rec = AskSegIndex( seg->id );
    if( rec == NULL ) {
        rec = AllocNewSegRec();
    }
    if( seg->attr & PRIVATE ) {
        rec->private = true;
    } else {
        rec->private = false;
    }
    rec->location = 0;
    rec->big = 0;
    rec->rom = 0;
    rec->need_base_set = true;
    rec->data_in_code = false;
    rec->start_data_in_code = false;
    rec->data_ptr_in_code = false;
    rec->prefix_comdat_state = PCS_OFF;
    rec->max_written = 0;
    rec->max_size = 0;
    rec->comdat_label = NULL;
    rec->comdat_symbol = NULL;
    rec->total_comdat_size = 0;
    rec->comdat_size = 0;
    rec->virt_func_refs = NULL;
    rec->data_prefix_size = 0;
    rec->comdat_prefix_import = NOT_IMPORTED;
    rec->sidx = ++SegmentIndex;
    rec->seg = seg->id;
    rec->attr = SegmentAttr( seg->align, seg->attr, use_16 );
    if( seg->attr & EXEC ) {
        rec->exec = true;
        rec->rom = true;   /* code is alway ROM */
        rec->cidx = _NIDX_CODE;
        if( seg->attr & GIVEN_NAME ) {
            rec->nidx = GetNameIdx( seg->str, "", true );
        } else if( CodeGroupGIdx != 0 ) {
            rec->nidx = GetNameIdx( CodeGroup, seg->str, true );
        } else if( _IsTargetModel( BIG_CODE ) ) {
            rec->nidx = GetNameIdx( FEModuleName(), seg->str, true );
        } else {
            rec->nidx = GetNameIdx( seg->str, "", true );
        }
        if( CodeGroupGIdx != 0 ) {
            rec->base = CodeGroupGIdx;
            rec->btype = BASE_GRP;
        } else {
            rec->base = SegmentIndex;
            rec->btype = BASE_SEG;
        }
    } else {
        rec->exec = false;
        if( seg->attr & ROM )
            rec->rom = true;
        if( seg->attr & PRIVATE ) {
            rec->nidx = GetNameIdx( seg->str, "", true );
            if( seg->attr & ROM ) {
                if( PrivateIndexRO == 0 ) {
                    PrivateIndexRO = GetNameIdx( "FAR_CONST", "", true );
                }
                rec->cidx = PrivateIndexRO;
            } else {
                if( PrivateIndexRW == 0 ) {
                    PrivateIndexRW = GetNameIdx( "FAR_DATA", "", true );
                }
                rec->cidx = PrivateIndexRW;
            }
            rec->base = SegmentIndex;
            rec->btype = BASE_SEG;
        } else {
            if( seg->attr & THREAD_LOCAL ) {
                if( TLSGIndex == 0 ) {
                    OutIdx( _NIDX_TLS, tgroup_def );
                }
                OutGroup( rec->sidx, tgroup_def, &TLSGIndex );
                rec->base = TLSGIndex;
                rec->btype = BASE_GRP;
            } else if( NoDGroup ) {
                rec->btype = BASE_SEG;
                rec->base = SegmentIndex;
            } else {
                OutGroup( rec->sidx, dgroup_def, &DGroupIndex );
                rec->base = DGroupIndex;
                rec->btype = BASE_GRP;
            }
            if( seg->attr & THREAD_LOCAL ) {
                rec->cidx = _NIDX_TLS;
            } else if( seg->attr & COMMON ) {
                rec->cidx = _NIDX_DATA;
            } else if( ( seg->attr & ( BACK | INIT ) ) == 0 ) {
                rec->cidx = _NIDX_BSS;
            } else {
                rec->cidx = _NIDX_DATA;
            }
            rec->nidx = GetNameIdx( DataGroup, seg->str, true );
        }
    }
    SegmentClass( rec );
    DoASegDef( rec, use_16 );
    if( (seg->attr & EXEC) && CodeGroupGIdx != 0 ) {
        OutCGroup( rec->sidx, &rec->obj->data );
    }
    CGFree( seg->str );
    CGFree( seg );
}


extern  void    DefSegment( segment_id id, seg_attr attr, const char *str, uint align, bool use_16 )
/**************************************************************************************************/
{
    segdef              *new;
    segdef              **owner;
    segment_id          first_code;

    new = CGAlloc( sizeof( segdef ) );
    new->id = id;
    new->attr = attr;
    new->align = align;
    new->str = CGAlloc( Length( str ) + 1 );
    CopyStr( str, new->str );
    for( owner = &SegDefs; *owner != NULL; ) {
        owner = &(*owner)->next;
    }
    first_code = BACKSEGS;
    *owner = new;
    new->next = NULL;
    if( attr & EXEC ) {
        if( CodeSeg == BACKSEGS ) {
            CodeSeg = id;
            first_code = id;
        }
        if( OptForSize == 0 && new->align < 16 ) {
            new->align = 16;
        }
    }
    if( attr & BACK ) {
        BackSeg = id;
    }
    if( NameIndex != 0 ) {    /* already dumped out segments*/
        DoSegment( new, NULL, NULL, use_16 ); /* don't allow DGROUP after BEStart */
        SegDefs = NULL;
    }
    if( first_code != BACKSEGS && _IsModel( DBG_DF ) ) {
        DFBegCCU( first_code, NULL );
    }
}

static void DoEmptyQueue( void )
/******************************/
{
    EmptyQueue();
    TellUnreachLabels();
}

static  void    InitFPPatches( void )
/***********************************/
{
    int i;

    for( i = FPP_NUMBER_OF_TYPES; i-- > 0; ) {
        FPPatchImp[i] = NOT_IMPORTED;
    }
}

static  void    OutName( const char *name, void *dst )
/****************************************************/
{
    int             len;
    array_control   *dest = dst;

    len = Length( name );
    if( len >= 256 ) {
        len = 255;
        FEMessage( MSG_INFO_PROC, "Code generator truncated name, its length exceeds allowed maximum." );
        FEMessage( MSG_INFO_PROC, (pointer)name );
    }
    OutByte( len, dest );
    OutBuffer( name, len, dest );
}

static  void    OutObjectName( cg_sym_handle sym, array_control *dest )
/******************************************************************/
{
    DoOutObjectName( sym, OutName, dest, NORMAL );
}

static  void    OutString( const char *name, array_control *dest )
/****************************************************************/
{
    int len;

    len = Length( name );
    OutBuffer( name, len, dest );
}

extern char GetMemModel( void )
/*****************************/
{
    char model;

    if( _IsTargetModel( BIG_CODE ) ) {
        if( _IsTargetModel( BIG_DATA ) ) {
            if( _IsntTargetModel( CHEAP_POINTER ) ) {
                model = 'h';
            } else {
                model = 'l';
            }
        } else {
            model = 'm';
        }
    } else if( _IsTargetModel( BIG_DATA ) ) {
        model = 'c';
    } else if( _IsTargetModel( FLAT_MODEL ) ) {
        model = 'f';
    } else {
        model = 's';
    }
    return( model );
}

static void OutModel( array_control *dest )
/*****************************************/
{
    char                model[6];

    if( _CPULevel( CPU_386 ) ) {
        model[0] = '3';
    } else if( _CPULevel( CPU_286 ) ) {
        model[0] = '2';
    } else {
        model[0] = '0';
    }
    model[1] = GetMemModel();
    model[2] = 'O';     /* Why? Ask microsoft! */
    if( _FPULevel( FPU_87 ) ) {
        if( _IsEmulation() ) {
            model[3] = 'e';
        } else {
            model[3] = 'p';
        }
    } else {
        model[3] = 'c';
    }
    model[4] = 'd';
    if( _IsModel( POSITION_INDEPENDANT ) ) {
        model[4] = 'i';
    }
    model[5] = '\0';
    OutString( model, dest );
}

extern segment_id DbgSegDef( const char *seg_name, const char *seg_class, int seg_modifier )
/******************************************************************************/
{
    index_rec   *rec;

    rec = AllocNewSegRec();
    rec->sidx = ++SegmentIndex;
    rec->cidx = GetNameIdx( seg_class, "", true );
    rec->nidx = GetNameIdx( seg_name, "", true );
    rec->location = 0;
    rec->big = 0;
    rec->need_base_set = true;
    rec->data_ptr_in_code = false;
    rec->data_in_code = false;
    rec->start_data_in_code = false;
    rec->private = true;
    rec->exec = false;
    rec->rom = false;
    rec->prefix_comdat_state = PCS_OFF;
    rec->max_written = 0;
    rec->max_size = 0;
    rec->comdat_label = NULL;
    rec->comdat_symbol = NULL;
    rec->total_comdat_size = 0;
    rec->comdat_nidx = 0;
    rec->comdat_size = 0;
    rec->virt_func_refs = NULL;
    rec->seg = --BackSegIdx;
    rec->attr = SEG_ALGN_BYTE | seg_modifier;
    rec->data_prefix_size = 0;
    rec->comdat_prefix_import = NOT_IMPORTED;
    rec->base = rec->sidx;
    rec->btype = BASE_SEG;
    DoASegDef( rec, true );
    return( rec->seg );
}

static  void    DoSegGrpNames( array_control *dgroup_def, array_control *tgroup_def )
/***********************************************************************************/
{
    segdef      *seg;
    segdef      *next;
    char        *dgroup;
    omf_idx     dgroup_idx;

    GetNameIdx( "", "", true );     // _NIDX_NULL
    GetNameIdx( "CODE", "", true ); // _NIDX_CODE
    GetNameIdx( "DATA", "", true ); // _NIDX_DATA
    GetNameIdx( "BSS", "", true );  // _NIDX_BSS
    GetNameIdx( "TLS", "", true );  // _NIDX_TLS

#ifdef _OMF_32
    if( _IsTargetModel( FLAT_MODEL ) && _IsntTargetModel( EZ_OMF ) ) {
        FlatNIndex = GetNameIdx( "FLAT", "", true );
    }
#endif
    SegmentIndex = 0;
    PrivateIndexRO = 0;
    PrivateIndexRW = 0;
    CopyStr( FEAuxInfo( NULL, CODE_GROUP ), CodeGroup );
    if( CodeGroup[0] == NULLCHAR ) {
        CodeGroupGIdx = 0;
    } else {
        CodeGroupNIdx = GetNameIdx( CodeGroup, "", true );
        CodeGroupGIdx = ++GroupIndex;
    }
    dgroup = FEAuxInfo( NULL, DATA_GROUP );
    if( dgroup == NULL ) {
        NoDGroup = true;
    } else {
        CopyStr( dgroup, DataGroup );
    }
    if( DataGroup[0] != NULLCHAR ) {
        TargetModel |= FLOATING_SS;
        dgroup_idx = GetNameIdx( DataGroup, "_GROUP", true );
    } else {
        dgroup_idx = GetNameIdx( "DGROUP", "", true );
    }
    OutIdx( dgroup_idx, dgroup_def );
    SegInfo = InitArray( sizeof( index_rec ), MODEST_INFO, INCREMENT_INFO );
    for( seg = SegDefs; seg != NULL; seg = next ) {
        next = seg->next;
        DoSegment( seg, dgroup_def, tgroup_def, false );
    }
    SegDefs = NULL;
    if( _IsModel( DBG_DF ) ) {
        if( _IsModel( DBG_LOCALS | DBG_TYPES ) ) {
            DFDefSegs();
        }
    } else if( _IsModel( DBG_CV ) ) {
        CVDefSegs();
    } else {
        DbgTypeSize = 0;
        if( _IsModel( DBG_LOCALS ) ) {
            DbgLocals = DbgSegDef( DbgSegs[0].seg_name, DbgSegs[0].class_name, SEG_COMB_PRIVATE );
        }
        if( _IsModel( DBG_TYPES ) ) {
            DbgTypes = DbgSegDef( DbgSegs[1].seg_name, DbgSegs[1].class_name, SEG_COMB_PRIVATE );
        }
    }
}


static  void    KillStatic( array_control *arr )
/**********************************************/
{
    CGFree( arr->array );
}

static  void    KillArray( array_control *arr )
/*********************************************/
{
    KillStatic( arr );
    CGFree( arr );
}

extern  void    ObjInit( void )
/*****************************/
{
    array_control       *names;         /* for LNAMES*/
    array_control       *dgroup_def;
    array_control       *tgroup_def;
    void                *depend;

    InitFPPatches();
    CodeSize = 0;
    DataSize = 0;
    CurrFNo = 0;
    OpenObj();
    names = InitArray( sizeof( byte ), MODEST_HDR, INCREMENT_HDR );
    OutName( FEAuxInfo( NULL, SOURCE_NAME ), names );
    PutObjOMFRec( CMD_THEADR, names->array, names->used );
    names->used = 0;
#ifdef _OMF_32
    if( _IsTargetModel( EZ_OMF ) || _IsTargetModel( FLAT_MODEL ) ) {
        names->used = 0;
        OutShort( PHAR_LAP_COMMENT, names );
        if( _IsntTargetModel( EZ_OMF ) ) {
            OutString( "OS220", names );
        } else {
            OutString( "80386", names );
        }
        PutObjOMFRec( CMD_COMENT, names->array, names->used );
    }
#else
    OutShort( DEBUG_COMMENT, names );
    PutObjOMFRec( CMD_COMENT, names->array, names->used );
#endif

    names->used = 0;
    OutShort( MODEL_COMMENT, names );
    OutModel( names );
    PutObjOMFRec( CMD_COMENT, names->array, names->used );
    if( _IsTargetModel( FLAT_MODEL ) && _IsModel( DBG_DF ) ) {
        names->used = 0;
        OutShort( LINKER_COMMENT, names );
        OutByte( LDIR_FLAT_ADDRS, names );
        PutObjOMFRec( CMD_COMENT, names->array, names->used );
    }
    if( _IsntModel( DBG_DF | DBG_CV ) ) {
        names->used = 0;
        OutShort( LINKER_COMMENT, names );
        OutByte( LDIR_SOURCE_LANGUAGE, names );
        OutByte( DEBUG_MAJOR_VERSION, names );
        if( _IsModel( DBG_TYPES | DBG_LOCALS ) ) {
            OutByte( DEBUG_MINOR_VERSION, names );
        } else {
            OutByte( 0, names );
        }
        OutString( FEAuxInfo( NULL, SOURCE_LANGUAGE ), names );
        PutObjOMFRec( CMD_COMENT, names->array, names->used );
    }
    names->used = 0;
    depend = NULL;
    for( ;; ) {
        depend = FEAuxInfo( depend, NEXT_DEPENDENCY );
        if( depend == NULL )
            break;
        OutShort( DEPENDENCY_COMMENT, names );
        OutLongInt( *(unsigned_32 *)FEAuxInfo( depend, DEPENDENCY_TIMESTAMP ), names );
        OutName( FEAuxInfo( depend, DEPENDENCY_NAME ), names );
        PutObjOMFRec( CMD_COMENT, names->array, names->used );
        names->used = 0;
    }
    /* mark end of dependancy list */
    OutShort( DEPENDENCY_COMMENT, names );
    PutObjOMFRec( CMD_COMENT, names->array, names->used );
    names->used = 0;

    dgroup_def = InitArray( sizeof( byte ), MODEST_INFO, INCREMENT_INFO );
    tgroup_def = InitArray( sizeof( byte ), MODEST_INFO, INCREMENT_INFO );
    CurrSeg = NULL;
    DoSegGrpNames( dgroup_def, tgroup_def );
    if( dgroup_def->used >= 1 ) {
        FlushNames();
        PutObjOMFRec( CMD_GRPDEF, dgroup_def->array, dgroup_def->used );
    }
    if( tgroup_def->used >= 1 ) {
        FlushNames();
        PutObjOMFRec( CMD_GRPDEF, tgroup_def->array, tgroup_def->used );
    }
    KillArray( tgroup_def );
#ifdef _OMF_32
    dgroup_def->used = 0;
    if( _IsTargetModel( FLAT_MODEL ) && _IsntTargetModel( EZ_OMF ) ) {
        FlatGIndex = ++GroupIndex;
        OutIdx( FlatNIndex, dgroup_def );
        FlushNames();
        PutObjOMFRec( CMD_GRPDEF, dgroup_def->array, dgroup_def->used );
        dgroup_def->used = 0;
    }
#endif
    KillArray( dgroup_def );
    CurrSeg = AskSegIndex( CodeSeg );
    KillArray( names );
    ImportHdl = IMPORT_BASE;
    Imports = NULL;
    GenStaticImports = false;
    AbsPatches = NULL;
    if( _IsModel( DBG_DF ) ) {
        if( _IsModel( DBG_LOCALS | DBG_TYPES ) ) {
            DFObjInitDbgInfo();
#if 0 // save for JimR and linker
        } else if( _IsModel( NUMBERS ) ) {
            DFObjLineInitDbgInfo();
#endif
        }
    } else if( _IsModel( DBG_CV ) ) {
        CVObjInitDbgInfo();
    } else {
        WVObjInitDbgInfo();
    }
}


extern  segment_id  SetOP( segment_id seg )
/*****************************************/
{
    segment_id  old;

    if( CurrSeg == NULL ) {
        old = UNDEFSEG;
    } else {
        old = CurrSeg->seg;
    }
    if( seg == UNDEFSEG ) {
        CurrSeg = NULL;
    } else {
        CurrSeg = AskSegIndex( seg );
    }
    return( old );
}

extern  offset  AskLocation( void )
/*********************************/
{
    return( (offset)CurrSeg->location );
}

extern void ChkDbgSegSize( offset max, bool typing )
/**************************************************/
{
    dbg_seg_info    *info;
    segment_id      old;
    long_offset     curr;

    info = &DbgSegs[typing ? 1 : 0];
    old = SetOP( *info->id );
    curr = (offset)CurrSeg->location;
    if( curr >= max ) {
        if( typing ) {
            DbgTypeSize += curr;
        }
        *info->id = DbgSegDef( info->seg_name, info->class_name, SEG_COMB_PRIVATE );
    }
    SetOP( old );
}


extern  bool    UseImportForm( fe_attr attr )
/*******************************************/
{
    if( attr & (FE_GLOBAL|FE_IMPORT) )
        return( true );
    if( attr & FE_INTERNAL )
        return( false );
    return( true );
}



extern  bool    AskSegNear( segment_id id )
/*****************************************/
{
    index_rec   *rec;

    if( id < 0 )
        return( false );
    rec = AskSegIndex( id );
    if( rec->btype != BASE_GRP )
        return( false );
    if( rec->base > CodeGroupGIdx )
        return( true );
    return( false );
}


extern  bool    AskSegBlank( segment_id id )
/******************************************/
{
    index_rec *rec;

    if( id < 0 )
        return( true );
    rec = AskSegIndex( id );
    return( rec->cidx == _NIDX_BSS );
}


extern  bool    AskSegPrivate( segment_id id )
/********************************************/
{
    index_rec   *rec;

    if( id < 0 )
        return( true );
    rec = AskSegIndex( id );
    return( rec->private || rec->exec );
}


extern  bool    AskSegROM( segment_id id )
/****************************************/
{
    index_rec   *rec;

    if( id < 0 )
        return( false );
    rec = AskSegIndex( id );
    return( rec->rom != 0 );
}


extern  segment_id  AskBackSeg( void )
/************************************/
{
    return( BackSeg );
}


extern  segment_id  AskCodeSeg( void )
/************************************/
{
    return( CodeSeg );
}


extern  bool    HaveCodeSeg( void )
/*********************************/
{
    return( CodeSeg != BACKSEGS );
}


extern  segment_id  AskAltCodeSeg( void )
/***************************************/
{
    return( CodeSeg );
}

static  segment_id  Code16Seg = 0;

extern  segment_id  AskCode16Seg( void )
/**************************************/
{
    if( Code16Seg == 0 ) {
        Code16Seg = --BackSegIdx;
        DefSegment( Code16Seg, EXEC | GIVEN_NAME, "_TEXT16", 16, true );
    }
    return( Code16Seg );
}


static  void    EjectImports( void )
/**********************************/
{
    cmd_omf     cmd;

    if( Imports != NULL && Imports->used != 0 ) {
        if( GenStaticImports ) {
            cmd = CMD_LEXTDEF;
        } else {
            cmd = CMD_EXTDEF;
        }
        PutObjOMFRec( cmd, Imports->array, Imports->used );
        Imports->used = 0;
    }
}


static  void    SetPatches( void )
/********************************/
{
    temp_patch          *curr_pat;
    temp_patch          *next;
    array_control       *ctl;
    obj_patch           *pat;
    unsigned            need;

    for( curr_pat = CurrSeg->obj->patches; curr_pat != NULL; curr_pat = next ) {
        ctl = AskLblPatch( curr_pat->lbl );
        need = ctl->used + 1;
        if( need > ctl->alloc ) {
            ReallocArray( ctl, need );
        }
        pat = &_ARRAYOF( ctl, obj_patch )[ctl->used++];
        pat->ref = AskObjHandle();
        pat->where = curr_pat->pat.where;
        pat->attr = curr_pat->pat.attr;
        next = curr_pat->link;
        CGFree( curr_pat );
    }
}


static  void    SetAbsPatches( void )
/***********************************/
{
    abspatch    *patch;
    object      *obj;

    obj = CurrSeg->obj;
    for( patch = AbsPatches; patch != NULL; patch = patch->link ) {
        if( patch->pat.ref == INVALID_OBJHANDLE && patch->obj == obj ) {
            patch->pat.ref = AskObjHandle();
            patch->flags |= AP_HAVE_OFFSET;
        }
    }
}


static void     EjectLEData( void )
/*********************************/
{
    object      *obj;
    cmd_omf     cmd;

    EjectImports();
    if( CurrSeg->obj->data.used > CurrSeg->data_prefix_size ) {
        SetPatches();
        SetAbsPatches();
        obj = CurrSeg->obj;
        if( CurrSeg->comdat_label != NULL ) {
            cmd = PickOMF( CMD_COMDAT );
        } else {
#ifdef _OMF_32
            cmd = PickOMF( CMD_LEDATA );
#else //SEG32DBG dwarf, codeview
            if( (CurrSeg->attr & SEG_USE_32) && (_IsntTargetModel( EZ_OMF )) ) {
                cmd = CMD_LEDATA32;
            } else {
                cmd = PickOMF( CMD_LEDATA );
            }
#endif
        }
        PutObjOMFRec( cmd, obj->data.array, obj->data.used );
        if( obj->fixes.used != 0 ) {
            if( CurrSeg->data_ptr_in_code ) {
                obj->data.used = 0;
                OutShort( LINKER_COMMENT, &obj->data );
                OutByte( LDIR_OPT_UNSAFE, &obj->data );
                PutObjOMFRec( CMD_COMENT, obj->data.array, obj->data.used );
            }
            PutObjOMFRec( PickOMF( CMD_FIXUPP ), obj->fixes.array, obj->fixes.used );
            obj->fixes.used = 0;
        }
        CurrSeg->data_ptr_in_code = false;
        obj->data.used = 0;
        obj->patches = NULL;
    }
}


static void GetSymLName( const char *name, void *nidx )
/*****************************************************/
{
    *(omf_idx *)nidx = GetNameIdx( name, "", true );
}

static omf_idx NeedComdatNidx( import_type kind )
/***********************************************/
{
    if( CurrSeg->comdat_nidx == 0 ) {
        DoOutObjectName( CurrSeg->comdat_symbol, GetSymLName, &CurrSeg->comdat_nidx, kind );
        FlushNames();
    }
    return( CurrSeg->comdat_nidx );
}


extern  void    OutSelect( bool starts )
/**************************************/
{
    object      *obj;

    if( starts ) {
        selIdx = CurrSeg->sidx;
        SelStart = (offset)CurrSeg->location;
    } else if( selIdx != 0 ) {
        if( !CurrSeg->start_data_in_code ) {
            EjectLEData();
            obj = CurrSeg->obj;
            obj->data.used = 0;
            OutShort( DISASM_COMMENT, &obj->data );
#ifdef _OMF_32
            OutByte( DDIR_SCAN_TABLE_32, &obj->data );
#else
            OutByte( DDIR_SCAN_TABLE, &obj->data );
#endif
            if( CurrSeg->comdat_label != NULL ) {
                OutIdx( 0, &obj->data );
                OutIdx( NeedComdatNidx( NORMAL ), &obj->data );
            } else {
                OutIdx( selIdx, &obj->data );
            }
            OutOffset( SelStart, &obj->data );
            OutOffset( (offset)CurrSeg->location, &obj->data );
            PutObjOMFRec( CMD_COMENT, obj->data.array, obj->data.used );
            obj->data.used = 0;
        }
        selIdx = 0;
    }
    CurrSeg->start_data_in_code = false;
}

static  void    OutLEDataStart( bool iterated )
/*********************************************/
{
    byte        flag;
    index_rec   *rec;
    object      *obj;

    rec = CurrSeg;
    obj = rec->obj;
    if( obj->data.used == 0 ) {
        if( rec->comdat_label != NULL ) {
            flag = 0;
            if( rec->location != 0 )
                flag |= 1; /* continued */
            if( iterated )
                flag |= 2;   /* LIDATA form */
            if( (FEAttr( rec->comdat_symbol ) & FE_GLOBAL) == 0 ) {
                flag |= 0x4;    /* local comdat */
            }
            OutByte( flag, &obj->data );
            OutByte( 0x10, &obj->data );
            OutByte( 0, &obj->data );
            OutOffset( (offset)rec->location, &obj->data );
            OutIdx( 0, &obj->data );
            if( rec->btype == BASE_GRP ) {
                OutIdx( CurrSeg->base, &obj->data );   /* group index*/
            } else {
                OutIdx( 0, &obj->data );
            }
            OutIdx( rec->sidx, &obj->data );   /* segment index*/
            OutIdx( NeedComdatNidx( NORMAL ), &obj->data );
        } else { // LEDATA
            OutIdx( rec->sidx, &obj->data );
#ifdef _OMF_32
            OutOffset( (offset)rec->location, &obj->data );
#else  //SEG32DBG dwarf, codeview
            if( (rec->attr & SEG_USE_32 ) ) {
                OutLongOffset( rec->location, &obj->data );
            } else {
                OutOffset( (offset)rec->location, &obj->data );
            }
#endif
        }
        obj->start = rec->location;
        rec->data_prefix_size = obj->data.used;
    }
    if( rec->start_data_in_code ) {
        OutSelect( true );
    }
}


static  void    CheckLEDataSize( unsigned max_size, bool need_init )
/******************************************************************/
{
    long_offset     start;
    unsigned        used;
    object          *obj;
    long_offset     end_valid;

    obj = CurrSeg->obj;
    start = obj->start;
    used = obj->data.used;
    if( CurrSeg->location < start ) {
        EjectLEData();
    } else if( CurrSeg->location - start + max_size > BUFFSIZE - TOLERANCE - CurrSeg->data_prefix_size ) {
        EjectLEData();
    } else {
        end_valid = start + used - CurrSeg->data_prefix_size;
        if( CurrSeg->max_written > end_valid && CurrSeg->location > end_valid ) {
            EjectLEData();
        }
    }
    if( need_init ) {
        OutLEDataStart( false );
    }
}

extern  void    SetUpObj( bool is_data )
/**************************************/
{
    object      *obj;
    bool        old_data;

    obj = CurrSeg->obj;
    if( obj == NULL )
        return;
    if( obj->fixes.used >= BUFFSIZE - TOLERANCE ) {
        EjectLEData();
        return;
    }
    if( (Imports != NULL) && (Imports->used >= BUFFSIZE - TOLERANCE) ) {
         EjectLEData();
         return;
    }
    /* so that a call will always fit */
    CheckLEDataSize( 4 * sizeof( offset ), false );
    if( CurrSeg->exec ) {
        old_data = ( CurrSeg->data_in_code != 0 );
        CurrSeg->data_in_code = is_data;
        if( is_data != old_data ) {
            if( is_data ) {
                CurrSeg->start_data_in_code = true;
            } else {
                OutSelect( false );
                SetUpObj( false );
            }
        }
    }
}


static  void    GenComdef( void )
/*******************************/
{
    array_control       *comdef;
    unsigned            count;
    unsigned_8          ind;
    unsigned            size;
    cg_sym_handle       sym;
    cmd_omf             cmd;

    if( CurrSeg->comdat_label != NULL &&
        CurrSeg->max_written < CurrSeg->comdat_size ) {
        if( CurrSeg->max_written != 0 ) {
            Zoiks( ZOIKS_080 );
        }
        /* have to eject any pending imports here or the ordering
           gets messed up */
        EjectImports();
        size = CurrSeg->comdat_size - CurrSeg->max_written;
        comdef = InitArray( sizeof( byte ), MODEST_EXP, INCREMENT_EXP );
        sym = CurrSeg->comdat_symbol;
        OutObjectName( sym, comdef );
        OutByte( 0, comdef );                  /* type index */
        if( CurrSeg->btype == BASE_GRP && CurrSeg->base == DGroupIndex ) {
            OutByte( COMDEF_NEAR, comdef );    /* common type */
        } else {
            OutByte( COMDEF_FAR, comdef );     /* common type */
            OutByte( 1, comdef );              /* number of elements */
        }
        /*
            Strictly speaking, this should be <= 0x80. However a number
            of tools (including our own!) have problems with doing a
            128 byte COMDEF size in 1 byte, so we'll waste some space
            and use 2 bytes, but sleep much sounder at night.
        */
        if( size < 0x80 ) {
            count = 1;
            ind = 0;
        } else if( size < 0x1000 ) {
            count = 2;
            ind = COMDEF_LEAF_2;
        } else if( size < 0x100000 ) {
            count = 3;
            ind = COMDEF_LEAF_3;
        } else {
            count = 4;
            ind = COMDEF_LEAF_4;
        }
        if( ind != 0 ) {
            /* multi-byte indicator */
            OutByte( ind, comdef );
        }
        do {
            /* element size */
            OutByte( (byte)size, comdef );
            size >>= 8;
            --count;
        } while( count != 0 );
        if( FEAttr( sym ) & FE_GLOBAL ) {
            cmd = CMD_COMDEF;
        } else {
            cmd = CMD_LCOMDEF;
        }
        PutObjOMFRec( cmd, comdef->array, comdef->used );
        KillArray( comdef );
        TellImportHandle( sym, ImportHdl++ );
    }
}

static  void    EjectExports( void )
/**********************************/
{
    object      *obj;
    cmd_omf     cmd;

    obj = CurrSeg->obj;
    if( obj->exports != NULL && obj->exports->used != 0 ) {
        if( obj->gen_static_exports ) {
            cmd = CMD_LPUBDEF;
        } else {
            cmd = CMD_PUBDEF;
        }
        PutObjOMFRec( PickOMF( cmd ), obj->exports->array, obj->exports->used );
        obj->exports->used = 0;
    }
}


static  void    FlushLineNum( object *obj )
/*****************************************/
{
    cmd_omf     cmd;

    if( obj->line_info ) {
        if( CurrSeg->comdat_label != NULL ) {
            cmd = CMD_LINSYM;
        } else {
            cmd = CMD_LINNUM;
        }
        PutObjOMFRec( PickOMF( cmd ), obj->lines->array, obj->lines->used );
        obj->lines->used = 0;
        obj->lines_generated = 1;
        obj->line_info = false;
    }
}


static  void    FlushObject( void )
/*********************************/
{
    object      *obj;

    SetUpObj( false );
    GenComdef();
    CurrSeg->total_comdat_size += CurrSeg->comdat_size;
    EjectLEData();
    EjectExports();
    obj = CurrSeg->obj;
    if( obj->lines != NULL ) {
        FlushLineNum( obj );
        KillArray( obj->lines );
        obj->lines = NULL;
    }
    FreeObjCache();
}


static  index_rec       *AskIndexRec( unsigned_16 sidx )
/******************************************************/
{
    index_rec   *rec;
    unsigned    i;

    rec = SegInfo->array;
    for( i = 0; i < SegInfo->used; ++i ) {
        if( rec->sidx == sidx ) {
            break;
        }
        rec++;
    }
    return( rec );
}

#if _TARGET & _TARG_IAPX86
#define _TargetInt _TargetShort
#else
#define _TargetInt _TargetBigInt
#endif

static  void    FiniTarg( void )
/******************************/
{
    union {
        offset           s;
        long_offset      l;
    } size;
    byte        attr;
    index_rec   *rec;
    object      *obj;

    FlushObject();
    obj = CurrSeg->obj;
    if( obj->exports != NULL ) {
        KillArray( obj->exports );
        obj->exports = NULL;
    }
    rec = AskIndexRec( obj->index );
#ifdef _OMF_32
    size.s = _TargetInt( rec->max_size );
#else //SEG32DBG dwarf, codeview
    if( rec->attr & SEG_USE_32 ) {
        size.l = _TargetLongInt( rec->max_size );
    } else {
        size.s = _TargetInt( rec->max_size );
    }
#endif
    if( rec->exec ) {
        CodeSize += rec->max_size + rec->total_comdat_size;
    } else if( rec->cidx == _NIDX_DATA ) {
        DataSize += rec->max_size + rec->total_comdat_size;
    }
    if( rec->big ) {
        attr = rec->attr | SEG_BIG;
        PatchObj( obj->segfix, SEGDEF_ATTR, &attr, sizeof( byte ) );
    } else {
#ifdef _OMF_32
        PatchObj( obj->segfix, SEGDEF_SIZE, (byte *)&size.s, sizeof( offset ) );
#else   //SEG32DBG dwarf, codeview
        if( rec->attr & SEG_USE_32 ) {
            PatchObj( obj->segfix, SEGDEF_SIZE, (byte *)&size.l, sizeof( long_offset ) );
        } else {
            PatchObj( obj->segfix, SEGDEF_SIZE, (byte *)&size.s, sizeof( offset ) );
        }
#endif
    }
    if( obj->exports != NULL ) {
        KillArray( obj->exports );
    }
    KillStatic( &obj->data );
    KillStatic( &obj->fixes );
    CGFree( obj );
}

extern  void    FlushOP( segment_id id )
/**************************************/
{
    segment_id  old;
    index_rec   *rec;

    old = SetOP( id );
    if( id == CodeSeg ) {
        DoEmptyQueue();
    }
    if( _IsModel( DBG_DF ) ) {
        rec = CurrSeg;
        if( rec->exec || rec->cidx == _NIDX_DATA ||rec->cidx == _NIDX_BSS ) {
            if( rec->max_size != 0 ) {
                DFSegRange();
            }
        }
    }
    FiniTarg();
    CurrSeg->obj = NULL;
    SetOP( old );
}

static void FiniWVTypes( void )
/*****************************/
{
    segment_id   old;
    long_offset  curr;
    dbg_seg_info *info;

    WVTypesEof();
    info = &DbgSegs[1];
    old = SetOP( *info->id );
    curr = (offset)CurrSeg->location;
    curr += DbgTypeSize;
    *info->id = DbgSegDef( info->seg_name, info->class_name, SEG_COMB_PRIVATE );
    SetOP( *info->id );
    WVDmpCueInfo( curr );
    SetOP( old );
}

static void FlushSelect( void )
/*****************************/
{
    if( selIdx != 0 ) {
        OutSelect( false );
    }
}


static  void    FlushData( void )
/*******************************/
{
    GenComdef();
    CurrSeg->total_comdat_size += CurrSeg->comdat_size;
    EjectLEData();
    FlushLineNum( CurrSeg->obj );
    FlushSelect();
}

static  void    NormalData( void )
/********************************/
{
    FlushData();
    CurrSeg->location = CurrSeg->max_written = CurrSeg->max_size;
    CurrSeg->comdat_size = 0;
    CurrSeg->comdat_label = NULL;
    CurrSeg->comdat_symbol = NULL;
    CurrSeg->need_base_set = true;
    CurrSeg->prefix_comdat_state = PCS_OFF;
    KillLblRedirects();
}


static void DoSegARange( offset *codesize, index_rec *rec )
/*********************************************************/
{
    segment_id  old;

    if( rec->exec || rec->cidx == _NIDX_DATA ||rec->cidx == _NIDX_BSS ) {
        if( rec->max_size != 0 ) {
            old = SetOP( rec->seg );
            if( CurrSeg->comdat_symbol != NULL ) {
                DFSymRange( rec->comdat_symbol, (offset)rec->comdat_size );
            }
            NormalData();
            DFSegRange();
            SetOP( old );
        }
        if( rec->exec ) {
            *codesize += rec->max_size + rec->total_comdat_size;
        }
    }
}

static  void    DoPatch( obj_patch *pat, offset lc )
/**************************************************/
{
    unsigned_32 lword_val;
    unsigned_16 word_val;
    byte        byte_val;

    if( pat->attr & LONG_PATCH ) {
        if( pat->attr & ADD_PATCH ) {
            GetFromObj( pat->ref, pat->where, (byte *)&lword_val, sizeof( lword_val ) );
            _TargetAddL( lword_val, lc );
        } else {
            lword_val = _TargetLongInt( lc );
        }
        PatchObj( pat->ref, pat->where, (byte *)&lword_val, sizeof( lword_val ) );
    } else if( pat->attr & WORD_PATCH ) {
        if( pat->attr & ADD_PATCH ) {
            GetFromObj( pat->ref, pat->where, (byte *)&word_val, sizeof( word_val ) );
            _TargetAddW( word_val, lc );
        } else {
            word_val = _TargetShort( lc );
        }
        PatchObj( pat->ref, pat->where, (byte *)&word_val, sizeof( word_val ) );
    } else {
        if( pat->attr & ADD_PATCH ) {
            GetFromObj( pat->ref, pat->where, (byte *)&byte_val, sizeof( byte_val ) );
            byte_val += lc;
        } else {
            byte_val = lc;
        }
        PatchObj( pat->ref, pat->where, (byte *)&byte_val, sizeof( byte_val ) );
    }
}



static  void    FiniAbsPatches( void )
/************************************/
{
    abspatch    *patch;
    abspatch    *next;

    for( patch = AbsPatches; patch != NULL; patch = next ) {
        next = patch->link;
        DoPatch( &patch->pat, (offset)patch->value );
        CGFree( patch );
    }
}


static  void    EndModule( void )
/*******************************/
{
    byte        b;

    b = 0;                     /* non-main module, no start address*/
//  There is a bug in MS's LINK386 program that causes it not to recognize a
//  MODEND386 record in some situations. We can get around it by only outputing
//  16-bit MODEND records. This causes us no pain, since we never need any
//  features provided by the 32-bit form anyway. --- BJS
//    PutObjOMFRec( PickOMF( CMD_MODEND ), &b, sizeof( byte ) );

    PutObjOMFRec( CMD_MODEND, &b, sizeof( byte ) );
}


extern  void    ObjFini( void )
/*****************************/
{

    index_rec   *rec;
    unsigned    i;
    pointer     auto_import;
    char        *lib;
    char        *alias;

    if( _IsModel( DBG_DF ) ) {
        if( _IsModel( DBG_LOCALS | DBG_TYPES ) ) {
            offset  codesize;

            codesize = 0;
            rec = SegInfo->array;
            for( i = 0; i < SegInfo->used; ++i ) {
                if( rec->obj != NULL ) {
                    DoSegARange( &codesize, rec );
                }
                rec++;
            }
            DFObjFiniDbgInfo( codesize );
#if 0 //save for Jimr
        } else if( _IsModel( NUMBERS ) ) {
            DFObjLineFini( );
#endif
        }
    } else if( _IsModel( DBG_CV ) ) {
        CVObjFiniDbgInfo();
    } else {
        if( _IsModel( DBG_TYPES ) ) {
            FiniWVTypes();
        }
        WVObjFiniDbgInfo();
    }
    rec = SegInfo->array;
    for( i = 0; i < SegInfo->used; ++i ) {
        if( rec->obj != NULL ) {
            CurrSeg = rec;
            FiniTarg();
        }
        rec++;
    }
    if( Imports == NULL ) {
        Imports = InitArray( sizeof( byte ), 20, 20 );
    }
    if( Used87 ) {
        (void)FEAuxInfo( NULL, USED_8087 );
    }
    auto_import = NULL;
    for(;;) {
        auto_import = FEAuxInfo( auto_import, NEXT_IMPORT );
        if( auto_import == NULL )
            break;
        OutName( FEAuxInfo( auto_import, IMPORT_NAME ), Imports );
        OutIdx( 0, Imports );           /* type index*/
        if( Imports->used >= BUFFSIZE - TOLERANCE ) {
            PutObjOMFRec( CMD_EXTDEF, Imports->array, Imports->used );
            Imports->used = 0;
        }
    }
    auto_import = NULL;
    for(;;) {
        auto_import = FEAuxInfo( auto_import, NEXT_IMPORT_S );
        if( auto_import == NULL )
            break;
        OutObjectName( FEAuxInfo( auto_import, IMPORT_NAME_S ), Imports );
        OutIdx( 0, Imports );           /* type index*/
        if( Imports->used >= BUFFSIZE - TOLERANCE ) {
            PutObjOMFRec( CMD_EXTDEF, Imports->array, Imports->used );
            Imports->used = 0;
        }
    }
    if( Imports->used != 0 ) {
        PutObjOMFRec( CMD_EXTDEF, Imports->array, Imports->used );
        Imports->used = 0;
    }
    /* Emit default library search records. */
    lib = NULL;
    for( ;; ) {
        lib = FEAuxInfo( lib, NEXT_LIBRARY );
        if( lib == NULL )
            break;
        OutShort( LIBNAME_COMMENT, Imports );
        OutString( ( (char *)FEAuxInfo( lib, LIBRARY_NAME ) ) + 1, Imports );
        PutObjOMFRec( CMD_COMENT, Imports->array, Imports->used );
        Imports->used = 0;
    }
    /* Emit alias definition records. */
    alias = NULL;
    for( ;; ) {
        char    *alias_name;
        char    *subst_name;

        alias = FEAuxInfo( alias, NEXT_ALIAS );
        if( alias == NULL )
            break;
        alias_name = FEAuxInfo( alias, ALIAS_NAME );
        if( alias_name == NULL ) {
            OutObjectName( FEAuxInfo( alias, ALIAS_SYMBOL ), Imports );
        } else {
            OutName( alias_name, Imports );
        }
        subst_name = FEAuxInfo( alias, ALIAS_SUBST_NAME );
        if( subst_name == NULL ) {
            OutObjectName( FEAuxInfo( alias, ALIAS_SUBST_SYMBOL ), Imports );
        } else {
            OutName( subst_name, Imports );
        }
        PutObjOMFRec( CMD_ALIAS, Imports->array, Imports->used );
        Imports->used = 0;
    }

    KillArray( Imports );
    Imports = NULL;
    KillArray( SegInfo );
    FiniAbsPatches();
    EndModule();
    CloseObj();
    FEMessage( MSG_CODE_SIZE, (pointer)(pointer_int)CodeSize );
    FEMessage( MSG_DATA_SIZE, (pointer)(pointer_int)DataSize );
}


static  void    FreeAbsPatch( abspatch *patch )
/*********************************************/
{
    abspatch    **owner;

    for( owner = &AbsPatches; *owner != patch; ) {
        owner = &(*owner)->link;
    }
    *owner = (*owner)->link;
    CGFree( patch );
}


static  void    OutExport( cg_sym_handle sym )
/*****************************************/
{
    array_control       *exp;
    object              *obj;
    fe_attr             attr;


    obj = CurrSeg->obj;
    exp = obj->exports;
    if( exp == NULL ) {
        exp = InitArray( sizeof( byte ), MODEST_EXP, INCREMENT_EXP );
        obj->exports = exp;
    }
    if( obj->exports->used >= BUFFSIZE - TOLERANCE ) {
        EjectExports();
    }
    attr = FEAttr( sym );
    /* are we switching from global to statics or vis-versa */
    if( obj->gen_static_exports ) {
        if( attr & FE_GLOBAL ) {
            EjectExports();
            obj->gen_static_exports = false;
        }
    } else {
        if( (attr & FE_GLOBAL) == 0 ) {
            EjectExports();
            obj->gen_static_exports = true;
        }
    }
    if( obj->exports->used == 0 ) {
        if( CurrSeg->btype == BASE_GRP ) {
            OutIdx( CurrSeg->base, exp );   /* group index*/
        } else {
#ifdef _OMF_32
            OutIdx( FlatGIndex, exp );      /* will be 0 if we have none */
#else
            OutIdx( 0, exp );
#endif
        }
        OutIdx( obj->index, exp );          /* segment index*/
    }
    OutObjectName( sym, exp );
    OutOffset( (offset)CurrSeg->location, exp );
    OutIdx( 0, exp );                       /* type index*/
}


static  void    CheckImportSwitch( bool next_is_static )
/******************************************************/
{
    /* are we switching from static imports to global or vis-versa */
    if( GenStaticImports ) {
        if( !next_is_static ) {
            EjectImports();
        }
    } else {
        if( next_is_static ) {
            EjectImports();
        }
    }
    GenStaticImports = next_is_static;
}


static void _TellImportHandle( cg_sym_handle sym, import_handle imp_idx, bool alt_dllimp )
/*************************************************************************************/
{
    if( alt_dllimp ) {
        FEBack( sym )->imp_alt = imp_idx;
    } else {
        TellImportHandle( sym, imp_idx );
    }
}

static import_handle _AskImportHandle( cg_sym_handle sym, bool alt_dllimp )
/**********************************************************************/
{
    if( alt_dllimp ) {
        return( FEBack( sym )->imp_alt );
    } else {
        return( AskImportHandle( sym ) );
    }
}

static  omf_idx     GenImport( cg_sym_handle sym, bool alt_dllimp )
/**************************************************************/
{
    omf_idx         imp_idx;
    fe_attr         attr;
    import_type     kind;

    imp_idx = _AskImportHandle( sym, alt_dllimp );
    if( imp_idx == NOT_IMPORTED ) {
        attr = FEAttr( sym );
        imp_idx = ImportHdl++;
        if( Imports == NULL ) {
            Imports = InitArray( sizeof( byte ), MODEST_IMP, INCREMENT_IMP );
        }
        CheckImportSwitch( (attr & FE_GLOBAL) == 0 );
        _TellImportHandle( sym, imp_idx, alt_dllimp );
        kind = NORMAL;
        if( attr & FE_DLLIMPORT ) {
            if( !alt_dllimp ) {
                kind = DLLIMPORT;
            }
        } else if( _IsModel( POSITION_INDEPENDANT ) ) {
            if( ( attr & FE_THREAD_DATA ) != 0 ) {
                kind = PIC_RW;
            }
        }
        DoOutObjectName( sym, OutName, Imports, kind );
        OutIdx( 0, Imports );           /* type index*/
        DumpImportResolve( sym, imp_idx );
    }
    return( imp_idx );
}

static  omf_idx     GenImportComdat( void )
/*****************************************/
{
    if( Imports == NULL ) {
        Imports = InitArray( sizeof( byte ), MODEST_IMP, INCREMENT_IMP );
    }
    CheckImportSwitch( (FEAttr( CurrSeg->comdat_symbol ) & FE_GLOBAL) == 0 );
    DoOutObjectName( CurrSeg->comdat_symbol, OutName, Imports, SPECIAL );
    OutIdx( 0, Imports );           /* type index*/
    return( ImportHdl++ );
}

static  void    ComdatData( label_handle lbl, cg_sym_handle sym )
/************************************************************/
{
    FlushData();
    CurrSeg->obj->lines_generated = 0;
    CurrSeg->location = CurrSeg->max_written = 0;
    CurrSeg->comdat_size = 0;
    CurrSeg->comdat_nidx = 0;
    CurrSeg->comdat_label = lbl;
    if( sym != NULL ) {
        CurrSeg->comdat_symbol = sym;
    } else {
        NeedComdatNidx( SPECIAL );
        CurrSeg->comdat_prefix_import = GenImportComdat();
        TellCommonLabel( lbl, CurrSeg->comdat_prefix_import );
    }
    CurrSeg->need_base_set = true;
    CurrSeg->prefix_comdat_state = PCS_OFF;
    KillLblRedirects();
}

static void     OutVirtFuncRef( cg_sym_handle virt )
/***********************************************/
{
    object      *obj;
    omf_idx     extdef;

    if( virt == NULL ) {
        extdef = NOT_IMPORTED;
    } else {
        extdef = GenImport( virt, false );
    }
    EjectLEData();
    obj = CurrSeg->obj;
    obj->data.used = 0;
    OutShort( LINKER_COMMENT, &obj->data );
    OutByte( LDIR_VF_REFERENCE, &obj->data );
    OutIdx( extdef, &obj->data );
    if( CurrSeg->comdat_symbol != NULL ) {
        OutIdx( 0, &obj->data );
        OutIdx( NeedComdatNidx( NORMAL ), &obj->data );
    } else {
        OutIdx( CurrSeg->sidx, &obj->data );
    }
    PutObjOMFRec( CMD_COMENT, obj->data.array, obj->data.used );
    obj->data.used = 0;
}


extern  void    OutDLLExport( uint words, cg_sym_handle sym )
/********************************************************/
{
    object      *obj;

    words = words;
    SetUpObj( false );
    EjectLEData();
    obj = CurrSeg->obj;
    obj->data.used = 0;
    OutShort( EXPORT_COMMENT, &obj->data );
    OutByte( 2, &obj->data );
#if _TARGET & _TARG_IAPX86
    OutByte( words, &obj->data );
#else
    // this should be 0 for everything except callgates to
    // 16-bit segments (from MS Knowledge Base)
    OutByte( 0, &obj->data );
#endif
    OutObjectName( sym, &obj->data );
    OutByte( 0, &obj->data );
    PutObjOMFRec( CMD_COMENT, obj->data.array, obj->data.used );
    obj->data.used = 0;
}


extern  void    OutLabel( label_handle lbl )
/******************************************/
{
    temp_patch          **owner;
    temp_patch          *curr_pat;
    array_control       *ctl;
    obj_patch           *pat;
    unsigned            i;
    pointer             patptr;
    object              *obj;
    offset              lc;
    cg_sym_handle       sym;
    fe_attr             attr;
    void                *cookie;
    virt_func_ref_list  *curr;
    virt_func_ref_list  *next;

    sym = AskForLblSym( lbl );
    if( sym != NULL ) {
        attr = FEAttr( sym );
        if( ( attr & FE_PROC ) == 0 ) {
            if( attr & FE_DLLEXPORT ) {
                OutDLLExport( 0, sym );
            }
        }
        if( attr & FE_COMMON ) {
            ComdatData( lbl, sym );
        } else {
            if( CurrSeg->comdat_label != NULL )
                NormalData();
            if( UseImportForm( attr ) ) {
                OutExport( sym );
            }
        }
        for( curr = CurrSeg->virt_func_refs; curr != NULL; curr = next ) {
            next = curr->next;
            for( cookie = curr->cookie; cookie != NULL; cookie = FEAuxInfo( cookie, VIRT_FUNC_NEXT_REFERENCE ) ) {
                OutVirtFuncRef( FEAuxInfo( cookie, VIRT_FUNC_SYM ) );
            }
            CGFree( curr );
        }
        CurrSeg->virt_func_refs = NULL;
    } else if( CurrSeg->prefix_comdat_state != PCS_OFF ) {
        /*
            We have data coming out before the COMDAT symbol (select table
            before a common procedure). Gen a 'magical' comdat name to
            deal with it.
        */
        if( CurrSeg->prefix_comdat_state == PCS_NEED ) {
            ComdatData( lbl, NULL );
            CurrSeg->prefix_comdat_state = PCS_ACTIVE;
        }
        TellCommonLabel( lbl, CurrSeg->comdat_prefix_import );
    }
    lc = (offset)CurrSeg->location;
    TellAddress( lbl, lc );
    for( i = SegInfo->used; i-- > 0; ) {
        obj = _ARRAYOF( SegInfo, index_rec )[i].obj;
        if( obj != NULL ) { /* twas flushed and not redefined*/
            owner = &obj->patches;
            for(;;) {
                curr_pat = *owner;
                if( curr_pat == NULL )
                    break;
                if( curr_pat->lbl == lbl ) {
                    patptr = &_ARRAYOF( &obj->data, byte )[curr_pat->pat.where];
                    if( curr_pat->pat.attr & ADD_PATCH ) {
                        if( curr_pat->pat.attr & LONG_PATCH ) {
                            _TargetAddL( *(unsigned_32 *)patptr, lc );
                        } else if( curr_pat->pat.attr & WORD_PATCH ) {
                            _TargetAddW( *(unsigned_16 *)patptr, lc );
                        } else {
                            *(byte *)patptr += lc;
                        }
                    } else {
                        if( curr_pat->pat.attr & LONG_PATCH ) {
                            *(unsigned_32 *)patptr = _TargetLongInt( lc );
                        } else if( curr_pat->pat.attr & WORD_PATCH ) {
                            *(unsigned_16 *)patptr = _TargetShort( lc );
                        } else {
                            *(byte *)patptr = lc;
                        }
                    }
                    *owner = curr_pat->link;
                    CGFree( curr_pat );
                } else {
                    owner = &curr_pat->link;
                }
            }
        }
    }
    ctl = AskLblPatch( lbl );
    pat = ctl->array;
    for( i = ctl->used; i > 0; --i ) {
        DoPatch( pat++, lc );
    }
    KillArray( ctl );
    TellDonePatch( lbl );
}


extern  void    AbsPatch( abspatch_handle patch_handle, offset lc )
/*****************************************************************/
{
    abspatch *patch = (abspatch *)patch_handle;
    if( patch->flags & AP_HAVE_OFFSET ) {
        DoPatch( &patch->pat, lc );
        FreeAbsPatch( patch );
    } else {
        patch->value = lc;
        patch->flags |= AP_HAVE_VALUE;
    }
}


extern  void    *InitPatch( void )
/********************************/
{
    return( InitArray( sizeof( obj_patch ),  MODEST_PAT, INCREMENT_PAT ) );
}


static omf_fix_class getOMFFixClass( fix_class class )
/****************************************************/
{
#if _TARGET & _TARG_80386
    if( class & F_FAR16 ) {
        /* want a 16:16 fixup for a __far16 call */
        return( OFC_PTR );
    }
#endif
    switch( F_CLASS( class ) ) {
    case F_BASE:
        return( OFC_BASE );
#if _TARGET & _TARG_IAPX86
    case F_OFFSET:
        return( OFC_OFFSET );
    case F_BIG_OFFSET:
        return( OFC_MS_OFFSET_32 );
    case F_LDR_OFFSET:
        return( OFC_LDR_OFFSET );
    case F_PTR:
        return( OFC_PTR );
#else
    case F_OFFSET:
    case F_BIG_OFFSET:
        if( _IsTargetModel( EZ_OMF ) ) {
            return( OFC_PHAR_OFFSET );
        } else {
            return( OFC_MS_OFFSET_32 );
        }
    case F_LDR_OFFSET:
        if( _IsTargetModel( EZ_OMF ) ) {
            return( OFC_PHAR_OFFSET );
        } else {
            return( OFC_MS_LDR_OFFSET_32 );
        }
    case F_PTR:
        if( _IsTargetModel( EZ_OMF ) ) {
            return( OFC_PHAR_PTR );
        } else {
            return( OFC_MS_PTR );
        }
#endif
    default:
        break;
    }
    // error
    return( 0 );
}


static void DoFix( omf_idx idx, bool rel, base_type base, fix_class class, omf_idx sidx )
/***************************************************************************************/
{
    fixup       *cursor;
    int         where;
    object      *obj;
    index_rec   *rec;
    byte        b;
    unsigned    need;

    b = rel ? LOCAT_REL : LOCAT_ABS;
    if( F_CLASS( class ) == F_PTR && CurrSeg->data_in_code ) {
        CurrSeg->data_ptr_in_code = true;
    }
    obj = CurrSeg->obj;
    need = obj->fixes.used + sizeof( fixup );
    if( need > obj->fixes.alloc ) {
        ReallocArray( &obj->fixes, need );
    }
    cursor = &_ARRAY( &obj->fixes, fixup );
    obj->fixes.used = need;
    where = CurrSeg->location - obj->start;
    cursor->locatof = b + ( getOMFFixClass( class ) << S_LOCAT_LOC ) + ( where >> 8 );
    cursor->fset = where;
    if( base != BASE_IMP ) {
        rec = AskIndexRec( sidx );
        /*
           fixups to a code segment that is currently in COMDAT mode must be
           done as imports relative to the comdat symbol.
        */
        if( rec->exec && rec->comdat_symbol != NULL ) {
            idx = GenImport( rec->comdat_symbol, false );
            base = BASE_IMP;
        }
    }
#ifdef _OMF_32
    if( _IsTargetModel( FLAT_MODEL ) && _IsntTargetModel( EZ_OMF ) && ( F_CLASS( class ) != F_PTR) ) {
        omf_idx     grp_idx;

  #if 0
        /* only generate a normal style fixup for now */
        if( class & F_TLS ) {
            grp_idx = TLSGIndex;
        } else {
            grp_idx = FlatGIndex;
        }
  #else
        grp_idx = FlatGIndex;
  #endif
        if( base == BASE_GRP ) {
            cursor->fixdat = FIXDAT_FRAME_GROUP + FIXDAT + BASE_SEG;
            OutIdx( grp_idx, &obj->fixes );
            OutIdx( sidx, &obj->fixes );
        } else {
            cursor->fixdat = FIXDAT_FRAME_GROUP + FIXDAT + base;
            OutIdx( grp_idx, &obj->fixes );
            OutIdx( idx, &obj->fixes );
        }
    } else
#endif
    if( base == BASE_GRP ) {
        cursor->fixdat = FIXDAT_FRAME_GROUP + FIXDAT + BASE_SEG;
        OutIdx( idx, &obj->fixes );
        OutIdx( sidx, &obj->fixes );
    } else {
        cursor->fixdat = FIXDAT_FRAME_IMPLIED + FIXDAT + base;
        OutIdx( idx, &obj->fixes );
    }
}


extern  void    SetBigLocation( long_offset loc )
/***********************************************/
{
    CurrSeg->location = loc;
    if( CurrSeg->comdat_label != NULL ) {
        if( loc > CurrSeg->comdat_size ) {
            CurrSeg->comdat_size = loc;
        }
    } else {
        if( loc > CurrSeg->max_size ) {
            CurrSeg->max_size = loc;
        }
    }
}

extern  void    IncLocation( offset by )
/**************************************/
{
    long_offset     sum;

    CurrSeg->obj->pending_line_number = 0;
    sum = CurrSeg->location + by;
    if( _IsntTargetModel( EZ_OMF ) && (CurrSeg->attr & SEG_USE_32) == 0 ) {
        sum &= 0xFFFF;
    }
    if( sum < CurrSeg->location ) { /* if wrapped*/
        if( sum != 0 || CurrSeg->big == 1 ) {
            FatalError( "segment too large" );
        } else if( CurrSeg->comdat_label == NULL ) {
            CurrSeg->big = 1;
            if( CurrSeg->attr & SEG_USE_32  ) {
                CurrSeg->max_size = (long_offset)(-1);
                CurrSeg->location = (long_offset)(-1);
            } else {
                CurrSeg->max_size = (short_offset)(-1);
                CurrSeg->location =  1L << 8 * sizeof( short_offset );
            }
        }
    } else {
        SetBigLocation( sum );
    }
}


extern  void    SetLocation( offset loc )
/***************************************/
{
    CurrSeg->location = loc;
    if( CurrSeg->comdat_label != NULL ) {
        if( loc > CurrSeg->comdat_size ) {
            CurrSeg->comdat_size = loc;
        }
    } else {
        if( loc > CurrSeg->max_size ) {
            CurrSeg->max_size = loc;
        }
    }
}

static  void    DecLocation( offset by )
/**************************************/
{
    SetLocation( (offset)CurrSeg->location - by );
}

extern  void    OutFPPatch( fp_patches i )
/****************************************/
{
    omf_idx     idx;

    idx = FPPatchImp[i];
    if( idx == NOT_IMPORTED ) {
        idx = ImportHdl++;
        FPPatchImp[i] = idx;
        if( Imports == NULL ) {
            Imports = InitArray( sizeof( byte ), MODEST_IMP, INCREMENT_IMP );
        }
        if( GenStaticImports ) {
            EjectImports();
            GenStaticImports = false;
        }
        OutName( FPPatchName[i], Imports );
        OutIdx( 0, Imports );                   /* type index*/
        if( FPPatchAltName[i] != NULL ) {
            ImportHdl++;
            OutName( FPPatchAltName[i], Imports );
            OutIdx( 0, Imports );               /* type index*/
        }
    }
    CheckLEDataSize( 2 * sizeof( offset ), true );
    DoFix( idx, false, BASE_IMP, F_OFFSET, 0 );
    if( FPPatchAltName[i] != NULL ) {
        IncLocation( sizeof( byte ) );
        DoFix( idx + 1, false, BASE_IMP, F_OFFSET, 0 );
        DecLocation( sizeof( byte ) );
    }
}


extern  void    OutPatch( label_handle lbl, patch_attr attr )
/***********************************************************/
{
    temp_patch  *pat;
    object      *obj;

     /* careful, might be patching offset of seg:off*/
    CheckLEDataSize( 3 * sizeof( offset ), true );
    pat = CGAlloc( sizeof( temp_patch ));
    obj = CurrSeg->obj;
    pat->link = obj->patches;
    pat->lbl = lbl;
    pat->pat.ref = INVALID_OBJHANDLE;
    pat->pat.where = CurrSeg->location - obj->start + CurrSeg->data_prefix_size;
    pat->pat.attr = attr;
    obj->patches = pat;
}

extern  abspatch        *NewAbsPatch( void )
/******************************************/
{
    abspatch    *new;

    new = CGAlloc( sizeof( *new ) );
    memset( new, 0, sizeof( *new ) );
    new->link = AbsPatches;
    AbsPatches = new;
    return( new );
}


static  void    InitLineInfo( object *obj )
/*****************************************/
{
    obj->line_info = false;
    if( CurrSeg->comdat_label != NULL ) {
        OutByte( obj->lines_generated, obj->lines );
        OutIdx( NeedComdatNidx( NORMAL ), obj->lines );
    } else {
        if( CurrSeg->btype == BASE_GRP ) {
            OutIdx( CurrSeg->base, obj->lines );
        } else {
            OutIdx( 0, obj->lines );
        }
        OutIdx( obj->index, obj->lines );
    }
}



static  void    AddLineInfo( cg_linenum line, object *obj, offset lc )
/********************************************************************/
{
    cue_state           info;

    if( _IsModel( DBG_DF ) || _IsModel( DBG_CV ) ) {
        CueFind( line, &info );
        if( _IsModel( DBG_DF ) ) {
            if( _IsModel( DBG_LOCALS | DBG_TYPES ) ) {
                 DFLineNum( &info, lc );
            }
        } else if( _IsModel( DBG_CV ) ) {
            char *fname;

            if( info.fno != CurrFNo ) {
                fname = SrcFNoFind( info.fno );
                CurrFNo = info.fno;
                FlushLineNum( obj );
                OutName( fname, obj->lines );
                PutObjOMFRec( CMD_THEADR, obj->lines->array, obj->lines->used );
                obj->lines->used = 0;
                InitLineInfo( obj );
            }
        }
        line = info.line;
    }
    if( obj->lines->used >= BUFFSIZE - TOLERANCE ) {
        FlushLineNum( obj );
        InitLineInfo( obj );
    }
    obj->line_info = true;
    OutShort( line, obj->lines );
    OutOffset( lc, obj->lines );
}

static  void    SetPendingLine( void )
/************************************/
{
    line_num_entry      *old_line;
    object              *obj;
    cg_linenum          line;

    obj = CurrSeg->obj;
    line = obj->pending_line_number;
    if( line == 0 )
        return;
    obj->pending_line_number = 0;
    obj->pending_label_line = false;
    if( obj->lines == NULL ) {
        obj->lines = InitArray( sizeof( byte ), MODEST_LINE, INCREMENT_LINE );
    }
    if( obj->lines->used == 0 ) {
        InitLineInfo( obj );
    } else {
        old_line = &_ARRAY( obj->lines, line_num_entry ) - 1;
        if( line == _HostShort( old_line->line ) )
            return;
        if( (offset)CurrSeg->location <= _HostOffset( old_line->off ) ) {
            old_line->line = _TargetShort( line );
            return;
        }
    }
    AddLineInfo( line, obj, (offset)CurrSeg->location );
}


static  void    SetMaxWritten( void )
/***********************************/
{
    if( CurrSeg->location > CurrSeg->max_written ) {
        CurrSeg->max_written = CurrSeg->location;
    }
}


extern  void    OutDataByte( byte value )
/***************************************/
{
    unsigned    i;
    unsigned    need;
    object      *obj;

    SetPendingLine();
    CheckLEDataSize( sizeof( byte ), true );
    obj = CurrSeg->obj;
    i = CurrSeg->location - obj->start + CurrSeg->data_prefix_size;
    IncLocation( sizeof( byte ) );
    need = i + sizeof( byte );
    if( need > obj->data.used ) {
        if( need > obj->data.alloc ) {
            ReallocArray( &obj->data, need );
        }
        obj->data.used = need;
    }
    SetMaxWritten();
    _ARRAYOF( &obj->data, byte )[i] = value;
}

extern  void    OutDataShort( unsigned_16 value )
/***********************************************/
{
    unsigned    i;
    unsigned    need;
    object      *obj;

    SetPendingLine();
    CheckLEDataSize( sizeof( unsigned_16 ), true );
    obj = CurrSeg->obj;
    i = CurrSeg->location - obj->start + CurrSeg->data_prefix_size;
    IncLocation( sizeof( unsigned_16 ) );
    need = i + sizeof( unsigned_16 );
    if( need > obj->data.used ) {
        if( need > obj->data.alloc ) {
            ReallocArray( &obj->data, need );
        }
        obj->data.used = need;
    }
    SetMaxWritten();
    *(unsigned_16 *)&_ARRAYOF( &obj->data, byte )[i] = _TargetShort( value );
}


extern  void    OutDataLong( unsigned_32 value )
/**********************************************/
{
    unsigned    i;
    unsigned    need;
    object      *obj;

    SetPendingLine();
    CheckLEDataSize( sizeof( unsigned_32 ), true );
    obj = CurrSeg->obj;
    i = CurrSeg->location - obj->start + CurrSeg->data_prefix_size;
    IncLocation( sizeof( unsigned_32 ) );
    need = i + sizeof( unsigned_32 );
    if( need > obj->data.used ) {
        if( need > obj->data.alloc ) {
            ReallocArray( &obj->data, need );
        }
        obj->data.used = need;
    }
    SetMaxWritten();
    *(unsigned_32 *)&_ARRAYOF( &obj->data, byte )[i] = _TargetLongInt( value );
}


extern  void    OutAbsPatch( abspatch *patch, patch_attr attr )
/*************************************************************/
{
    object      *obj;
    long_offset value;

    CheckLEDataSize( 2 * sizeof( offset ), true );
    if( patch->flags & AP_HAVE_VALUE ) {
        value = patch->value;
        FreeAbsPatch( patch );
    } else {
        obj = CurrSeg->obj;
        patch->obj = obj;
        patch->pat.ref = INVALID_OBJHANDLE;
        patch->pat.where = CurrSeg->location - obj->start + CurrSeg->data_prefix_size;
        patch->pat.attr = attr;
        value = 0;
    }
    if( attr & LONG_PATCH ) {
        OutDataLong( value );
    } else if( attr & WORD_PATCH ) {
        OutDataShort( value );
    } else {
        OutDataByte( (byte)value );
    }
}


static void DumpImportResolve( cg_sym_handle sym, omf_idx idx )
/**********************************************************/
{
    cg_sym_handle       def_resolve;
    omf_idx             def_idx;
    array_control       *cmt;
    omf_idx             nidx;
    pointer             cond;
    import_type         type;

    def_resolve = FEAuxInfo( sym, DEFAULT_IMPORT_RESOLVE );
    if( def_resolve != NULL && def_resolve != sym ) {
        def_idx = GenImport( def_resolve, false );
        EjectImports();
        cmt = InitArray( sizeof( byte ), MODEST_HDR, INCREMENT_HDR );
        type = (import_type)(pointer_int)FEAuxInfo( sym, IMPORT_TYPE );
        switch( type ) {
        case IMPORT_IS_LAZY:
            OutShort( LAZY_EXTRN_COMMENT, cmt );
            OutIdx( idx, cmt );
            OutIdx( def_idx, cmt );
            break;
        case IMPORT_IS_WEAK:
            OutShort( WEAK_EXTRN_COMMENT, cmt );
            OutIdx( idx, cmt );
            OutIdx( def_idx, cmt );
            break;
        case IMPORT_IS_CONDITIONAL_PURE:
            OutShort( WEAK_EXTRN_COMMENT, cmt );
            OutIdx( idx, cmt );
            OutIdx( def_idx, cmt );
            PutObjOMFRec( CMD_COMENT, cmt->array, cmt->used );
            cmt->used = 0;
            /* fall through */
        case IMPORT_IS_CONDITIONAL:
            OutShort( LINKER_COMMENT, cmt );
            if( type == IMPORT_IS_CONDITIONAL ) {
                OutByte( LDIR_VF_TABLE_DEF, cmt );
            } else {
                OutByte( LDIR_VF_PURE_DEF, cmt );
            }
            OutIdx( idx, cmt );
            OutIdx( def_idx, cmt );
            for( cond = FEAuxInfo( sym, CONDITIONAL_IMPORT ); cond != NULL; cond = FEAuxInfo( cond, NEXT_CONDITIONAL ) ) {
                sym = FEAuxInfo( cond, CONDITIONAL_SYMBOL );
                DoOutObjectName( sym, GetSymLName, &nidx, NORMAL );
                OutIdx( nidx, cmt );
            }
            FlushNames();
            break;
        }
        PutObjOMFRec( CMD_COMENT, cmt->array, cmt->used );
        KillArray( cmt );
    }
}


extern  void    OutReloc( segment_id seg, fix_class class, bool rel )
/*******************************************************************/
{
    index_rec   *rec;

    rec = AskSegIndex( seg );
    if( F_CLASS( class ) == F_BIG_OFFSET ) {
        CheckLEDataSize( 3 * sizeof( long_offset ), true );
    } else {
        CheckLEDataSize( 3 * sizeof( offset ), true );
    }
    DoFix( rec->base, rel, rec->btype, class, rec->sidx );
}


extern void OutSpecialCommon( import_handle imp_idx, fix_class class, bool rel )
/******************************************************************************/
{
    CheckLEDataSize( 3 * sizeof( offset ), true );
    DoFix( imp_idx, rel, BASE_IMP, class, 0 );
}


extern  void    OutImport( cg_sym_handle sym, fix_class class, bool rel )
/********************************************************************/
{
    fe_attr     attr;

    attr = FEAttr( sym );
#if  _TARGET & _TARG_80386
    if( !rel && F_CLASS( class ) == F_OFFSET && (attr & FE_PROC) ) {
        if( *(call_class *)FindAuxInfoSym( sym, CALL_CLASS ) & FAR16_CALL ) {
            class |= F_FAR16;
        }
    }
#endif
    if( attr & FE_THREAD_DATA )
        class |= F_TLS;
    OutSpecialCommon( GenImport( sym, (class & F_ALT_DLLIMP) != 0 ), class, rel );
}


extern  void    OutRTImportRel( rt_class rtindex, fix_class class, bool rel )
/***************************************************************************/
{
    omf_idx     idx;

    idx = AskRTHandle( rtindex );
    if( idx == NOT_IMPORTED ) {
        idx = ImportHdl++;
        if( Imports == NULL ) {
            Imports = InitArray( sizeof( byte ), MODEST_IMP, INCREMENT_IMP );
        }
        CheckImportSwitch( false );
        TellRTHandle( rtindex, idx );
        OutName( AskRTName( rtindex ), Imports );
        OutIdx( 0, Imports );           /* type index*/
    }
    OutSpecialCommon( idx, class, rel );
}


extern  void    OutRTImport( rt_class rtindex, fix_class class )
/**************************************************************/
{
    OutRTImportRel( rtindex, class, ( F_CLASS( class ) == F_OFFSET || F_CLASS( class ) == F_LDR_OFFSET ) );
}

extern  void    OutBckExport( const char *name, bool is_export )
/**************************************************************/
{
    array_control       *exp;
    object              *obj;


    obj = CurrSeg->obj;
    exp = obj->exports;
    if( exp == NULL ) {
        exp = InitArray( sizeof( byte ), MODEST_EXP, INCREMENT_EXP );
        obj->exports = exp;
    }
    if( obj->exports->used >= BUFFSIZE - TOLERANCE ) {
        EjectExports();
    }
    /* are we switching from global to statics or vis-versa */
    if( obj->gen_static_exports ) {
        if( is_export ) {
            EjectExports();
            obj->gen_static_exports = false;
        }
    } else {
        if( !is_export  ) {
            EjectExports();
            obj->gen_static_exports = true;
        }
    }
    if( obj->exports->used == 0 ) {
        if( CurrSeg->btype == BASE_GRP ) {
            OutIdx( CurrSeg->base, exp );   /* group index*/
        } else {
#ifdef _OMF_32
            OutIdx( FlatGIndex, exp );      /* will be 0 if we have none */
#else
            OutIdx( 0, exp );
#endif
        }
        OutIdx( obj->index, exp );          /* segment index*/
    }
    OutName( name, exp );
    OutOffset( (offset)CurrSeg->location, exp );
    OutIdx( 0, exp );                       /* type index*/
}

extern  void    OutBckImport( const char *name, back_handle bck, fix_class class )
/******************************************************************************/
{
    omf_idx     idx;

    idx = bck->imp;
    if( idx == NOT_IMPORTED ) {
        idx = ImportHdl++;
        if( Imports == NULL ) {
            Imports = InitArray( sizeof( byte ), MODEST_IMP, INCREMENT_IMP );
        }
        CheckImportSwitch( false );
        bck->imp = idx;
        OutName( name, Imports );
        OutIdx( 0, Imports );               /* type index*/
    }
    OutSpecialCommon( idx, class, false );
}


extern  void    OutLineNum( cg_linenum  line, bool label_line )
/*************************************************************/
{
    object      *obj;

    obj = CurrSeg->obj;
    if( obj->pending_line_number == 0
        || obj->pending_label_line
        || line < obj->pending_line_number ) {
        obj->pending_line_number = line;
        obj->pending_label_line = label_line;
    }
}


extern  unsigned        SavePendingLine( unsigned new )
/******************************************************

        We're about to dump some alignment bytes. Save and restore
        the pending_line_number field so the that line number info
        offset is after the alignment.
*/
{
    unsigned    old;

    old = CurrSeg->obj->pending_line_number;
    CurrSeg->obj->pending_line_number = new;
    return( old );
}


/*%%     Utility routines for filling the buffer*/

#if 0
static  void    OutConcat( char *name1, char *name2, array_control *dest )
/************************************************************************/
{
    unsigned    len1;
    unsigned    len2;

    len1 = Length( name1 );
    len2 = Length( name2 );
    NeedMore( dest, len1 + len2 + 1 );
    _ARRAY( dest, char ) = len1 + len2;
    dest->used++;
    _CopyTrans( name1, &_ARRAY( dest, char ), len1 );
    dest->used += len1;
    _CopyTrans( name2, &_ARRAY( dest, char ), len2 );
    dest->used += len2;
}
#endif


extern  void    OutDBytes( unsigned len, const byte *src )
/********************************************************/
{
    unsigned    i;
    unsigned    max;
    unsigned    n;
    unsigned    need;
    object      *obj;

    SetPendingLine();
    CheckLEDataSize( sizeof( byte ), true );
    obj = CurrSeg->obj;
    i = CurrSeg->location - obj->start + CurrSeg->data_prefix_size;
    max = (BUFFSIZE - TOLERANCE) - i;
    while( len != 0 ) {
        if( len > max ) {
            n = max;
        } else {
            n = len;
        }
        need = i + n;
        if( need > obj->data.used ) {
            if( need > obj->data.alloc ) {
                ReallocArray( &obj->data, need );
            }
            obj->data.used = need;
        }
        IncLocation( n );
        SetMaxWritten();
        memcpy( &_ARRAYOF( &obj->data, byte )[i], src, n );
        src += n;
        len -= n;
        if( len == 0 )
            break;
        EjectLEData();
        OutLEDataStart( false );
        i = CurrSeg->data_prefix_size;
        max = (BUFFSIZE - TOLERANCE);
    }
}


extern  void    OutIBytes( byte pat, offset len )
/***********************************************/
{
    cmd_omf     cmd;
    object      *obj;

    SetPendingLine();
    if( len <= TRADEOFF ) {
        for( ; len != 0; --len ) {
            OutDataByte( pat );
        }
    } else {
        EjectLEData();
        OutLEDataStart( true );
        obj = CurrSeg->obj;
#ifdef _OMF_32
        if( _IsntTargetModel( EZ_OMF ) ) {
            OutOffset( len, &obj->data );          /* repeat count */
        } else {
            OutShort( len, &obj->data );           /* repeat count */
        }
#else
        OutShort( len, &obj->data );               /* repeat count */
#endif
        OutShort( 0, &obj->data );                 /* nesting count */
        OutByte( 1, &obj->data );                  /* pattern length */
        OutByte( pat, &obj->data );
        if( CurrSeg->comdat_label != NULL ) {
            cmd = CMD_COMDAT;
        } else {
            cmd = CMD_LIDATA;
        }
        PutObjOMFRec( PickOMF( cmd ), obj->data.array, obj->data.used );
        obj->data.used = 0;
        IncLocation( len );
        SetMaxWritten();
    }
}


extern  segment_id  AskOP( void )
/*******************************/
{
    return( CurrSeg->seg );
}

extern  bool    NeedBaseSet( void )
/*********************************/
{
    bool        need;

    need = ( CurrSeg->need_base_set != 0 );
    CurrSeg->need_base_set = false;
    return( need );
}


extern  offset  AskMaxSize( void )
/********************************/
{
    return( (offset)CurrSeg->max_size );
}

extern  long_offset  AskBigLocation( void )
/*****************************************/
{
    return( CurrSeg->location );
}

extern  long_offset  AskBigMaxSize( void )
/****************************************/
{
    return( CurrSeg->max_size );
}

extern  void    TellObjNewLabel( cg_sym_handle lbl )
/***********************************************/
{
    if( lbl == NULL )
        return;
    if( CurrSeg == NULL )
        return;
    if( CodeSeg != CurrSeg->seg )
        return;

    /*
       We've got a data label going into a code segment.
       Make sure everything's OK with regards to COMDATs.
    */
    if( FEAttr( lbl ) & FE_COMMON ) {
        DoEmptyQueue();
        if( _IsModel( DBG_DF ) ) {
            if( CurrSeg->comdat_symbol != NULL ) {
                DFSymRange( CurrSeg->comdat_symbol, (offset)CurrSeg->comdat_size );
            }
        }
    } else if( CurrSeg->comdat_symbol != NULL ) {
        DoEmptyQueue();
        SetUpObj( false );
        if( _IsModel( DBG_DF ) ) {
            if( CurrSeg->comdat_symbol != NULL ) {
                DFSymRange( CurrSeg->comdat_symbol, (offset)CurrSeg->comdat_size );
            }
        }
    }
}

extern  void    TellObjNewProc( cg_sym_handle proc )
/***********************************************/
{
    segment_id  old;
    segment_id  proc_id;


    old = SetOP( CodeSeg );
    proc_id = FESegID( proc );
    if( CodeSeg != proc_id ) {
        if( _IsModel( DBG_DF ) ) {
            if( CurrSeg->comdat_symbol != NULL ) {
                DFSymRange( CurrSeg->comdat_symbol, (offset)CurrSeg->comdat_size );
            }
        }
        if( CurrSeg->obj != NULL ) {
            DoEmptyQueue();
            FlushObject();
        }
        CodeSeg = proc_id;
        SetOP( CodeSeg );
        CurrSeg->need_base_set = true;
        if( !CurrSeg->exec ) {
            Zoiks( ZOIKS_088 );
        }
    }
    if( FEAttr( proc ) & FE_COMMON ) {
        DoEmptyQueue();
        if( _IsModel( DBG_DF ) ) {
            if( CurrSeg->comdat_symbol != NULL ) {
                DFSymRange( CurrSeg->comdat_symbol, (offset)CurrSeg->comdat_size );
            }
        }
        CurrSeg->comdat_symbol = proc;
        CurrSeg->prefix_comdat_state = PCS_NEED;
    } else if( CurrSeg->comdat_symbol != NULL ) {
        DoEmptyQueue();
        SetUpObj( false );
        if( _IsModel( DBG_DF ) ) {
            if( CurrSeg->comdat_symbol != NULL ) {
                DFSymRange( CurrSeg->comdat_symbol, (offset)CurrSeg->comdat_size );
            }
        }
        NormalData();
    }
    SetOP( old );
}

extern void     TellObjVirtFuncRef( void *cookie )
/************************************************/
{
    segment_id          old;
    virt_func_ref_list  *new;

    old = SetOP( CodeSeg );
    new = CGAlloc( sizeof( virt_func_ref_list ) );
    new->cookie = cookie;
    new->next = CurrSeg->virt_func_refs;
    CurrSeg->virt_func_refs = new;
    SetOP( old );
}

static  bool            InlineFunction( cg_sym_handle sym )
/******************************************************/
{
    if( FEAttr( sym ) & FE_PROC ) {
        if( FindAuxInfoSym( sym, CALL_BYTES ) != NULL )
            return( true );
        if( (*(call_class *)FindAuxInfoSym( sym, CALL_CLASS ) & MAKE_CALL_INLINE) ) {
            return( true );
        }
    }
    return( false );
}

extern  segment_id      AskSegID( pointer hdl, cg_class class )
/*************************************************************/
{
    switch( class ) {
    case CG_FE:
        if( InlineFunction( (cg_sym_handle)hdl ) ) {
            return( AskCodeSeg() );
        }
        return( FESegID( (cg_sym_handle)hdl ) );
    case CG_BACK:
        return( ((back_handle)hdl)->seg );
    case CG_TBL:
    case CG_VTB:
        return( AskCodeSeg() );
    case CG_CLB:
        return( AskAltCodeSeg() );
    default:
        return( AskBackSeg() );
    }
}

extern  bool            AskNameCode( pointer hdl, cg_class class )
/****************************************************************/
{
    switch( class ) {
    case CG_FE:
        return( (FEAttr( hdl ) & FE_PROC) != 0 );
    case CG_TBL:
    case CG_VTB:
    case CG_CLB:
        return( true );
    default:
        break;
    }
    return( false );
}

extern  bool    AskNameROM( pointer hdl, cg_class class )
/*******************************************************/
{
    return( AskSegROM( AskSegID( hdl, class ) ) );
}
