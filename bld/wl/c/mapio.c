/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2024 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Map file formatting routines
*
****************************************************************************/


#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "linkstd.h"
#include "pcobj.h"
#include "newmem.h"
#include "msg.h"
#include "alloc.h"
#include "wlnkmsg.h"
#include "objnode.h"
#include "loadfile.h"
#include "fileio.h"
#include "overlays.h"
#include "ring.h"
#include "objio.h"
#include "mapio.h"
#include "dwarf.h"
#include "virtmem.h"
#include "exeelf.h"
#include "dbgcomm.h"
#include "dbgdwarf.h"

#include "clibext.h"


#define MAP_BUFFER_SIZE (16 * SECTOR_SIZE)

typedef struct {
    unsigned_32         address;
    unsigned            file;
    unsigned_32         line;
    unsigned_32         column;
    unsigned_16         segment;
    unsigned_16         col;
    boolbit             is_stmt         : 1;
    boolbit             is_32           : 1;
    boolbit             has_seg         : 1;
    boolbit             basic_block     : 1;
    boolbit             end_sequence    : 1;
} line_state_info;

typedef struct symrecinfo {
    struct symrecinfo   *next;
    symbol              *sym;
    mod_entry           *mod;
} symrecinfo;

static symrecinfo       *UndefList;
static symrecinfo       *SymTraceList;
static size_t           MapCol;
static time_t           StartT;
static clock_t          ClockTicks;
static bool             Absolute_Seg;
static size_t           MapBufferSize;
static char             *MapBuffer;

void ResetMapIO( void )
/****************************/
{
    MapFlags = 0;
    UndefList = NULL;
    SymTraceList = NULL;
}

void StartTime( void )
/********************/
{
    StartT = time( NULL );
    ClockTicks = clock();
}

static char *PutDec( char *ptr, unsigned num )
/*********************************************/
{
    *ptr++ = (( num / 10 ) % 10) + '0';
    *ptr++ = (num % 10) + '0';
    return( ptr );
}

static void WriteMapDirect( const char *buffer, size_t len )
/***********************************************************
 * buffering the write to the map file
 */
{
    size_t      size;

    while( MapBufferSize + len >= MAP_BUFFER_SIZE ) {
        size = MAP_BUFFER_SIZE - MapBufferSize;
        memcpy( MapBuffer + MapBufferSize, buffer, size );
        QWrite( MapFile, MapBuffer, MAP_BUFFER_SIZE, MapFName );
        MapBufferSize = 0;
        buffer += size;
        len -= size;
    }
    if( len > 0 ) {
        memcpy( MapBuffer + MapBufferSize, buffer, len );
        MapBufferSize += len;
    }
}

static void WriteMapDirectNL( void )
{
    WriteMapDirect( NLSeq, strlen( NLSeq ) );
    MapCol = 0;
}

static void WriteMapDirectString( const char *buffer, bool nl )
/*************************************************************/
{
    WriteMapDirect( buffer, strlen( buffer ) );
    if( nl ) {
        WriteMapDirectNL();
    }
}

static void WriteMapMsgPrintf( int resourceid, ... )
{
    char        format[RESOURCE_MAX_SIZE];
    va_list     args;
    char        buff[MAX_MSG_SIZE];
    size_t      len;

    if( MapFlags & MAP_FLAG ) {
        Msg_Get( resourceid, format );
        va_start( args, resourceid );
        len = DoFmtStr( buff, sizeof( buff ), format, args );
        va_end( args );
        WriteMapDirect( buff, len );
        WriteMapDirectNL();
    }
}

static void WriteMapMsg( int resourceid )
{
    char        buff[RESOURCE_MAX_SIZE];

    if( MapFlags & MAP_FLAG ) {
        Msg_Get( resourceid, buff );
        WriteMapDirect( buff, strlen( buff ) );
        WriteMapDirectNL();
    }
}

