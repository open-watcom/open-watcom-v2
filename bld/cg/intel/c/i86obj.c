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


#include <string.h>
#include "standard.h"
#include "sysmacro.h"
#include "cg.h"
#include "bckdef.h"
#include "cgaux.h"
#include "objrep.h"
#include "system.h"
#include "i86obj.h"
#include "model.h"
#include "hostsys.h"
#include "zoiks.h"
#include "fppatch.h"
#include "ocentry.h"
#include "dbcue.h"
#include "import.h"
#include "feprotos.h"


#ifdef _PHAR_LAP /* This is a misnomer. Let's rename it */
    #define _OMF_32
#endif

#define _CopyTrans( src, dst, len )        Copy( src, dst, len )

extern  pointer         Copy(pointer,pointer,uint);
extern  char            *AskRTName(int);
extern  void            TellRTHandle(int,import_handle);
extern  import_handle   AskRTHandle(int);
extern  void            TellImportHandle(sym_handle,import_handle);
extern  import_handle   AskImportHandle(sym_handle);
extern  void            TellDonePatch(label_handle);
extern  pointer         AskLblPatch(label_handle);
extern  void            TellAddress(label_handle,offset);
extern  sym_handle      AskForLblSym(label_handle);
extern  void            FatalError(char *);
extern  void            CloseObj();
extern  void            PatchObj(objhandle,uint,byte*,int);
extern  void            GetFromObj(objhandle,uint,byte*,int);
extern  void            ScratchObj();
extern  objhandle       AskObjHandle();
extern  void            PutObjRec(byte,byte*,uint);
extern  void            OpenObj();
extern  char            *CopyStr(char*,char*);
extern  void            EmptyQueue();
extern  uint            Length(pointer);
extern  void            TellCommonLabel(label_handle, int );
extern  void            TellUnreachLabels(void);
extern  void            KillLblRedirects();
/* DF interface */
extern  void            DFObjInitInfo( void );
extern  void            DFObjLineInitInfo( void );
extern  void            DFBegCCU( seg_id code, long dbg_pch );
extern  void            DFDefSegs();
extern  void            DFObjFiniDbgInfo( offset codesize );
extern  void            DFObjLineFiniDbgInfo( void );
extern  void            DFLineNum( cue_state *, offset );
extern  void            DFSegRange( void );
extern  void            DFSymRange( sym_handle, offset );
/* CV interface */
extern  void            CVObjInitInfo( void );
extern  void            CVDefSegs();
extern  void            CVLineNum( cue_state *, offset );
extern  void            CVObjFiniDbgInfo();
/* WV interface */
extern  void            WVObjInitInfo( void );
extern  void            WVTypesEof( void );
extern  void            WVDmpCueInfo( long_offset here );

/* Forward ref's */
extern  array_control   *InitArray(int ,int ,int );
extern  seg_id          SetOP(seg_id );
extern  offset          AskLocation(void);
extern  void            OutOffset(offset );
extern  void            OutInt(int );
extern  void            IncLocation(offset );
extern  void            DecLocation(offset );
extern  void            OutDataInt(int );
static  void            ChangeObjSrc( char *fname );

extern  seg_id          DbgLocals;
extern  seg_id          DbgTypes;

extern  bool            Used87;
extern  byte            OptForSize;


static    array_control *Out;
static    byte          *OutBuff;
static    bool          GenStaticImports;
static    import_handle ImportHdl;
static    array_control *Imports;
static    array_control *SegInfo;
static    abspatch      *AbsPatches;
static    seg_id        CodeSeg;
static    seg_id        BackSeg;
static    segdef        *SegDefs;
static    long_offset   CodeSize;
static    long_offset   DataSize;
static    long_offset   DbgTypeSize;
static    index_rec     *CurrSeg;
static    int           GroupIndex;
static    int           DGroupIndex;
static    int           SegmentIndex;
static    int           PrivateIndexRW;
static    int           PrivateIndexRO;
static    int           CodeGroupGIdx;
static    int           CodeGroupNIdx;
static    char          CodeGroup[80];
static    char          DataGroup[80];
static    offset        SelStart;
static    unsigned_16   SelIdx;
static    unsigned      BackSegIdx;
static    import_handle FPPatchImp[FPP_NUMBER_OF_TYPES];
static    int           SegsDefd;
static    bool          NoDGroup;
static    short         CurrFNo;
#ifdef _OMF_32
static    int           FlatGIndex;
static    int           FlatNIndex;
#endif
static    int           TLSGIndex;
extern    void          DoOutObjectName(sym_handle,void(*)(char*,void*),void*,import_type);

typedef struct lname_cache lname_cache;

struct lname_cache {
    lname_cache         *next;
    unsigned            idx;
    unsigned_8          name[1];        /* var sized, first byte is length */
};

static  unsigned        NameIndex;
static  lname_cache     *NameCache;
static  lname_cache     *NameCacheDumped;

static char *FPPatchName[] = {
    NULL,
    "FIWRQQ",
    "FIDRQQ",
    "FIERQQ",
    "FICRQQ",
    "FISRQQ",
    "FIARQQ",
    "FIFRQQ",
    "FIGRQQ"
};

static char *FPPatchAltName[] = {
    NULL,
    NULL,
    NULL,
    NULL,
    "FJCRQQ",
    "FJSRQQ",
    "FJARQQ",
    "FJFRQQ",
    "FJGRQQ"
};

typedef struct virt_func_ref_list {
    struct virt_func_ref_list   *next;
    void                        *cookie;
} virt_func_ref_list;

#define _ARRAY( what, type )    (*(type *)((char*)(what)->array + (what)->used))
#define _ARRAYOF( what, type )  ((type *)(what)->array)
#define _CHGTYPE( what, type )  (*(type *)&(what))
static  void    OutLongOffset( long_offset value );/*forward ref*/

extern  void    InitSegDefs() {
/*****************************/

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

static unsigned GetNameIdx( char *name, char *suff, bool alloc )
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
        if( curr == NULL ) break;
        if( (name_len + suff_len) == curr->name[0]
         && memcmp( name, &curr->name[1], name_len ) == 0
         && memcmp( suff, &curr->name[name_len+1], suff_len ) == 0 ) {
            return( curr->idx );
        }
        owner = &curr->next;
    }
    if( !alloc ) return( 0 );
    _Alloc( curr, sizeof( *curr ) + name_len + suff_len );
    *owner = curr;
    curr->next = NULL;
    curr->idx = ++NameIndex;
    assert(( name_len + suff_len ) < 256 );
    curr->name[0] = name_len + suff_len;
    memcpy( &curr->name[1], name, name_len );
    memcpy( &curr->name[name_len+1], suff, suff_len );
    return( NameIndex );
}

static void FlushNames()
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
            PutObjRec( CMD_LNAMES, buff, i );
            i = 0;
        }
        buff[i++] = dmp->name[0];
        _CopyTrans( &dmp->name[1], &buff[i], dmp->name[0] );
        i += dmp->name[0];
        NameCacheDumped = dmp;
    }
    if( i > 0 ) {
        PutObjRec( CMD_LNAMES, buff, i );
    }
}

bool FreeObjCache()
{
    lname_cache         *tmp;

    if( NameCache == NULL ) return( FALSE );
    FlushNames();
    while( NameCache != NULL ) {
        tmp = NameCache->next;
        _Free( NameCache, sizeof( *NameCache ) + NameCache->name[0] );
        NameCache = tmp;
    }
    NameCacheDumped = NULL;
    return( TRUE );
}

