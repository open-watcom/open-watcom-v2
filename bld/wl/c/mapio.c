/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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
#include <errno.h>
#include "linkstd.h"
#include "pcobj.h"
#include "newmem.h"
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
#include "impexp.h"
#include "dbgcomm.h"
#include "dbgdwarf.h"

#include "clibext.h"


/*
 * size of map file buffer
 */
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

typedef struct {
    unsigned    idx;
    seg_leader  *seg;
} seg_info;

static symrecinfo       *UndefList;
static symrecinfo       *SymTraceList;
static size_t           MapCol;
static time_t           StartT;
static clock_t          ClockTicks;
static bool             Absolute_Seg;
static unsigned long    NumMapSyms;

void ResetWriteMapIO( void )
/**************************/
{
    MapFlags = 0;
    MapFName = NULL;
    MapFile = NULL;
    UndefList = NULL;
    SymTraceList = NULL;
}

static char *PutDec( char *ptr, unsigned num )
/********************************************/
{
    *ptr++ = (( num / 10 ) % 10) + '0';
    *ptr++ = (num % 10) + '0';
    return( ptr );
}

static void WriteMap( const char *buffer, size_t len )
/*****************************************************
 * buffering the write to the map file
 */
{
    if( len > 0 ) {
        if( fwrite( buffer, len, 1, MapFile ) == 0 ) {
            LnkMsg( ERR+MSG_IO_PROBLEM, "12", MapFName, strerror( errno ) );
        }
    }
}

static void WriteMapNL( void )
/****************************/
{
    WriteMap( "\n", 1 );
    MapCol = 0;
}

static void WriteMapString( const char *buffer )
/**********************************************/
{
    WriteMap( buffer, strlen( buffer ) );
}

static void WriteMapMsgPrintf( int msgid, ... )
/*********************************************/
{
    char        format[RESOURCE_MAX_SIZE];
    va_list     args;
    char        buff[MAX_MSG_SIZE];
    size_t      len;

    Msg_Get( msgid, format );
    va_start( args, msgid );
    len = DoFmtStr( buff, sizeof( buff ), format, args );
    va_end( args );
    WriteMap( buff, len );
    WriteMapNL();
}

static void WriteMapMsg( int msgid )
/**********************************/
{
    char        buff[RESOURCE_MAX_SIZE];

    Msg_Get( msgid, buff );
    WriteMap( buff, strlen( buff ) );
    WriteMapNL();
}

static void WriteMapPrintf( const char *format, ... )
/***************************************************/
{
    va_list     args;
    char        buff[MAX_MSG_SIZE];
    size_t      len;

    va_start( args, format );
    len = DoFmtStr( buff, sizeof( buff ), format, args );
    va_end( args );
    WriteMap( buff, len );
    WriteMapNL();
}

static void WriteMapBox( int msgid )
/**********************************/
{
    char        box_buff[RESOURCE_MAX_SIZE];
    char        msg_buff[RESOURCE_MAX_SIZE];
    size_t      i;

    Msg_Get( msgid, msg_buff );
    WriteMapNL();
    WriteMapNL();
    box_buff[0] = '+';
    for( i = 2; i < strlen( msg_buff ); i++ ) {
        box_buff[i - 1] = '-';
    }
    box_buff[i - 1] = '+';
    box_buff[i] = '\0';
    WriteMapPrintf( "%t24%s", "", box_buff );
    WriteMapPrintf( "%t24%s", "", msg_buff );
    WriteMapPrintf( "%t24%s", "", box_buff );
    WriteMapNL();
}

static void WriteMapColPrintf( size_t col, const char *str, ... )
/***************************************************************/
{
    va_list         args;
    size_t          num;
    static  char    Blanks[]={"                                      "};
    char            buff[MAX_MSG_SIZE];
    size_t          len;

    num = 0;
    if( col > MapCol ) {
        num = col - MapCol;
    } else if( MapCol != 0 ) {
        num = 1;
    }
    MapCol += num;
    WriteMap( Blanks, num );
    va_start( args, str );
    len = DoFmtStr( buff, sizeof( buff ), str, args );
    va_end( args );
    WriteMap( buff, len );
    MapCol += len;
}