void MapInit( void )
/******************/
{
    char                tim[8 + 1];
    char                dat[8 + 1];
    char                *ptr;
    struct tm           *localt;

    Absolute_Seg = false;
    if( MapFlags & MAP_FLAG ) {
        MapFile = QOpenRW( MapFName );
        _LnkAlloc( MapBuffer, MAP_BUFFER_SIZE );
        MapBufferSize = 0;
        localt = localtime( &StartT );
        MapCol = 0;
        WriteMapDirectString( MsgStrings[PRODUCT], true );
        WriteMapDirectString( MsgStrings[COPYRIGHT], true );
        WriteMapDirectString( MsgStrings[COPYRIGHT2], true );
        ptr = tim;
        ptr = PutDec( ptr, localt->tm_hour );
        *ptr++ = ':';
        ptr = PutDec( ptr, localt->tm_min );
        *ptr++ = ':';
        ptr = PutDec( ptr, localt->tm_sec );
        *ptr = '\0';

        ptr = dat;
        ptr = PutDec( ptr, localt->tm_year );
        *ptr++ = '/';
        ptr = PutDec( ptr, localt->tm_mon + 1 );
        *ptr++ = '/';
        ptr = PutDec( ptr, localt->tm_mday );
        *ptr = '\0';

        LnkMsg( MAP+MSG_CREATED_ON, "12", dat, tim );
    }
}

void MapFini( void )
/*******************
 * Finish map processing
 */
{
    if( MapFlags & MAP_FLAG ) {
        if( MapFlags & MAP_LINES ) {
            WriteMapLines();
        }
        if( MapBufferSize > 0 ) {
            QWrite( MapFile, MapBuffer, MapBufferSize, MapFName );
            MapBufferSize = 0;
        }
        if( MapFile != NIL_FHANDLE ) {
            QClose( MapFile, MapFName );
            MapFile = NIL_FHANDLE;
        }
        _LnkFree( MapBuffer );
        MapBuffer = NULL;
    }
}

static void WriteBox( unsigned int msgnum )
/*****************************************/
{
    char        box_buff[RESOURCE_MAX_SIZE];
    char        msg_buff[RESOURCE_MAX_SIZE];
    size_t      i;

    Msg_Get( msgnum, msg_buff );
    WriteMapNL();
    WriteMapNL();
    box_buff[0] = '+';
    for( i = 2; i < strlen( msg_buff ); i++ ) {
        box_buff[i-1] = '-';
    }
    box_buff[i-1] = '+';
    box_buff[i] = '\0';
    WriteMap( "%t24%s", "", box_buff );
    WriteMap( "%t24%s", "", msg_buff );
    WriteMap( "%t24%s", "", box_buff );
    WriteMapNL();
}

void WriteGroups( void )
/*****************************/
{
    group_entry     *currgrp;

    if( Groups != NULL ) {
        WriteBox( MSG_MAP_BOX_GROUP );
        WriteMapMsg( MSG_MAP_TITLE_GROUP_0 );
        WriteMapMsg( MSG_MAP_TITLE_GROUP_1 );
        WriteMapNL();
        for( currgrp = Groups; currgrp != NULL; currgrp = currgrp->next_group ) {
            if( !currgrp->isautogrp ) { /* if not an autogroup */
                WriteFormat( 0, "%s", currgrp->sym->name );
                WriteFormat( 32, "%a", &currgrp->grp_addr );
                WriteFormat( 53, "%h", currgrp->totalsize );
                WriteMapNL();
            }
        }
    }
}

static void WriteAbsSeg( void *_leader )
/**************************************/
{
    seg_leader  *leader = _leader;

    if( leader->info & SEG_ABSOLUTE ) {
        WriteFormat( 0, "%s", leader->segname );
        WriteFormat( 24, "%s", leader->class->name );
        if( (FmtData.type & MK_16BIT) && (leader->info & USE_32) ) {
            WriteFormat( 40, "%A", &leader->seg_addr );
        } else {
            WriteFormat( 40, "%a", &leader->seg_addr );
        }
        WriteFormat( 60, "%h", leader->size );
        WriteMapNL();
    }
}

