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
#include "disasm.h"
#include "wdismsg.h"
#include "hashtabl.h"

#define IMPORT_LOOKUP_TABLE_SIZE    127
#define EXPORT_LOOKUP_TABLE_SIZE    29

static  uint_32         DataOffset;

/*
 * Static function prototypes
 */
static  void            ThreadFix( uint_16 );
static  void            FixupFix( uint_16 );
static  fixup           *FixField( char, int, char );
static  code_block      *IteratedData( void );
static  void            DecodeLIData( code_block * );
static  void            Add2List( handle **, handle * );


static segment *FindComdat( unsigned lname )
{
    segment     *seg;

    seg = Mod->segments;
    while( seg->class != TYPE_COMDAT || seg->id != lname ) {
        seg = seg->next_segment;
    }
    return( seg );
}


void  GrpDef()
/************/

{
    group               *grp;
    char                grp_type;
    segment             *seg;
    handle              *hndl;

    grp = NewGroup();
    PutTab( ++GrpIndex, GrpTab, grp );
    grp->id = GrpIndex;
    grp->name = GetTab( GetIndex(), NameTab, FALSE );
    while( EndOfRecord == FALSE ) {
        grp_type = GetByte();
        if( grp_type == GRP_SEGIDX ) {
            seg = GetTab( GetIndex(), SegTab, TRUE );
            seg->grouped = grp;
        } else {
            Error( ERR_UNIMPLE_GROUP, TRUE );
        }
        hndl = NewHandle( seg );
        Add2List( &grp->list, hndl );
    }
}


void  PubDef( bool public )
/*************************/
{
    uint_16             grp_id;
    uint_16             seg_id;
    char                *name;
    uint_32             addr;
    export_sym          *exp;

    grp_id = GetIndex();
    seg_id = GetIndex();
    if( grp_id == 0 && seg_id == 0 ) {
        SkipPcoRec(); /* absolute PUBDEF */
        return;
    }
    Segment = GetTab( seg_id, SegTab, TRUE );
    while( EndOfRecord == FALSE ) {
        name = GetName( MAX_NAME_LEN );
        if( IsPharLap || Is32Record ) {
            addr = GetLong();
        } else {
            addr = GetWord();
        }
        GetIndex();
        exp = AddLabel( addr, name, Segment, public, FALSE );
        if( Options & FORM_ASSEMBLER ) {
            import_sym  *imp;
            hash_data   *ptr;

            if( !Segment->exp_lookup ) {
                Segment->exp_lookup = HashTableCreate( EXPORT_LOOKUP_TABLE_SIZE, HASH_STRING,
                    (hash_table_comparison_func)stricmp );
                if( !Segment->exp_lookup ) {
                    SysError( ERR_OUT_OF_MEM, FALSE );
                }
            }
            if( !HashTableInsert( Segment->exp_lookup, (hash_value)name, (hash_data)exp ) ) {
                SysError( ERR_OUT_OF_MEM, FALSE );
            }
            ptr = HashTableQuery( Mod->imp_lookup, (hash_value)name );
            if( ptr ) {
                imp = (import_sym *)*ptr;
                imp->exported = TRUE;
                imp->u.also_exp = exp;
            }
        }
    }
}


static void  DoExtDef( char *name, unsigned type, bool public )
/*************************************************************/

{
    import_sym          *imp;
    segment             *save_seg;

    imp = AllocMem( sizeof( import_sym ) );
    PutTab( ++ExtIndex, ExtTab, imp );
    imp->class = TYPE_IMPORT;
    imp->name = name;
    imp->type_id = type;
    imp->next_imp = Mod->imports;
    imp->far_common = FALSE;
    imp->exported = FALSE;
    imp->public = public;
    Mod->imports = imp;
    if( Options & FORM_ASSEMBLER ) {
        if( !HashTableInsert( Mod->imp_lookup, (hash_value)name, (hash_data)imp ) ) {
            SysError( ERR_OUT_OF_MEM, FALSE );
        }
        save_seg = Segment;
        Segment = Mod->segments;
        while( Segment ) {
            if( Segment->exp_lookup != NULL ) {
                hash_data   *ptr;

                ptr = HashTableQuery( Segment->exp_lookup, (hash_value)imp->name );
                if( ptr ) {
                    imp->exported = TRUE;
                    imp->u.also_exp = (export_sym *)*ptr;
                }
            }
            Segment = Segment->next_segment;
        }
        Segment = save_seg;
    }
}

