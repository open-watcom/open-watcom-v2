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
* Description:  Front end routines defined for optimizing code generator.
*
****************************************************************************/


#include "ftnstd.h"
#include "global.h"
#include "wf77defs.h"
#include "wf77aux.h"
#include "fcgbls.h"
#include "ecflags.h"
#include "segsw.h"
#include "progsw.h"
#include "cpopt.h"
#include "cgflags.h"
#include "types.h"
#include "errcod.h"
#include "csetinfo.h"
#include "ferror.h"
#include "inout.h"
#include "fctypes.h"
#include "cspawn.h"
#include "stdio.h"

#include "langenvd.h"
#if _CPU == 386 || _CPU == 8086
  #define __TGT_SYS     __TGT_SYS_X86
#elif _CPU == _AXP
  #define __TGT_SYS     __TGT_SYS_AXP_NT
#elif _CPU == _PPC
  #define __TGT_SYS     __TGT_SYS_PPC_NT
#else
  #error Unknown platform
#endif
#include "langenv.h"

#include "cg.h"
#include "cgswitch.h"
#include "cgprotos.h"
#include "feprotos.h"

#include "wf77segs.h"

#include <string.h>
#include <stdlib.h>


extern  int             MakeName(char *,char *,char *);
extern  char            *SDExtn(char *,char *);
extern  char            *SDFName(char *);
extern  char            *STGetName(sym_id,char *);
extern  char            *STExtractName(sym_id,char *);
extern  intstar4        GetComBlkSize(sym_id);
extern  aux_info        *AuxLookup(sym_id);
extern  void            SendBlip(void);
extern  void            SendStd(char *);
extern  char            *STFieldName(sym_id,char *);
extern  char            *ErrorInitializer(void);
extern  bool            ForceStatic(unsigned_16);
extern  sym_id          FindArgShadow(sym_id);
extern  sym_id          STEqSetShadow(sym_id);
extern  char *          StackBuffer(int *);

extern  global_seg      *CurrGSeg;
extern  global_seg      *GlobalSeg;
extern  cgflags_t       CGFlags;
extern  aux_info        FortranInfo;
extern  aux_info        DefaultInfo;
extern  character_set   CharSetInfo;
extern  char            ProgName[];
extern  char            ObjExtn[];
extern  default_lib     *DefaultLibs;
extern  dep_info        *DependencyInfo;

static  void            DefDbgStruct( sym_id sym );

#define CS_SUFF_LEN             5
#define G_DATA_LEN              6
#define BLANK_COM_LEN           6

static  char            GData[] = { "GDATA@" };
#if _CPU == 8086 || _CPU == 386
static  char            *CSSuff = TS_SEG_CODE;
static  unsigned char   CodeAlignSeq[] = { 2, sizeof( inttarg ), 1 };
static  unsigned char   DefCodeAlignSeq[] = { 2, 1, 1 };
#endif
static  sym_id          ImpSym;
static  segment_id      CurrSegId;
static  segment_id      CurrImpSegId;
static  cg_type         UserType;

static  dbg_type        DBGTypes[] = {
    #define ONLY_BASE_TYPES
    #define pick(id,type,dbgtype,cgtype,inpfun,outfun,typnam) dbgtype,
    #include "ptypdefn.h"
    #undef pick
    #undef ONLY_BASE_TYPES
};

static  char * DBGNames[] = {
    #define ONLY_BASE_TYPES
    #define pick(id,type,dbgtype,cgtype,inpfun,outfun,typnam) typnam,
    #include "ptypdefn.h"
    #undef pick
    #undef ONLY_BASE_TYPES
};


extern  sym_id                  STShadow(sym_id);
extern  sym_id                  FindShadow(sym_id);
extern  sym_id                  FindEqSetShadow(sym_id);
extern  uint                    SymAlign(sym_id);

/* Forward declarations */
static  void    SegBytes( unsigned_32 size );
static  void    DefineGlobalSeg( global_seg *seg );
static  void    DefineGlobalSegs( void );
static  void    DefineCommonSegs( void );
static  void    AllocGlobalSegs( void );
static  void    AllocCommonSegs( void );
static  void    DefCodeSeg( void );
static  void    BldCSName( char *buff );
static  void    AllocComBlk( sym_id cb );
segment_id       GetGlobalSeg( unsigned_32 g_offset );


#define _Shadow( s )    if( (s->ns.flags & SY_CLASS) == SY_VARIABLE ) { \
                            if( s->ns.flags & SY_SPECIAL_PARM ) { \
                                s = FindShadow( s ); \
                            } else { \
                                s = STShadow( s ); \
                                s->ns.u3.address = NULL; \
                                _MgcSetClass( s, MAGIC_SHADOW ); \
                            } \
                        }
#define _UnShadow( s )  if( (s != NULL) && (_MgcClass(s) == MAGIC_SHADOW) ) { \
                            s = sym->ns.si.ms.sym; \
                        }

segment_id              CurrCodeSegId;

#define SYM_MANGLE_PRE          "_COMMON_"
#define SYM_MANGLE_POST         "_DATA"
#define SYM_MANGLE_PRE_LEN      8
#define SYM_MANGLE_POST_LEN     5
#define SYM_MANGLE_LEN          (SYM_MANGLE_PRE_LEN + SYM_MANGLE_POST_LEN)
#if _CPU == 8086 || _CPU == 386
static char                     MangleSymBuff[MAX_SYMLEN+4+SYM_MANGLE_LEN];
#endif


static  unsigned        MangleCommonBlockName( sym_id sym, char *buffer,
                                               bool class ) {
//===========================================================

    unsigned    cb_len;

    cb_len = sym->ns.u2.name_len;
    if( CGOpts & CGOPT_MANGLE ) {
        cb_len += SYM_MANGLE_PRE_LEN;
        strcpy( buffer, SYM_MANGLE_PRE );
        STGetName( sym, &buffer[SYM_MANGLE_PRE_LEN] );
        if( class ) {
            strcpy( &buffer[cb_len], SYM_MANGLE_POST );
            cb_len += SYM_MANGLE_POST_LEN;
        }
    } else {
        STGetName( sym, buffer );
    }
    return( cb_len );
}


static  segment_id      AllocSegId( void ) {
//====================================

    segment_id  seg;

    seg = CurrSegId;
    ++CurrSegId;
    return( seg );
}


void    InitSubSegs( void ) {
//=====================

    CurrImpSegId = -1;
}


segment_id      AllocImpSegId( void ) {
//===============================

    segment_id  seg;

    seg = CurrImpSegId;
    --CurrImpSegId;
    return( seg );
}


void    InitSegs( void ) {
//==================

// Define segments.

    CurrSegId = SEG_FREE;
#if _CPU == _AXP || _CPU == _PPC
    BEDefSeg( SEG_TDATA, EXEC|GLOBAL|GIVEN_NAME, TS_SEG_CODE, ALIGN_DWORD );
    CurrCodeSegId = SEG_TDATA;
#endif
    BEDefSeg( SEG_CDATA, BACK | INIT | ROM, TS_SEG_CONST, ALIGN_SEGMENT );
    BEDefSeg( SEG_LDATA, INIT, TS_SEG_DATA, ALIGN_SEGMENT );
    BEDefSeg( SEG_UDATA, 0, TS_SEG_BSS, ALIGN_SEGMENT );
    DefineGlobalSegs();
    DefineCommonSegs();
    LDSegOffset = 0;
    GSegOffset = 0;
}


void    FiniSegs( void ) {
//==================

// Finish segment processing.

    sym_id      sym;

    for( sym = GList; sym != NULL; sym = sym->ns.link ) {
        if( ( sym->ns.flags & SY_CLASS ) != SY_COMMON ) continue;
        BEFreeBack( sym->ns.u3.address );
    }
}


void    AllocSegs( void ) {
//===================

// Allocate segments.

    AllocGlobalSegs();
    AllocCommonSegs();
}


void    SubCodeSeg( void ) {
//====================

// Define a code segment for a subprogram.
#if _CPU == 8086 || _CPU == 386
    DefCodeSeg();
#endif
}


#if _CPU == 8086 || _CPU == 386
static  unsigned char   *AlignmentSeq( void ) {
//=============================================

    if( OZOpts & OZOPT_O_TIME ) {
        return( CodeAlignSeq );
    } else {
        return( DefCodeAlignSeq );
    }
}