static void WriteNonAbsSeg( void *_seg )
/**************************************/
{
    seg_leader  *seg = _seg;

    if( (seg->info & SEG_ABSOLUTE) == 0 ) {
        WriteFormat( 0, "%s", seg->segname );
        WriteFormat( 23, "%s", seg->class->name );
        if( seg->group != NULL ) {
            WriteFormat( 38, "%s", seg->group->sym->name );
        }
        if( (FmtData.type & MK_16BIT) && (seg->info & USE_32) ) {
            WriteFormat( 53, "%A", &seg->seg_addr );
        } else {
            WriteFormat( 53, "%a", &seg->seg_addr );
        }
        WriteFormat( 69, "%h", seg->size );
        WriteMapNL();
    } else {
        Absolute_Seg = true;
    }
}

typedef struct {
    unsigned    idx;
    seg_leader  *seg;
} seg_info;

static int cmp_seg( const void *a, const void *b )
/*************************************************/
{
    if( ((seg_info *)a)->seg->seg_addr.seg == ((seg_info *)b)->seg->seg_addr.seg ) {
        if( ((seg_info *)a)->seg->seg_addr.off == ((seg_info *)b)->seg->seg_addr.off )
            return( ((seg_info *)a)->idx - ((seg_info *)b)->idx );
        return( ((seg_info *)a)->seg->seg_addr.off - ((seg_info *)b)->seg->seg_addr.off );
    }
    return( ((seg_info *)a)->seg->seg_addr.seg - ((seg_info *)b)->seg->seg_addr.seg );
}

void WriteSegs( section *sect )
/*******************************************/
/* write segment info into mapfile */
{
    class_entry     *class;
    size_t          count;
    size_t          i;
    seg_leader      *seg;
    seg_info        *segs;

    if( sect->classlist != NULL ) {
        WriteBox( MSG_MAP_BOX_SEGMENTS );
        WriteMapMsg( MSG_MAP_TITLE_SEGMENTS_0 );
        WriteMapMsg( MSG_MAP_TITLE_SEGMENTS_1 );
        WriteMapNL();
        count = 0;
        for( class = sect->classlist; class != NULL; class = class->next_class ) {
            if( (class->flags & CLASS_DEBUG_INFO) == 0 ) {
                count += RingCount( class->segs );
            }
        }
        _ChkAlloc( segs, count * sizeof( seg_info ) );
        count = 0;
        for( class = sect->classlist; class != NULL; class = class->next_class ) {
            if( (class->flags & CLASS_DEBUG_INFO) == 0 ) {
                for( seg = NULL; (seg = RingStep( class->segs, seg )) != NULL; ) {
                    segs[count].idx = count;
                    segs[count].seg = seg;
                    count++;
                }
            }
        }
        qsort( segs, count, sizeof( seg_info ), cmp_seg );
        for( i = 0; i < count; ++i ) {
            WriteNonAbsSeg( segs[i].seg );
        }
        if( Absolute_Seg ) {
            WriteBox( MSG_MAP_BOX_ABS_SEG );
            WriteMapMsg( MSG_MAP_TITLE_ABS_SEG_0 );
            WriteMapMsg( MSG_MAP_TITLE_ABS_SEG_1 );
            WriteMapNL();
            for( i = 0; i < count; ++i ) {
                WriteAbsSeg( segs[i].seg );
            }
        }
        _LnkFree( segs );
    }
}

void WritePubHead( void )
/******************************/
{
    WriteBox( MSG_MAP_BOX_MEMORY_MAP );
    WriteMapMsg( MSG_MAP_UNREF_SYM );
    WriteMapMsg( MSG_MAP_REF_LOCAL_SYM );
    if( MapFlags & MAP_STATICS ) {
        WriteMapMsg( MSG_MAP_SYM_STATIC );
    }
    WriteMapNL();
    WriteMapMsg( MSG_MAP_TITLE_MEMORY_MAP_0 );
    WriteMapMsg( MSG_MAP_TITLE_MEMORY_MAP_1 );
    WriteMapNL();
}