void ExtDef( bool public )
/************************/
{
    char        *name;
    unsigned    type;

    while( EndOfRecord == FALSE ) {
        name = GetName( MAX_NAME_LEN );
        type = GetIndex();
        DoExtDef( name, type, public );
    }
}


void CExtDef()
/************/
{
    char        *name;
    unsigned    type;

    while( EndOfRecord == FALSE ) {
        name = GetTab( GetIndex(), NameTab, FALSE );
        name = NameAlloc( name, strlen( name ) );
        type = GetIndex();
        DoExtDef( name, type, TRUE );
    }
}



void  ComDef( bool public )
/*************************/

{
    import_sym          *imp;
    char                data_seg_type;

    while( EndOfRecord == FALSE ) {
        imp = AllocMem( sizeof( import_sym ) );
        PutTab( ++ExtIndex, ExtTab, imp );
        imp->class = TYPE_COMDEF;
        imp->name = GetName( MAX_NAME_LEN );
        imp->public = public;
        GetIndex();
        data_seg_type = GetByte();
        imp->u.size = GetVarSize();
        if( data_seg_type == COMDEF_FAR ) {   /* FAR */
            imp->u.size *= GetVarSize();
            imp->far_common = TRUE;
        } else {
            imp->far_common = FALSE;
        }
        imp->next_imp = Mod->imports;
        Mod->imports = imp;
        if( ( Options & FORM_ASSEMBLER ) &&
            !HashTableInsert( Mod->imp_lookup, (hash_value)imp->name, (hash_data)imp ) ) {
            SysError( ERR_OUT_OF_MEM, FALSE );
        }
    }
}


static void  DoLinNum( segment *seg )
/***********************************/
{
    line_num            *line;
    line_num            *new_line;
    line_num            **owner;
    unsigned            num;
    uint_32             addr;

    owner = (line_num **)Mod->src_rover;
    if( owner == NULL ) owner = &Mod->src;
    while( EndOfRecord == FALSE ) {
        num = GetWord();
        if( IsPharLap || Is32Record ) {
            addr = GetLong();
        } else {
            addr = GetWord();
        }

        if( num <= 0x7fff ) {
            line = *owner;
            if( line == NULL || line->seg != seg || line->address > addr ) {
                owner = &Mod->src;
            }
            for( ;; ) {
                line = *owner;
                if( line == NULL ) break;
                if( line->seg == seg ) {
                    if( line->address > addr ) break;
                } else {
                    if( line->num > num ) break;
                }
                owner = &line->next_num;
            }
            new_line = AllocMem( sizeof( line_num ) );
            new_line->num = num;
            new_line->address = addr;
            new_line->seg = seg;
            new_line->next_num = line;
            *owner = new_line;
        }
    }
    Mod->src_rover = (line_num *)owner;
}


void LinNum()
/***********/
{
    uint_16             grp_id;
    uint_16             seg_id;

    grp_id = GetIndex();
    seg_id = GetIndex();
    if( grp_id == 0 && seg_id == 0 ) {
        GetWord();
    }
    DoLinNum( GetTab( seg_id, SegTab, TRUE ) );
}


void LinSym()
/***********/
{
    GetByte(); /* skip flag byte */
    DoLinNum( FindComdat( GetIndex() ) );
}


void  FixUpp()
/************/
{
    uint_16             info;

    while( EndOfRecord == FALSE ) {
        info = GetByte();
        if( info & FIXUPP_FIXUP ) {
            FixupFix( info );
        } else {
            ThreadFix( info );
        }
    }
}