static  void    DefCodeSeg( void ) {
//============================

// Define a code segment.

    char            seg_name[MAX_SYMLEN+CS_SUFF_LEN+1];
    unsigned char   *align_info_bytes;
    int             i;
    int             alignment;

    align_info_bytes = AlignmentSeq();  // variable length
    alignment = 1;
    for( i = 1; i < align_info_bytes[0]; ++i ) {
        if( alignment < align_info_bytes[i] ) {
            alignment = align_info_bytes[i];
        }
    }
    BldCSName( seg_name );
    CurrCodeSegId = AllocSegId();
    BEDefSeg( CurrCodeSegId, EXEC|GLOBAL|GIVEN_NAME, seg_name, alignment );
}


static  void    BldCSName( char *buff ) {
//=======================================

// Build code segment name.

#if _CPU == 8086
    strcpy( STGetName( SubProgId, buff ), CSSuff );
#else
    strcpy( buff, CSSuff );
#endif
}
#endif


static  void    DefineCommonSegs( void ) {
//==================================

// Define segments for a common blocks.

    unsigned_32 com_size;
    int         seg_count;
    sym_id      sym;
    int         cb_len;
    int         private;

    char        cb_name[MAX_SYMLEN+4+SYM_MANGLE_LEN];

#if _CPU == 386 || _CPU == 8086
    if( _SmallDataModel( CGOpts ) ) {
        private = INIT; // so segment doesn't get put in BSS class
    } else {
        private = PRIVATE;
    }
#endif
    for( sym = GList; sym != NULL; sym = sym->ns.link ) {
        if( ( sym->ns.flags & SY_CLASS ) != SY_COMMON ) continue;
#if _CPU == _AXP || _CPU == _PPC
        if( sym->ns.flags & SY_COMMON_INIT ) {
            private = INIT | COMDAT;
        } else {
            private = 0;
        }
#endif
        cb_len = MangleCommonBlockName( sym, cb_name, FALSE );

        sym->ns.si.cb.seg_id = AllocSegId();
        if( CGOpts & CGOPT_ALIGN ) {
            BEDefSeg( sym->ns.si.cb.seg_id, COMMON | private, cb_name, ALIGN_SEGMENT );
        } else {
            BEDefSeg( sym->ns.si.cb.seg_id, COMMON | private, cb_name, ALIGN_BYTE );
        }
        seg_count = 0;
        cb_name[ cb_len ] = '@';
        com_size = GetComBlkSize( sym );
        for(;;) {
            if( com_size <= MaxSegSize ) break;
            com_size -= MaxSegSize;
            seg_count++;
            itoa( seg_count, &cb_name[ cb_len + 1 ], 10 );
            if( CGOpts & CGOPT_ALIGN ) {
                BEDefSeg( AllocSegId(), COMMON | private , cb_name, ALIGN_SEGMENT );
            } else {
                BEDefSeg( AllocSegId(), COMMON | private , cb_name, ALIGN_BYTE );
            }
        }
    }
}


static  void    AllocCommonSegs( void ) {
//=================================

// Allocate segments for a common blocks.

    sym_id      sym;

    for( sym = GList; sym != NULL; sym = sym->ns.link ) {
        if( ( sym->ns.flags & SY_CLASS ) != SY_COMMON ) continue;
        AllocComBlk( sym );
    }
}


static  void    AllocComBlk( sym_id cb ) {
//========================================

// Allocate a common block.

    segment_id  segment;
    unsigned_32 size;

    segment = cb->ns.si.cb.seg_id;
    BESetSeg( segment );
    cb->ns.u3.address = BENewBack( cb );
    DGLabel( cb->ns.u3.address );
    size = GetComBlkSize( cb );
    while( size > MaxSegSize ) {
        BESetSeg( segment );
        SegBytes( MaxSegSize );
        size -= MaxSegSize;
        segment++;
    }
    BESetSeg( segment );
    SegBytes( size );
}


static  void    SegBytes( unsigned_32 size ) {
//============================================

#if _CPU == 386
    DGUBytes( size );
#else
    if( size == MaxSegSize ) {
        DGUBytes( size - 1 ); // back end can't handle value of 64k
        DGUBytes( 1 );
    } else {
        DGUBytes( size );
    }
#endif
}


static  void   DefineGlobalSeg( global_seg *seg ) {
//==========================================================

// Define a global segment.

    int         private;
    char        g_name[G_DATA_LEN+3];

    seg->segment = AllocSegId();
    memcpy( g_name, GData, G_DATA_LEN );
    itoa( seg->segment - GlobalSeg->segment, &g_name[ G_DATA_LEN ], 10 );

#if _CPU == 386 || _CPU == 8086
    if( _SmallDataModel( CGOpts ) ) {
        if( seg->initialized ) {
            private = INIT; // so segment doesn't get put in BSS class
        } else {
            private = 0;
        }
    } else {
        private = PRIVATE;
    }
#else
    if( seg->initialized ) {
        private = INIT; // so segment doesn't get put in BSS class
    } else {
        private = 0;
    }
#endif

    BEDefSeg( seg->segment, private, g_name, ALIGN_SEGMENT );
}


static  void    DefineGlobalSegs( void ) {
//==================================

// Define global segments.

    global_seg  *g_seg;

    g_seg = GlobalSeg;
    while( g_seg != NULL ) {
        DefineGlobalSeg( g_seg );
        g_seg = g_seg->link;
    }
}


static  void    AllocGlobalSegs( void ) {
//=================================

// Allocate global segments.

    global_seg  *g_seg;

    g_seg = GlobalSeg;
    while( g_seg != NULL ) {
        BESetSeg( g_seg->segment );
        SegBytes( g_seg->size );
        g_seg = g_seg->link;
    }
}


static  global_seg      *GSegDesc( unsigned_32 g_offset ) {
//=========================================================

// Find global segment descriptor for given offset.

    global_seg  *g_seg;
    unsigned_32 g_size;

    g_seg = GlobalSeg;
    g_size = 0;
    for(;;) {
        if( g_size + g_seg->size > g_offset ) break;
        g_size += g_seg->size;
        g_seg = g_seg->link;
    }
    return( g_seg );
}


void    DtInit( segment_id seg, seg_offset offset ) {
//===================================================

// Set to do DATA initialization.

    if( DtOffset >= MaxSegSize - offset ) {
        seg++;
        DtSegOffset = DtOffset - (MaxSegSize - offset);
        while( DtSegOffset >= MaxSegSize ) {
            seg++;
            DtSegOffset -= MaxSegSize;
        }
    } else {
        DtSegOffset = offset + DtOffset;
    }
    DtSegment = seg;
}


struct {
    segment_id  seg;
    seg_offset  offset;
    uint        size;
    char        byte_value;
} CurrDt;


static  void    InitBytes( unsigned long size, byte value ) {
//===========================================================

#if _CPU == 386
    DGIBytes( size, value );
#else
    if( size == MaxSegSize ) {
        // back end can't handle size of 64k
        DGIBytes( MaxSegSize / 2, value );
        DGIBytes( MaxSegSize / 2, value );
    } else {
        DGIBytes( size, value );
    }
#endif
}


static  void    UndefBytes( unsigned long size, byte *data ) {
//============================================================

#if _CPU == 386
    DGBytes( size, data );
#else
    if( size == MaxSegSize ) {
        // back end can't handle size of 64k
        DGBytes( MaxSegSize / 2, data );
        DGBytes( MaxSegSize / 2, data );
    } else {
        DGBytes( size, data );
    }
#endif
}


static  void    FlushCurrDt( void ) {
//=============================

    if( CurrDt.seg != SEG_NULL ) {
        BESetSeg( CurrDt.seg );
        DGSeek( CurrDt.offset );
        InitBytes( CurrDt.size, CurrDt.byte_value );
    }
}


static  void    InitCurrDt( void ) {
//============================

    CurrDt.seg = SEG_NULL;
    CurrDt.offset = 0;
    CurrDt.byte_value = 0;
    CurrDt.size = 0;
}


void    DtIBytes( byte data, int size ) {
//=======================================

// Initialize with specified data.

    if( (DtSegment == CurrDt.seg) &&
        (DtSegOffset == CurrDt.offset + CurrDt.size) &&
        (data == CurrDt.byte_value) &&
        ((MaxSegSize - (CurrDt.offset + CurrDt.size)) >= size ) ) {
        // We are continuing where we left off
        CurrDt.size += size;
        DtSegOffset += size;
    } else {
        FlushCurrDt();
        if( MaxSegSize - DtSegOffset > size ) {
            CurrDt.seg = DtSegment;
            CurrDt.offset = DtSegOffset;
            CurrDt.byte_value = data;
            CurrDt.size = size;
            DtSegOffset += size;
        } else {
            BESetSeg( DtSegment );
            DGSeek( DtSegOffset );
            DGIBytes( MaxSegSize - DtSegOffset, data );
            size -= MaxSegSize - DtSegOffset;
            DtSegment++;
            DtSegOffset = size;
            CurrDt.seg = DtSegment;
            CurrDt.offset = DtSegOffset;
            CurrDt.byte_value = data;
            CurrDt.size = size;
        }
    }
}