void WritePubModHead( void )
/**************************/
{
    char        full_name[PATH_MAX];

    if( CurrMod->f.source == NULL ) {
        strcpy( full_name, CurrMod->name.u.ptr );
    } else {
        MakeFileName( CurrMod->f.source->infile, full_name );
    }
    WriteMapMsgPrintf( MSG_MAP_DEFINING_MODULE, full_name, CurrMod->name.u.ptr );
}

void WriteOvlHead( void )
/******************************/
{
    WriteBox( MSG_MAP_BOX_OVERLAY_VECTOR );
}

static void WriteModSegHead( void )
/*********************************/
{
    WriteBox( MSG_MAP_BOX_MOD_SEG );
    if( Absolute_Seg ) {
        WriteMapMsg( MSG_MAP_ABS_ADDR );
    }
    WriteMapMsg( MSG_MAP_32BIT_SEG );
    WriteMapMsg( MSG_MAP_COMDAT );
    WriteMapNL();
    WriteMapMsg( MSG_MAP_TITLE_MOD_SEG_0 );
    WriteMapMsg( MSG_MAP_TITLE_MOD_SEG_1 );
    WriteMapNL();
}

static void WriteImports( void )
/******************************/
{
    if( FmtData.type & (MK_NOVELL | MK_OS2 | MK_WIN_NE | MK_PE) ) {
        WriteBox( MSG_MAP_BOX_IMP_SYM );
        if( FmtData.type & (MK_NOVELL | MK_ELF) ) {
            WriteMapMsg( MSG_MAP_TITLE_IMP_SYM_0 );
            WriteMapMsg( MSG_MAP_TITLE_IMP_SYM_1 );
        } else {
            WriteMapMsg( MSG_MAP_TITLE_IMP_SYM_2 );
            WriteMapMsg( MSG_MAP_TITLE_IMP_SYM_3 );
        }
        WriteMapNL();
        XWriteImports();
    }
}

static void WriteVerbSeg( void *_seg )
/************************************/
// NYI: completely broken for absolute segments
{
    segdata     *seg = _seg;
    char        star;
    char        bang;
    char        see;
    targ_addr   addr;
    seg_leader  *leader;

    if( seg->isdead )
        return;
    leader = seg->u.leader;
    if( leader->class->flags & CLASS_DEBUG_INFO )
        return;
    WriteFormat( 16, "%s", leader->segname );
    WriteFormat( 38, "%s", leader->class->name );
    if( leader->info & SEG_ABSOLUTE ) {
        star = '*';
    } else {
        star = ' ';
    }
    if( seg->bits == BITS_32 ) {
        bang = '!';
    } else {
        bang = ' ';
    }
    if( seg->iscdat ) {
        see = 'c';
    } else {
        see = ' ';
    }
    addr = leader->seg_addr;
    addr.off += seg->a.delta;
    if( (FmtData.type & MK_16BIT) && ( seg->bits == BITS_32 ) ) {
        WriteFormat( 53, "%A%c%c%c", &addr, star, see, bang );
    } else {
        WriteFormat( 53, "%a%c%c%c", &addr, star, see, bang );
    }
    WriteFormat( 70, "%h", seg->length );
    WriteMapNL();
}

static void WriteVerbMod( mod_entry *mod )
/****************************************/
{
    if( (mod->modinfo & MOD_NEED_PASS_2) && mod->segs != NULL ) {
        WriteFormat( 0, "%s", mod->name.u.ptr );
        if( strlen( mod->name.u.ptr ) > 15 )
            WriteMapNL();
        Ring2Walk( mod->segs, WriteVerbSeg );
    }
}

void WriteModSegs( void )
/******************************/
{
    WriteModSegHead();
    WalkMods( WriteVerbMod );
}

static void init_state( line_state_info *state, bool default_is_stmt )
{
    state->address = 0;
    state->segment = 0;
    state->file = 1;
    state->line = 1;
    state->column = 0;
    state->is_stmt = default_is_stmt;
    state->basic_block = false;
    state->end_sequence = false;
}