static  void  ThreadFix( uint_16 trd_dat )
/****************************************/
{
    char                method;
    char                D;
    char                thred;
    thread              *trd;

    D = ( trd_dat & 0x40 ) >> 4;
    thred = ( trd_dat & 0x03 ) | D;
    method = ( trd_dat & 0x1c ) >> 2;
    trd = GetTab( thred, ThrdTab, FALSE );
    if( D == 0 ) {
        method &= 3;
    }
    FindTarget( method, trd );
}


static  void  FixupFix( uint_16 locat )
/*************************************/
{
    fixup               *fix;
    char                M;
    char                LOC;

    M = ( locat & 0x40 ) << 1;
    /* we take 4-bits as if every record is a Microsoft Fixup rec */
    LOC = ( locat >> 2 ) & 0x0f;
    if( IsPharLap && LOC > LOC_BASE_OFFSET_32 ) {
        Error( ERR_INV_FIXUP_LOC, FALSE );
        return;
    }
    DataOffset = ( ( locat & 3 ) << 8 ) | GetByte();
    fix = FixField( LOC, 1, M );
    AddFix( Segment, fix );
}


static  fixup  *FixField( char LOC, int is_valid_loc, char M )
/************************************************************/
/*
    is_valid_loc == 1 is used to parse a FIXUP
    is_valid_loc == 0 is used to parse fields such as the MODEND start
        address field.
    M is 0 for self-relative, 1 for seg-relative
*/
{
    fixup               *fix;
    char                fix_dat;
    thread              *trd;
    char                F;
    char                P;
    char                T;
    char                TARGT;
    char                FRAME;

    if( ! IsPharLap && is_valid_loc ) {
        /* we want PharLap OMF fixup LOC's... so we'll just translate here */
        switch( LOC ) {
        case LOC_MS_LINK_OFFSET:        LOC = LOC_OFFSET;           break;
        case LOC_MS_OFFSET_32:      /* fall through */
        case LOC_MS_LINK_OFFSET_32:     LOC = LOC_OFFSET_32;        break;
        case LOC_MS_BASE_OFFSET_32:     LOC = LOC_BASE_OFFSET_32;   break;
        }
    }
    fix_dat = GetByte();
    fix = NewFixup( LOC | M );
    FRAME = ( fix_dat & 0x70 ) >> 4;
    F = fix_dat & 0x80;
    if( F ) {
        trd = GetTab( FRAME | 4, ThrdTab, TRUE );
        fix->frame = trd->datum;
        fix->seg_address = trd->address;
    } else {
        FindTarget( FRAME, (thread *) &fix->frame );
    }
    TARGT = fix_dat & 0x03;
    T = fix_dat & 0x08;
    if( T ) {
        trd = GetTab( TARGT, ThrdTab, TRUE );
        fix->target = trd->datum;
        fix->imp_address = trd->address;
    } else {
        FindTarget( TARGT, (thread *) &fix->target );
    }
    P = fix_dat & 0x04;
    if( P == 0 ) {
        if( IsPharLap || Is32Record ) {
            fix->imp_address += GetLong();
        } else {
            fix->imp_address += GetWord();
        }
    }
    if( is_valid_loc ) {
        switch( LOC ) {
        case LOC_OFFSET_LO:
            fix->imp_address += Addr( DataOffset ) & 0xff;
            break;
        case LOC_OFFSET:
            fix->imp_address += Addr( DataOffset );
            break;
        case LOC_BASE:
            fix->seg_address += Addr( DataOffset );
            break;
        case LOC_BASE_OFFSET:
            fix->imp_address += Addr( DataOffset );
            fix->seg_address += Addr( DataOffset + 2 );
            break;
        case LOC_OFFSET_32:
            fix->imp_address += Addr32( DataOffset );
            break;
        case LOC_OFFSET_HI:
            fix->imp_address += Addr( DataOffset ) >> 8;
            break;
        case LOC_BASE_OFFSET_32:
            fix->imp_address += Addr32( DataOffset );
            fix->seg_address += Addr( DataOffset + 4 );
            break;
        }
        fix->address = DataOffset + Offset;
    }
    return( fix );
}