void    DtStreamBytes( byte *data, int size ) {
//=============================================

// Initialize with specified data.

    FlushCurrDt();
    InitCurrDt();
    BESetSeg( DtSegment );
    DGSeek( DtSegOffset );
    if( MaxSegSize - DtSegOffset > size ) {
        UndefBytes( size, data );
        DtSegOffset += size;
    } else {
        UndefBytes( MaxSegSize - DtSegOffset, data );
        size -= MaxSegSize - DtSegOffset;
        DtSegment++;
        if( size != 0 ) {
            BESetSeg( DtSegment );
            DGSeek( 0 );
            UndefBytes( size, data + MaxSegSize - DtSegOffset );
            DtSegOffset = size;
        } else {
            DtSegOffset = 0;
        }
    }
}


void    DtBytes( byte *data, int size ) {
//=======================================

// Initialize with specified data.

    byte        byte_value;
    int         i;

    byte_value = *data;
    for( i = 1; i < size; ++i ) {
        if( data[i] != byte_value ) {
            DtStreamBytes( data, size );
            return;
        }
    }
    DtIBytes( byte_value, size );
}


void    DtStartSequence( void ) {
//=========================

    InitCurrDt();
}


void    DtFiniSequence( void ) {
//========================

    FlushCurrDt();
}


segment_id      GetComSeg( sym_id sym, unsigned_32 offset ) {
//===========================================================

// Get segment id of common block for variable in common.

    segment_id  segment;

    offset += sym->ns.si.va.vi.ec_ext->offset;
    segment = sym->ns.si.va.vi.ec_ext->com_blk->ns.si.cb.seg_id;
    while( offset > MaxSegSize ) {
        segment++;
        offset -= MaxSegSize;
    }
    return( segment );
}


segment_id      GetDataSegId( sym_id sym ) {
//==========================================

// Get segment containing data for given variable.

    segment_id  id;
    unsigned_32 offset;
    com_eq      *ce_ext;

    if( sym->ns.flags & SY_IN_EQUIV ) {
        offset = 0;
        for(;;) {
            ce_ext = sym->ns.si.va.vi.ec_ext;
            if( ce_ext->ec_flags & LEADER ) break;
            offset += ce_ext->offset;
            sym = ce_ext->link_eqv;
        }
        if( ce_ext->ec_flags & MEMBER_IN_COMMON ) {
            id = GetComSeg( sym, offset );
        } else {
            id = GetGlobalSeg( ce_ext->offset + offset );
        }
    } else if( sym->ns.flags & SY_IN_COMMON ) {
        id = GetComSeg( sym, 0 );
    } else if( sym->ns.flags & SY_SUBSCRIPTED ) {
        id = sym->ns.si.va.vi.seg_id;
    } else if( sym->ns.u1.s.typ == FT_CHAR ) {
        id = sym->ns.si.va.vi.seg_id;
    } else if( sym->ns.u1.s.typ == FT_STRUCTURE ) {
        id = sym->ns.si.va.vi.seg_id;
    } else if( sym->ns.flags & SY_DATA_INIT ) {
        id = SEG_LDATA;
    } else {
        id = SEG_UDATA;
    }
    return( id );
}


seg_offset      GetGlobalOffset( unsigned_32 g_offset ) {
//=======================================================

// Find offset in the global segment containing data at given offset.

    global_seg  *g_seg;
    unsigned_32 g_size;

    g_seg = GlobalSeg;
    g_size = 0;
    for(;;) {
        if( g_size + g_seg->size > g_offset ) break;
        g_size += g_seg->size;
        g_seg = g_seg->link;
    }
    return( g_offset - g_size );
}


seg_offset      GetComOffset( unsigned_32 offset ) {
//==================================================

// Get segment offset in common block for variable in common.

    while( offset >= MaxSegSize ) {
        offset -= MaxSegSize;
    }
    return( offset );
}


struct bck_info *FEBack( cg_sym_handle _sym ) {
//=============================================

// Return the back handle for the given symbol.

    void        *back_handle;
    sym_id      sym = _sym;

    if( ( sym->ns.flags & SY_CLASS ) == SY_COMMON ) {
        back_handle = sym->ns.u3.address;
    } else {
        if( sym->ns.u3.address == NULL ) {
            sym->ns.u3.address = BENewBack( sym );
        }
        back_handle = sym->ns.u3.address;
    }
    return( back_handle );
}


seg_offset      GetDataOffset( sym_id sym ) {
//===========================================

// Get offset in segment containing data for given variable.

    seg_offset  seg_offset;
    unsigned_32 offset;
    com_eq      *ce_ext;

    if( sym->ns.flags & SY_IN_EQUIV ) {
        offset = 0;
        for(;;) {
            ce_ext = sym->ns.si.va.vi.ec_ext;
            if( ce_ext->ec_flags & LEADER ) break;
            offset += ce_ext->offset;
            sym = ce_ext->link_eqv;
        }
        if( ce_ext->ec_flags & MEMBER_IN_COMMON ) {
            seg_offset = GetComOffset( ce_ext->offset + offset );
        } else {
            seg_offset = GetGlobalOffset( ce_ext->offset + offset );
        }
    } else if( sym->ns.flags & SY_IN_COMMON ) {
        seg_offset = GetComOffset( sym->ns.si.va.vi.ec_ext->offset );
    } else if( sym->ns.flags & SY_SUBSCRIPTED ) {
        seg_offset = DGBackTell( FEBack( sym ) );
    } else if( sym->ns.u1.s.typ == FT_CHAR ) {
        seg_offset = DGBackTell( sym->ns.si.va.u.bck_hdl );
    } else {
        seg_offset = DGBackTell( FEBack( sym ) );
    }
    return( seg_offset );
}


segment_id  GetGlobalSeg( unsigned_32 g_offset ) {
//================================================

// Find global segment containing data at given offset.

    return( GSegDesc( g_offset )->segment );
}


void    DefTypes( void ) {
//==================

// Define FORTRAN 77 data types.

    int         adv_cnt;
    int         adv_size;
    int         total_size;

#if _CPU == 386 || _CPU == 8086
    if( _BigDataModel( CGOpts ) ) {
        BEAliasType( TY_LOCAL_POINTER, TY_LONG_POINTER );
        BEAliasType( TY_GLOBAL_POINTER, TY_LONG_POINTER );
    } else {
        BEAliasType( TY_LOCAL_POINTER, TY_NEAR_POINTER );
        BEAliasType( TY_GLOBAL_POINTER, TY_NEAR_POINTER );
    }
#else
    BEAliasType( TY_LOCAL_POINTER, TY_POINTER );
    BEAliasType( TY_GLOBAL_POINTER, TY_POINTER );
#endif
    BEAliasType( TY_ADV_LO, TY_INT_4 );
    BEAliasType( TY_ADV_HI, TY_UNSIGNED );
    BEAliasType( TY_ADV_HI_CV, TY_INT_4 );

    BEDefType( TY_COMPLEX, ALIGN_BYTE, 2*BETypeLength( TY_SINGLE ) );
    BEDefType( TY_DCOMPLEX, ALIGN_BYTE, 2*BETypeLength( TY_DOUBLE ) );
    BEDefType( TY_XCOMPLEX, ALIGN_BYTE, 2*BETypeLength( TY_LONGDOUBLE ) );
    BEDefType( TY_CHAR, ALIGN_BYTE,
               BETypeLength( TY_UNSIGNED ) + BETypeLength( TY_GLOBAL_POINTER ) );
#if _CPU == 386
    BEDefType( TY_CHAR16, ALIGN_BYTE,
               BETypeLength( TY_UINT_2 ) + BETypeLength( TY_GLOBAL_POINTER ) );
#endif
    BEDefType( TY_CHAR_ALLOCATABLE, ALIGN_BYTE,
               BETypeLength( TY_CHAR ) + BETypeLength( TY_UINT_2 ) );

    BEDefType( TY_ADV_ENTRY_CV, ALIGN_BYTE,
               BETypeLength( TY_ADV_LO ) + BETypeLength( TY_ADV_HI_CV ) );

    BEDefType( TY_ADV_ENTRY, ALIGN_BYTE,
               BETypeLength( TY_ADV_LO ) + BETypeLength( TY_ADV_HI ) );

    adv_size = BETypeLength( TY_ADV_ENTRY );
    if( CGOpts & CGOPT_DI_CV ) {
        adv_size += BETypeLength( TY_ADV_ENTRY_CV );
    }
    if( Options & OPT_BOUNDS ) {
        total_size = BETypeLength( TY_POINTER );
    } else {
        total_size = 0;
    }
    for( adv_cnt = 0; adv_cnt < MAX_DIM; adv_cnt++ ) {
        total_size += adv_size;
        BEDefType( ( TY_ADV_ENTRY_1 + adv_cnt ), ALIGN_BYTE, total_size );
    }

    BEDefType( TY_ARR_ALLOCATABLE, ALIGN_BYTE,
               ( BETypeLength( TY_UINT_2 ) + BETypeLength( TY_POINTER ) ) );

#if _CPU == 386
    total_size = BETypeLength( TY_LONG_POINTER );
#elif _CPU == 8086
    if( CGOpts & CGOPT_M_LARGE ) {
        total_size = BETypeLength( TY_HUGE_POINTER );
    } else { // if( CGOpts & CGOPT_M_MEDIUM ) {
        total_size = BETypeLength( TY_LONG_POINTER );
    }
#else
    total_size = BETypeLength( TY_POINTER );
#endif
    BEDefType( TY_ARR_ALLOCATABLE_EXTENDED, ALIGN_BYTE,
                BETypeLength( TY_UINT_2 ) + total_size );
}