extern  void    DefSegment( seg_id id, seg_attr attr, char *str, uint align, bool use_16 ) {
/******************************************************************************************/

    segdef              *new;
    segdef              **owner;
    seg_id              first_code;

    _Alloc( new, sizeof( segdef ) );
    new->id = id;
    new->attr = attr;
    new->align = align;
    _Alloc( new->str, Length( str ) + 1 );
    CopyStr( str, new->str );
    owner = &SegDefs;
    while( *owner != NULL ) {
        owner = &(*owner)->next;
    }
    first_code = BACKSEGS;
    *owner = new;
    new->next = NULL;
    if( attr & EXEC ) {
        if( CodeSeg == BACKSEGS ){
             CodeSeg = id;
             first_code = id;
        }
        if( OptForSize == 0 && new->align < 16 ) new->align = 16;
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

static DoEmptyQueue()
/*******************/
{
    EmptyQueue();
    TellUnreachLabels();
}

static  index_rec       *AskSegIndex( seg_id seg ) {
/**************************************************/

    index_rec   *rec;
    int         i;

    i = 0;
    rec = SegInfo->array;
    for( ;; ) {
        if( ++i > SegInfo->used ) return( NULL );
        if( rec->seg == seg ) return( rec );
        ++rec;
    }
}



extern  void    ObjInit() {
/*************************/

    array_control       *names; /* for LNAMES*/
    array_control       *dgroup_def;
    array_control       *tgroup_def;
    void                *depend;

#define MODEST_HDR 50
#define INCREMENT_HDR 50

#define MODEST_INFO 5
#define INCREMENT_INFO 5

    InitFPPatches();
    CodeSize = 0;
    DataSize = 0;
    CurrFNo = 0;
    OpenObj();
    names = InitArray( sizeof( byte ), MODEST_HDR, INCREMENT_HDR );
    OutName( FEAuxInfo( NULL, SOURCE_NAME ), names );
    PutObjRec( CMD_THEADR, names->array, names->used );
    #ifdef _OMF_32
        if( _IsTargetModel( EZ_OMF ) || _IsTargetModel( FLAT_MODEL ) ) {
            names->used = 0;
            NeedMore( names, sizeof( unsigned_16 ) );
            _ARRAY( names, unsigned_16 ) = _TargetInt( PHAR_LAP_COMMENT );
            names->used += sizeof( unsigned_16 );
            if( _IsntTargetModel( EZ_OMF ) ) {
                OutString( "OS220", names );
            } else {
                OutString( "80386", names );
            }
            PutObjRec( CMD_COMENT, names->array, names->used );
        }
    #endif
    #ifndef _OMF_32
        names->used = 0;
        NeedMore( names, sizeof( unsigned_16 ) );
        _ARRAY( names, unsigned_16 ) = _TargetInt( DEBUG_COMMENT );
        names->used += sizeof( unsigned_16 );
        PutObjRec( CMD_COMENT, names->array, names->used );
    #endif

    names->used = 0;
    NeedMore( names, sizeof( unsigned_16 ) );
    _ARRAY( names, unsigned_16 ) = _TargetInt( MODEL_COMMENT );
    names->used += sizeof( unsigned_16 );
    OutModel( names );
    PutObjRec( CMD_COMENT, names->array, names->used );
    if( _IsTargetModel( FLAT_MODEL ) && _IsModel( DBG_DF ) ) {
        names->used = 0;
        NeedMore( names, sizeof( unsigned_16 ) );
        _ARRAY( names, unsigned_16 ) = _TargetInt( LINKER_COMMENT );
        names->used += sizeof( unsigned_16 );
        NeedMore( names, sizeof( char ) );
        _ARRAY( names, char ) = LDIR_FLAT_ADDRS;
        names->used++;
        PutObjRec( CMD_COMENT, names->array, names->used );
    }
    if(  _IsntModel( DBG_DF | DBG_CV  ) ){
        names->used = 0;
        NeedMore( names, sizeof( unsigned_16 ) );
        _ARRAY( names, unsigned_16 ) = _TargetInt( LINKER_COMMENT );
        names->used += sizeof( unsigned_16 );
        NeedMore( names, sizeof( char ) );
        _ARRAY( names, char ) = LDIR_SOURCE_LANGUAGE;
        names->used++;
        NeedMore( names, sizeof( char ) );
        _ARRAY( names, char ) = DEBUG_MAJOR_VERSION;
        names->used++;
        NeedMore( names, sizeof( char ) );
        if( _IsModel( DBG_TYPES | DBG_LOCALS ) ) {
            _ARRAY( names, char ) = DEBUG_MINOR_VERSION;
        } else {
            _ARRAY( names, char ) = 0;
        }
        names->used++;
        OutString( FEAuxInfo( NULL, SOURCE_LANGUAGE ), names );
        PutObjRec( CMD_COMENT, names->array, names->used );
    }
    names->used = 0;
    depend = NULL;
    for( ;; ) {
        depend = FEAuxInfo( depend, NEXT_DEPENDENCY );
        if( depend == NULL ) break;
        NeedMore( names, sizeof( unsigned_16 ) );
        _ARRAY( names, unsigned_16 ) = _TargetInt( DEPENDENCY_COMMENT );
        names->used += sizeof( unsigned_16 );
        NeedMore( names, sizeof( unsigned_32 ) );
        _ARRAY( names, unsigned_32 ) = _TargetLongInt(
                *(unsigned_32 *)FEAuxInfo( depend, DEPENDENCY_TIMESTAMP )
                          );
        names->used += sizeof( unsigned_32 );
        OutName( FEAuxInfo( depend, DEPENDENCY_NAME ), names );
        PutObjRec( CMD_COMENT, names->array, names->used );
        names->used = 0;
    }
    /* mark end of dependancy list */
    NeedMore( names, sizeof( unsigned_16 ) );
    _ARRAY( names, unsigned_16 ) = _TargetInt( DEPENDENCY_COMMENT );
    names->used += sizeof( unsigned_16 );
    PutObjRec( CMD_COMENT, names->array, names->used );
    names->used = 0;

    dgroup_def = InitArray( sizeof( byte ), MODEST_INFO, INCREMENT_INFO );
    tgroup_def = InitArray( sizeof( byte ), MODEST_INFO, INCREMENT_INFO );
    CurrSeg = NULL;
    DoSegGrpNames( dgroup_def, tgroup_def );
    if( dgroup_def->used >= 1 ) {
        FlushNames();
        PutObjRec( CMD_GRPDEF, dgroup_def->array, dgroup_def->used );
    }
    if( tgroup_def->used >= 1 ) {
        FlushNames();
        PutObjRec( CMD_GRPDEF, tgroup_def->array, tgroup_def->used );
    }
    KillArray( tgroup_def );
    #ifdef _OMF_32
        dgroup_def->used = 0;
        if( _IsTargetModel( FLAT_MODEL ) && _IsntTargetModel( EZ_OMF ) ) {
            FlatGIndex = ++GroupIndex;
            OutIdx( FlatNIndex, dgroup_def );
            FlushNames();
            PutObjRec( CMD_GRPDEF, dgroup_def->array, dgroup_def->used );
            dgroup_def->used = 0;
        }
    #endif
    KillArray( dgroup_def );
    CurrSeg = AskSegIndex( CodeSeg );
    KillArray( names );
    ImportHdl = IMPORT_BASE;
    Imports = NULL;
    GenStaticImports = FALSE;
    AbsPatches = NULL;
    if( _IsModel( DBG_DF ) ) {
        if( _IsModel( DBG_LOCALS | DBG_TYPES ) ){
            DFObjInitInfo();
#if 0 // save for JimR and linker
        }else if( _IsModel( NUMBERS ) ){
            DFObjLineInitInfo();
#endif
        }
    }else if( _IsModel( DBG_CV ) ) {
        CVObjInitInfo();
    }else{
        WVObjInitInfo();
    }
}


extern seg_id DbgSegDef( char *seg_name, char *seg_class,
                                int            seg_modifier )
/****************************************************/
{
    index_rec   *rec;

    NeedMore( SegInfo, 1 );
    rec = &_ARRAYOF( SegInfo, index_rec )[ SegInfo->used++ ];
    rec->sidx = ++SegmentIndex;
    rec->cidx = GetNameIdx( seg_class, "", TRUE );
    rec->nidx = GetNameIdx( seg_name, "", TRUE );
    rec->location = 0;
    rec->big = 0;
    rec->need_base_set = 1;
    rec->data_ptr_in_code = FALSE;
    rec->data_in_code = FALSE;
    rec->start_data_in_code = FALSE;
    rec->private = TRUE;
    rec->exec = FALSE;
    rec->rom = FALSE;
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
    rec->comdat_prefix_import = 0;
    rec->base = rec->sidx;
    rec->btype = BASE_SEG;
    DoASegDef( rec, TRUE );
    return( rec->seg );
}

struct dbg_seg_info {
    seg_id      *id;
    char        *seg_name;
    char        *class_name;
};

static struct dbg_seg_info DbgSegs[] = {
    { &DbgLocals, "$$SYMBOLS", "DEBSYM" },
    { &DbgTypes,  "$$TYPES",   "DEBTYP" },
};

extern void ChkDbgSegSize( offset max, bool typing )
/**************************************************/
{
    struct dbg_seg_info *info;
    seg_id              old;
    long_offset         curr;

    info = &DbgSegs[ typing ? 1 : 0 ];
    old = SetOP( *info->id );
    curr = AskLocation();
    if( curr >= max ) {
        if( typing ){
            DbgTypeSize += curr;
        }
        *info->id = DbgSegDef( info->seg_name, info->class_name,
                               SEG_COMB_PRIVATE  );
    }
    SetOP( old );
}


static  void    DoSegGrpNames( array_control *dgroup_def,
                                array_control *tgroup_def ) {
/**********************************************************/

    segdef      *seg;
    segdef      *next;
    char        *dgroup;
    unsigned    dgroup_idx;

    GetNameIdx( "", "", TRUE );
#define _NIDX_NULL 1
    GetNameIdx( "CODE", "", TRUE );
#define _NIDX_CODE 2
    GetNameIdx( "DATA", "", TRUE );
#define _NIDX_DATA 3
    GetNameIdx( "BSS", "", TRUE );
#define _NIDX_BSS 4
    GetNameIdx( "TLS", "", TRUE );
#define _NIDX_TLS 5

#ifdef _OMF_32
    if( _IsTargetModel( FLAT_MODEL ) && _IsntTargetModel( EZ_OMF ) ) {
        FlatNIndex = GetNameIdx( "FLAT", "", TRUE );
    }
#endif
    SegmentIndex = 0;
    PrivateIndexRO = 0;
    PrivateIndexRW = 0;
    CopyStr( FEAuxInfo( NULL, CODE_GROUP ), CodeGroup );
    if( CodeGroup[ 0 ] == NULLCHAR ) {
        CodeGroupGIdx = 0;
    } else {
        CodeGroupNIdx = GetNameIdx( CodeGroup, "", TRUE );
        CodeGroupGIdx = ++GroupIndex;
    }
    dgroup = FEAuxInfo( NULL, DATA_GROUP );
    if( dgroup == NULL ) {
        NoDGroup = TRUE;
    } else {
        CopyStr( dgroup, DataGroup );
    }
    if( DataGroup[ 0 ] != NULLCHAR ) {
        TargetModel |= FLOATING_SS;
        dgroup_idx = GetNameIdx( DataGroup, "_GROUP", TRUE );
    } else {
        dgroup_idx = GetNameIdx( "DGROUP", "", TRUE );
    }
    OutIdx( dgroup_idx, dgroup_def );
    seg = SegDefs;
    SegInfo = InitArray( sizeof( index_rec ), MODEST_INFO, INCREMENT_INFO );
    while( seg != NULL ) {
        next = seg->next;
        DoSegment( seg, dgroup_def, tgroup_def, FALSE );
        seg = next;
    }
    SegDefs = NULL;
    if( _IsModel( DBG_DF ) ) {
        if( _IsModel( DBG_LOCALS | DBG_TYPES ) ){
            DFDefSegs();
        }
    }else if( _IsModel( DBG_CV ) ) {
        CVDefSegs();
    }else{
        DbgTypeSize = 0;
        if( _IsModel( DBG_LOCALS ) ) {
            DbgLocals = DbgSegDef( DbgSegs[0].seg_name, DbgSegs[0].class_name,
                                   SEG_COMB_PRIVATE );
        }
        if( _IsModel( DBG_TYPES ) ) {
            DbgTypes = DbgSegDef( DbgSegs[1].seg_name, DbgSegs[1].class_name,
                                  SEG_COMB_PRIVATE );
        }
    }
}


static  void    SegmentClass( index_rec *rec ) {
/**********************************************/

    char        *class_name;

    class_name = FEAuxInfo( (pointer)rec->seg, CLASS_NAME );
    if( class_name == NULL ) return;
    rec->cidx = GetNameIdx( class_name, "", TRUE );
}

static  byte    SegmentAttr( byte align, seg_attr tipe, bool use_16 ) {
/*********************************************************************/

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
                if( use_16 == FALSE ) {
                    attr |= SEG_USE_32;
                }
            }
        }
    #endif
    return( attr );
}