static  void  FindTarget( char method, thread *trd )
/**************************************************/
{
    trd->datum = NULL;
    trd->address = NULL;
    if( method <= 3 ) {
        if( method == T_SEGWD ) {
            trd->datum = GetTab( GetIndex(), SegTab, TRUE );
        } else if( method == T_GRPWD ) {
            trd->datum = GetTab( GetIndex(), GrpTab, TRUE );
        } else if( method == T_EXTWD ) {
            trd->datum = GetTab( GetIndex(), ExtTab, TRUE );
        } else {    /* method == T_ABSWD */
            trd->address = GetWord();
        }
    }
}


void  THeadr()
/************/

{
    Mod = NewModule();
    Mod->name = GetName( MAX_NAME_LEN );
    if( Options & FORM_ASSEMBLER ) {
        Mod->imp_lookup = HashTableCreate( IMPORT_LOOKUP_TABLE_SIZE,
            HASH_STRING, (hash_table_comparison_func)stricmp );
        if( !Mod->imp_lookup ) {
            SysError( ERR_OUT_OF_MEM, FALSE );
        }
    }
}


void  LName()
/***********/

{
    while( EndOfRecord == FALSE ) {
        PutTab( ++NameIndex, NameTab, GetName( MAX_NAME_LEN ) );
    }
}


void  SegDef()
/************/

{
    uint_32             seg_len;
    segment             *seg;

    char                A;
    char                attr;

    seg = NewSegment();
    PutTab( ++SegIndex, SegTab, seg );
    seg->id = SegIndex;
    seg->attr = GetByte();
    A = _SegAlign( seg );
    if( A == ALIGN_ABS ) {
        seg->address = GetWord();
        if( IsPharLap ) {
            seg->offset = GetLong();
        } else {
            seg->offset = GetWord();
        }
    }
    if( IsPharLap || Is32Record ) {
        seg_len = GetLong();
    } else {
        seg_len = GetWord();
    }
    if( _Size64K( seg ) ) {
            /* FIXME if Is32Record then should be 2 ** 32 */
        seg_len = 0x10000;  /* exactly 64K */
    }
    seg->name = GetTab( GetIndex(), NameTab, FALSE );
    if( strstr( seg->name, WTLSEGStr ) != NULL ) {
        WtlsegPresent = TRUE;
    }
    seg->u.seg.class_name = GetTab( GetIndex(), NameTab, FALSE );
    seg->u.seg.overlay_id = GetIndex();
    seg->size = seg_len;
    seg->start = seg_len;
    if( !IsPharLap ) {
        seg->use_32 = ( seg->attr & 0x01 ) != 0;
    } else if( !EndOfRecord ) {
        attr = GetByte();
        seg->use_32 = ( attr & EASY_USE32_FIELD ) != 0;
        seg->access_attr = attr & EASY_PROTECT_FIELD;
        seg->access_valid = TRUE;
    } else {
        seg->use_32 = TRUE;
        seg->access_valid = FALSE;
    }
}


void ComDat()
/***********/
{
    segment     *seg;
    uint_8      flags;
    uint_8      attr;
    uint_8      align;
    unsigned    grp_id;
    unsigned    seg_id;
    unsigned    lname;

    flags = GetByte();
    attr = GetByte();
    align = GetByte();
    if( IsPharLap || Is32Record ) {
        Offset = GetLong();
    } else {
        Offset = GetWord();
    }
    GetIndex(); /* skip type index */
    grp_id = 0;
    seg_id = 0;
    if( (attr & COMDAT_ALLOC_MASK) == COMDAT_EXPLICIT ) {
        grp_id = GetIndex();
        seg_id = GetIndex();
        if( grp_id == 0 && seg_id == 0 ) {
            GetWord();
        }
    }
    lname = GetIndex();
    if( flags & COMDAT_CONTINUE ) {
        /* old comdat */
        seg = FindComdat( lname );
    } else {
        seg = NewSegment();
        seg->name = GetTab( lname, NameTab, FALSE );
        seg->class = TYPE_COMDAT;
        seg->id = lname;
        seg->attr = attr;
        seg->u.com.align = align;
        if( seg_id != 0 ) seg->u.com.seg = GetTab( seg_id, SegTab, TRUE );
        if( grp_id != 0 ) seg->u.com.grp = GetTab( grp_id, GrpTab, TRUE );
        switch( attr & COMDAT_ALLOC_MASK ) {
        case COMDAT_EXPLICIT:
            seg->use_32 = seg->u.com.seg->use_32;
            break;
        case COMDAT_FAR_CODE:
        case COMDAT_FAR_DATA:
            seg->use_32 = FALSE;
            break;
        case COMDAT_CODE32:
        case COMDAT_DATA32:
            seg->use_32 = TRUE;
            break;
        }
        if( !(flags & COMDAT_LOCAL) ) seg->public = TRUE;
    }
    Segment = seg;
    if( flags & COMDAT_ITERATED ) {
        DoLIData();
    } else {
        DoLEData();
    }
    if( seg->size < DataOffset ) seg->size = DataOffset;
}