void    DefStructs( void ) {
//====================

// Define user-defined data types.

    sym_id      sym;

    UserType = TY_USER_DEFINED;
    for( sym = RList; sym != NULL; sym = sym->sd.link ) {
        BEDefType( UserType, ALIGN_BYTE, sym->sd.size );
        sym->sd.cg_typ = UserType;
        sym->sd.dbi = DBG_NIL_TYPE;
        ++UserType;
    }
    if( Options & OPT_AUTOMATIC ) {
        for( sym = NList; sym != NULL; sym = sym->ns.link ) {
            if( ( sym->ns.flags & SY_CLASS ) != SY_VARIABLE ) continue;
            if( sym->ns.flags & (SY_SUB_PARM | SY_IN_COMMON) ) continue;
            if( ForceStatic( sym->ns.flags ) ) continue;
            if( sym->ns.flags & SY_IN_EQUIV ) {
                com_eq  *ce_ext;
                sym_id  eqv_set;
                ce_ext = sym->ns.si.va.vi.ec_ext;
                if( !(ce_ext->ec_flags & LEADER) ) continue;
                if( ce_ext->ec_flags & MEMBER_IN_COMMON ) continue;
                if( ce_ext->ec_flags & MEMBER_INITIALIZED ) continue;
                eqv_set = STEqSetShadow( sym );
                BEDefType( UserType, ALIGN_DWORD, ce_ext->high - ce_ext->low );
                eqv_set->ns.si.ms.u.cg_typ = UserType;
                ++UserType;
            } else if( sym->ns.flags & SY_SUBSCRIPTED ) {
                if( _Allocatable( sym ) ) continue;
                BEDefType( UserType, SymAlign( sym ),
                   _SymSize( sym ) * sym->ns.si.va.u.dim_ext->num_elts );
                sym->ns.si.va.u.dim_ext->l.cg_typ = UserType;
                ++UserType;
            } else if( sym->ns.u1.s.typ == FT_CHAR ) {
                BEDefType( UserType, ALIGN_BYTE, sym->ns.xt.size );
                sym->ns.si.va.vi.cg_typ = UserType;
                ++UserType;
            }
        }
        for( sym = MList; sym != NULL; sym = sym->ns.link ) {
            if( sym->ns.flags & (SY_IN_EQUIV | SY_SUBSCRIPTED) ) continue;
            if( (sym->ns.u1.s.typ == FT_CHAR) && (sym->ns.xt.size != 0) ) {
                BEDefType( UserType, ALIGN_BYTE, sym->ns.xt.size );
                sym->ns.si.ms.u.cg_typ = UserType;
                ++UserType;
            }
        }
    }
}


char    *FEModuleName( void ) {
//=======================

// Return pointer to module name (no file extension).

    return( SDFName( SrcName ) );
}


int     FETrue( void ) {
//================

// Return the value for "true".

    return( _LogValue( TRUE ) );
}


fe_attr FEAttr( cg_sym_handle _sym ) {
//====================================

// Return the front end attributes for the given symbol.

// FE_NOALIAS:  variable can't be modified by an indirect store
//              (unless its address has been taken)
// FE_VISIBLE:  variable can be modified by a call even though
//              it's not global

    unsigned_16 flags;
    fe_attr     attr;
    sym_id      sym = _sym;

    _UnShadow( sym );
    if( ( sym == EPValue ) || ( sym == ReturnValue ) ) return( 0 );
    attr = 0;
    flags = sym->ns.flags;
    if( ( flags & SY_CLASS ) == SY_VARIABLE ) {
        // SY_VARIABLE with SY_PS_ENTRY is shadow for function return value
        if( !(flags & (SY_SUB_PARM | SY_PS_ENTRY)) ) {
            if( flags & SY_IN_COMMON ) {
                attr |= FE_STATIC | FE_VISIBLE;
            } else if( !_MgcIsMagic( sym ) &&
                       ( (SgmtSw & SG_BIG_SAVE) || (Options & OPT_SAVE) ) ) {
                attr |= FE_STATIC;
            } else if( flags & (SY_DATA_INIT | SY_SAVED) ) {
                attr |= FE_STATIC;
            } else if( flags & SY_IN_EQUIV ) {
                if( Options & OPT_AUTOMATIC ) {
                    // magic symbol with SY_IN_EQUIV is shadow for leader of
                    // equivalence set
                    if( !_MgcIsMagic( sym ) ) {
                        com_eq  *ce_ext;
                        for(;;) {
                            ce_ext = sym->ns.si.va.vi.ec_ext;
                            if( ce_ext->ec_flags & LEADER ) break;
                            sym = ce_ext->link_eqv;
                        }
                        if( ce_ext->ec_flags & (MEMBER_IN_COMMON | MEMBER_INITIALIZED) ) {
                            attr |= FE_STATIC;
                        }
                    }
                } else {
                    attr |= FE_STATIC;
                }
            } else if( (flags & SY_SUBSCRIPTED) || (sym->ns.u1.s.typ == FT_STRUCTURE) ) {
                if( !(Options & OPT_AUTOMATIC) ) {
                    attr |= FE_STATIC;
                }
            } else if( sym->ns.u1.s.typ == FT_CHAR ) {
                // SCB's with length 0 are automatic temporaries
                // We mustn't allow the codegen to blow away non magical symbols
                if( (Options & OPT_AUTOMATIC ) && !_MgcIsMagic( sym ) ) {
                    attr |= FE_VOLATILE;
                }
                if( (sym->ns.xt.size != 0) || _Allocatable( sym ) ) {
                    if( !(Options & OPT_AUTOMATIC) ) {
                        // if the assignment of the data pointer into the
                        // static SCB gets optimized out, remove this line
                        attr &= ~FE_VOLATILE;
                        attr |= FE_STATIC;
                    }
                }
            }
            if( ( flags & ( SY_SUBSCRIPTED | SY_IN_COMMON ) ) == 0 ) {
                attr |= FE_NOALIAS;
            }
        }
    } else if( ( flags & SY_CLASS ) == SY_SUBPROGRAM ) {
        if( ( flags & SY_SUBPROG_TYPE ) != SY_STMT_FUNC ) {
            if( ( flags & SY_SUB_PARM ) == 0 ) {
                attr |= FE_PROC | FE_GLOBAL | FE_STATIC;
                if( ( flags & SY_PS_ENTRY ) == 0 ) {
                    attr |= FE_IMPORT;
                }
            }
        }
    } else if( ( flags & SY_CLASS ) == SY_COMMON ) {
        attr |= FE_GLOBAL | FE_STATIC | FE_VISIBLE;
    }
    if( !(attr & FE_GLOBAL) ) {
        attr |= FE_INTERNAL;
    }
    return( attr );
}


void    FEGenProc( cg_sym_handle sym, call_handle handle) {
//=========================================================

    sym = sym;
}