static void WriteMapGroups( void )
/********************************/
{
    group_entry     *currgrp;

    if( Groups != NULL ) {
        WriteMapBox( MSG_MAP_BOX_GROUP );
        WriteMapMsg( MSG_MAP_TITLE_GROUP_0 );
        WriteMapMsg( MSG_MAP_TITLE_GROUP_1 );
        WriteMapNL();
        for( currgrp = Groups; currgrp != NULL; currgrp = currgrp->next_group ) {
            if( !currgrp->isautogrp ) { /* if not an autogroup */
                WriteMapColPrintf( 0, "%s", currgrp->sym->name );
                WriteMapColPrintf( 32, "%a", &currgrp->grp_addr );
                WriteMapColPrintf( 53, "%h", currgrp->totalsize );
                WriteMapNL();
            }
        }
        WriteMapNL();
    }
}

static void WriteMapAbsSeg( void *_leader )
/*****************************************/
{
    seg_leader  *leader = _leader;

    if( leader->info & SEG_ABSOLUTE ) {
        WriteMapColPrintf( 0, "%s", leader->segname );
        WriteMapColPrintf( 24, "%s", leader->class->name );
        if( (FmtData.type & MK_16BIT)
          && (leader->info & USE_32) ) {
            WriteMapColPrintf( 40, "%A", &leader->seg_addr );
        } else {
            WriteMapColPrintf( 40, "%a", &leader->seg_addr );
        }
        WriteMapColPrintf( 60, "%h", leader->size );
        WriteMapNL();
    }
}

static void WriteMapNonAbsSeg( void *_seg )
/*****************************************/
{
    seg_leader  *seg = _seg;

    if( seg->info & SEG_ABSOLUTE ) {
        Absolute_Seg = true;
    } else {
        WriteMapColPrintf( 0, "%s", seg->segname );
        WriteMapColPrintf( 23, "%s", seg->class->name );
        if( seg->group != NULL ) {
            WriteMapColPrintf( 38, "%s", seg->group->sym->name );
        }
        if( (FmtData.type & MK_16BIT)
          && (seg->info & USE_32) ) {
            WriteMapColPrintf( 53, "%A", &seg->seg_addr );
        } else {
            WriteMapColPrintf( 53, "%a", &seg->seg_addr );
        }
        WriteMapColPrintf( 69, "%h", seg->size );
        WriteMapNL();
    }
}

static int cmp_seg( const void *a, const void *b )
/************************************************/
{
    if( ((seg_info *)a)->seg->seg_addr.seg == ((seg_info *)b)->seg->seg_addr.seg ) {
        if( ((seg_info *)a)->seg->seg_addr.off == ((seg_info *)b)->seg->seg_addr.off )
            return( ((seg_info *)a)->idx - ((seg_info *)b)->idx );
        return( ((seg_info *)a)->seg->seg_addr.off - ((seg_info *)b)->seg->seg_addr.off );
    }
    return( ((seg_info *)a)->seg->seg_addr.seg - ((seg_info *)b)->seg->seg_addr.seg );
}

static void WriteMapSectSegs( section *sect )
/********************************************
 * write segment info into map file
 */
{
    class_entry     *class;
    size_t          count;
    size_t          i;
    seg_leader      *seg;
    seg_info        *segs;

    if( sect->classlist != NULL ) {
        WriteMapBox( MSG_MAP_BOX_SEGMENTS );
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
            WriteMapNonAbsSeg( segs[i].seg );
        }
        if( Absolute_Seg ) {
            WriteMapBox( MSG_MAP_BOX_ABS_SEG );
            WriteMapMsg( MSG_MAP_TITLE_ABS_SEG_0 );
            WriteMapMsg( MSG_MAP_TITLE_ABS_SEG_1 );
            WriteMapNL();
            for( i = 0; i < count; ++i ) {
                WriteMapAbsSeg( segs[i].seg );
            }
        }
        _LnkFree( segs );
    }
}