static void DoBackPatch( unsigned loc_type )
/******************************************/
{
    uint_32     offset;
    uint_32     value;

    while( EndOfRecord == FALSE ) {
        if( IsPharLap || Is32Record ) {
            offset = GetLong();
            value  = GetLong();
        } else {
            offset = GetWord();
            value  = GetWord();
        }
        switch( loc_type ) {
        case 0: /* 8-bit lobyte */
            PutSegByte( offset, GetSegByte( offset ) + value );
            break;
        case 1: /* 16-bit offset */
            PutSegWord( offset, GetSegWord( offset ) + value );
            break;
        case 2: /* 32-bit offset */
            PutSegDWord( offset, GetSegDWord( offset ) + value );
            break;
        }
    }
}

void BackPatch()
/**************/
{
    unsigned    seg_id;

    seg_id = GetIndex();
    Segment = GetTab( seg_id, SegTab, TRUE );
    DoBackPatch( GetByte() );
}

void NBackPatch()
/***************/
{
    unsigned    loc_type;

    loc_type = GetByte();
    Segment = FindComdat( GetIndex() );
    DoBackPatch( loc_type );
}


static void DoLEData()
/********************/
{
    MarkBegData();
    DataOffset = Offset;
    if( DataOffset < Segment->start ) {
        Segment->start = DataOffset;
    }
    while( EndOfRecord == FALSE ) {
        PutSegByte( DataOffset++, GetByte() );
    }
}


void  LEData()
/************/
{
    uint_16     seg_id;

    seg_id = GetIndex();
    if( IsPharLap || Is32Record ) {
        Offset = GetLong();
    } else {
        Offset = GetWord();
    }
    Segment = GetTab( seg_id, SegTab, TRUE );
    DoLEData();
    if( DataOffset > Segment->size ) {
        Error( ERR_TOO_MUCH_LEDATA, TRUE );
    }
}

static void DoLIData()
/********************/
{
    code_block          *block;

    MarkBegData();
    DataOffset = Offset;
    if( DataOffset < Segment->start ) {
        Segment->start = DataOffset;
    }
    while( !EndOfRecord ) {
        block = IteratedData();
        DecodeLIData( block );
    }
}


void  LIData()
/************/
{
    uint_16             seg_id;

    seg_id = GetIndex();
    Segment = GetTab( seg_id, SegTab, TRUE );
    if( IsPharLap || Is32Record ) {
        Offset = GetLong();
    } else {
        Offset = GetWord();
    }
    DoLIData();
    if( DataOffset > Segment->size ) {
        Error( ERR_TOO_MUCH_LIDATA, TRUE );
    }
}


static  code_block  *IteratedData()
/*********************************/