segment_id      FESegID( cg_sym_handle _sym ) {
//=============================================

// Return identifier of the segment that the given symbol is defined in.

    segment_id  id;
    unsigned_16 flags;
    unsigned_16 sp_type;
    sym_id      sym = _sym;

    _UnShadow( sym );
    id = SEG_LDATA;
    flags = sym->ns.flags;
    if( ( flags & SY_CLASS ) == SY_VARIABLE ) {
        if( ( flags & SY_SUB_PARM ) == 0 ) {
            if( flags & SY_SUBSCRIPTED ) {
                if( !_Allocatable( sym ) ) {
                    id = GetDataSegId( sym );
                }
            } else if( sym->ns.u1.s.typ != FT_CHAR ) {
                id = GetDataSegId( sym );
            }
        }
    } else if( ( flags & SY_CLASS ) == SY_SUBPROGRAM ) {
        sp_type = flags & SY_SUBPROG_TYPE;
        if( sp_type != SY_STMT_FUNC ) {
            if( !(flags & SY_SUB_PARM) ) {
                if( !(flags & SY_PS_ENTRY) ) {
                    if( (sp_type == SY_FUNCTION) ||
                        (sp_type == SY_SUBROUTINE) ||
                        (sp_type == SY_FN_OR_SUB) ) {
                        if( flags & SY_INTRINSIC ) {
                            id = sym->ns.si.fi.u.imp_segid;
                        } else {
                            id = sym->ns.si.sp.u.imp_segid;
                        }
                    }
                } else {
                    id = CurrCodeSegId;
                }
            }
        }
    } else if( ( flags & SY_CLASS ) == SY_COMMON ) {
        id = sym->ns.si.cb.seg_id;
    }
    return( id );
}


static char *GetName( sym_id sym ) {
//==================================

// Return pointer to the name of the given symbol.

    if( _MgcIsMagic( sym ) ) {
        if( ( sym->ns.flags & SY_PS_ENTRY ) == 0 ) {
            return( "*MAGIC*" );
        } else {
            sym = sym->ns.si.ms.sym;
        }
    }
    if( ( ( sym->ns.flags & SY_CLASS ) == SY_SUBPROGRAM ) &&
        ( ( sym->ns.flags & SY_SUBPROG_TYPE ) == SY_PROGRAM ) ) {
        return( ProgName );
    }
    STExtractName( sym, SymBuff );
    return( SymBuff );
}

static char *GetBaseName( sym_id sym )
{
    int         len;
    char        *buff;
    aux_info    *aux;

    _UnShadow( sym );
    buff = StackBuffer( &len );
    aux = AuxLookup( sym );
    if( aux->sym_len > 0 ) {
        strncpy( buff, aux->sym_name, aux->sym_len );
        buff[ aux->sym_len ] = 0;
    } else {
        strncpy( buff, sym->ns.name, sym->ns.u2.name_len );
        buff[ sym->ns.u2.name_len ] = 0;
    }
    return( buff );
}

static char *GetNamePattern( sym_id sym )
{
    aux_info    *aux;

    _UnShadow( sym );
    aux = AuxLookup( sym );
    return( aux->objname );
}

static int GetParmsSize( sym_id sym )
{
    int         args_size;
    pass_by     *arg;
    aux_info    *aux;

    _UnShadow( sym );
    aux = AuxLookup( sym );
    args_size = 0;
    for( arg = aux->arg_info; arg != NULL; arg = arg->link ) {
        if( arg->info & ARG_SIZE_1 ) {
            args_size += 1;
        } else if( arg->info & ARG_SIZE_2 ) {
            args_size += 2;
        } else if( arg->info & ARG_SIZE_4 ) {
            args_size += 4;
        } else if( arg->info & ARG_SIZE_8 ) {
            args_size += 8;
        } else if( arg->info & ARG_SIZE_16 ) {
            args_size += 16;
        } else if( arg->info & PASS_BY_REFERENCE ) {
            args_size += BETypeLength( TY_POINTER );
        }
    }
    return( args_size );
}

extern char *FEExtName( cg_sym_handle _sym, int request ) {
//=========================================================

// Return symbol name related info for object file.

    sym_id      sym = _sym;

    switch( request ) {
    case EXTN_BASENAME:
        return( GetBaseName( sym ) );
    case EXTN_PATTERN:
        return( GetNamePattern( sym ) );
    case EXTN_PRMSIZE:
        return( (char *)GetParmsSize( sym ) );
    default:
        return( NULL );
    }
}

char    *FEName( cg_sym_handle _sym ) {
//=====================================

// Return pointer to the name of the given symbol.

    sym_id      sym = _sym;

    _UnShadow( sym );
    if( sym == NULL ) return( "*NULL*" );
    return( GetName( sym ) );
}


void    *ConstBack( sym_id c_ptr ) {
//==================================

// Get a back handle for a literal.

    if( c_ptr->cn.address == NULL ) {
        c_ptr->cn.address = BENewBack( NULL );
    }
    return( c_ptr->cn.address );
}


int     FELexLevel( cg_sym_handle _sym ) {
//========================================

// Return scoping level of given symbol.

    sym_id      sym = _sym;

    _UnShadow( sym );
    return( 0 );
}


cg_type FEParmType( cg_sym_handle _fn, cg_sym_handle parm, cg_type tipe ) {
//=========================================================================

// Return the type that an argument of the given type should be converted
// to.

    sym_id      fn = _fn;

    parm = parm;
    switch( tipe ) {
#if _CPU == 386
    case TY_UINT_2:
    case TY_INT_2:
#endif
    case TY_INT_1:
    case TY_UINT_1:
#if _CPU == 386
        {
            aux_info    *aux;
            aux = AuxLookup( fn );
            if( aux != NULL ) {
                if( aux->cclass & FAR16_CALL ) {
                    return( TY_INT_2 );
                }
            }
        }
#else
        fn = fn;
#endif
        tipe = TY_INTEGER;
    }
    return( tipe );
}


int     FEMoreMem( unsigned size ) {
//==================================

// We can't free any memory for use by the back end.

    size = size;
    return( FALSE );
}


int     FEStackChk( cg_sym_handle _sym ) {
//========================================

// Do we want to generate stack overflow checking in the prologue for the
// given symbol?

    sym_id      sym = _sym;

    _UnShadow( sym );
    return( ( CGOpts & CGOPT_STACK_CHK ) != 0 );
}


static  char    *AuxName( aux_info *aux, char *buff ) {
//=====================================================

    if( (aux == &FortranInfo) || (aux == &DefaultInfo) ) {
        STGetName( SubProgId, buff );
        return( buff );
    }
    return( aux->sym_name );
}


void    FCMessage( fc_msg_class tipe, void *x ) {
//===============================================

    char        name[MAX_SYMLEN+1];

    switch( tipe ) {
    case FCMSG_EQUIV_TOO_LARGE :
        STGetName( (sym_id)x, name );
        Error( CP_AUTO_EQUIV_TOO_LARGE, name );
        break;
    case FCMSG_RET_VAL_TOO_LARGE :
        STGetName( (sym_id)x, name );
        Error( CP_AUTO_RET_TOO_LARGE, name );
        break;
    case FCMSG_VARIABLE_TOO_LARGE :
        STGetName( (sym_id)x, name );
        Error( CP_AUTO_VAR_TOO_LARGE, name );
        break;
    }
}

void    FEMessage( int msg, void *x ) {
//======================================

// Print a message for the back end.

    char        name[MAX_SYMLEN+1];

    if( ( CGFlags & CG_INIT ) == 0 ) {
        SendStd( x );
        exit( 1 );
    }
    switch( msg ) {
    case MSG_SYMBOL_TOO_LONG:
        /*  symbol too long, truncated (sym) */
        break;
    case MSG_CODE_SIZE :
#if _CPU == 8086
        CodeSize = (unsigned short)x;
#else
        CodeSize = (unsigned long)x;
#endif
        break;
    case MSG_DATA_SIZE :
        break;
    case MSG_ERROR :
        Error( CP_ERROR, x );
        break;
    case MSG_FATAL :
        Error( CP_FATAL_ERROR, x );
        CGFlags |= CG_FATAL;
        CSuicide();
        break;
    case MSG_BAD_PARM_REGISTER :
        Error( CP_BAD_PARM_REGISTER, x );
        break;
    case MSG_BAD_RETURN_REGISTER :
        Error( CP_BAD_RETURN_REGISTER, AuxName( x, name ) );
        break;
    case MSG_REGALLOC_DIED :
    case MSG_SCOREBOARD_DIED :
    case MSG_SCHEDULER_DIED :
        if( CGFlags & CG_MEM_LOW_ISSUED ) break;
        Warning( CP_LOW_ON_MEMORY, FEName( x ) );
        CGFlags |= CG_MEM_LOW_ISSUED;
        break;
    case MSG_PEEPHOLE_FLUSHED :
        if( CGFlags & CG_MEM_LOW_ISSUED ) break;
        STGetName( SubProgId, name );
        Warning( CP_LOW_ON_MEMORY, name );
        CGFlags |= CG_MEM_LOW_ISSUED;
        break;
    case MSG_BACK_END_ERROR :
        Error( CP_BACK_END_ERROR, (int)x );
        break;
    case MSG_BAD_SAVE :
        Error( CP_BAD_SAVE, AuxName( x, name ) );
        break;
    case MSG_BLIP :
        if( !(Options & OPT_QUIET) ) {
            SendBlip();
        }
        break;
    case MSG_INFO_PROC :
        PrintErr( x );
        break;
    case MSG_NO_SEG_REGS :      // can't be generated by FORTRAN 77
    case MSG_WANT_MORE_DATA :   // not used
    case MSG_INFO_FILE :        // not used
        break;
    }
}