static void WriteMapMemoryMapHead( void )
/***************************************/
{
    WriteMapBox( MSG_MAP_BOX_MEMORY_MAP );
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

void WriteMapHead( section *sect )
/********************************/
{
    WriteMapGroups();
    WriteMapSectSegs( sect );
    WriteMapMemoryMapHead();
}

static void WriteMapPubModHead( void )
/************************************/
{
    char        full_name[PATH_MAX];

    if( CurrMod->f.source == NULL ) {
        strcpy( full_name, CurrMod->name.u.ptr );
    } else {
        MakeFileName( CurrMod->f.source->infile, full_name );
    }
    WriteMapMsgPrintf( MSG_MAP_DEFINING_MODULE, full_name, CurrMod->name.u.ptr );
}

static void WriteMapSym( symbol * sym, char star )
/************************************************/
{
    if( sym->info & SYM_STATIC ) {
        star = 's';
    }
    if( (FmtData.type & MK_16BIT)
      && ( sym->p.seg != NULL )
      && ( sym->p.seg->bits == BITS_32 ) ) {
        WriteMapColPrintf( 0, "%A%c", &sym->addr, star );
    } else {
        WriteMapColPrintf( 0, "%a%c", &sym->addr, star );
    }
    WriteMapColPrintf( 15, "%S", sym );
}

static void WriteMapSymAddr( symbol *sym )
/****************************************/
{
    char                star;

    if( sym->info & SYM_REFERENCED ) {
        if( IS_SYM_IMPORTED( sym )
          || ((FmtData.type & MK_ELF)
          && IsSymElfImported( sym )) ) {
            star = 'i';
        } else {
            star = ' ';
        }
    } else if( sym->info & SYM_LOCAL_REF ) {
        star = '+';
    } else {
        star = '*';
    }
    WriteMapSym( sym, star );
    WriteMapNL();
}

void WriteMapLnkMsgCallback( unsigned msgnum, const char *str, size_t len )
/**************************************************************************
 * write link message to the map file
 */
{
    char        msgprefix[MAX_MSG_SIZE];
    size_t      msgprefixlen;

    msgprefixlen = GetMsgPrefix( msgnum, msgprefix, MAX_MSG_SIZE );
    WriteMap( msgprefix, msgprefixlen );
    WriteMap( str, len );
    WriteMapNL();
}

void WriteMapOvlVectHead( vect_state *VectState )
/***********************************************/
{
    vecnode             *vectnode;
    int                 n;
    targ_addr           addr;
    symbol              *sym;

    WriteMapNL();
    WriteMapNL();
    WriteMapSymAddr( VectState->OverlayTable );
    WriteMapSymAddr( VectState->OverlayTableEnd );
    WriteMapSymAddr( VectState->OvlVecStart );
    WriteMapSymAddr( VectState->OvlVecEnd );
    WriteMapBox( MSG_MAP_BOX_OVERLAY_VECTOR );
    n = 1;
    for( vectnode = VectState->OvlVectors; vectnode != NULL; vectnode = vectnode->next ) {
        OvlGetVecAddr( n++, &addr );
        sym = vectnode->sym;
        WriteMapPrintf( "%a section %d : %S",
            &addr, sym->p.seg->u.leader->class->section->ovlref, sym );
    }
}


void WriteMapOvlPubHead( section *sec )
/*************************************/
{
    WriteMapNL();
    WriteMapNL();
    WriteMapPrintf( "Overlay section %d address %a", sec->ovlref, &sec->sect_addr );
    WriteMapPrintf( "====================================" );
    WriteMapSectSegs( sec );
    WriteMapMemoryMapHead();
}

static void WriteMapModulesSegmentsHead( void )
/*********************************************/
{
    WriteMapBox( MSG_MAP_BOX_MOD_SEG );
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

static void WriteMapImports( void )
/*********************************/
{
    symbol *    sym;

    for( sym = HeadSym; sym != NULL; sym = sym->link ) {
        if( IS_SYM_IMPORTED( sym )
          && sym->p.import != NULL ) {
            if( (FmtData.type & MK_NOVELL) == 0
              || sym->p.import != DUMMY_IMPORT_PTR ) {
                if( sym->prefix != NULL
                  && ( sym->prefix[0] != '\0' ) ) {
                    WriteMapColPrintf( 0, "%s@%s", sym->prefix, sym->name );
                } else {
                    WriteMapColPrintf( 0, "%s", sym->name );
                }
#ifdef _OS2
                if( FmtData.type & (MK_OS2 | MK_WIN_NE | MK_PE | MK_WIN_VXD) ) {
                    WriteMapColPrintf( 36, "%s", ImpModuleName( sym->p.import ) );
                }
#endif
                WriteMapNL();
            }
        }
    }
}

static void WriteMapImportsHead( void )
/*************************************/
{
    WriteMapBox( MSG_MAP_BOX_IMP_SYM );
    if( FmtData.type & (MK_NOVELL | MK_ELF) ) {
        WriteMapMsg( MSG_MAP_TITLE_IMP_SYM_0 );
        WriteMapMsg( MSG_MAP_TITLE_IMP_SYM_1 );
    } else {
        WriteMapMsg( MSG_MAP_TITLE_IMP_SYM_2 );
        WriteMapMsg( MSG_MAP_TITLE_IMP_SYM_3 );
    }
    WriteMapNL();
}

static void WriteMapSegment( void *_seg )
/****************************************
 * NYI: completely broken for absolute segments
 */
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
    WriteMapColPrintf( 16, "%s", leader->segname );
    WriteMapColPrintf( 38, "%s", leader->class->name );
    star = ( leader->info & SEG_ABSOLUTE ) ? '*' : ' ';
    bang = ( seg->bits == BITS_32 ) ? '!' : ' ';
    see = ( seg->iscdat ) ? 'c' : ' ';
    addr = leader->seg_addr;
    addr.off += seg->a.delta;
    if( (FmtData.type & MK_16BIT) && ( seg->bits == BITS_32 ) ) {
        WriteMapColPrintf( 53, "%A%c%c%c", &addr, star, see, bang );
    } else {
        WriteMapColPrintf( 53, "%a%c%c%c", &addr, star, see, bang );
    }
    WriteMapColPrintf( 70, "%h", seg->length );
    WriteMapNL();
}

static void WriteMapModuleSegments( mod_entry *mod )
/**************************************************/
{
    if( (mod->modinfo & MOD_NEED_PASS_2) && mod->segs != NULL ) {
        WriteMapColPrintf( 0, "%s", mod->name.u.ptr );
        if( strlen( mod->name.u.ptr ) > 15 )
            WriteMapNL();
        Ring2Walk( mod->segs, WriteMapSegment );
    }
}

void WriteMapModulesSegments( void )
/**********************************/
{
    WriteMapModulesSegmentsHead();
    WalkMods( WriteMapModuleSegments );
}

static void InitDwarfVMState( line_state_info *state, bool default_is_stmt )
/**************************************************************************/
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

static void DumpDwarfVMState( line_state_info *state )
/****************************************************/
{
    char        str[40];
    const char  *fmt;

    if( state->has_seg ) {
        if( state->is_32 ) {
            fmt = "%5d %04Xh:%08Xh ";
        } else {
            fmt = "%5d %04Xh:%04Xh ";
        }
        sprintf( str, fmt, state->line, state->segment, state->address );
    } else {
        if( state->is_32 ) {
            fmt = "%6d %08Xh ";
        } else {
            fmt = "%6d %04Xh ";
        }
        sprintf( str, fmt, state->line, state->address );
    }
    WriteMapString( str );
    state->col++;
    if( state->col == 4 ) {
        WriteMapNL();
        state->col = 0;
    }
}

static unsigned_8 *DecodeULEB128( const unsigned_8 *input, unsigned_32 *value )
/*****************************************************************************/
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
/***************************************************************************/
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
    if( ( shift < 32 )
      && (byte & 0x40) ) {
        result |= - ( 1 << shift );
    }
    *value = result;
    return( (unsigned_8 *)input );
}