static void dump_state( line_state_info *state )
{
    char str[40];

    if( state->has_seg ) {
        if( state->is_32 ) {
            sprintf( str, "%5d %04Xh:%08Xh ", state->line, state->segment, state->address );
            WriteMapDirectString( str, false );
        } else {
            sprintf( str, "%5d %04Xh:%04Xh ", state->line, state->segment, state->address );
            WriteMapDirectString( str, false );
        }
    } else {
        if( state->is_32 ) {
            sprintf( str, "%6d %08Xh ", state->line, state->address );
            WriteMapDirectString( str, false );
        } else {
            sprintf( str, "%6d %04Xh ", state->line, state->address );
            WriteMapDirectString( str, false );
        }
    }
    state->col++;
    if( state->col == 4 ) {
        WriteMapNL();
        state->col = 0;
    }
}

static unsigned_8 *DecodeULEB128( const unsigned_8 *input, unsigned_32 *value )
{
    unsigned_32     result;
    unsigned        shift;
    unsigned_8      byte;

    result = 0;
    shift = 0;
    for( ;; ) {
        byte = *input++;
        result |= (byte & 0x7f) << shift;
        if( (byte & 0x80) == 0 )
            break;
        shift += 7;
    }
    *value = result;
    return( (unsigned_8 *)input );
}

static unsigned_8 *DecodeSLEB128( const unsigned_8 *input, signed_32 *value )
{
    signed_32       result;
    unsigned        shift;
    unsigned_8      byte;

    result = 0;
    shift = 0;
    for( ;; ) {
        byte = *input++;
        result |= (byte & 0x7f) << shift;
        shift += 7;
        if( (byte & 0x80) == 0 ) {
            break;
        }
    }
    if( ( shift < 32 ) && (byte & 0x40) ) {
        result |= - ( 1 << shift );
    }
    *value = result;
    return( (unsigned_8 *)input );
}

static unsigned_8 *SkipLEB128( const unsigned_8 *input )
{
    unsigned_8      byte;

    do {
        byte = *input++;
    } while( byte & 0x80 );
    return( (unsigned_8 *)input );
}