{
    uint_32             rpt_count;
    uint_16             blk_count;
    char                num_bytes;
    code_block          *block;
    uint_16             i;

    if( Is32Record ) {
        rpt_count = GetLong();
    } else {
        rpt_count = GetWord();
    }
    blk_count = GetWord();
    if( blk_count == 0 ) {
        num_bytes = GetByte();
        block = AllocMem( sizeof( code_block ) + num_bytes );
        i = 1;
        do {
            block->data[ i ] = GetByte();
        } while( ++i <= num_bytes );
        block->data[ 0 ] = num_bytes;
    } else {
        block = AllocMem( sizeof( code_block ) + blk_count * sizeof( char * )
                          - sizeof( char ) );
        i = 0;
        do {
            ((ptr_table)block->data)[ i ] = IteratedData();
        } while( ++i != blk_count );
    }
    block->rpt_count = rpt_count;
    block->blk_count = blk_count;
    return( block );
}


static  void  DecodeLIData( code_block *block )
/*********************************************/
{
    uint_16             num;
    uint_32             i;

    i = 0;
    do {
        if( block->blk_count == 0 ) {
            num = 1;
            do {
                PutSegByte( DataOffset++, block->data[ num ] );
            } while( ++num <= block->data[ 0 ] );
        } else {
            num = 0;
            do {
                DecodeLIData( ( (ptr_table) block->data )[ num ] );
            } while( ++num != block->blk_count );
        }
    } while( ++i != block->rpt_count );
}


void  ModEnd()
/************/
{
    char                mod_typ;
    char                mattr;
    fixup               *fix;

    mod_typ = GetByte();
    mattr = ( mod_typ & 192 ) >> 6;
    if( mattr & 1 ) {   /* has start address */
        if( mod_typ & 1 ) {
            fix = FixField( TYPE_MODULE, 0, SEG_RELATIVE );
        } else {
            fix = NewFixup( TYPE_MODULE | SEG_RELATIVE );
            fix->seg_address = GetWord();
            fix->imp_address = GetByte();
        }
        Mod->start = fix;
    }
    if( mattr > 1 ) {
        Mod->main = TRUE;
    } else {
        Mod->main = FALSE;
    }
}


static  void  Add2List( handle **list, handle *hndl )
/***************************************************/
{
    handle              *tmp;

    for( ;; ) {
        tmp = *list;
        if( tmp == NULL ) break;
        list = &tmp->next_hndl;
    }
    *list = hndl;
}


void Coment()
/***********/

{
    char                attr;
    char                class;
    int                 len;
    int                 i;
    segment             *seg;
    scan_table          *scan_tab;
    int                 sidx;
    char                name[ 80 ];

    attr = GetByte();
    class = GetByte();
    if( ( attr == CMT_SOURCE_NAME && class == 0x80 ) ||
        ( attr == 0x80 && class == CMT_SOURCE_NAME ) ) {
        len = RecLen - 3;       /* class, attr, chksum */
        for( i = 0; i < len; ++i ) {
            name[ i ] = GetByte();
        }
        CommentName = NameAlloc( name, len );
    } else if( attr == 0x80 && class == CMT_DISASM_DIRECTIVE ) {
        class = GetByte();
        if( class == DDIR_SCAN_TABLE ||
            class == DDIR_SCAN_TABLE_32 ) {        /* 'S'can table indices */
            sidx = GetIndex();
            if( sidx == 0 ) {
                /* select table in a COMDAT record */
                seg = FindComdat( GetIndex() );
            } else {
                seg = GetTab( sidx, SegTab, TRUE );
            }
            scan_tab = AllocMem( sizeof( scan_table ) );
            if( class == DDIR_SCAN_TABLE_32 ) {
                scan_tab->starts = GetLong();
                scan_tab->ends = GetLong();
            } else {
                scan_tab->starts = GetWord();
                scan_tab->ends = GetWord();
            }
            scan_tab->next = seg->scan_tabs;
            seg->scan_tabs = scan_tab;
        } else {
            SkipPcoRec();
        }
    } else if( attr == 0x80 && class == CMT_EASY_OMF ) {
        len = RecLen - 3;       /* class, attr, chksum */
        for( i = 0; i < len; ++i ) {
            name[ i ] = GetByte();
        }
        if( memcmp( name, EASY_OMF_SIGNATURE, 5 ) == 0 ) {
            IsPharLap = TRUE;
            Is32BitObj = TRUE;
        }
    } else {
        SkipPcoRec();
    }
}