static  void    DoSegment( segdef *seg, array_control *dgroup_def,
                                array_control *tgroup_def, bool use_16 ) {
/******************************************************************************************************/

    index_rec   *rec;
    seg_id      old;

    rec = AskSegIndex( seg->id );
    if( rec == NULL ) {
        if( CurrSeg != NULL ) {
            old = CurrSeg->seg;
        }
        NeedMore( SegInfo, 1 );
        rec = &_ARRAYOF( SegInfo, index_rec )[ SegInfo->used++ ];
        if( CurrSeg != NULL ) {
            // CurrSeg might have moved on us
            CurrSeg = AskSegIndex( old );
        }
    }
    if( seg->attr & PRIVATE ) {
        rec->private = TRUE;
    } else {
        rec->private = FALSE;
    }
    rec->location = 0;
    rec->big = 0;
    rec->rom = 0;
    rec->need_base_set = 1;
    rec->data_in_code = FALSE;
    rec->start_data_in_code = FALSE;
    rec->data_ptr_in_code = FALSE;
    rec->prefix_comdat_state = PCS_OFF;
    rec->max_written = 0;
    rec->max_size = 0;
    rec->comdat_label = NULL;
    rec->comdat_symbol = NULL;
    rec->total_comdat_size = 0;
    rec->comdat_size = 0;
    rec->virt_func_refs = NULL;
    rec->data_prefix_size = 0;
    rec->comdat_prefix_import = 0;
    rec->sidx = ++SegmentIndex;
    rec->seg = seg->id;
    rec->attr = SegmentAttr( seg->align, seg->attr, use_16 );
    if( seg->attr & EXEC ) {
        rec->exec = TRUE;
        rec->rom = TRUE;   /* code is alway ROM */
        rec->cidx = _NIDX_CODE;
        if( seg->attr & GIVEN_NAME ) {
            rec->nidx = GetNameIdx( seg->str, "", TRUE );
        } else if( CodeGroupGIdx != 0 ) {
            rec->nidx = GetNameIdx( CodeGroup, seg->str, TRUE );
        } else if( _IsTargetModel( BIG_CODE ) ) {
            rec->nidx = GetNameIdx( FEModuleName(), seg->str, TRUE );
        } else {
            rec->nidx = GetNameIdx( seg->str, "", TRUE );
        }
        SegmentClass( rec );
        if( CodeGroupGIdx != 0 ) {
            rec->base = CodeGroupGIdx;
            rec->btype = BASE_GRP;
            DoASegDef( rec, use_16 );
            OutCGroup( rec->sidx );
        } else {
            rec->base = SegmentIndex;
            rec->btype = BASE_SEG;
            DoASegDef( rec, use_16 );
        }
    } else {
        rec->exec = FALSE;
        if( seg->attr & ROM ) rec->rom = TRUE;
        if( seg->attr & PRIVATE ) {
            rec->nidx = GetNameIdx( seg->str, "", TRUE );
            if( seg->attr & ROM ) {
                if( PrivateIndexRO == 0 ) {
                    PrivateIndexRO = GetNameIdx( "FAR_CONST", "", TRUE );
                }
                rec->cidx = PrivateIndexRO;
            } else {
                if( PrivateIndexRW == 0 ) {
                    PrivateIndexRW = GetNameIdx( "FAR_DATA", "", TRUE );
                }
                rec->cidx = PrivateIndexRW;
            }
            SegmentClass( rec );
            rec->base = SegmentIndex;
            rec->btype = BASE_SEG;
            DoASegDef( rec, use_16 );
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
            rec->nidx = GetNameIdx( DataGroup, seg->str, TRUE );
            SegmentClass( rec );
            DoASegDef( rec, use_16 );
        }
    }
    _Free( seg->str, Length( seg->str ) + 1 );
    _Free( seg, sizeof( segdef ) );
}


extern  bool    UseImportForm( fe_attr attr ) {
/*********************************************/

    if( attr & (FE_GLOBAL|FE_IMPORT) ) return( TRUE );
    if( attr & FE_INTERNAL ) return( FALSE );
    return( TRUE );
}



extern  bool    AskSegNear( segment_id id ) {
/*******************************************/

    index_rec   *rec;

    if( id < 0 ) return( FALSE );
    rec = AskSegIndex( id );
    if( rec->btype != BASE_GRP ) return( FALSE );
    if( rec->base > CodeGroupGIdx ) return( TRUE );
    return( FALSE );
}


extern        bool    AskSegBlank( segment_id id ) {
/*******************************************/

    index_rec *rec;

    if( id < 0 ) return( TRUE );
    rec = AskSegIndex( id );
    return( rec->cidx == _NIDX_BSS );
}


extern  bool    AskSegPrivate( segment_id id ) {
/**********************************************/

    index_rec   *rec;

    if( id < 0 ) return( TRUE );
    rec = AskSegIndex( id );
    return( rec->private || rec->exec );
}


extern  bool    AskSegROM( segment_id id ) {
/******************************************/

    index_rec   *rec;

    if( id < 0 ) return( FALSE );
    rec = AskSegIndex( id );
    return( rec->rom );
}


extern  seg_id  AskBackSeg() {
/****************************/

    return( BackSeg );
}


extern  seg_id  AskCodeSeg() {
/****************************/

    return( CodeSeg );
}


extern  bool    HaveCodeSeg() {
/*****************************/

    return( CodeSeg != BACKSEGS );
}


extern  seg_id  AskAltCodeSeg() {
/****************************/

    return( CodeSeg );
}

static  seg_id  Code16Seg;

extern  seg_id  AskCode16Seg() {
/******************************/
    if( Code16Seg == 0 ) {
        Code16Seg = --BackSegIdx;
        DefSegment( Code16Seg, EXEC | GIVEN_NAME, "_TEXT16", 16, TRUE );
    }
    return( Code16Seg );
}


static  cmd_omf    PickOMF( cmd_omf cmd ) {
/***********************************/

    #ifdef _OMF_32
        if( _IsntTargetModel( EZ_OMF ) ) ++cmd;
    #endif
    return( cmd );
}

extern  void    FlushOP( seg_id id ) {
/************************************/

    seg_id      old;
    index_rec   *rec;

    old = SetOP( id );
    if( id == CodeSeg ) {
        DoEmptyQueue();
    }
    if( _IsModel( DBG_DF ) ){
        rec = CurrSeg;
        if( rec->exec || rec->cidx == _NIDX_DATA ||rec->cidx == _NIDX_BSS ) {
            if( rec->max_size != 0 ){
                DFSegRange();
            }
        }
    }
    FiniTarg();
    CurrSeg->obj = NULL;
    SetOP( old );
}

static  void    DoASegDef( index_rec *rec, bool use_16 ) {
/**********************************************************/

#define MODEST_OBJ BUFFSIZE
#define INCREMENT_OBJ 256
#define NOMINAL_FIX 20
#define INCREMENT_FIX 50
#define MODEST_EXP BUFFSIZE
#define INCREMENT_EXP 50

    object      *obj;

    use_16 = use_16;
    _Alloc( obj, sizeof( object ) );
    rec->obj = obj;
    obj->index = rec->sidx;
    obj->start = rec->location;
    obj->patches = NULL;
    obj->gen_static_exports = FALSE;
    obj->pending_line_number = 0;
    Out = &obj->data;
    FillArray( &obj->data, sizeof( byte ), MODEST_OBJ, INCREMENT_OBJ );
    FillArray( &obj->fixes, sizeof( byte ), NOMINAL_FIX, INCREMENT_FIX );
    if( rec->exec ) { /* try to bag all the memory we'll need now*/
        obj->exports = InitArray( sizeof( byte ), MODEST_EXP, INCREMENT_EXP );
    } else {
        obj->exports = NULL;
    }
    obj->lines = NULL;
    obj->line_info = FALSE;
    Out = &obj->data;
    OutBuff = Out->array;
    OutByte( rec->attr );
    #ifdef _OMF_32
        OutOffset( 0 );    /* segment size (for now)*/
    #else  //SEG32DBG dwarf, codview
        if( rec->attr & SEG_USE_32 ){
            OutLongOffset( 0 ); /* segment size (for now)*/
        }else{
            OutOffset( 0 );    /* segment size (for now)*/
        }
    #endif
    OutIdx( rec->nidx, Out );               /* segment name index*/
    OutIdx( rec->cidx, Out );               /* class name index*/
    OutIdx( _NIDX_NULL, Out );              /* overlay name index*/
    #ifdef _OMF_32
        if( _IsTargetModel( EZ_OMF ) ){
            if( _IsntTargetModel( USE_32 ) || use_16 ){
                OutByte( 2 ); /* to indicate USE16 EXECUTE/READ */
            }
        }
    #endif
    FlushNames();
    obj->segfix = AskObjHandle();
    if( ++SegsDefd > 32000 ) {
        FEMessage( MSG_FATAL, "too many segments" );
    }
    #ifdef _OMF_32
        PutObjRec( PickOMF( CMD_SEGDEF ), Out->array, Out->used );
    #else //SEG32DBG dwarf, codview
        if( (rec->attr & SEG_USE_32) && (_IsntTargetModel( EZ_OMF )) ){
            PutObjRec( CMD_SEGDEF32, Out->array, Out->used );
        }else{
            PutObjRec( CMD_SEGDEF, Out->array, Out->used );
        }
    #endif
    if( rec->exec ) {
        Out->used = 0;
        OutInt( LINKER_COMMENT );
        OutByte( LDIR_OPT_FAR_CALLS );
        OutIdx( rec->sidx, Out );
        PutObjRec( CMD_COMENT, Out->array, Out->used );
    }
    Out->used = 0;
    rec->location = 0;
}


static  void    OutGroup( int sidx, array_control *group_def, int *index_p ) {
/***************************************************************************/

    if( *index_p == 0 ) {
        *index_p = ++GroupIndex;
    }
    NeedMore( group_def, 1 );
    _ARRAYOF( group_def, byte )[ group_def->used++ ] = GRP_SEGIDX;
    OutIdx( sidx, group_def );
}

static  void    OutCGroup( int sidx ) {
/*************************************/

    char        out[ 4 ];
    int         i;

    out[ 0 ] = CodeGroupNIdx;
    out[ 1 ] = GRP_SEGIDX;
    i = 1;
    if( sidx >= 128 ) {
        out[ ++i ] = (sidx >> 8) | 0x80;
    }
    out[ ++i ] = sidx;
    FlushNames();
    PutObjRec( CMD_GRPDEF, out, ++i );
}

static  index_rec       *AskIndexRec( unsigned_16 sidx ) {
/********************************************************/

    index_rec   *rec;
    int         i;

    i = 0;
    rec = SegInfo->array;
    while( ++i <= SegInfo->used ) {
        if( rec->sidx == sidx ) break;
        rec++;
    }
    return( rec );
}

static FiniWVTypes( void ){
/**********************/
    seg_id       old;
    long_offset  curr;
    struct dbg_seg_info *info;

    WVTypesEof();
    info = &DbgSegs[1];
    old = SetOP( *info->id );
    curr = AskLocation();
    curr += DbgTypeSize;
    *info->id = DbgSegDef( info->seg_name, info->class_name,
                               SEG_COMB_PRIVATE  );
    SetOP( *info->id );
    WVDmpCueInfo( curr );
    SetOP( old );
}

extern  void    AbortObj() {
/**************************/

    ScratchObj();
}