void WriteMapLines( void )
{
    unsigned_8                  *input;
    virt_mem                    input_vm;
    virt_mem_size               length;
    unsigned_8                  *p;
    int                         opcode_base;
    int                         i;
    unsigned                    *opcode_lengths;
    unsigned                    u;
    unsigned_8                  *name;
    unsigned_32                 op_len;
    unsigned_32                 tmp;
    signed_32                   itmp;
    int                         line_range;
    int                         line_base;
    bool                        default_is_stmt;
    line_state_info             state;
    unsigned                    min_instr;
    unsigned_32                 unit_length;
    unsigned_8                  *unit_base;
    int                         value;
    dw_lne                      value_lne;
    dw_lns                      value_lns;

    input_vm = DwarfGetLineInfo( &length );
    if( input_vm == 0 || length == 0 )
       return;

    _ChkAlloc( input, length );
    ReadInfo( input_vm, input, length );

    for( p = input; p - input < length; ) {
        state.col = 0;
        state.is_32 = false;
        state.has_seg = false;
        unit_length = MGET_U32( p );
        p += sizeof( unsigned_32 );
        unit_base = p;

        p += sizeof( unsigned_16 );
        p += sizeof( unsigned_32 );
        min_instr = *p;
        p += 1;

        default_is_stmt = ( *p != 0 );
        p += 1;

        line_base = MGET_S8( p );
        p += 1;

        line_range = MGET_U8( p );
        p += 1;

        opcode_base = *p;
        p += 1;

        _ChkAlloc( opcode_lengths, sizeof( unsigned ) * ( opcode_base - 1 ) );
        for( i = 0; i < opcode_base - 1; ++i ) {
            opcode_lengths[i] = *p++;
        }

        if( p - input >= length ) {
            _LnkFree( opcode_lengths );
            _LnkFree( input );
            return;
        }

        while( *p != 0 ) {
            p += strlen( (char *)p ) + 1;
            if( p - input >= length ) {
                _LnkFree( opcode_lengths );
                _LnkFree( input );
                return;
            }
        }
        p++;
        while( *p != 0 ) {
            WriteMapNL();
            WriteMap( "Line numbers for %s", p );
            WriteMapNL();
            p += strlen( (char *)p ) + 1;
            p = SkipLEB128( p );
            p = SkipLEB128( p );
            p = SkipLEB128( p );
            if( p - input >= length ) {
                _LnkFree( opcode_lengths );
                _LnkFree( input );
                return;
            }
        }

        p++;
        init_state( &state, default_is_stmt );
        while( p - unit_base < unit_length ) {
            value_lns = *p++;
            if( value_lns == 0 ) {
                p = DecodeULEB128( p, &op_len );
                value_lne = *p++;
                --op_len;
                switch( value_lne ) {
                case DW_LNE_end_sequence:
                    state.end_sequence = true;
                    init_state( &state, default_is_stmt );
                    p += op_len;
                    break;
                case DW_LNE_set_address:
                    if( op_len == 4 ) {
                        tmp = MGET_U32( p );
                        state.is_32 = true;
                    } else if( op_len == 2 ) {
                        tmp = MGET_U16( p );
                        state.is_32 = false;
                    } else {
                        tmp = 0xffffffff;
                    }
                    state.address = tmp;
                    p += op_len;
                    break;
                case DW_LNE_WATCOM_set_segment_OLD:
                case DW_LNE_WATCOM_set_segment:
                    state.has_seg = true;
                    if( op_len == 4 ) {
                        tmp = MGET_U32( p );
                    } else if( op_len == 2 ) {
                        tmp = MGET_U16( p );
                    } else {
                        tmp = 0xffffffff;
                    }
                    state.segment = tmp;
                    p += op_len;
                    break;
                case DW_LNE_define_file:
                    name = p;
                    p += strlen( (char *)p ) + 1;
                    p = SkipLEB128( p );
                    p = SkipLEB128( p );
                    p = SkipLEB128( p );
                    WriteMap( "File:  " );
                    WriteMap( (char *)name );
                    break;
                default:
                    p += op_len;
                    break;
                }
            } else if( value_lns < opcode_base ) {
                switch( value_lns ) {
                case DW_LNS_copy:
                    dump_state( &state );
                    state.basic_block = false;
                    break;
                case DW_LNS_advance_pc:
                    p = DecodeULEB128( p, &tmp );
                    state.address += tmp * min_instr;
                    break;
                case DW_LNS_advance_line:
                    p = DecodeSLEB128( p, &itmp );
                    state.line += itmp;
                    break;
                case DW_LNS_set_file:
                    p = DecodeULEB128( p, &tmp );
                    state.file = tmp;
                    break;
                case DW_LNS_set_column:
                    p = DecodeULEB128( p, &tmp );
                    state.column = tmp;
                    break;
                case DW_LNS_negate_stmt:
                    state.is_stmt = !state.is_stmt;
                    break;
                case DW_LNS_set_basic_block:
                    state.basic_block = true;
                    break;
                case DW_LNS_const_add_pc:
                    value = 255 - opcode_base;
                    state.address += ( value / line_range ) * min_instr;
                    break;
                case DW_LNS_fixed_advance_pc:
                    tmp = MGET_U16( p );
                    p += sizeof( unsigned_16 );
                    state.address += tmp;
                    break;
                default:
                    for( u = opcode_lengths[value_lns - 1]; u > 0 ; --u ) {
                        p = SkipLEB128( p );
                    }
                }
            } else {
                value = value_lns - opcode_base;
                state.line += line_base + ( value % line_range );
                state.address += ( value / line_range ) * min_instr;
                dump_state( &state );
                state.basic_block = false;
            }
        }
        WriteMapNL();
        _LnkFree( opcode_lengths );
    }
    _LnkFree( input );
}

static bool CheckSymRecList( void *_info, void *sym )
/***************************************************/
{
    symrecinfo *info = _info;

    return( ( sym == info->sym ) && ( CurrMod == info->mod ) );
}