static  dbg_type        BaseDbgType( TYPE typ, uint size ) {
//==========================================================

    if( typ == FT_CHAR ) {
        return( DBCharBlock( size ) );
    } else {
        return( DBGTypes[ ParmType( typ, size ) ] );
    }
}


static  dbg_type        GetDbgType( sym_id sym ) {
//================================================

// Get debugging information type.

    dbg_loc     loc;
    dbg_type    type;

    if( (sym->ns.u1.s.typ == FT_CHAR) && (sym->ns.xt.size == 0) ) {
        if( (sym->ns.flags & SY_CLASS) == SY_SUBPROGRAM ) {
            // return value for character*(*) function
            loc = DBLocInit();
            if( Options & OPT_DESCRIPTOR ) {
                loc = DBLocSym( loc, ReturnValue );
                loc = DBLocOp( loc, DB_OP_POINTS, TY_POINTER );
                loc = DBLocConst( loc, BETypeLength( TY_POINTER ) );
                loc = DBLocOp( loc, DB_OP_ADD, 0 );
            } else {
                loc = DBLocSym( loc, FindArgShadow( ReturnValue ) );
            }
            type = DBLocCharBlock( loc, TY_INTEGER );
            DBLocFini( loc );
            return( type );
        } else {
            // character*(*) variable/array
            if( sym->ns.flags & SY_VALUE_PARM ) {
                char    new_name[32];
                sprintf( new_name, "%s*(*)", DBGNames[ PT_CHAR ] );
                return( DBCharBlockNamed( new_name, 0 ) );        
            }
            loc = DBLocInit();
            if( Options & OPT_DESCRIPTOR ) {
                loc = DBLocSym( loc, sym );
                loc = DBLocOp( loc, DB_OP_POINTS, TY_POINTER );
                loc = DBLocConst( loc, BETypeLength( TY_POINTER ) );
                loc = DBLocOp( loc, DB_OP_ADD, 0 );
            } else {
                loc = DBLocSym( loc, FindArgShadow( sym ) );
            }
            type = DBLocCharBlock( loc, TY_INTEGER );
            DBLocFini( loc );
            return( type );
        }
    } else if( sym->ns.u1.s.typ == FT_STRUCTURE ) {
        return( sym->ns.xt.record->dbi );
    } else if( (sym->ns.u1.s.typ == FT_CHAR) ) {
        char    new_name[32];
        sprintf( new_name, "%s*%u", DBGNames[ PT_CHAR ], sym->ns.xt.size );
        return( DBCharBlockNamed( new_name, sym->ns.xt.size ) );        
    } else {
        return( BaseDbgType( sym->ns.u1.s.typ, sym->ns.xt.size ) );
    }
}


static dbg_type ArrayDbgType( act_dim_list *dim_ptr, dbg_type db_type ) {
//=======================================================================

    int         dims;
    intstar4    *bounds;
    intstar4    lo;
    intstar4    hi;
    dbg_array   *db_arr;

    dims = 1;
    bounds = &dim_ptr->subs_1_lo;
    db_arr = DBBegArray( db_type, TY_UNKNOWN, TRUE );
    while( dims <= _DimCount( dim_ptr->dim_flags ) ) {
        lo = *bounds;
        ++bounds;
        hi = *bounds;
        ++bounds;
        DBDimCon( db_arr, DBGTypes[PT_INT_4], lo, hi );
        ++dims;
    }
    return( DBEndArray( db_arr ) );
}


static  dbg_type        GetDBGSubProgType( sym_id sym ) {
//=======================================================

// Get debugging information type for subprograms.

    if( (sym->ns.flags & SY_SUBPROG_TYPE) == SY_SUBROUTINE ) {
#if _CPU == 8086
        return( DBGTypes[ PT_INT_2 ] );
#elif _CPU == 386 || _CPU == _AXP || _CPU == _PPC
        return( DBGTypes[ PT_INT_4 ] );
#else
        #error Unknown platform
#endif
    } else if( (sym->ns.flags & SY_SUBPROG_TYPE) == SY_FUNCTION ) {
        if( sym->ns.u1.s.typ == FT_CHAR ) {
            // for character*(*) functions, we want to pass 0 so that
            // the debugger can tell that it's a character*(*) function
            return( DBCharBlock( sym->ns.xt.size ) );
        } else {
            return( GetDbgType( sym ) );
        }
    } else if( (sym->ns.flags & SY_SUBPROG_TYPE) == SY_FN_OR_SUB ) {
        // Consider:
        //      subroutine foo( bar )
        //      external bar
        //      call qux( bar )
        //      end
        // We must assign a return type to bar, assume that it is a subroutine
        // Since we don't really know what it is.
#if _CPU == 8086
        return( DBGTypes[ PT_INT_2 ] );
#elif _CPU == 386 || _CPU == _AXP || _CPU == _PPC
        return( DBGTypes[ PT_INT_4 ] );
#else
        #error Unknown platform
#endif
    } else {
        return( DBG_NIL_TYPE );
    }
}


static  dbg_type        DefDbgSubprogram( sym_id sym, dbg_type db_type ) {
//========================================================================

// Define debugging information for subprograms.

    dbg_proc    db_proc;
    entry_pt    *ep;
    parameter   *arg;
    dbg_type    arg_type;

    if( sym->ns.u1.s.typ == FT_CHAR ) {
        db_type = DBDereference( TY_POINTER, db_type );
    }
    db_proc = DBBegProc( TY_CODE_PTR, db_type );
    for( ep = Entries; ep != NULL; ep = ep->link ) {
        if( ep->id != sym ) continue;
        for( arg = ep->parms; arg != NULL; arg = arg->link ) {
            if( arg->flags & ARG_STMTNO ) continue;
            arg_type = GetDbgType( arg->id );
            if( ( arg->id->ns.flags & SY_CLASS ) == SY_SUBPROGRAM ) {
                arg_type = DBDereference( TY_CODE_PTR,
                               DBEndProc( DBBegProc( TY_CODE_PTR,
                                          GetDBGSubProgType( arg->id ) ) ) );

            } else {
                if( arg->id->ns.u1.s.typ == FT_CHAR ) {
                    if( !(arg->id->ns.flags & SY_VALUE_PARM) ) {
                        if( Options & OPT_DESCRIPTOR ) {
                            arg_type = DBDereference( TY_POINTER, arg_type );
                        }
                    }
                } else {
                    if( !(arg->id->ns.flags & SY_VALUE_PARM) ) {
                        arg_type = DBDereference( TY_POINTER, arg_type );
                    }
                }
            }
            DBAddParm( db_proc, arg_type );
        }
        break;
    }
    return( DBEndProc( db_proc ) );
}


static  void    DefDbgFields( sym_id sd, dbg_struct db, unsigned_32 f_offset ) {
//==============================================================================

    sym_id      map;
    sym_id      field;
    unsigned_32 size;
    dbg_type    db_type;
    char        field_name[MAX_SYMLEN+1];

    field = sd->sd.fl.sym_fields;
    while( field != NULL ) {
        if( field->fd.typ == FT_UNION ) {
            size = 0;
            map = field->fd.xt.sym_record;
            while( map != NULL ) {
                DefDbgFields( map, db, f_offset );
                if( size < map->sd.size ) {
                    size = map->sd.size;
                }
                map = map->sd.link;
            }
        } else {
            STFieldName( field, field_name );
            if( field->fd.typ == FT_STRUCTURE ) {
                DefDbgStruct( field->fd.xt.sym_record );
                size = field->fd.xt.record->size;
                db_type = field->fd.xt.record->dbi;
            } else {
                size = field->fd.xt.size;
                db_type = BaseDbgType( field->fd.typ, field->fd.xt.size );
            }
            if( field->fd.dim_ext != NULL ) {
                size *= field->fd.dim_ext->num_elts;
                db_type = ArrayDbgType( field->fd.dim_ext, db_type );
            }
            DBAddField( db, f_offset, field_name, db_type );
        }
        f_offset += size;
        field = field->fd.link;
    }
}