static void DoSegARange( offset *codesize, index_rec *rec ){
/*************************************************/
    seg_id  old;

    if( rec->exec || rec->cidx == _NIDX_DATA ||rec->cidx == _NIDX_BSS ) {
        if( rec->max_size != 0 ){
            old = SetOP( rec->seg );
            if( CurrSeg->comdat_symbol != NULL ){
                DFSymRange( rec->comdat_symbol, rec->comdat_size );
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

extern  void    ObjFini() {
/*************************/

    index_rec   *rec;
    int         i;
    pointer     auto_import;
    char        *lib;

    if( _IsModel( DBG_DF ) ){
        if( _IsModel( DBG_LOCALS | DBG_TYPES ) ){
            offset  codesize;

            codesize = 0;
            i = 0;
            rec = SegInfo->array;
            while( ++i <= SegInfo->used ) {
                if( rec->obj != NULL ) {
                    DoSegARange( &codesize, rec );
                }
                rec++;
            }
            DFObjFiniDbgInfo( codesize );
#if 0 //save for Jimr
        }else if( _IsModel( NUMBERS ) ){
            DFObjLineFini( );
#endif
        }
    }else if( _IsModel( DBG_CV ) ){
        CVObjFiniDbgInfo();
    }else{
        if( _IsModel( DBG_TYPES ) ) {
            FiniWVTypes();
        }
    }
    i = 0;
    rec = SegInfo->array;
    while( ++i <= SegInfo->used ) {
        if( rec->obj != NULL ) {
            CurrSeg = rec;
            FiniTarg();
        }
        rec++;
    }
    auto_import = NULL;
    if( Imports == NULL ) {
        Imports = InitArray( sizeof( byte ), 20, 20 );
    }
    if( Used87 ) {
        (void)FEAuxInfo( NULL, USED_8087 );
    }
    for(;;) {
        auto_import = FEAuxInfo( auto_import, NEXT_IMPORT );
        if( auto_import == NULL ) break;
        OutName( FEAuxInfo( auto_import, IMPORT_NAME ), Imports );
        OutIdx( 0, Imports );           /* type index*/
        if( Imports->used >= BUFFSIZE - TOLERANCE ) {
            PutObjRec( CMD_EXTDEF, Imports->array, Imports->used );
            Imports->used = 0;
        }
    }
    if( Imports->used != 0 ) {
        PutObjRec( CMD_EXTDEF, Imports->array, Imports->used );
        Imports->used = 0;
    }
    lib = NULL;
    for(;;) {
        lib = FEAuxInfo( lib, NEXT_LIBRARY );
        if( lib == NULL ) break;
        NeedMore( Imports, sizeof( unsigned_16 ) );
        _ARRAY( Imports, unsigned_16 ) = _TargetInt( LIBNAME_COMMENT );
        Imports->used += sizeof( unsigned_16 );
        OutString( ( (char*)FEAuxInfo( lib, LIBRARY_NAME ) ) + 1, Imports );
        PutObjRec( CMD_COMENT, Imports->array, Imports->used );
        Imports->used = 0;
    }
    KillArray( Imports );
    Imports = NULL;
    KillArray( SegInfo );
    FiniAbsPatches();
    EndModule();
    CloseObj();
    FEMessage( MSG_CODE_SIZE, (pointer)CodeSize );
    FEMessage( MSG_DATA_SIZE, (pointer)DataSize );
}


static  void    FiniTarg() {
/**************************/

    union{
        offset           s;
        long_offset      l;
    }size;
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
    #else //SEG32DBG dwarf, codview
        if( rec->attr & SEG_USE_32 ){
            size.l = _TargetLongInt( rec->max_size );
        }else{
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
        #else   //SEG32DBG dwarf, codview
            if( rec->attr & SEG_USE_32 ){
                PatchObj( obj->segfix, SEGDEF_SIZE, (byte *)&size.l, sizeof( long_offset ) );
            }else{
                PatchObj( obj->segfix, SEGDEF_SIZE, (byte *)&size.s, sizeof( offset ) );
            }
        #endif
    }
    if( obj->exports != NULL ) {
        KillArray( obj->exports );
    }
    KillStatic( &obj->data );
    KillStatic( &obj->fixes );
    _Free( obj, sizeof( object ) );
}

static  void    DoPatch( patch *pat, offset lc ) {
/************************************************/

    unsigned_32 lword;
    unsigned_16 word;
    byte        bite;

    if( pat->attr & LONG_PATCH ) {
        if( pat->attr & ADD_PATCH ) {
            GetFromObj( pat->ref, pat->where,
                       (byte *)&lword, sizeof( long_offset ) );
            _TargetAddL( lword, lc );
        } else {
            lword = _TargetLongInt( lc );
        }
        PatchObj( pat->ref, pat->where, (byte *)&lword, sizeof( long_offset ) );
    } else if( pat->attr & WORD_PATCH ) {
        if( pat->attr & ADD_PATCH ) {
            GetFromObj( pat->ref, pat->where, (byte *)&word, sizeof( short_offset ) );
            _TargetAddW( word, lc );
        } else {
            word = _TargetInt( lc );
        }
        PatchObj( pat->ref, pat->where, (byte *)&word, sizeof( short_offset ) );
    } else {
        if( pat->attr & ADD_PATCH ) {
            GetFromObj( pat->ref, pat->where, &bite, sizeof( byte ) );
            _TargetAddB( bite, lc );
        } else {
            bite = _TargetInt( lc );
        }
        PatchObj( pat->ref, pat->where, &bite, sizeof( byte ) );
    }
}



static  void    FreeAbsPatch( abspatch *patch ) {
/***********************************************/

    abspatch    **owner;

    owner = &AbsPatches;
    while( *owner != patch ) {
        owner = &(*owner)->link;
    }
    *owner = (*owner)->link;
    _Free( patch, sizeof( abspatch ) );
}


static  void    FiniAbsPatches() {
/********************************/

    abspatch    *patch;
    abspatch    *junk;

    patch = AbsPatches;
    while( patch != NULL ) {
        DoPatch( &patch->pat, patch->value );
        junk = patch;
        patch = patch->link;
        _Free( junk, sizeof( abspatch ) );
    }
}


/*%% Code Burst Routines*/


extern  void    SetUpObj( bool is_data ) {
/****************************************/

    object      *obj;
    bool        old_data;

    obj = CurrSeg->obj;
    if( obj == NULL ) return;
    Out = &obj->data;
    OutBuff = obj->data.array;
    if( obj->fixes.used >= BUFFSIZE - TOLERANCE ) {
        EjectLEData();
        return;
    }
    if( (Imports != NULL) && (Imports->used >= BUFFSIZE - TOLERANCE) ) {
         EjectLEData();
         return;
    }
    /* so that a call will always fit */
    CheckLEDataSize( 4*sizeof( offset ), FALSE );
    if( CurrSeg->exec ) {
        old_data = CurrSeg->data_in_code;
        CurrSeg->data_in_code = is_data;
        if( is_data != old_data ) {
            if( is_data ) {
                CurrSeg->start_data_in_code = TRUE;
            } else {
                OutSelect( FALSE );
                SetUpObj( FALSE );
            }
        }
    }
}


static  void    OutExport( sym_handle sym ) {
/*******************************************/

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
            obj->gen_static_exports = FALSE;
        }
    } else {
        if( !(attr & FE_GLOBAL) ) {
            EjectExports();
            obj->gen_static_exports = TRUE;
        }
    }
    if( obj->exports->used == 0 ) {
        if( CurrSeg->btype == BASE_GRP ) {
            OutIdx( CurrSeg->base, exp );   /* group index*/
        } else {
            #ifdef _OMF_32
                OutIdx( FlatGIndex, exp ); // will be 0 if we have none
            #else
                OutIdx( 0, exp );
            #endif
        }
        OutIdx( obj->index, exp );      /* segment index*/
    }
    OutObjectName( sym, exp );
    NeedMore( exp, sizeof( offset ) );
    _ARRAY( exp, offset ) = _TargetOffset( CurrSeg->location );
    exp->used += sizeof( offset );
    OutIdx( 0, exp );                   /* type index*/
}


static  void    GenComdef() {
/****************************/

    array_control       *comdef;
    unsigned            count;
    unsigned_8          type;
    unsigned_8          ind;
    unsigned long       size;
    sym_handle          sym;
    unsigned            rec;

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
        NeedMore( comdef, 4 );
        _ARRAY( comdef, unsigned_8 ) = 0; /* type index */
        comdef->used += sizeof( unsigned_8 );
        if( CurrSeg->btype == BASE_GRP && CurrSeg->base == DGroupIndex ) {
            type = COMDEF_NEAR;
        } else {
            type = COMDEF_FAR;
        }
        _ARRAY( comdef, unsigned_8 ) = type; /* common type */
        comdef->used += sizeof( unsigned_8 );
        if( type == COMDEF_FAR ) {
            _ARRAY( comdef, unsigned_8 ) = 1; /* number of elements */
            comdef->used += sizeof( unsigned_8 );
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
            _ARRAY( comdef, unsigned_8 ) = ind;
            comdef->used += sizeof( unsigned_8 );
        }
        NeedMore( comdef, count );
        do {
            /* element size */
            _ARRAY( comdef, unsigned_8 ) = size & 0xff;
            comdef->used += sizeof( unsigned_8 );
            size >>= 8;
            --count;
        } while( count != 0 );
        if( FEAttr( sym ) & FE_GLOBAL ) {
            rec = CMD_COMDEF;
        } else {
            rec = CMD_LCOMDEF;
        }
        PutObjRec( rec, comdef->array, comdef->used );
        KillArray( comdef );
        TellImportHandle( sym, ImportHdl++ );
    }
}

static void GetSymLName( char *name, unsigned *nidx )
{
    *nidx = GetNameIdx( name, "", TRUE );
}

static unsigned NeedComdatNidx( import_type kind ) {
/***************************************************/

    if( CurrSeg->comdat_nidx == 0 ) {
        DoOutObjectName( CurrSeg->comdat_symbol, GetSymLName,
                        &CurrSeg->comdat_nidx, kind );
        FlushNames();
    }
    return( CurrSeg->comdat_nidx );
}


static  void    NormalData() {
/****************************/

    GenComdef();
    CurrSeg->total_comdat_size += CurrSeg->comdat_size;
    EjectLEData();
    FlushLineNum();
    FlushSelect();
    CurrSeg->location = CurrSeg->max_written = CurrSeg->max_size;
    CurrSeg->comdat_size = 0;
    CurrSeg->comdat_label = NULL;
    CurrSeg->comdat_symbol = NULL;
    CurrSeg->need_base_set = TRUE;
    CurrSeg->prefix_comdat_state = PCS_OFF;
    KillLblRedirects();
}


static  void    ComdatData( label_handle lbl, sym_handle sym ) {
/**************************************************************/

    GenComdef();
    CurrSeg->total_comdat_size += CurrSeg->comdat_size;
    EjectLEData();
    FlushLineNum();
    FlushSelect();
    CurrSeg->obj->lines_generated = FALSE;
    CurrSeg->location = CurrSeg->max_written = 0;
    CurrSeg->comdat_size = 0;
    CurrSeg->comdat_nidx = 0;
    CurrSeg->comdat_label = lbl;
    if( sym != NULL ) {
        CurrSeg->comdat_symbol = sym;
    } else {
        NeedComdatNidx( SPECIAL );
        CurrSeg->comdat_prefix_import =
                GenImport( CurrSeg->comdat_symbol, SPECIAL );
        TellCommonLabel( lbl, CurrSeg->comdat_prefix_import );
    }
    CurrSeg->need_base_set = TRUE;
    CurrSeg->prefix_comdat_state = PCS_OFF;
    KillLblRedirects();
}

static void     OutVirtFuncRef( sym_handle virt ) {
/*************************************************/

    object      *obj;
    unsigned    extdef;

    if( virt == NULL ) {
        extdef = 0;
    } else {
        extdef = GenImport( virt, NORMAL );
    }
    EjectLEData();
    obj = CurrSeg->obj;
    Out = &obj->data;
    OutBuff = obj->data.array;
    Out->used = 0;
    NeedMore( Out, sizeof( unsigned_16 ) );
    _ARRAY( Out, unsigned_16 ) = _TargetInt( LINKER_COMMENT );
    Out->used += sizeof( unsigned_16 );
    OutByte( LDIR_VF_REFERENCE );
    OutIdx( extdef, Out );
    if( CurrSeg->comdat_symbol != NULL ) {
        OutIdx( 0, Out );
        OutIdx( NeedComdatNidx( NORMAL ), Out );
    } else {
        OutIdx( CurrSeg->sidx, Out );
    }
    PutObjRec( CMD_COMENT, Out->array, Out->used );
    Out->used = 0;
}


extern  void    OutLabel( label_handle lbl ) {
/********************************************/

    temp_patch          **owner;
    temp_patch          *curr_pat;
    array_control       *ctl;
    patch               *pat;
    int                 i;
    pointer             patptr;
    object              *obj;
    offset              lc;
    sym_handle          sym;
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
            if( CurrSeg->comdat_label != NULL ) NormalData();
            if( UseImportForm( attr ) ) OutExport( sym );
        }
        for( curr = CurrSeg->virt_func_refs; curr != NULL; curr = next ) {
            cookie = curr->cookie;
            while( cookie != NULL ) {
                OutVirtFuncRef( FEAuxInfo( cookie, VIRT_FUNC_SYM ) );
                cookie = FEAuxInfo( cookie, VIRT_FUNC_NEXT_REFERENCE );
            }
            next = curr->next;
            _Free( curr, sizeof( virt_func_ref_list ) );
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
    i = SegInfo->used;
    lc = CurrSeg->location;
    TellAddress( lbl, lc );
    while( --i >= 0 ) {
        obj = _ARRAYOF( SegInfo, index_rec )[ i ].obj;
        if( obj != NULL ) { /* twas flushed and not redefined*/
            owner = &obj->patches;
            for(;;) {
                curr_pat = *owner;
                if( curr_pat == NULL ) break;
                if( curr_pat->lbl == lbl ) {
                    patptr = &_ARRAYOF( &obj->data, byte )[  curr_pat->pat.where  ];
                    if( curr_pat->pat.attr & ADD_PATCH ) {
                        if( curr_pat->pat.attr & LONG_PATCH ) {
                            _TargetAddL( *(unsigned_32 *)patptr, lc );
                        } else if( curr_pat->pat.attr & WORD_PATCH ) {
                            _TargetAddW( *(unsigned_16 *)patptr, lc );
                        } else {
                            _TargetAddB( *(byte *)patptr, lc );
                        }
                    } else {
                        if( curr_pat->pat.attr & LONG_PATCH ) {
                            *(unsigned_32 *)patptr = _TargetLongInt( lc );
                        } else if( curr_pat->pat.attr & WORD_PATCH ) {
                            *(unsigned_16 *)patptr = _TargetInt( lc );
                        } else {
                            *(byte *)patptr = _TargetInt( lc );
                        }
                    }
                    *owner = curr_pat->link;
                    _Free( curr_pat, sizeof( temp_patch ) );
                } else {
                     owner = &curr_pat->link;
                }
            }
        }
    }
    ctl = AskLblPatch( lbl );
    i = ctl->used;
    pat = ctl->array;
    while( i > 0 ) {
        DoPatch( pat, lc );
        pat++;
        i--;
    }
    KillArray( ctl );
    TellDonePatch( lbl );
}


extern  void    AbsPatch( abspatch *patch, offset lc ) {
/******************************************************/

    if( patch->flags & AP_HAVE_OFFSET ) {
        DoPatch( &patch->pat, lc );
        FreeAbsPatch( patch );
    } else {
        patch->value = lc;
        patch->flags |= AP_HAVE_VALUE;
    }
}


static  void    SetAbsPatches() {
/*******************************/

    abspatch    *patch;
    object      *obj;

    obj = CurrSeg->obj;
    patch = AbsPatches;
    while( patch != NULL ) {
        if( patch->pat.ref == INVALID && patch->obj == obj ) {
            patch->pat.ref = AskObjHandle();
            patch->flags |= AP_HAVE_OFFSET;
        }
        patch = patch->link;
    }
}


static  void    SetPatches() {
/****************************/

    temp_patch          *curr_pat;
    temp_patch          *junk;
    array_control       *ctl;
    patch               *pat;

    curr_pat = CurrSeg->obj->patches;
    while( curr_pat != NULL ) {
        ctl = AskLblPatch( curr_pat->lbl );
        NeedMore( ctl, 1 );
        pat = &_ARRAYOF( ctl, patch )[  ctl->used++  ];
        pat->ref = AskObjHandle();
        pat->where = curr_pat->pat.where;
        pat->attr = curr_pat->pat.attr;
        junk = curr_pat;
        curr_pat = curr_pat->link;
        _Free( junk, sizeof( temp_patch ) );
    }
}


extern  array_control   *InitPatch() {
/************************************/

#define MODEST_PAT 10
#define INCREMENT_PAT 10

    return( InitArray( sizeof( patch ),  MODEST_PAT, INCREMENT_PAT ) );
}


static  void    InitFPPatches() {
/*******************************/

    int i;

    i = FPP_NUMBER_OF_TYPES;
    while( --i >= 0 ) {
        FPPatchImp[  i  ] = NOT_IMPORTED;
    }
}

#define MODEST_IMP BUFFSIZE
#define INCREMENT_IMP 50

extern  void    OutFPPatch( fp_patches i ) {
/******************************************/

    import_handle       idx;

    idx = FPPatchImp[  i  ];
    if( idx == NOT_IMPORTED ) {
        idx = ImportHdl++;
        FPPatchImp[  i  ] = idx;
        if( Imports == NULL ) {
            Imports = InitArray( sizeof( byte ), MODEST_IMP, INCREMENT_IMP );
        }
        if( GenStaticImports ) {
            EjectImports();
            GenStaticImports = FALSE;
        }
        OutName( FPPatchName[ i ], Imports );
        OutIdx( 0, Imports );           /* type index*/
        if( FPPatchAltName[ i ] != NULL ) {
            ImportHdl++;
            OutName( FPPatchAltName[ i ], Imports );
            OutIdx( 0, Imports );           /* type index*/
        }
    }
    CheckLEDataSize( 2*sizeof( offset ), TRUE );
    DoFix( idx, FALSE, BASE_IMP, F_OFFSET, 0 );
    if( FPPatchAltName[ i ] != NULL ) {
        IncLocation( sizeof( byte ) );
        DoFix( idx+1, FALSE, BASE_IMP, F_OFFSET, 0 );
        DecLocation( sizeof( byte ) );
    }
}


extern  void    OutPatch( label_handle lbl, patch_attr attr ) {
/*************************************************************/

    temp_patch  *pat;
    object      *obj;

     /* careful, might be patching offset of seg:off*/
    CheckLEDataSize( 3*sizeof( offset ), TRUE );
    _Alloc( pat, sizeof( temp_patch ));
    obj = CurrSeg->obj;
    pat->link = obj->patches;
    pat->lbl = lbl;
    pat->pat.ref = INVALID;
    pat->pat.where = CurrSeg->location - obj->start + CurrSeg->data_prefix_size;
    pat->pat.attr = attr;
    obj->patches = pat;
}

extern  abspatch        *NewAbsPatch() {
/**************************************/

    abspatch    *new;

    _Alloc( new, sizeof( *new ) );
    memset( new, 0, sizeof( *new ) );
    new->link = AbsPatches;
    AbsPatches = new;
    return( new );
}


extern  void    OutAbsPatch( abspatch *patch, patch_attr attr ) {
/***************************************************************/

    object      *obj;
    long_offset value;

    CheckLEDataSize( 2*sizeof( offset ), TRUE );
    if( patch->flags & AP_HAVE_VALUE ) {
        value = patch->value;
        FreeAbsPatch( patch );
    } else {
        obj = CurrSeg->obj;
        patch->obj = obj;
        patch->pat.ref = INVALID;
        patch->pat.where = CurrSeg->location - obj->start + CurrSeg->data_prefix_size;
        patch->pat.attr = attr;
        value = 0;
    }
    if( attr & LONG_PATCH ) {
        OutDataLong( value );
    } else if( attr & WORD_PATCH ) {
        OutDataInt( value );
    } else {
        OutDataByte( (byte)value );
    }
}


static void DumpImportResolve( sym_handle sym, import_handle idx ) {
/******************************************************************/

    sym_handle          def_resolve;
    import_handle       def_idx;
    array_control       *cmt;
    unsigned            nidx;
    pointer             cond;
    int                 type;

    def_resolve = FEAuxInfo( sym, DEFAULT_IMPORT_RESOLVE );
    if( def_resolve != NULL && def_resolve != sym ) {
        def_idx = GenImport( def_resolve, NORMAL );
        EjectImports();
        cmt = InitArray( sizeof( byte ), MODEST_HDR, INCREMENT_HDR );
        type = (int) FEAuxInfo( sym, IMPORT_TYPE );
        switch( type ) {
        case IMPORT_IS_LAZY:
            NeedMore( cmt, sizeof( unsigned_16 ) );
            _ARRAY( cmt, unsigned_16 ) = _TargetInt( LAZY_EXTRN_COMMENT );
            cmt->used += sizeof( unsigned_16 );
            OutIdx( idx, cmt );
            OutIdx( def_idx, cmt );
            break;
        case IMPORT_IS_WEAK:
            NeedMore( cmt, sizeof( unsigned_16 ) );
            _ARRAY( cmt, unsigned_16 ) = _TargetInt( WEAK_EXTRN_COMMENT );
            cmt->used += sizeof( unsigned_16 );
            OutIdx( idx, cmt );
            OutIdx( def_idx, cmt );
            break;
        case IMPORT_IS_CONDITIONAL_PURE:
            NeedMore( cmt, sizeof( unsigned_16 ) );
            _ARRAY( cmt, unsigned_16 ) = _TargetInt( WEAK_EXTRN_COMMENT );
            cmt->used += sizeof( unsigned_16 );
            OutIdx( idx, cmt );
            OutIdx( def_idx, cmt );
            PutObjRec( CMD_COMENT, cmt->array, cmt->used );
            cmt->used = 0;
            /* fall through */
        case IMPORT_IS_CONDITIONAL:
            NeedMore( cmt, sizeof( unsigned_16 ) );
            _ARRAY( cmt, unsigned_16 ) = _TargetInt( LINKER_COMMENT );
            cmt->used += sizeof( unsigned_16 );
            NeedMore( cmt, sizeof( byte ) );
            if( type == IMPORT_IS_CONDITIONAL ) {
                _ARRAY( cmt, byte ) = LDIR_VF_TABLE_DEF;
            } else {
                _ARRAY( cmt, byte ) = LDIR_VF_PURE_DEF;
            }
            cmt->used += sizeof( byte );
            OutIdx( idx, cmt );
            OutIdx( def_idx, cmt );
            cond = FEAuxInfo( sym, CONDITIONAL_IMPORT );
            while( cond != NULL ) {
                sym = FEAuxInfo( cond, CONDITIONAL_SYMBOL );
                DoOutObjectName( sym, GetSymLName, &nidx, NORMAL );
                OutIdx( nidx, cmt );
                cond = FEAuxInfo( cond, NEXT_CONDITIONAL );
            }
            FlushNames();
            break;
        }
        PutObjRec( CMD_COMENT, cmt->array, cmt->used );
        KillArray( cmt );
    }
}


static  import_handle   GenImport( sym_handle sym, import_type kind ) {
/**********************************************************************/

    import_handle       idx;
    fe_attr             attr;

    idx = AskImportHandle( sym );
    if( idx == NOT_IMPORTED || kind == SPECIAL ) {
        idx = ImportHdl++;
        if( Imports == NULL ) {
            Imports = InitArray( sizeof( byte ), MODEST_IMP, INCREMENT_IMP );
        }
        attr = FEAttr( sym );
        CheckImportSwitch( !( attr & FE_GLOBAL) );
        if( kind != SPECIAL ) TellImportHandle( sym, idx );
        if( kind == NORMAL ) {
             if( (attr & FE_DLLIMPORT) ) {
                 kind = DLLIMPORT;
             } else if( _IsModel( POSITION_INDEPENDANT ) ) {
                 if( ( attr & FE_THREAD_DATA ) != 0 ) {
                     kind = PIC_RW;
                 }
             }
        }
        DoOutObjectName( sym, OutName, Imports, kind );
        OutIdx( 0, Imports );           /* type index*/
        if( kind != SPECIAL ) DumpImportResolve( sym, idx );
    }
    return( idx );
}


static  void    DoFix( int idx, bool rel, base_type base,
                       fix_class class, int sidx ) {
/**************************************************/

    fixup       *cursor;
    int         where;
    object      *obj;
    index_rec   *rec;
    byte        b;
    fix_class   class_flags;

    b = rel ? LOCAT_REL : LOCAT_ABS;
    if( (class & F_MASK) == F_PTR && CurrSeg->data_in_code ) {
        CurrSeg->data_ptr_in_code = TRUE;
    }
    class_flags = (class & ~F_MASK);
    class &= F_MASK;
    #if  _TARGET & _TARG_80386
        if( class_flags & F_FAR16 ) {
            /* want a 16:16 fixup for a __far16 call */
            class = F_PTR;
        } else if( _IsTargetModel( EZ_OMF ) ) {
            switch( class ) {
            case F_OFFSET:
            case F_LDR_OFFSET:
                class = F_PHAR_OFFSET;
                break;
            case F_PTR:
                class = F_PHAR_PTR;
                break;
            }
        } else {
            switch( class ) {
            case F_OFFSET:
                class = F_MS_OFFSET_32;
                break;
            case F_LDR_OFFSET:
                class = F_MS_LDR_OFFSET_32;
                break;
            case F_PTR:
                class = F_MS_PTR;
                break;
            }
        }
    #endif
    obj = CurrSeg->obj;
    NeedMore( &obj->fixes, sizeof( fixup ) );
    cursor = &_ARRAY( &obj->fixes, fixup );
    where = CurrSeg->location - obj->start;
    cursor->locatof = b + ( class << S_LOCAT_LOC ) + ( where >> 8 );
    cursor->fset = where;
    obj->fixes.used += sizeof( fixup );
    if( base != BASE_IMP ) {
        rec = AskIndexRec( sidx );
        /*
           fixups to a code segment that is currently in COMDAT mode must be
           done as imports relative to the comdat symbol.
        */
        if( rec->exec && rec->comdat_symbol != NULL ) {
            idx = GenImport( rec->comdat_symbol, NORMAL );
            base = BASE_IMP;
        }
    }
#ifdef _OMF_32
    if( _IsTargetModel( FLAT_MODEL ) && _IsntTargetModel( EZ_OMF ) && (class != F_PTR) ) {
        int             grp_idx;

#if 0
        /* only generate a normal style fixup for now */
        if( class_flags & F_TLS ) {
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


extern  void    OutReloc( seg_id seg, fix_class class, bool rel ) {
/*****************************************************************/

    index_rec   *rec;

    rec = AskSegIndex( seg );
    if( class == F_MS_OFFSET_32 ){
        CheckLEDataSize( 3*sizeof( long_offset ), TRUE );
    }else{
        CheckLEDataSize( 3*sizeof( offset ), TRUE );
    }
    DoFix( rec->base, rel, rec->btype, class, rec->sidx );
}


static  void    CheckImportSwitch( bool next_is_static ) {
/********************************************************/

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


extern  void    OutSpecialCommon( int imp_idx, fix_class class, bool rel ) {
/**************************************************************************/

    CheckLEDataSize( 3*sizeof( offset ), TRUE );
    DoFix( imp_idx, rel, BASE_IMP, class, 0 );
}


extern  void    OutImport( sym_handle sym, fix_class class, bool rel ) {
/**********************************************************************/

    fe_attr     attr;

    attr = FEAttr( sym );
#if  _TARGET & _TARG_80386
    {
        aux_handle              aux;
        call_class              *pcclass;

        if( !rel && class == F_OFFSET && (attr & FE_PROC) ) {
            aux = FEAuxInfo( sym, AUX_LOOKUP );
            pcclass = FEAuxInfo( aux, CALL_CLASS );
            if( *pcclass & FAR16_CALL ) class |= F_FAR16;
        }
    }
#endif
    if( attr & FE_THREAD_DATA ) class |= F_TLS;
    OutSpecialCommon( GenImport( sym, NORMAL ), class, rel );
}


extern  void    OutRTImport( int rtindex, fix_class class ) {
/***********************************************************/

    OutRTImportRel( rtindex, class, (class==F_OFFSET)||(class==F_LDR_OFFSET) );
}

extern  void    OutRTImportRel( int rtindex, fix_class class, bool rel ) {
/************************************************************************/

    import_handle       idx;

    idx = AskRTHandle( rtindex );
    if( idx == NOT_IMPORTED ) {
        idx = ImportHdl++;
        if( Imports == NULL ) {
            Imports = InitArray( sizeof( byte ), MODEST_IMP, INCREMENT_IMP );
        }
        CheckImportSwitch( FALSE );
        TellRTHandle( rtindex, idx );
        OutName( AskRTName( rtindex ), Imports );
        OutIdx( 0, Imports );           /* type index*/
    }
    OutSpecialCommon( idx, class, rel );
}


extern  void    OutBckExport( char *name, bool is_export ) {
/*******************************************/

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
            obj->gen_static_exports = FALSE;
        }
    } else {
        if( !is_export  ) {
            EjectExports();
            obj->gen_static_exports = TRUE;
        }
    }
    if( obj->exports->used == 0 ) {
        if( CurrSeg->btype == BASE_GRP ) {
            OutIdx( CurrSeg->base, exp );   /* group index*/
        } else {
            #ifdef _OMF_32
                OutIdx( FlatGIndex, exp ); // will be 0 if we have none
            #else
                OutIdx( 0, exp );
            #endif
        }
        OutIdx( obj->index, exp );      /* segment index*/
    }
    OutName( name, exp );
    NeedMore( exp, sizeof( offset ) );
    _ARRAY( exp, offset ) = _TargetOffset( CurrSeg->location );
    exp->used += sizeof( offset );
    OutIdx( 0, exp );                   /* type index*/
}

extern  void    OutBckImport( char *name, bck_info  *bck, fix_class class ) {
/***************************************************************************/

    import_handle       idx;

    idx = bck->imp;
    if( idx == NOT_IMPORTED ) {
        idx = ImportHdl++;
        if( Imports == NULL ) {
            Imports = InitArray( sizeof( byte ), MODEST_IMP, INCREMENT_IMP );
        }
        CheckImportSwitch( FALSE );
        bck->imp = idx;
        OutName( name, Imports );
        OutIdx( 0, Imports );           /* type index*/
    }
    OutSpecialCommon( idx, class, FALSE );
}


extern  void    OutLineNum( cg_linenum  line, bool label_line ) {
/***************************************************************/

    object      *obj;

    obj = CurrSeg->obj;
    if( obj->pending_line_number == 0
        || obj->pending_label_line
        || line < obj->pending_line_number ) {
        obj->pending_line_number = line;
        obj->pending_label_line = label_line;
    }
}




#include <cgnoalgn.h>
typedef struct line_num_entry {
    unsigned_16     line;
    offset          off;
} line_num_entry;
#include <cgrealgn.h>


static  void    ChangeObjSrc( char *fname ) {
    array_control       *names; /* for LNAMES*/

    FlushLineNum();
    InitLineInfo();
    names = InitArray( sizeof( byte ), MODEST_HDR, INCREMENT_HDR );
    OutName( fname, names );
    PutObjRec( CMD_THEADR, names->array, names->used );
    KillArray( names );
}

static  void    AddLineInfo( cg_linenum line, object *obj, offset lc ) {
/***********************************************************************/

    array_control       *old;
    cue_state            info;

    old = Out;
    if( _IsModel( DBG_DF ) || _IsModel( DBG_CV ) ){
        CueFind( line, &info );
        if( _IsModel( DBG_DF ) ){
            if( _IsModel( DBG_LOCALS | DBG_TYPES ) ){
                 DFLineNum( &info, lc );
            }
        }else if( _IsModel( DBG_CV ) ){
            char *fname;

            if( info.fno != CurrFNo ){
                fname = SrcFNoFind( info.fno );
                CurrFNo = info.fno;
                ChangeObjSrc( fname );
            }
        }
        line = info.line;
    }
    if( obj->lines->used >= BUFFSIZE - TOLERANCE ) {
        FlushLineNum();
        InitLineInfo();
    }
    obj->line_info = TRUE;
    Out = obj->lines;
    OutBuff = obj->lines->array;
    OutInt( line );
    OutOffset( lc );
    Out = old;
    OutBuff = old->array;
}

static  void    SetPendingLine() {
/********************************/

    line_num_entry      *old_line;
    object              *obj;
    cg_linenum          line;

    obj = CurrSeg->obj;
    line = obj->pending_line_number;
    if( line == 0 ) return;
    obj->pending_line_number = 0;
    obj->pending_label_line = FALSE;
    if( obj->lines == NULL || obj->lines->used == 0 ) {
        InitLineInfo();
        AddLineInfo( line, obj, CurrSeg->location );
        return;
    }
    old_line = (struct line_num_entry *)
              &_ARRAY(obj->lines,unsigned_16) - 1;
    if( line == _HostInt( old_line->line ) ) return;
    if( CurrSeg->location > _HostOffset( old_line->off ) ) {
        AddLineInfo( line, obj, CurrSeg->location );
    } else {
        old_line->line = _TargetInt( line );
    }
}


extern  unsigned        SavePendingLine( unsigned new ) {
/********************************************************

        We're about to dump some alignment bytes. Save and restore
        the pending_line_number field so the that line number info
        offset is after the alignment.
*/
    unsigned    old;

    old = CurrSeg->obj->pending_line_number;
    CurrSeg->obj->pending_line_number = new;
    return( old );
}


#define MODEST_LINE     BUFFSIZE
#define INCREMENT_LINE  200


static  void    InitLineInfo() {
/******************************/

    object      *obj;

    obj = CurrSeg->obj;
    if( obj->lines == NULL ) {
        obj->lines = InitArray( sizeof( byte ), MODEST_LINE, INCREMENT_LINE );
    } else {
        obj->lines->used = 0;
    }
    obj->line_info = FALSE;
    if( CurrSeg->comdat_label != NULL ) {
        NeedMore( obj->lines, 1 );
        _ARRAY( obj->lines, unsigned_8 ) = obj->lines_generated;
        obj->lines->used += sizeof( unsigned_8 );
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



static  void    FlushLineNum() {
/******************************/

    object      *obj;
    cmd_omf     rec;

    obj = CurrSeg->obj;
    if( obj->line_info ) {
        if( CurrSeg->comdat_label != NULL ) {
            rec = CMD_LINSYM;
        } else {
            rec = CMD_LINNUM;
        }
        PutObjRec( PickOMF( rec ),
                   obj->lines->array, obj->lines->used );
        obj->lines->used = 0;
        obj->lines_generated = TRUE;
        obj->line_info = FALSE;
    }
}


static  void    EjectImports() {
/******************************/

    unsigned    rec;

    if( Imports != NULL && Imports->used != 0 ) {
        if( GenStaticImports ) {
            rec = CMD_LEXTDEF;
        } else {
            rec = CMD_EXTDEF;
        }
        PutObjRec( rec, Imports->array, Imports->used );
        Imports->used = 0;
    }
}


static void     EjectLEData() {
/*****************************/

    object      *obj;
#include "cgnoalgn.h"
    struct {
        unsigned_16     cmt_type;
        unsigned_8      lnk_dir;
    }           cmt;
#include "cgnoalgn.h"

    EjectImports();
    obj = CurrSeg->obj;
    if( obj->data.used > CurrSeg->data_prefix_size ) {
        SetPatches();
        SetAbsPatches();
        if( CurrSeg->comdat_label != NULL ) {
            PutObjRec( PickOMF( CMD_COMDAT ), obj->data.array,
                        obj->data.used );
        } else {
            #ifdef _OMF_32
                PutObjRec( PickOMF( CMD_LEDATA ), obj->data.array,
                        obj->data.used );
            #else //SEG32DBG dwarf, codview
                if( (CurrSeg->attr & SEG_USE_32) && (_IsntTargetModel( EZ_OMF )) ){
                    PutObjRec( CMD_LEDATA32, obj->data.array,
                                obj->data.used );
                }else{
                    PutObjRec( PickOMF( CMD_LEDATA ), obj->data.array,
                                obj->data.used );
                }
            #endif
        }
        if( obj->fixes.used != 0 ) {
            if( CurrSeg->data_ptr_in_code ) {
                cmt.cmt_type = LINKER_COMMENT;
                cmt.lnk_dir = LDIR_OPT_UNSAFE;
                PutObjRec( CMD_COMENT, (void *)&cmt, sizeof( cmt ) );
            }
            PutObjRec( PickOMF( CMD_FIXUPP ), obj->fixes.array, obj->fixes.used );
            obj->fixes.used = 0;
        }
        CurrSeg->data_ptr_in_code = FALSE;
        obj->data.used = 0;
        obj->patches = NULL;
    }
}


static  void    EjectExports() {
/******************************/

    object      *obj;
    cmd_omf     rec;

    obj = CurrSeg->obj;
    if( obj->exports != NULL && obj->exports->used != 0 ) {
        if( obj->gen_static_exports ) {
            rec = CMD_LPUBDEF;
        } else {
            rec = CMD_PUBDEF;
        }
        PutObjRec( PickOMF( rec ), obj->exports->array, obj->exports->used );
        obj->exports->used = 0;
    }
}


static  void    FlushObject() {
/*****************************/

    object      *obj;

    SetUpObj( FALSE );
    GenComdef();
    CurrSeg->total_comdat_size += CurrSeg->comdat_size;
    EjectLEData();
    EjectExports();
    obj = CurrSeg->obj;
    if( obj->lines != NULL ) {
        FlushLineNum();
        KillArray( obj->lines );
        obj->lines = NULL;
    }
    FreeObjCache();
}


static  void    EndModule() {
/***************************/

    byte        b;

    b = 0;                     /* non-main module, no start address*/
//  There is a bug in MS's LINK386 program that causes it not to recognize a
//  MODEND386 record in some situations. We can get around it by only outputing
//  16-bit MODEND records. This causes us no pain, since we never need any
//  features provided by the 32-bit form anyway. --- BJS
//    PutObjRec( PickOMF( CMD_MODEND ), &b, sizeof( byte ) );

    PutObjRec( CMD_MODEND, &b, sizeof( byte ) );
}


/*%%     Utility routines for filling the buffer*/
extern char GetMemModel( void ){
/***************************/
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

static void OutModel( array_control *dest ) {
/*******************************************/

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
    model[ 4 ] = 'd';
    if( _IsModel( POSITION_INDEPENDANT ) ) {
        model[ 4 ] = 'i';
    }
    model[5] = '\0';
    OutString( model, dest );
}

static  void    OutString( char *name, array_control *dest ) {
/************************************************************/

    int len;

    len = Length( name );
    NeedMore( dest, len );
    _CopyTrans( name, &_ARRAY( dest, char ), len );
    dest->used += len;
}

static  void    OutName( char *name, array_control *dest ) {
/**********************************************************/

    int len;

    len = Length( name );
    NeedMore( dest, len + 1 );
    _ARRAY( dest, char ) = len;
    dest->used++;
    _CopyTrans( name, &_ARRAY( dest, char ), len );
    dest->used += len;
}

static  void    OutObjectName( sym_handle sym, array_control *dest ) {
/********************************************************************/

    DoOutObjectName( sym, OutName, dest, NORMAL );
}


#if 0
static  void    OutConcat( char *name1, char *name2, array_control *dest ) {
/**************************************************************************/

    int len1;
    int len2;

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


static  void    SetMaxWritten() {
/*******************************/

    if( CurrSeg->location > CurrSeg->max_written ) {
        CurrSeg->max_written = CurrSeg->location;
    }
}


extern  void    OutDataByte( byte value ) {
/*****************************************/

    int i;

    SetPendingLine();
    CheckLEDataSize( sizeof( byte ), TRUE );
    i = CurrSeg->location - CurrSeg->obj->start + CurrSeg->data_prefix_size;
    IncLocation( sizeof( byte ) );
    if( i >= Out->used ) {
        NeedMore( Out, i - Out->used + sizeof( byte ) );
        Out->used = i + sizeof( byte );
    }
    SetMaxWritten();
    OutBuff[ i ] = value;
}

extern  void    OutDBytes( unsigned_32 len, byte *src ) {
/*******************************************************/

    int         i;
    unsigned    max;
    unsigned    n;

    SetPendingLine();
    CheckLEDataSize( sizeof( byte ), TRUE );
    i = CurrSeg->location - CurrSeg->obj->start + CurrSeg->data_prefix_size;
    max = (BUFFSIZE - TOLERANCE) - i;
    while( len != 0 ) {
        if( len > max ) {
            n = max;
        } else {
            n = len;
        }
        NeedMore( Out, i - Out->used + n );
        if( i + n >= Out->used ) {
            Out->used = i + n;
        }
        IncLocation( n );
        SetMaxWritten();
        memcpy( &OutBuff[ i ], src, n );
        src += n;
        len -= n;
        if( len == 0 ) break;
        EjectLEData();
        OutLEDataStart( FALSE );
        i = CurrSeg->data_prefix_size;
        max = (BUFFSIZE - TOLERANCE);
    }
}


extern  void    OutDLLExport( uint words, sym_handle sym ) {
/**********************************************************/


    SetUpObj( FALSE );
    EjectLEData();
    NeedMore( Out, sizeof( unsigned_16 ) );
    _ARRAY( Out, unsigned_16 ) = _TargetInt( EXPORT_COMMENT );
    Out->used += sizeof( unsigned_16 );
    OutByte( 2 );
#if _TARGET & _TARG_IAPX86
    OutByte( words );
#else
    // this should be 0 for everything except callgates to
    // 16-bit segments (from MS Knowledge Base)
    OutByte( 0 );
#endif
    OutObjectName( sym, Out );
    OutByte( 0 );
    PutObjRec( CMD_COMENT, Out->array, Out->used );
    Out->used = 0;
    EjectLEData();
}


extern  void    OutSelect( bool starts ) {
/****************************************/

    object      *obj;

    if( starts ) {
        SelIdx = CurrSeg->sidx;
        SelStart = CurrSeg->location;
    } else if( SelIdx != 0 ) {
        if( !CurrSeg->start_data_in_code ) {
            EjectLEData();
            obj = CurrSeg->obj;
            Out = &obj->data;
            OutBuff = obj->data.array;
            Out->used = 0;
            NeedMore( Out, sizeof( unsigned_16 ) );
            _ARRAY( Out, unsigned_16 ) = _TargetInt( DISASM_COMMENT );
            Out->used += sizeof( unsigned_16 );
            #ifdef _OMF_32
                OutByte( DDIR_SCAN_TABLE_32  );
            #else
                OutByte( DDIR_SCAN_TABLE  );
            #endif
            if( CurrSeg->comdat_label != NULL ) {
                OutIdx( 0, Out );
                OutIdx( NeedComdatNidx( NORMAL ), Out );
            } else {
                OutIdx( SelIdx, Out );
            }
            OutOffset( SelStart );
            OutOffset( CurrSeg->location );
            PutObjRec( CMD_COMENT, Out->array, Out->used );
            Out->used = 0;
        }
        SelIdx = 0;
    }
    CurrSeg->start_data_in_code = FALSE;
}

static void FlushSelect()
{
    if( SelIdx != 0 ) OutSelect( FALSE );
}


static  void    OutByte( byte value ) {
/*************************************/

    NeedMore( Out, 1 );
    OutBuff[  Out->used++  ] = value;
}

extern  void    OutIBytes( byte pat, offset len ) {
/**************************************************/

    object      *obj;

    SetPendingLine();
    if( len <= TRADEOFF ) {
        while( len != 0 ) {
            OutDataByte( pat );
            --len;
        }
    } else {
        EjectLEData();
        obj = CurrSeg->obj;
        OutLEDataStart( TRUE );
#ifdef _OMF_32
        if( _IsntTargetModel( EZ_OMF ) )
            OutOffset( len );           /* repeat count */
        else
#endif
            OutInt( len );              /* repeat count*/
        OutInt( 0 );                    /* nesting count*/
        OutByte( 1 );                   /* pattern length*/
        OutByte( pat );
        PutObjRec(
            PickOMF((CurrSeg->comdat_label!=NULL) ? CMD_COMDAT : CMD_LIDATA),
            obj->data.array, obj->data.used );
        obj->data.used = 0;
        IncLocation( len );
        SetMaxWritten();
    }
}


extern  void    OutDataLong( long value ) {
/*****************************************/

    OutDataInt( value );
    OutDataInt( value >> 16 );
}


extern  void    OutDataInt( int value ) {
/***************************************/

    int i;

    SetPendingLine();
    CheckLEDataSize( sizeof( unsigned_16 ), TRUE );
    i = CurrSeg->location - CurrSeg->obj->start + CurrSeg->data_prefix_size;
    IncLocation( sizeof( unsigned_16 ) );
    if( i + sizeof( unsigned_16 ) > Out->used ) {
        NeedMore( Out, i - Out->used + sizeof( unsigned_16 ) );
        Out->used = i + sizeof( unsigned_16 );
    }
    SetMaxWritten();
    *(unsigned_16 *)&OutBuff[ i ] = _TargetInt( value );
}


static  void    OutOffset( offset value ) {
/***********************************/

    NeedMore( Out, sizeof( offset ) );
    *(offset *)&OutBuff[ Out->used ] = _TargetOffset( value );
    Out->used += sizeof( offset );
}

#ifndef _OMF_32  // 32bit debug seg support dwarf,codview
static  void    OutLongOffset( long_offset value ) {
/***********************************/

    NeedMore( Out, sizeof( long_offset ) );
    *(long_offset *)&OutBuff[ Out->used ] = _TargetLongInt( value );
    Out->used += sizeof( long_offset );
}
#endif
static  void    OutInt( int value ) {
/***********************************/

    NeedMore( Out, sizeof( unsigned_16 ) );
    *(unsigned_16 *)&OutBuff[ Out->used ] = _TargetInt( value );
    Out->used += sizeof( unsigned_16 );
}

static  void    CheckLEDataSize( int max_size, bool need_init ) {
/***************************************************************/

    long_offset      start;
    int              used;
    object          *obj;
    long_offset      end_valid;

    obj = CurrSeg->obj;
    start = obj->start;
    used = obj->data.used;
    if( CurrSeg->location < start ) {
        EjectLEData();
    } else if( CurrSeg->location - start + max_size
        > BUFFSIZE - TOLERANCE - CurrSeg->data_prefix_size ) {
        EjectLEData();
    } else {
        end_valid = start + used - CurrSeg->data_prefix_size;
        if( CurrSeg->max_written > end_valid
            && CurrSeg->location > end_valid ) {
            EjectLEData();
        }
    }
    if( need_init ) OutLEDataStart( FALSE );
}

static  void    OutIdx( int value, array_control *dest ) {
/********************************************************/

    NeedMore( dest, 2 );
    if( value >= 128 ) {
        _ARRAY( dest, byte ) = (value >> 8) | 0x80;
        dest->used++;
    }
    _ARRAY( dest, byte ) = value;
    dest->used++;
}


static  void    OutLEDataStart( bool iterated ) {
/***********************************************/

    byte        flag;
    index_rec   *rec;

    rec = CurrSeg;
    if( Out->used == 0 ) {
        if( rec->comdat_label != NULL ) {
            flag = 0;
            if( rec->location != 0 ) flag |= 1; /* continued */
            if( iterated ) flag |= 2;   /* LIDATA form */
            if( !(FEAttr( rec->comdat_symbol ) & FE_GLOBAL) ) {
                flag |= 0x4;    /* local comdat */
            }
            OutByte( flag );
            OutByte( 0x10 );
            OutByte( 0 );
            OutOffset( rec->location );
            OutIdx( 0, Out );
            if( rec->btype == BASE_GRP ) {
                OutIdx( CurrSeg->base, Out );   /* group index*/
            } else {
                OutIdx( 0, Out );
            }
            OutIdx( rec->sidx, Out );   /* segment index*/
            OutIdx( NeedComdatNidx( NORMAL ), Out );
        } else { // LEDATA
            OutIdx( rec->sidx, Out );
            #ifdef _OMF_32
                OutOffset( rec->location );
            #else  //SEG32DBG dwarf, codview
                if( (rec->attr & SEG_USE_32 ) ){
                    OutLongOffset( rec->location );
                }else{
                    OutOffset( rec->location );
                }
            #endif
        }
        rec->obj->start = rec->location;
        rec->data_prefix_size = Out->used;
    }
    if( rec->start_data_in_code ) OutSelect( TRUE );
}


/* Array Control Routines*/

/* DO NOT call InitArray with size or starting value zero*/

static  array_control   *InitArray( int size, int starting, int increment ) {
/***************************************************************************/

    array_control       *res;

    _Alloc( res, sizeof( array_control ) );
    FillArray( res, size, starting, increment );
    return( res );
}


static  void    FillArray( array_control *res, int size,
                           int starting, int increment ) {
/********************************************************/

    _Alloc( res->array, starting * size );
    res->alloc = starting;
    res->used = 0;
    res->entry = size;
    res->inc = increment;
}

static  void    NeedMore( array_control *arr, int more ) {
/********************************************************/

    byte        *p;
    unsigned    need;
    unsigned    new;

    need = arr->used + more;
    if( need > arr->alloc ) {
        new = arr->alloc;
        for( ;; ) {
            new += arr->inc;
            if( new >= need ) break;
        }
        _Alloc( p, arr->entry * new );
        Copy( arr->array, p, arr->entry * arr->used );
        if( arr == Out ) {
            OutBuff = p;
        }
        _Free( arr->array, arr->entry * arr->alloc );
        arr->array = p;
        arr->alloc = new;
    }
}

static  void    KillArray( array_control *arr ) {
/***********************************************/

    KillStatic( arr );
    _Free( arr, sizeof( array_control ) );
}

static  void    KillStatic( array_control *arr ) {
/************************************************/

    _Free( arr->array, arr->entry * arr->alloc );
}

extern  seg_id  SetOP( seg_id seg ) {
/***********************************/

    seg_id      old;

    if( CurrSeg == NULL ) {
        old = (seg_id)-1;
    } else {
        old = CurrSeg->seg;
    }
    if( seg == (seg_id)-1 ) {
        CurrSeg = NULL;
    } else {
        CurrSeg = AskSegIndex( seg );
    }
    return( old );
}

extern  seg_id  AskOP() {
/************************/

    return( CurrSeg->seg );
}

extern  bool    NeedBaseSet() {
/****************************/

    bool        need;

    need = CurrSeg->need_base_set;
    CurrSeg->need_base_set = 0;
    return( need );
}


extern  offset  AskLocation() {
/*****************************/

    return( CurrSeg->location );
}

extern  offset  AskMaxSize() {
/*****************************/

    return( CurrSeg->max_size );
}

extern  void    SetBigLocation( long_offset loc ) {
/*****************************************/

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

extern  long_offset  AskBigLocation() {
/*****************************/

    return( CurrSeg->location );
}

extern  long_offset  AskBigMaxSize() {
/*****************************/

    return( CurrSeg->max_size );
}

extern  void    SetLocation( offset loc ) {
/*****************************************/

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

extern  void    IncLocation( offset by ) {
/****************************************/

    unsigned_32 sum;

    CurrSeg->obj->pending_line_number = 0;
    sum = CurrSeg->location + by;
    if( _IsntTargetModel( EZ_OMF ) && !(CurrSeg->attr & SEG_USE_32 ) ) {
        sum &= 0xFFFF;
    }
    if( sum < CurrSeg->location ) { /* if wrapped*/
        if( sum != 0 || CurrSeg->big == 1 ) {
            FatalError( "segment too large" );
        } else if( CurrSeg->comdat_label == NULL ) {
            CurrSeg->big = 1;
            if( CurrSeg->attr & SEG_USE_32  ){
                CurrSeg->max_size = (long_offset)(-1);
                CurrSeg->location = (long_offset)(-1);
            }else{
                CurrSeg->max_size = (short_offset)(-1);
                CurrSeg->location =  1L << 8*sizeof( short_offset );
            }
        }
    } else {
        SetBigLocation( sum );
    }
}


static  void    DecLocation( offset by ) {
/****************************************/

    SetLocation( CurrSeg->location - by );
}

extern  void    TellObjNewLabel( sym_handle lbl ) {
/*************************************************/

    if( lbl == NULL ) return;
    if( CurrSeg == NULL ) return;
    if( CodeSeg != CurrSeg->seg ) return;

    /*
       We've got a data label going into a code segment.
       Make sure everything's OK with regards to COMDATs.
    */
    if( FEAttr( lbl ) & FE_COMMON ) {
        DoEmptyQueue();
        if( _IsModel( DBG_DF ) ){
            if( CurrSeg->comdat_symbol != NULL ){
                DFSymRange( CurrSeg->comdat_symbol, CurrSeg->comdat_size );
            }
        }
    } else if( CurrSeg->comdat_symbol != NULL ) {
        DoEmptyQueue();
        SetUpObj( FALSE );
        if( _IsModel( DBG_DF ) ){
            if( CurrSeg->comdat_symbol != NULL ){
                DFSymRange( CurrSeg->comdat_symbol, CurrSeg->comdat_size );
            }
        }
    }
}

extern  void    TellObjNewProc( sym_handle proc ) {
/*************************************************/

    seg_id      old;
    segment_id  proc_id;


    old = SetOP( CodeSeg );
    proc_id = FESegID( proc );
    if( CodeSeg != proc_id ) {
        if( _IsModel( DBG_DF ) ){
            if( CurrSeg->comdat_symbol != NULL ){
                DFSymRange( CurrSeg->comdat_symbol, CurrSeg->comdat_size );
            }
        }
        if( CurrSeg->obj != NULL ) {
            DoEmptyQueue();
            FlushObject();
        }
        CodeSeg = proc_id;
        SetOP( CodeSeg );
        CurrSeg->need_base_set = TRUE;
        if( !CurrSeg->exec ) Zoiks( ZOIKS_088 );
    }
    if( FEAttr( proc ) & FE_COMMON ) {
        DoEmptyQueue();
        if( _IsModel( DBG_DF ) ){
            if( CurrSeg->comdat_symbol != NULL ){
                DFSymRange( CurrSeg->comdat_symbol, CurrSeg->comdat_size );
            }
        }
        CurrSeg->comdat_symbol = proc;
        CurrSeg->prefix_comdat_state = PCS_NEED;
    } else if( CurrSeg->comdat_symbol != NULL ) {
        DoEmptyQueue();
        SetUpObj( FALSE );
        if( _IsModel( DBG_DF ) ){
            if( CurrSeg->comdat_symbol != NULL ){
                DFSymRange( CurrSeg->comdat_symbol, CurrSeg->comdat_size );
            }
        }
        NormalData();
    }
    SetOP( old );
}

extern void     TellObjVirtFuncRef( void *cookie ) {
/**************************************************/

    seg_id              old;
    virt_func_ref_list  *new;

    old = SetOP( CodeSeg );
    _Alloc( new, sizeof( virt_func_ref_list ) );
    new->cookie = cookie;
    new->next = CurrSeg->virt_func_refs;
    CurrSeg->virt_func_refs = new;
    SetOP( old );
}

static  bool            InlineFunction( pointer hdl ) {
/*****************************************************/
    call_class          rtn_class;
    aux_handle          aux;

    if( ( FEAttr( hdl ) & FE_PROC ) == 0 ) return( FALSE );
    aux = FEAuxInfo( hdl, AUX_LOOKUP );
    if( FEAuxInfo( aux, CALL_BYTES ) != NULL ) return( TRUE );
    rtn_class = *(call_class *)FEAuxInfo( aux, CALL_CLASS );
    return( rtn_class & MAKE_CALL_INLINE );
}

extern  segment_id      AskSegID( pointer hdl, cg_class class ) {
/******************************************************************/

    switch( class ) {
    case CG_FE:
        if( InlineFunction( hdl ) ) {
            return( AskCodeSeg() );
        }
        return( FESegID( hdl ) );
    case CG_BACK:
        return( ((bck_info*)hdl)->seg );
    case CG_TBL:
    case CG_VTB:
        return( AskCodeSeg() );
    case CG_CLB:
        return( AskAltCodeSeg() );
    default:
        return( AskBackSeg() );
    }
}

extern  bool            AskNameCode( pointer hdl, cg_class class ) {
/******************************************************************/

    switch( class ) {
    case CG_FE:
        return( (FEAttr( hdl ) & FE_PROC) != 0 );
    case CG_TBL:
    case CG_VTB:
    case CG_CLB:
        return( TRUE );
    }
    return( FALSE );
}

extern  bool            AskNameROM( pointer hdl, cg_class class ) {
/*****************************************************************/

    return( AskSegROM( AskSegID( hdl, class ) ) );
}

