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


/*
   MAPIO : Map file formatting routines

*/

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
#include "mapio.h"

#undef pick
#define pick( num, string ) string

static char *MsgStrings[] = {
#include "msg.h"

typedef struct symrecinfo {
    struct symrecinfo * next;
    symbol *            sym;
    mod_entry *         mod;
} symrecinfo;

static symrecinfo *     UndefList;
static symrecinfo *     SymTraceList;
static  int             MapCol;
static  time_t          StartT;
static  clock_t         ClockTicks;
static  bool            Absolute_Seg;
static bool             Buffering;  // buffering on/off.
static int              BufferSize;          // # of chars in buffer.


extern void ResetMapIO( void )
/****************************/
{
    MapFlags = 0;
    UndefList = NULL;
    SymTraceList = NULL;
}

extern void StartTime( void )
/***************************/
{
    StartT = time( NULL );
    ClockTicks = clock();
}


static char * PutDec( char *ptr, unsigned num )
/*********************************************/
{
    *ptr++ = (num / 10) % 10 + '0';
    *ptr++ = num % 10 + '0';
    return( ptr );
}

extern void StartMapBuffering( void )
/***********************************/
{
    Buffering = TRUE;
    BufferSize = 0;
}

extern void StopMapBuffering( void )
/**********************************/
// flush buffer & shut buffering off.
{
    if( BufferSize != 0 ) {
        QWrite( MapFile, TokBuff, BufferSize, MapFName );
    }
    Buffering = FALSE;
    BufferSize = 0;
}

extern void MapInit( void )
/*************************/
{
    char                tim[ 8 + 1 ];
    char                dat[ 8 + 1 ];
    char                *ptr;
    struct tm           *localt;
    char                *msg;

    Absolute_Seg = FALSE;
    Buffering = FALSE;  // buffering on/off.
    if( (MapFlags & MAP_FLAG) == 0 ) return;
    MapFile = QOpenRW( MapFName );
    StartMapBuffering();
    localt = localtime( &StartT );
    MapCol = 0;
    msg = MsgStrings[ PRODUCT ];
    BufWrite( msg, strlen( msg ) );
    WriteMapNL( 1 );
    msg = MsgStrings[ COPYRIGHT ];
    BufWrite( msg, strlen( msg ) );
    WriteMapNL( 1 );
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
    StopMapBuffering();
}

static void WriteBox( unsigned int msgnum )
/*****************************************/
{
    char        box_buff[RESOURCE_MAX_SIZE];
    char        msg_buff[RESOURCE_MAX_SIZE];
    int         i;

    Msg_Get( msgnum, msg_buff );
    WriteMapNL( 2 );
    box_buff[0] = '+';
    for( i = 1; i < strlen( msg_buff ) - 1; i++ ) {
        box_buff[i] = '-';
    }
    box_buff[i] = '+';
    box_buff[i+1] = '\0';
    WriteMap( "%t24%s", "", box_buff );
    WriteMap( "%t24%s", "", msg_buff );
    WriteMap( "%t24%s", "", box_buff );
    WriteMapNL( 1 );
}

extern void WriteGroups( void )
/*****************************/
{
    group_entry *   currgrp;

    if( Groups != NULL ) {
        WriteBox( MSG_MAP_BOX_GROUP );
        Msg_Write_Map( MSG_MAP_TITLE_GROUP_0 );
        Msg_Write_Map( MSG_MAP_TITLE_GROUP_1 );
        WriteMapNL( 1 );
        currgrp = Groups;
        while( currgrp != NULL ) {
            if( !currgrp->isautogrp ) { /* if not an autogroup */
                WriteFormat( 0, currgrp->sym->name );
                WriteFormat( 32, "%a", &currgrp->grp_addr );
                WriteFormat( 53, "%h", currgrp->totalsize );
                WriteMapNL( 1 );
            }
            currgrp = currgrp->next_group;
        }
    }
}

static void WriteAbsSeg( seg_leader *leader )
/*******************************************/
{
    if( leader->info & SEG_ABSOLUTE ) {
        WriteFormat( 0, leader->segname );
        WriteFormat( 24, leader->class->name );
        WriteFormat( 40, "%a", &leader->seg_addr );
        WriteFormat( 60, "%h", leader->size );
        WriteMapNL( 1 );
    }
}

static void WriteAbsSegs( class_entry *cl )
/*****************************************/
/* write absolute segment info into mapfile */
{
    WriteBox( MSG_MAP_BOX_ABS_SEG );
    Msg_Write_Map( MSG_MAP_TITLE_ABS_SEG_0 );
    Msg_Write_Map( MSG_MAP_TITLE_ABS_SEG_1 );
    WriteMapNL( 1 );
    while( cl != NULL ) {
        if( (cl->flags & CLASS_HANDS_OFF) == 0 ) {
            RingWalk( cl->segs, WriteAbsSeg );
        }
        cl = cl->next_class;
    }
}

static void WriteNonAbsSeg( seg_leader *seg )
/*******************************************/
{
    if( !(seg->info & SEG_ABSOLUTE) ) {
        WriteFormat( 0, seg->segname );
        WriteFormat( 23, seg->class->name );
        if( seg->group != NULL ) {
            WriteFormat( 38, seg->group->sym->name );
        }
        WriteFormat( 53, "%a", &seg->seg_addr );
        WriteFormat( 69, "%h", seg->size );
        WriteMapNL( 1 );
    } else {
        Absolute_Seg = TRUE;
    }
}

extern void WriteSegs( class_entry *firstcl )
/*******************************************/
/* write segment info into mapfile */
{
    class_entry         *cl;
    bool                abs;

    abs = FALSE;
    cl = firstcl;
    if( cl != NULL ) {
        WriteBox( MSG_MAP_BOX_SEGMENTS );
        Msg_Write_Map( MSG_MAP_TITLE_SEGMENTS_0 );
        Msg_Write_Map( MSG_MAP_TITLE_SEGMENTS_1 );
        WriteMapNL( 1 );
        while( cl != NULL ) {
            if( (cl->flags & CLASS_HANDS_OFF) == 0 ) {
                RingWalk( cl->segs, WriteNonAbsSeg );
            }
            cl = cl->next_class;
        }
        if( Absolute_Seg ) {
            WriteAbsSegs( firstcl );
        }
    }
}

extern void WritePubHead( void )
/******************************/
{
    WriteBox( MSG_MAP_BOX_MEMORY_MAP );
    Msg_Write_Map( MSG_MAP_UNREF_SYM );
    Msg_Write_Map( MSG_MAP_REF_LOCAL_SYM );
    if( MapFlags & MAP_STATICS ) {
        Msg_Write_Map( MSG_MAP_SYM_STATIC );
    }
    WriteMapNL( 1 );
    Msg_Write_Map( MSG_MAP_TITLE_MEMORY_MAP_0 );
    Msg_Write_Map( MSG_MAP_TITLE_MEMORY_MAP_1 );
    WriteMapNL( 1 );
}

extern void WritePubModHead( void )
/*********************************/
{
    char        full_name[ PATH_MAX ];

    if (CurrMod->f.source == NULL) {
        strcpy(full_name , CurrMod->name);
    } else {
        char *  path_ptr;
        path_ptr = CurrMod->f.source->file->prefix;
        if( path_ptr != NULL ) {
            QMakeFileName( &path_ptr, CurrMod->f.source->file->name, full_name );
        } else {
            strcpy( full_name, CurrMod->f.source->file->name );
        }
    }
    Msg_Write_Map( MSG_MAP_DEFINING_MODULE, full_name, CurrMod->name );
}

extern void WriteOvlHead( void )
/******************************/
{
    WriteBox( MSG_MAP_BOX_OVERLAY_VECTOR );
}

static void WriteModSegHead( void )
/*********************************/
{
    WriteBox( MSG_MAP_BOX_MOD_SEG );
    if( Absolute_Seg ) {
        Msg_Write_Map( MSG_MAP_ABS_ADDR );
    }
    Msg_Write_Map( MSG_MAP_32BIT_SEG );
    Msg_Write_Map( MSG_MAP_COMDAT );
    WriteMapNL( 1 );
    Msg_Write_Map( MSG_MAP_TITLE_MOD_SEG_0 );
    Msg_Write_Map( MSG_MAP_TITLE_MOD_SEG_1 );
    WriteMapNL( 1 );
}

static void WriteImports( void )
/******************************/
{
    if( FmtData.type & (MK_NOVELL | MK_OS2 | MK_PE) ) {
        WriteBox( MSG_MAP_BOX_IMP_SYM );
        if( FmtData.type & (MK_NOVELL|MK_ELF) ) {
            Msg_Write_Map( MSG_MAP_TITLE_IMP_SYM_0 );
            Msg_Write_Map( MSG_MAP_TITLE_IMP_SYM_1 );
        } else {
            Msg_Write_Map( MSG_MAP_TITLE_IMP_SYM_2);
            Msg_Write_Map( MSG_MAP_TITLE_IMP_SYM_3);
        }
        WriteMapNL( 1 );
        XWriteImports();
    }
}

static void WriteVerbSeg( segdata *seg )
/**************************************/
// NYI: complelety broken for absolute segments
{
    char        star;
    char        bang;
    char        see;
    targ_addr   addr;
    seg_leader *leader;

    if( seg->isdead ) return;
    leader = seg->u.leader;
    WriteFormat( 16, leader->segname );
    WriteFormat( 38, leader->class->name );
    if( leader->info & SEG_ABSOLUTE ) {
        star = '*';
    } else {
        star = ' ';
    }
    if( seg->is32bit ) {
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
    WriteFormat( 53, "%a%c%c%c", &addr, star, see, bang );
    WriteFormat( 70, "%h", seg->length );
    WriteMapNL( 1 );
}

static void WriteVerbMod( mod_entry *mod )
/****************************************/
{
    if( mod->modinfo & MOD_NEED_PASS_2 && mod->segs != NULL ) {
        WriteFormat( 0, mod->name );
        Ring2Walk( mod->segs, WriteVerbSeg );
    }
}

extern void WriteModSegs( void )
/******************************/
{
    WriteModSegHead();
    WalkMods( WriteVerbMod );
}

static bool CheckSymRecList( symrecinfo *info, symbol *sym )
/**********************************************************/
{
    return sym == info->sym && CurrMod == info->mod;
}

static void AddSymRecList( symbol *sym, symrecinfo **head )
/*********************************************************/
{
    symrecinfo *        info;

    if( RingLookup( *head, CheckSymRecList, sym ) == NULL ) {
        _ChkAlloc( info, sizeof(symrecinfo) );
        info->next = NULL;
        info->sym = sym;
        info->mod = CurrMod;
        RingAppend( head, info );
    }
}

extern void ProcUndefined( symbol *sym )
/***************************************/
{
    if( (LinkFlags & UNDEFS_ARE_OK) == 0 ) LinkState |= LINK_ERROR;
    AddSymRecList( sym, &UndefList );
}

extern void RecordTracedSym( symbol *sym )
/****************************************/
{
    if( sym->mod != CurrMod ) {
        AddSymRecList( sym, &SymTraceList );
    }
}

static void PrintUndefined( symrecinfo *info )
/********************************************/
{
    mod_entry * mod;

    mod = info->mod;
    LnkMsg( YELL+MSG_UNDEF_SYM, "12S", mod->f.source->file->name, mod->name,
                                       info->sym );
    WriteFormat( 0, "%S", info->sym );
    WriteFormat( 32, "%s(%s)", mod->f.source->file->name, mod->name );
    WriteMapNL( 1 );
}

static void PrintSymTrace( symrecinfo *info )
/*******************************************/
{
    LnkMsg( MAP+MSG_MOD_TRACE, "Ss", info->sym, info->mod->name );
}

extern void WriteUndefined( void )
/********************************/
{
    if( SymTraceList != NULL ) {
        WriteBox( MSG_MAP_BOX_TRACE_SYM );
        RingWalk( SymTraceList, PrintSymTrace );
        WriteMapNL( 1 );
    }
    if( UndefList != NULL ) {
        WriteBox( MSG_MAP_BOX_UNRES_REF );
        Msg_Write_Map( MSG_MAP_TITLE_UNRES_REF_0 );
        Msg_Write_Map( MSG_MAP_TITLE_UNRES_REF_1 );
        WriteMapNL( 1 );
        RingWalk( UndefList, PrintUndefined );
    }
    if( LinkState & UNDEFED_SYM_ERROR ) {
        LinkState |= LINK_ERROR;
    }
}

extern void FreeUndefs( void )
/****************************/
{
    RingFree( &SymTraceList );
    RingFree( &UndefList );
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

extern void WriteLibsUsed( void )
/*******************************/
{
    file_list * lib;
    char *      name;
    char *      path_ptr;
    char        new_name[ PATH_MAX ];

    if( LinkState & GENERATE_LIB_LIST ) {
        WriteBox( MSG_MAP_BOX_LIB_USED );
        for( lib = ObjLibFiles; lib != NULL; lib = lib->next_file ) {
            if( lib->status & STAT_LIB_USED ) {
                name = lib->file->name;
                if( lib->file->prefix != NULL ) {
                    path_ptr = lib->file->prefix;
                    QMakeFileName( &path_ptr, name, new_name );
                    name = new_name;
                }
                WriteMap( "%s", name );
            }
        }
        LinkState &= ~GENERATE_LIB_LIST;
    }
}

extern void MapSizes( void )
/**************************/
/*
  Write out code size to map file and print libraries used.
*/
{
    char        msg_buff[RESOURCE_MAX_SIZE];

    if( UndefList != NULL ) {
        WriteMapNL( 1 );
    }
    WriteImports();
    WriteLibsUsed();
    WriteBox( MSG_MAP_BOX_LINKER_STAT );
    Msg_Get( MSG_MAP_STACK_SIZE, msg_buff );
    Write32( msg_buff, StackSize );
    Msg_Get( MSG_MAP_MEM_SIZE, msg_buff );
    Write32( msg_buff, MemorySize() );
    if( FmtData.type & MK_OVERLAYS && FmtData.u.dos.dynamic ) {
        Msg_Get( MSG_MAP_OVL_SIZE, msg_buff );
        Write32( msg_buff, (unsigned long)AreaSize * 16 );
    }
    if( !(FmtData.type & MK_NOVELL) && (!FmtData.dll || FmtData.type & MK_PE)){
        Msg_Write_Map( MSG_MAP_ENTRY_PT_ADDR, &StartInfo.addr );
    }
}

extern void EndTime( void )
/*************************/
{
    char *      ptr;
    signed_16   h;
    signed_16   m;
    signed_16   s;
    signed_16   t;
    char        tim[ 11 + 1 ];

    if( MapFlags & MAP_FLAG ) {

        ClockTicks = clock() - ClockTicks;
        t = (unsigned_16)(ClockTicks % CLOCKS_PER_SEC);
        ClockTicks /= CLOCKS_PER_SEC;
        s = (unsigned_16)(ClockTicks % 60);
        ClockTicks /= 60;
        m = (unsigned_16)(ClockTicks % 60);
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
        Msg_Write_Map( MSG_MAP_LINK_TIME, tim );
    }
}

extern void WriteMapNL( unsigned count )
/**************************************/
{
    unsigned    len;

    if( MapFlags & MAP_FLAG ) {
        len = strlen( NLSeq );
        while( count != 0 ) {
            BufWrite( NLSeq, len );
            --count;
        }
        MapCol = 0;
    }
}

static unsigned MapPrint( char *str, va_list * args )
/***************************************************/
{
    char        buff[MAX_MSG_SIZE];
    unsigned    len;

    len = DoFmtStr( buff, MAX_MSG_SIZE, str, args );
    BufWrite( buff, len );
    return( len );
}

extern void DoWriteMap( char *format, va_list * arglist )
/*******************************************************/
{
    if( MapFlags & MAP_FLAG ) {
        MapPrint( format, arglist );
        WriteMapNL( 1 );
    }
}

extern void WriteMap( char *format, ... )
/***************************************/
{
    va_list arglist;

    va_start( arglist, format );
    DoWriteMap( format, &arglist );
}

extern void WriteFormat( int col, char *str, ... )
/************************************************/
{
    va_list         arglist;
    int             num;
    static  char    Blanks[]={"                                      "};

    if( MapFlags & MAP_FLAG ) {
        num = 0;
        if( col > MapCol ) {
            num = col - MapCol;
        } else if( MapCol != 0 ) {
            num = 1;
        }
        MapCol += num;
        BufWrite( Blanks, num );
        va_start( arglist, str );
        MapCol += MapPrint( str, &arglist );
    }
}

extern void BufWrite( char *buffer, int len )
/*******************************************/
// write to the map file, buffering the write if buffering is on.
{
    int     diff;

    if( Buffering ) {
        diff = BufferSize + len - TokSize;
        if( diff >= 0 ) {
            memcpy( TokBuff+BufferSize, buffer, len - diff );
            QWrite( MapFile, TokBuff, TokSize, MapFName );
            BufferSize = diff;
            if( diff > 0 ) {
                memcpy( TokBuff, buffer + len - diff, diff );
            }
        } else {
            memcpy( TokBuff+BufferSize, buffer, len );
            BufferSize += len;
        }
    } else {
        QWrite( MapFile, buffer, len, MapFName );
    }
}