static void AddSymRecList( symbol *sym, symrecinfo **head )
/*********************************************************/
{
    symrecinfo      *info;

    if( RingLookup( *head, CheckSymRecList, sym ) == NULL ) {
        _ChkAlloc( info, sizeof( symrecinfo ) );
        info->next = NULL;
        info->sym = sym;
        info->mod = CurrMod;
        RingAppend( head, info );
    }
}

void RecordUndefinedSym( symbol *sym )
/************************************/
{
    if( (LinkFlags & LF_UNDEFS_ARE_OK) == 0 )
        LinkState |= LS_LINK_ERROR;
    AddSymRecList( sym, &UndefList );
}

void RecordTracedSym( symbol *sym )
/*********************************/
{
    if( sym->mod != CurrMod ) {
        AddSymRecList( sym, &SymTraceList );
    }
}

static void PrintUndefinedSym( void *_info )
/*********************************************/
{
    symrecinfo  *info = _info;
    mod_entry   *mod;

    mod = info->mod;
    LnkMsg( YELL+MSG_UNDEF_SYM, "12S", mod->f.source->infile->name, mod->name,
                                       info->sym );
    WriteFormat( 0, "%S", info->sym );
    WriteFormat( 32, "%s(%s)", mod->f.source->infile->name, mod->name );
    WriteMapNL();
}

static void PrintTracedSym( void *_info )
/******************************************/
{
    symrecinfo  *info = _info;

    LnkMsg( MAP+MSG_MOD_TRACE, "Ss", info->sym, info->mod->name );
}

void PrintUndefinedSyms( void )
/*****************************/
{
    if( UndefList != NULL ) {
        WriteBox( MSG_MAP_BOX_UNRES_REF );
        WriteMapMsg( MSG_MAP_TITLE_UNRES_REF_0 );
        WriteMapMsg( MSG_MAP_TITLE_UNRES_REF_1 );
        WriteMapNL();
        RingWalk( UndefList, PrintUndefinedSym );
    }
}

void PrintTracedSyms( void )
/**************************/
{
    if( SymTraceList != NULL ) {
        WriteBox( MSG_MAP_BOX_TRACE_SYM );
        RingWalk( SymTraceList, PrintTracedSym );
        WriteMapNL();
    }
}

void FreeUndefinedSyms( void )
/****************************/
{
    RingFree( &UndefList );
}

void FreeTracedSyms( void )
/*************************/
{
    RingFree( &SymTraceList );
}

static void Write32( char *s, unsigned_32 size )
/**********************************************/
{
    unsigned_16         lo;

    if( size <= 0xffff ) {
        lo = size & 0xffff;
        WriteMap( "%s  %x (%d.)", s, lo, lo );
    } else {
        WriteMap( "%s  %h (%l.)", s, size, size );
    }
}

void WriteLibsUsed( void )
/*******************************/
{
    file_list   *lib;
    char        new_name[PATH_MAX];

    if( LinkState & LS_GENERATE_LIB_LIST ) {
        WriteBox( MSG_MAP_BOX_LIB_USED );
        for( lib = ObjLibFiles; lib != NULL; lib = lib->next_file ) {
            if( lib->flags & STAT_LIB_USED ) {
                MakeFileName( lib->infile, new_name );
                WriteMap( "%s", new_name );
            }
        }
        LinkState &= ~LS_GENERATE_LIB_LIST;
    }
}

static const char *getStubName( void )
/************************************/
{
#ifdef _OS2
    if( FmtData.u.os2fam.no_stub ) {
        return( "none" );
    }
    if( FmtData.type & (MK_OS2 | MK_WIN_NE | MK_PE | MK_WIN_VXD) ) {
        return( FmtData.u.os2fam.stub_file_name );
    }
#endif
#ifdef _DOS16M
    if( FmtData.type & MK_DOS16M ) {
        return( FmtData.u.d16m.stub );
    }
#endif
#ifdef _PHARLAP
    if( FmtData.type & MK_PHAR_LAP ) {
        return( FmtData.u.phar.stub );
    }
#endif
    return( NULL );
}