static  void    DefDbgStruct( sym_id sym ) {
//==========================================

// Define debugging information for structure.

    dbg_struct  db;

    if( sym->sd.dbi != DBG_NIL_TYPE ) return;
    db = DBBegStruct( sym->sd.cg_typ, TRUE );
    DefDbgFields( sym, db, 0 );
    sym->sd.dbi = DBEndStruct( db );
}


static  dbg_type        DefCommonStruct( sym_id sym ) {
//=====================================================

// Define debugging information for a COMMON block.

    dbg_struct  db;
    unsigned_32 com_offset;
    unsigned_32 size;
    char        field_name[MAX_SYMLEN+1];
    dbg_type    db_type;
    com_eq      *com_ext;

    BEDefType( UserType, ALIGN_BYTE, GetComBlkSize( sym ) );
    db = DBBegNameStruct( "COMMON BLOCK", UserType, TRUE );
    com_offset = 0;
    sym = sym->ns.si.cb.first;
    for(;;) {
        com_ext = sym->ns.si.va.vi.ec_ext;
        STGetName( sym, field_name );
        if( sym->ns.u1.s.typ == FT_STRUCTURE ) {
            DefDbgStruct( sym->ns.xt.sym_record );
        }
        size = _SymSize( sym );
        db_type = GetDbgType( sym );
        if( sym->ns.flags & SY_SUBSCRIPTED ) {
            size *= sym->ns.si.va.u.dim_ext->num_elts;
            DBAddField( db, com_offset, field_name,
                        ArrayDbgType( sym->ns.si.va.u.dim_ext, db_type ) );
        } else {
            DBAddField( db, com_offset, field_name, db_type );
        }
        if( com_ext->ec_flags & LAST_IN_COMMON ) break;
        com_offset += size;
        sym = com_ext->link_com;
    }
    ++UserType;
    return( DBEndStruct( db ) );
}


static  void    InitDBGTypes( void ) {
//==============================

    int         typ;

    if( DBGTypes[ PT_LOG_1 ] == DBG_NIL_TYPE ) {
        for( typ = PT_LOG_1; typ <= PT_REAL_16; ++typ ) {
            DBGTypes[ typ ] = DBScalar( DBGNames[ typ ], MkCGType( typ ) );
        }
        DBGTypes[ PT_CPLX_8 ]  = DBFtnType( DBGNames[ PT_CPLX_8 ],  T_DBG_COMPLEX );
        DBGTypes[ PT_CPLX_16 ] = DBFtnType( DBGNames[ PT_CPLX_16 ], T_DBG_DCOMPLEX );
        DBGTypes[ PT_CPLX_32 ] = DBFtnType( DBGNames[ PT_CPLX_32 ], T_DBG_XCOMPLEX );
    }
}


dbg_type        FEDbgRetType( cg_sym_handle _sym ) {
//==================================================

// Return the debug type handle for a subprogram.

    sym_id      sym = _sym;

    _UnShadow( sym );
    InitDBGTypes();
    if( sym->ns.u1.s.typ == FT_STRUCTURE ) {
        DefDbgStruct( sym->ns.xt.sym_record );
    }
    return( GetDBGSubProgType( sym ) );
}


static  dbg_type        DbgADV( act_dim_list *dim_ptr, dbg_type db_type ) {
//=========================================================================

    int         dims;
    int         len;
    int         idx;
    dbg_array   *db_arr;

    idx = 0;
    dims = _DimCount( dim_ptr->dim_flags );
    db_arr = DBBegArray( db_type, TY_UNKNOWN, TRUE );
    if( dim_ptr->adv == NULL ) {
        // ADV allocated on the stack (debugging API's can't support this)
        // Create a 1x1x1x..1 array of appropriate dimension to approximate
        // an allocated array, until we get a decent db_loc system.
        while( idx < dims ) {
            DBDimCon( db_arr, DBGTypes[PT_INT_4], 1, 1 );
            idx++;
        }
        return( DBEndArray( db_arr ) );
    }
    len = dims * BETypeLength( TY_ADV_ENTRY );
    if( Options & OPT_BOUNDS ) {
        len += BETypeLength( TY_POINTER );
    }
    while( idx < dims ) {
        if( CGOpts & CGOPT_DI_CV ) {
            DBDimVar( db_arr, dim_ptr->adv,
                      ( len + ( idx * BETypeLength( TY_ADV_ENTRY_CV ) ) ),
                      TY_ADV_LO, TY_ADV_HI_CV );
        } else {
            DBDimVar( db_arr, dim_ptr->adv,
                      idx * BETypeLength( TY_ADV_ENTRY ),
                      TY_ADV_LO, TY_ADV_HI );
        }
        ++idx;
    }
    return( DBEndArray( db_arr ) );
}


dbg_type        FEDbgType( cg_sym_handle _sym ) {
//===============================================

// Return the debug type handle for the given symbol.

    dbg_type            db_type;
    act_dim_list        *dim_ptr;
    sym_id              sym = _sym;

    _UnShadow( sym );
    InitDBGTypes();
    if( (sym->ns.flags & SY_CLASS) == SY_COMMON ) {
        db_type = DefCommonStruct( sym );
    } else {
        if( sym->ns.u1.s.typ == FT_STRUCTURE ) {
            DefDbgStruct( sym->ns.xt.sym_record );
        }
        if( (sym->ns.flags & SY_CLASS) == SY_SUBPROGRAM ) {
            db_type = GetDBGSubProgType( sym );
            // define the subprogram
            db_type = DefDbgSubprogram( sym, db_type );
            if( sym->ns.flags & SY_SUB_PARM ) {
                // subprogram is an argument
                db_type = DBDereference( TY_CODE_PTR, db_type );
            }
        } else {
            if( sym->ns.flags & SY_PS_ENTRY ) {
                // shadow symbols for all function entry points
                // return value always points to the return value
                db_type = GetDbgType( sym->ns.si.ms.sym );
                db_type = DBDereference( TY_POINTER, db_type );
                if( SubProgId->ns.u1.s.typ == FT_CHAR ) { // character function
                    db_type = DBDereference( TY_POINTER, db_type );
                }
            } else {
                db_type = GetDbgType( sym );
                if( sym->ns.flags & SY_SUBSCRIPTED ) {
                    dim_ptr = sym->ns.si.va.u.dim_ext;
                    if( _AdvRequired( dim_ptr ) || _Allocatable( sym ) ) {
                        db_type = DbgADV( dim_ptr, db_type );
                    } else {
                        db_type = ArrayDbgType( dim_ptr, db_type );
                    }
                    if( sym->ns.flags & SY_SUB_PARM ) {
                        db_type = DBDereference( TY_POINTER, db_type );
                        if( sym->ns.u1.s.typ == FT_CHAR ) {
                            if( !(sym->ns.flags & SY_VALUE_PARM) ) {
                                if( Options & OPT_DESCRIPTOR ) {
                                    db_type = DBDereference( TY_POINTER, db_type );
                                }
                            }
                        }
                    }
                    if( _Allocatable( sym ) ) {
                        db_type = DBDereference( TY_POINTER, db_type );
                    }
                } else if( sym->ns.u1.s.typ == FT_CHAR ) {
                    // character variable
                    db_type = DBDereference( TY_POINTER, db_type );
                    if( sym->ns.flags & SY_SUB_PARM ) {
                        if( !(sym->ns.flags & SY_VALUE_PARM) ) {
                            if( Options & OPT_DESCRIPTOR ) {
                                db_type = DBDereference( TY_POINTER, db_type );
                            }
                        }
                    }
                } else if( sym->ns.flags & SY_SUB_PARM ) {
                    if( !(sym->ns.flags & SY_VALUE_PARM) ) {
                        db_type = DBDereference( TY_POINTER, db_type );
                    }
                }
            }
        }
    }
    return( db_type );
}

enum {
    TIME_SEC_B  = 0,
    TIME_MIN_B  = 5,
    TIME_HOUR_B = 11,
};

enum {
    DATE_DAY_B  = 0,
    DATE_MON_B  = 5,
    DATE_YEAR_B = 9,
};


static uint_32 *makeDOSTimeStamp( time_t ts ) {
//=============================================

    struct tm           *ltime;
    uint_16             dos_date;
    uint_16             dos_time;
    static uint_32      dos_stamp;

    ltime = localtime( &ts );
    dos_date = (( ltime->tm_year - 80 ) << DATE_YEAR_B )
             | (( ltime->tm_mon + 1 ) << DATE_MON_B )
             | (( ltime->tm_mday ) << DATE_DAY_B );
    dos_time = (( ltime->tm_hour ) << TIME_HOUR_B )
             | (( ltime->tm_min ) << TIME_MIN_B )
             | (( ltime->tm_sec / 2 ) << TIME_SEC_B );
    dos_stamp = dos_time | ( dos_date << 16 );
    return( &dos_stamp );
}