static unsigned_8 *SkipLEB128( const unsigned_8 *input )
/******************************************************/
{
    unsigned_8      byte;

    do {
        byte = *input++;
    } while( byte & 0x80 );
    return( (unsigned_8 *)input );
}

static void WriteMapLines( void )
/*******************************/
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
    if( input_vm == 0
      || length == 0 )
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
            WriteMapPrintf( "Line numbers for %s", p );
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
        InitDwarfVMState( &state, default_is_stmt );
        while( p - unit_base < unit_length ) {
            value_lns = *p++;
            if( value_lns == 0 ) {
                p = DecodeULEB128( p, &op_len );
                value_lne = *p++;
                --op_len;
                switch( value_lne ) {
                case DW_LNE_end_sequence:
                    state.end_sequence = true;
                    InitDwarfVMState( &state, default_is_stmt );
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
                    WriteMapPrintf( "File:  %s", (char *)name );
                    break;
                default:
                    p += op_len;
                    break;
                }
            } else if( value_lns < opcode_base ) {
                switch( value_lns ) {
                case DW_LNS_copy:
                    DumpDwarfVMState( &state );
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
                DumpDwarfVMState( &state );
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

static void PrintUndefinedSym( void *_info )
/******************************************/
{
    symrecinfo  *info = _info;
    mod_entry   *mod;

    mod = info->mod;
    LnkMsg( YELL+MSG_UNDEF_SYM, "12S", mod->f.source->infile->name, mod->name, info->sym );
    if( MapFile != NULL ) {
        WriteMapColPrintf( 0, "%S", info->sym );
        WriteMapColPrintf( 32, "%s(%s)", mod->f.source->infile->name, mod->name );
        WriteMapNL();
    }
}

void PrintUndefinedSyms( void )
/*****************************/
{
    if( UndefList != NULL ) {
        if( MapFile != NULL ) {
            WriteMapBox( MSG_MAP_BOX_UNRES_REF );
            WriteMapMsg( MSG_MAP_TITLE_UNRES_REF_0 );
            WriteMapMsg( MSG_MAP_TITLE_UNRES_REF_1 );
            WriteMapNL();
        }
        RingWalk( UndefList, PrintUndefinedSym );
    }
}

void WriteMapTracedSymRecord( symbol *sym )
/*****************************************/
{
    if( sym->mod != CurrMod ) {
        AddSymRecList( sym, &SymTraceList );
    }
}

static void WriteMapTracedSym( void *_info )
/******************************************/
{
    symrecinfo  *info = _info;

    WriteMapLnkMsg( MSG_MOD_TRACE, "Ss", info->sym, info->mod->name );
}

void WriteMapTracedSyms( void )
/*****************************/
{
    if( SymTraceList != NULL ) {
        WriteMapBox( MSG_MAP_BOX_TRACE_SYM );
        RingWalk( SymTraceList, WriteMapTracedSym );
        WriteMapNL();
    }
}

static void WriteMapSize( int msgid, unsigned_32 size )
/*****************************************************/
{
    char        msg[RESOURCE_MAX_SIZE];

    Msg_Get( msgid, msg );
    if( size > 0xffff ) {
        WriteMapPrintf( "%s  %h (%l.)", msg, size, size );
    } else {
        WriteMapPrintf( "%s  %x (%d.)", msg, (unsigned_16)size, (unsigned_16)size );
    }
}

void WriteMapLibsUsed( void )
/***************************/
{
    file_list   *lib;
    char        new_name[PATH_MAX];

    if( LinkState & LS_GENERATE_LIB_LIST ) {
        WriteMapBox( MSG_MAP_BOX_LIB_USED );
        for( lib = ObjLibFiles; lib != NULL; lib = lib->next_file ) {
            if( lib->flags & STAT_LIB_USED ) {
                MakeFileName( lib->infile, new_name );
                WriteMapPrintf( "%s", new_name );
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

static void WriteMapSizes( void )
/********************************
 * Write out code size to map file and print libraries used.
 */
{
    const char  *stubname;

    if( UndefList != NULL ) {
        WriteMapNL();
    }
    if( FmtData.type & (MK_NOVELL | MK_OS2 | MK_WIN_NE | MK_PE) ) {
        WriteMapImportsHead();
        WriteMapImports();
    }
    WriteMapLibsUsed();
    WriteMapBox( MSG_MAP_BOX_LINKER_STAT );
    WriteMapSize( MSG_MAP_STACK_SIZE, StackSize );
    WriteMapSize( MSG_MAP_MEM_SIZE, MemorySize() );
#ifdef _EXE
    if( (FmtData.type & MK_OVERLAYS)
      && FmtData.u.dos.dynamic ) {
        WriteMapSize( MSG_MAP_OVL_SIZE, (unsigned long)OvlAreaSize * 16 );
    }
#endif
    if( (FmtData.type & MK_NOVELL) == 0
      && ( !FmtData.dll || (FmtData.type & MK_PE) ) ) {
        WriteMapMsgPrintf( MSG_MAP_ENTRY_PT_ADDR, &StartInfo.addr );
    }
    stubname = getStubName();
    if( stubname != NULL ) {
        WriteMapMsgPrintf( MSG_MAP_STUB_FILE, stubname );
    }
}

void StartTime( void )
/********************/
{
    StartT = time( NULL );
    ClockTicks = clock();
}

void EndTime( void )
/******************/
{
    ClockTicks = clock() - ClockTicks;
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
        MapFile = fopen( MapFName, "wt" );
        if( MapFile == NULL ) {
            MapFlags = 0;
            LnkMsg( FTL+MSG_CANT_OPEN, "12", MapFName, strerror( errno ) );
            return;
        }
        setvbuf( MapFile, NULL, _IOFBF, MAP_BUFFER_SIZE );
        localt = localtime( &StartT );
        MapCol = 0;
        WriteMapString( MsgStrings[PRODUCT] );
        WriteMapNL();
        WriteMapString( MsgStrings[COPYRIGHT] );
        WriteMapNL();
        WriteMapString( MsgStrings[COPYRIGHT2] );
        WriteMapNL();
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

        WriteMapLnkMsg( MSG_CREATED_ON, "12", dat, tim );
    }
}

void MapFini( void )
/*******************
 * Finish map processing
 */
{
    char        *ptr;
    signed_16   h;
    signed_16   m;
    signed_16   s;
    signed_16   t;
    char        tim[11 + 1];

    if( MapFile != NULL ) {

        WriteMapSizes();

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

        if( MapFlags & MAP_LINES ) {
            WriteMapLines();
        }

        MapFlags = 0;

        if( fclose( MapFile ) ) {
            LnkMsg( ERR+MSG_IO_PROBLEM, "12", MapFName, strerror( errno ) );
        }
        MapFile = NULL;

        if( MapFName != NULL ) {
            _LnkFree( MapFName );
            MapFName = NULL;
        }
    }
    RingFree( &SymTraceList );
    RingFree( &UndefList );
}

static int SymAddrCompare( const void *a, const void *b )
/*******************************************************/
{
    symbol  *left;
    symbol  *right;

    left = *((symbol **)a);
    right = *((symbol **)b);
    if( left->addr.seg < right->addr.seg ) {
        return( -1 );
    } else if( left->addr.seg > right->addr.seg ) {
        return( 1 );
    } else {
        if( left->addr.off < right->addr.off ) {
            return( -1 );
        } else if( left->addr.off > right->addr.off ) {
            return( 1 );
        }
    }
    return( 0 );
}

static void WriteMapSymArray( symbol **symarray, size_t num )
/***********************************************************/
{
    if( MapFlags & MAP_ALPHA ) {
        qsort( symarray, num, sizeof( symbol * ), SymAlphaCompare );
    } else {
        qsort( symarray, num, sizeof( symbol * ), SymAddrCompare );
    }
    do {
        WriteMapSymAddr( *symarray );
        symarray++;
        num--;
    } while( num > 0 );
}

void WriteMapPubStart( void )
/***************************/
{
    NumMapSyms = 0;
}

void WriteMapPubEnd( void )
/*************************/
{
    symbol      **symarray;
    symbol      **currsym;
    symbol      *sym;
    bool        ok;

    if( (MapFlags & MAP_GLOBAL)
      && ( NumMapSyms > 0 ) ) {
        symarray = NULL;
        if( NumMapSyms < ( UINT_MAX / sizeof( symbol * ) ) - 1 ) {
            _LnkAlloc( symarray, NumMapSyms * sizeof( symbol * ) );
        }
        currsym = symarray;
        ok = ( symarray != NULL );
        for( sym = HeadSym; sym != NULL; sym = sym->link ) {
            if( sym->info & SYM_MAP_GLOBAL ) {
                sym->info &= ~SYM_MAP_GLOBAL;
                if( ok ) {
                    *currsym = sym;
                    currsym++;
                } else {
                    WriteMapSymAddr( sym );
                }
            }
        }
        if( !ok ) {
            LnkMsg( WRN+MSG_CANT_SORT_SYMBOLS, NULL );
        } else {
            WriteMapSymArray( symarray, NumMapSyms );
            _LnkFree( symarray );
        }
    }
}

void WriteMapPubSortStart( pubdefinfo *info )
/*******************************************/
{
    info->symarray = NULL;
    if( (MapFlags & MAP_SORT)
      && (MapFlags & MAP_GLOBAL) == 0
      && ( CurrMod->publist != NULL ) ) {
        _ChkAlloc( info->symarray, Ring2Count( CurrMod->publist ) * sizeof( symbol * ) );
    }
}

void WriteMapPubSortEnd( pubdefinfo *info )
/*****************************************/
{
    if( info->num > 0 ) {
        WriteMapSymArray( info->symarray, info->num );
    }
    if( info->symarray != NULL ) {
        _LnkFree( info->symarray );
        info->symarray = NULL;
    }
}

void WriteMapPubEntry( pubdefinfo *info, symbol *sym )
/****************************************************/
{
    if( !SkipSymbol( sym ) ) {
        if( info->first
          && (MapFlags & MAP_GLOBAL) == 0 ) {
            WriteMapPubModHead();
            info->first = false;
        }
        if( MapFlags & MAP_SORT ) {
            if( MapFlags & MAP_GLOBAL ) {
                NumMapSyms++;
                sym->info |= SYM_MAP_GLOBAL;
            } else {
                info->symarray[info->num] = sym;
                info->num++;
            }
        } else {
            WriteMapSymAddr( sym );
        }
    }
}