void MapSizes( void )
/**************************/
/*
  Write out code size to map file and print libraries used.
*/
{
    char        msg_buff[RESOURCE_MAX_SIZE];
    const char  *stubname;

    if( UndefList != NULL ) {
        WriteMapNL();
    }
    WriteImports();
    WriteLibsUsed();
    WriteBox( MSG_MAP_BOX_LINKER_STAT );
    Msg_Get( MSG_MAP_STACK_SIZE, msg_buff );
    Write32( msg_buff, StackSize );
    Msg_Get( MSG_MAP_MEM_SIZE, msg_buff );
    Write32( msg_buff, MemorySize() );
#ifdef _EXE
    if( (FmtData.type & MK_OVERLAYS) && FmtData.u.dos.dynamic ) {
        Msg_Get( MSG_MAP_OVL_SIZE, msg_buff );
        Write32( msg_buff, (unsigned long)OvlAreaSize * 16 );
    }
#endif
    if( (FmtData.type & MK_NOVELL) == 0 && ( !FmtData.dll || (FmtData.type & MK_PE) ) ) {
        WriteMapMsgPrintf( MSG_MAP_ENTRY_PT_ADDR, &StartInfo.addr );
    }
    stubname = getStubName();
    if( stubname != NULL ) {
        WriteMapMsgPrintf( MSG_MAP_STUB_FILE, stubname );
    }
}

void EndTime( void )
/*************************/
{
    char        *ptr;
    signed_16   h;
    signed_16   m;
    signed_16   s;
    signed_16   t;
    char        tim[11 + 1];

    if( MapFlags & MAP_FLAG ) {

        ClockTicks = clock() - ClockTicks;
        t = (unsigned_16)( ClockTicks % CLOCKS_PER_SEC );
        ClockTicks /= CLOCKS_PER_SEC;
        s = (unsigned_16)( ClockTicks % 60 );
        ClockTicks /= 60;
        m = (unsigned_16)( ClockTicks % 60 );
        ClockTicks /= 60;
        h = (unsigned_16)ClockTicks;

        ptr = tim;
        if( h > 0 ) {
            ptr = PutDec( ptr, h );
            *ptr++ = ':';
        }
        ptr = PutDec( ptr, m );
        *ptr++ = ':';
        ptr = PutDec( ptr, s );
        *ptr++ = '.';
        ptr = PutDec( ptr, t );
        *ptr = '\0';
        WriteMapMsgPrintf( MSG_MAP_LINK_TIME, tim );
    }
}

void WriteMapNL( void )
/*********************/
{
    if( MapFlags & MAP_FLAG ) {
        WriteMapDirectNL();
    }
}

void WriteMap( const char *format, ... )
/**************************************/
{
    va_list     args;
    char        buff[MAX_MSG_SIZE];
    size_t      len;

    if( MapFlags & MAP_FLAG ) {
        va_start( args, format );
        len = DoFmtStr( buff, sizeof( buff ), format, args );
        va_end( args );
        WriteMapDirect( buff, len );
        WriteMapNL();
    }
}

void WriteFormat( size_t col, const char *str, ... )
/****************************************************/
{
    va_list         args;
    size_t          num;
    static  char    Blanks[]={"                                      "};
    char            buff[MAX_MSG_SIZE];
    size_t          len;

    if( MapFlags & MAP_FLAG ) {
        num = 0;
        if( col > MapCol ) {
            num = col - MapCol;
        } else if( MapCol != 0 ) {
            num = 1;
        }
        MapCol += num;
        WriteMapDirect( Blanks, num );
        va_start( args, str );
        len = DoFmtStr( buff, sizeof( buff ), str, args );
        va_end( args );
        WriteMapDirect( buff, len );
        MapCol += len;
    }
}

void WriteMapDirect2Str( const char *s1, size_t len1, const char *s2, size_t len2 )
{
    WriteMapDirect( s1, len1 );
    WriteMapDirect( s2, len2 );
    WriteMapDirectNL();
}