char    *GetFullSrcName( void ) {
//===============================

    int         idx;

    idx = MakeName( SrcName, SrcExtn, TokenBuff ) + sizeof( char );
    if( _fullpath( &TokenBuff[idx], TokenBuff, TOKLEN-idx ) != NULL ) {
        return( &TokenBuff[idx] );
    } else {
        return( TokenBuff );
    }
}

void    *FEAuxInfo( aux_handle aux, int request ) {
//=================================================

// Return specified auxiliary information for given auxiliary entry.

    unsigned_16 flags;
#if _CPU == 8086 || _CPU == 386
    int         idx;
    unsigned_32 com_size;
#endif
    sym_id      sym;
    char        *fn;
    char        *fe;
    char        *ptr;

    switch( request ) {
    case CALL_CLASS :
        {
            static call_class CallClass;

            CallClass = ((aux_info *)aux)->cclass ^ REVERSE_PARMS;
            return( &CallClass );
        }
    case SAVE_REGS :
        return( &((aux_info *)aux)->save );
    case RETURN_REG :
        return( &((aux_info *)aux)->returns );
    case PARM_REGS :
        return( ((aux_info *)aux)->parms );
    case CALL_BYTES :
#if _CPU == _AXP || _CPU == _PPC
        return( NULL );
#else
        return( ((aux_info *)aux)->code );
#endif
#if _CPU == 8086 || _CPU == 386
    case CODE_GROUP :
    case DATA_GROUP :
        return( "" );
    case STRETURN_REG :
        return( &((aux_info *)aux)->streturn );
#endif
    case NEXT_IMPORT :
        switch( (int)aux ) {
        case 0:
            if( CGFlags & CG_HAS_PROGRAM )
                return( (void *)1 );
#if _CPU == 386 || _CPU == _AXP || _CPU == _PPC
            if( CGOpts & CGOPT_BD )
                return( (void *)1 );
#endif
        case 1:
#if _CPU == 386 || _CPU == 8086
            if(( CGFlags & CG_FP_MODEL_80x87 )
              && ( CGFlags & CG_USED_80x87 ))
                return( (void *)2 );
        case 2:
#if _CPU == 386
            if( CPUOpts & CPUOPT_FPI )
                return( (void *)3 );
        case 3:
            if( CGOpts & CGOPT_BW )
                return( (void *)4 );
        case 4:
#endif
#endif
            return( (void *)5 );
        case 5:
            return( (void *)6 );
        case 6:
            if( Options & OPT_UNIT_6_CC )
                return( (void *)7 );
        case 7:
            if( Options & OPT_LF_WITH_FF )
                return( (void *)8 );
        case 8:
#if _CPU == 386 || _CPU == _PPC || _CPU == _AXP
            if( CGOpts & ( CGOPT_BM | CGOPT_BD ) )
                return( (void *)9 );
        case 9:
#endif
            if( Options & OPT_COMMA_SEP )
                return( (void *)10 );
        default:
            break;
        }
        return( (void *)0 );
    case NEXT_IMPORT_S :
        if( aux == NULL ) {
            ImpSym = GList;
        } else {
            ImpSym = ImpSym->ns.link;
        }
        for(;;) {
            if( ImpSym == NULL )
                return( (void *)0 );
            flags = ImpSym->ns.flags;
            if(( ( flags & SY_CLASS ) == SY_SUBPROGRAM )
              && ( flags & SY_EXTERNAL )
              && ( ( flags & ( SY_SUB_PARM | SY_REFERENCED | SY_RELAX_EXTERN ) ) == 0 ))
                break;
            ImpSym = ImpSym->ns.link;
        }
        return( (void *)1 );
    case IMPORT_NAME :
        switch( (int)aux ) {
        case 1:
#if _CPU == 386 || _CPU == _AXP || _CPU == _PPC
            if( CGOpts & CGOPT_BD )
                return( "__DLLstart_" );
#endif
            return( "_cstart_" );
#if _CPU == 8086 || _CPU == 386
        case 2:
            if( CPUOpts & CPUOPT_FPR ) {
                return( "__old_8087" );
            } else {
                return( "__8087" );
            }
#endif
#if _CPU == 386
        case 3:
            return( "__init_387_emulator" );
        case 4:
            return( "__init_default_win" );
#endif
        case 5:
            return( CharSetInfo.initializer );
        case 6:
            return( ErrorInitializer() );
        case 7:
            return( "__unit_6_cc" );
        case 8:
            return( "__lf_with_ff" );
#if _CPU == 386 || _CPU == _PPC || _CPU == _AXP
        case 9:
            return( "__fthread_init" );
#endif
        case 10:
            return( "__comma_inp_sep" );
        }
    case IMPORT_NAME_S :
        return( ImpSym );
    case NEXT_LIBRARY :
        if( aux == NULL ) {
            return( DefaultLibs );
        } else {
            return( ((default_lib *)(aux))->link );
        }
    case LIBRARY_NAME :
        return( &((default_lib *)(aux))->lib );
    case SOURCE_NAME :
        return( GetFullSrcName() );
    case AUX_LOOKUP :
        sym = (sym_id)aux;
        _UnShadow( sym );
        return( AuxLookup( sym ) );
    case OBJECT_FILE_NAME :
        if( ObjName == NULL ) {
            MakeName( SDFName( SrcName ), ObjExtn, TokenBuff );
        } else {
            ptr = TokenBuff;
            fn = SDFName( ObjName );
            if( fn != ObjName ) { // a path was specified
                memcpy( ptr, ObjName, fn - ObjName );
                ptr += fn - ObjName;
            }
            fe = SDExtn( fn, ObjExtn );
            if(( *fn == NULLCHAR )
              || (( *fn == '*' ) && ( fn[1] == NULLCHAR ))) {
                fn = SDFName( SrcName );
            }
            MakeName( fn, fe, ptr );
        }
        return( &TokenBuff );
    case FREE_SEGMENT :
        return( 0 );
    case REVISION_NUMBER :
        return( (void *)II_REVISION );
#if _CPU == 8086 || _CPU == 386
    case CLASS_NAME :
        for( sym = GList; sym != NULL; sym = sym->ns.link ) {
            if( ( sym->ns.flags & SY_CLASS ) != SY_COMMON )
                continue;
            idx = 0;
            com_size = GetComBlkSize( sym );
            for(;;) {
                if( com_size <= MaxSegSize )
                    break;
                com_size -= MaxSegSize;
                idx++;
            }
            if(( (segment_id)aux >= sym->ns.si.cb.seg_id )
              && ( (segment_id)aux <= sym->ns.si.cb.seg_id + idx )) {
                MangleCommonBlockName( sym, MangleSymBuff, TRUE );
                return( &MangleSymBuff );
            }
        }
        return( NULL );
    case USED_8087 :
        CGFlags |= CG_USED_80x87;
        return( NULL );
#endif
    case SHADOW_SYMBOL :
        sym = (sym_id)aux;
        _Shadow( sym );
        return( sym );
#if _CPU == 8086 || _CPU == 386
    case STACK_SIZE_8087 :
        // return the number of floating-point registers
        // that are NOT used as cache
        if( CPUOpts & CPUOPT_FPR ) return( (void *)4 );
        return( (void *)8 );
    case CODE_LABEL_ALIGNMENT :
        return( AlignmentSeq() );
#endif
    case TEMP_LOC_NAME :
        return( TEMP_LOC_QUIT );
    case TEMP_LOC_TELL :
        return( NULL );
    case NEXT_DEPENDENCY :
        if( !(Options & OPT_DEPENDENCY) ) {
            return( NULL );
        } else {
            if( aux == NULL ) {
                return( DependencyInfo );
            } else {
                return( ((dep_info *)aux)->link );
            }
        }
    case DEPENDENCY_TIMESTAMP :
        return( makeDOSTimeStamp( ((dep_info *)aux)->time_stamp ) );
    case DEPENDENCY_NAME :
        return( ((dep_info *)aux)->fn );
    case SOURCE_LANGUAGE:
        return( "FORTRAN" );
#if _CPU == 8086 || _CPU == 386
    case PEGGED_REGISTER:
        return( NULL );
#endif
    case UNROLL_COUNT:
        return( 0 );
    default:
        return( NULL );
    }
}


int     FECodeBytes( const char *buffer, int len )
//================================================
// not used - just a stub for JIT compatibility
{
    return( 0 );
}

char    *FEGetEnv( char const *name )
//===================================
// do a getenv
{
    return( getenv( name ) );
}
