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
 *  PERMDATA:  routines for making some linker data permanent
 *
 */

#include <string.h>
#include <stdio.h>
#include "linkstd.h"
#include "strtab.h"
#include "dbgcomm.h"
#include "dbgall.h"
#include "carve.h"
#include "alloc.h"
#include "command.h"
#include "reloc.h"
#include "fileio.h"
#include "virtmem.h"
#include "impexp.h"
#include "loadfile.h"
#include "msg.h"
#include "objio.h"
#include "ring.h"
#include "wlnkmsg.h"
#include "objcalc.h"
#include "permdata.h"

stringtable             PermStrings;
carve_t                 CarveLeader;
carve_t                 CarveModEntry;
carve_t                 CarveSymbol;
carve_t                 CarveSegData;
carve_t                 CarveClass;
carve_t                 CarveGroup;
carve_t                 CarveDLLInfo;
carve_t                 CarveExportInfo;
char *                  IncFileName;
incgroupdef *           IncGroupDefs;
group_entry **          IncGroups;
libnamelist *           SavedUserLibs;
libnamelist *           SavedDefLibs;

static stringtable      StoredRelocs;
static char *           ReadRelocs;
static unsigned         SizeRelocs;
static char *           OldExe;
static char *           OldSymFile;
static char *           AltDefData;
static char *           IncStrTab;

#define SEG_CARVE_SIZE          (2*1024)
#define MOD_CARVE_SIZE          (5*1024)
#define SDATA_CARVE_SIZE        (16*1024)
#define SYM_CARVE_SIZE          (32*1024)

extern void ResetPermData( void )
/******************************/
{
    IncFileName = NULL;
    IncStrTab = NULL;
    ReadRelocs = NULL;
    OldExe = NULL;
    AltDefData = NULL;
    OldSymFile = NULL;
    IncGroupDefs = NULL;
    IncGroups = NULL;
    SavedUserLibs = NULL;
    SavedDefLibs = NULL;
    CarveClass = CarveCreate( sizeof(class_entry), 20 * sizeof(class_entry) );
    CarveGroup = CarveCreate( sizeof(group_entry), 20 * sizeof(group_entry) );
    CarveDLLInfo = CarveCreate( sizeof(dll_sym_info), 100*sizeof(dll_sym_info));
    CarveExportInfo = CarveCreate(sizeof(entry_export),20*sizeof(entry_export));
    CarveLeader = CarveCreate( sizeof(seg_leader), SEG_CARVE_SIZE );
    CarveModEntry = CarveCreate( sizeof(mod_entry), MOD_CARVE_SIZE );
    CarveSegData = CarveCreate( sizeof(segdata), SDATA_CARVE_SIZE );
    CarveSymbol = CarveCreate( sizeof(symbol), SYM_CARVE_SIZE );
    InitStringTable( &PermStrings, TRUE );
    InitStringTable( &StoredRelocs, FALSE );
}

typedef struct {
    stringtable strtab;
    unsigned    currpos;
    f_handle    incfhdl;
    void        (*prepfn)( void *, void * );
} perm_write_info;


static void MarkDLLInfo( dll_sym_info *dll )
/******************************************/
{
    dll->isfree = TRUE;
}

static void MarkExportInfo( entry_export *exp )
/*********************************************/
{
    exp->isfree = TRUE;
}

static void MarkModEntry( mod_entry *mod )
/****************************************/
{
    mod->modinfo |= MOD_IS_FREE;
}

static void MarkSegData( segdata *sdata )
/***************************************/
{
    sdata->isfree = TRUE;
}

static void MarkSymbol( symbol *sym )
/***********************************/
{
    sym->info |= SYM_IS_FREE;
}

static void * GetString( perm_write_info *info, char *str )
/*********************************************************/
{
    unsigned idx;

    idx = GetStringTableSize( &info->strtab );
    StringStringTable( &info->strtab, str );
    return (void *)idx;
}

static bool WriteLeaderName( seg_leader *leader, perm_write_info *info )
/**********************************************************************/
{
    BufWritePermFile( info, &leader->class->name, sizeof(unsigned_32) );
    BufWritePermFile( info, &leader->segname, sizeof(unsigned_32) );
    return FALSE;
}

static unsigned WriteGroups( perm_write_info *info )
/**************************************************/
{
    group_entry *group;
    unsigned    num;
    unsigned_32 count;

    num = 0;
    for( group = Groups; group != NULL; group = group->next_group ) {
        if( !group->isautogrp && group->leaders != NULL ) {
            num++;
            count = Ring2Count( group->leaders );
            BufWritePermFile( info, &count, sizeof(unsigned_32) );
            group->sym->name = GetString( info, group->sym->name );
            BufWritePermFile( info, &group->sym->name, sizeof(unsigned_32) );
            Ring2Lookup( group->leaders, WriteLeaderName, info );
        }
    }
    return num;
}

static bool CheckFree( bool isfree, perm_write_info *info )
/*********************************************************/
{
    unsigned_32 dummy;

    if( isfree ) {
        dummy = CARVE_INVALID_INDEX;
        BufWritePermFile( info, &dummy, sizeof(unsigned_32) );
    }
    return isfree;
}

static void WriteDLLInfo( dll_sym_info *dll, perm_write_info *info )
/******************************************************************/
{
    if( !CheckFree( dll->isfree, info ) ) {
        dll->m.modname = dll->m.modnum->name;
        if( !dll->isordinal ) {
            dll->u.entname = dll->u.entry->name;
        }
        BufWritePermFile( info, dll, offsetof(dll_sym_info, iatsym) );
    }
}

static void WriteExportInfo( entry_export *exp, perm_write_info *info )
/*********************************************************************/
{
    if( !CheckFree( exp->isfree, info ) ) {
        exp->next = CarveGetIndex( CarveExportInfo, exp->next );
        if( exp->name != NULL ) {
            exp->name = GetString( info, exp->name );
        }
        BufWritePermFile( info, exp, offsetof(entry_export, sym) );
    }
}

static void FixSymAddr( symbol *sym )
/***********************************/
{
    if( !IS_SYM_IMPORTED(sym) && !(sym->info & SYM_DEAD) && sym->addr.off > 0
                                                     && sym->p.seg != NULL ) {
        sym->addr.off -= sym->p.seg->u.leader->seg_addr.off;
        sym->addr.off -= sym->p.seg->a.delta;
    }
}

static void PrepModEntry( mod_entry *mod, perm_write_info *info )
/***************************************************************/
{
    if( mod->modinfo & MOD_IS_FREE ) {
        *((unsigned_32 *)mod) = CARVE_INVALID_INDEX;
        return;
    }
    Ring2Walk( mod->publist, FixSymAddr );
    mod->n.next_mod = CarveGetIndex( CarveModEntry, mod->n.next_mod );
    mod->name = GetString( info, mod->name );
    mod->publist = CarveGetIndex( CarveSymbol, mod->publist );
    mod->segs = CarveGetIndex( CarveSegData, mod->segs );
    mod->modinfo &= ~MOD_CLEAR_ON_INC;
    if( mod->f.source != NULL ) {
        mod->f.fname = mod->f.source->file->name;
    }
}

static void PrepSegData( segdata *sdata, perm_write_info *info )
/**************************************************************/
{
    info = info;
    if( sdata->isfree ) {
        *((unsigned_32 *)sdata) = CARVE_INVALID_INDEX;
        return;
    }
    sdata->next = CarveGetIndex( CarveSegData, sdata->next );  // not used
    sdata->mod_next = CarveGetIndex( CarveSegData, sdata->mod_next );
    if( !sdata->isdead || sdata->iscdat ) {
        sdata->o.clname = sdata->u.leader->class->name;
        sdata->u.name = sdata->u.leader->segname;
    }
}

static void PrepSymbol( symbol *sym, perm_write_info *info )
/**********************************************************/
{
    char *      save;
    symbol *    mainsym;

    if( sym->info & SYM_IS_FREE ) {
        *((unsigned_32 *)sym) = CARVE_INVALID_INDEX;
        return;
    }
    sym->hash = CarveGetIndex( CarveSymbol, sym->hash );
    sym->link = CarveGetIndex( CarveSymbol, sym->link );
    sym->publink = CarveGetIndex( CarveSymbol, sym->publink );
    if( sym->info & SYM_IS_ALTDEF ) {
        mainsym = sym->e.mainsym;
        if( !(mainsym->info & SYM_NAME_XLATED) ) {
            mainsym->name = GetString( info, mainsym->name );
            mainsym->info |= SYM_NAME_XLATED;
        }
        sym->name = mainsym->name;
    } else if( !(sym->info & SYM_NAME_XLATED) ) {
        sym->name = GetString( info, sym->name );
        sym->info |= SYM_NAME_XLATED;
    }
    sym->mod = CarveGetIndex( CarveModEntry, sym->mod );
    if( IS_SYM_ALIAS( sym ) ) {
        save = sym->p.alias;
        sym->p.alias = GetString( info, sym->p.alias );
        if( sym->info & SYM_FREE_ALIAS ) {
            _LnkFree( save );
        }
    } else if( IS_SYM_IMPORTED(sym) ) {
        if( FmtData.type & (MK_OS2 | MK_PE) ) {
            sym->p.import = CarveGetIndex( CarveDLLInfo, sym->p.import );
        }
    } else if( !(sym->info & SYM_IS_ALTDEF) || IS_SYM_COMDAT(sym) ) {
        sym->p.seg = CarveGetIndex( CarveSegData, sym->p.seg );
        sym->u.altdefs = CarveGetIndex( CarveSymbol, sym->u.altdefs );
    }
    if( sym->info & SYM_EXPORTED ) {
        if( FmtData.type & (MK_OS2 | MK_PE) ) {
            sym->e.export = CarveGetIndex( CarveExportInfo, sym->e.export );
        }
    } else if( sym->e.def != NULL ) {
        sym->e.def = CarveGetIndex( CarveSymbol, sym->e.def );
    }
}

static void PrepNameTable( name_list *list, perm_write_info *info )
/*****************************************************************/
{
    char *      savename;

    while( list != NULL ) {
        savename = list->name;
        list->name = (char *) GetStringTableSize( &info->strtab );
        AddStringTable( &info->strtab, savename, list->len + 1 );
        list = list->next;
    }
}

static void PrepFileList( perm_write_info *info )
/***********************************************/
{
    infilelist *list;
    char *      path_ptr;
    char *      name;
    char        new_name[ PATH_MAX ];

    for( list = CachedFiles; list != NULL; list = list->next ) {
        if( list->prefix != NULL ) {
            path_ptr = list->prefix;
            QMakeFileName( &path_ptr, list->name, new_name );
            name = new_name;
        } else {
            name = list->name;
        }
        list->name = GetString( info, name );
    }
}

static bool PrepLeaders( seg_leader *leader, perm_write_info *info )
/******************************************************************/
{
    leader->segname = GetString( info, leader->segname );
    return FALSE;
}

static void PrepClasses( perm_write_info *info )
/**********************************************/
{
    class_entry *class;

    for( class = Root->classlist; class != NULL; class = class->next_class ) {
        class->name = GetString( info, class->name );
        RingLookup( class->segs, PrepLeaders, info );
    }
}

static void VMemWritePermFile( perm_write_info *info, virt_mem data,
                               unsigned len )
/******************************************************************/
{
    DoWritePermFile( info, (void *) data, len, TRUE );
}

static void BufWritePermFile( perm_write_info *info, void *data, unsigned len )
/*****************************************************************************/
{
    DoWritePermFile( info, data, len, FALSE );
}

static void DoWritePermFile( perm_write_info *info, char *data, unsigned len,
                             bool isvmem )
/***************************************************************************/
{
    unsigned modpos;
    unsigned adjust;

    modpos = info->currpos % MAX_HEADROOM;
    info->currpos += len;
    while( modpos + len >= MAX_HEADROOM ) {
        adjust = MAX_HEADROOM - modpos;
        if( !isvmem ) {
            memcpy( TokBuff + modpos, data, adjust );
        } else {
            ReadInfo( (virt_mem) data, TokBuff + modpos, adjust );
        }
        QWrite( info->incfhdl, TokBuff, MAX_HEADROOM, IncFileName );
        data += adjust;
        len -= adjust;
        modpos = 0;
    }
    if( len > 0 ) {
        if( !isvmem ) {
            memcpy( TokBuff + modpos, data, len );
        } else {
            ReadInfo( (virt_mem) data, TokBuff + modpos, len );
        }
    }
}

extern void WritePermFile( perm_write_info *info, void *data, unsigned len )
/**************************************************************************/
{
    QWrite( info->incfhdl, data, len, IncFileName );
}

static void FlushPermBuf( perm_write_info *info )
/***********************************************/
{
    unsigned    modpos;
    unsigned    adjust;

    modpos = info->currpos % MAX_HEADROOM;
    if( modpos == 0 ) return;
    adjust = SECTOR_SIZE - (info->currpos % SECTOR_SIZE);
    if( adjust != SECTOR_SIZE ) {
        memset( TokBuff + modpos, 0, adjust );
        info->currpos += adjust;
        modpos += adjust;
    }
    QWrite( info->incfhdl, TokBuff, modpos, IncFileName );
}

static void WriteStringBlock( perm_write_info *info, char *data, unsigned size )
/******************************************************************************/
{
    QWrite( info->incfhdl, data, size, IncFileName );
}

static void FiniStringBlock( stringtable *tab, unsigned *size, void *info,
                             void (*writefn)(void *,char *,unsigned) )
/************************************************************************/
{
    unsigned    rawsize;

    rawsize = GetStringTableSize( tab );
    *size = ROUND_UP( rawsize, SECTOR_SIZE );
    if( *size != rawsize ) {
        ZeroStringTable( tab, *size - rawsize );
    }
    WriteStringTable( tab, writefn, info );
    FiniStringTable( tab );
}

static void DumpBlock( carve_t carver, void *block, perm_write_info *info )
/*************************************************************************/
{
    unsigned    size;

    size = CarveBlockSize( carver );
    CarveBlockScan( carver, block, info->prepfn, info );
    QWrite( info->incfhdl, CarveBlockData( block ), size, IncFileName );
}

static void DumpBlockStruct( carve_t carver, void (*markfn)(void *),
                             carve_info *loc, perm_write_info *info )
/*******************************************************************/
{
    CarveWalkAllFree( carver, markfn );
    loc->num = CarveNumElements( carver );
    CarveWalkBlocks( carver, DumpBlock, info );
}

static unsigned WriteSmallCarve( carve_t carver, void (*markfn)(void *),
                                 void (*writefn)(void *, void *),
                                 perm_write_info *info )
/**************************************************************************/
{
    CarveWalkAllFree( carver, markfn );
    CarveWalkAll( carver, writefn, info );
    return CarveNumElements( carver );
}

static void PrepStartValue( inc_file_header *hdr )
/************************************************/
{
    if( StartInfo.mod != NULL && !StartInfo.user_specd ) {
        hdr->startmodidx = (unsigned_32) CarveGetIndex( CarveModEntry,
                                                        StartInfo.mod );
        if( StartInfo.type == START_IS_SDATA ) {
            hdr->flags |= INC_FLAG_START_SEG;
            hdr->startidx = (unsigned_32) CarveGetIndex( CarveSegData,
                                                        StartInfo.targ.sdata );
        } else {
            DbgAssert( StartInfo.type == START_IS_SYM );
            hdr->startidx = (unsigned_32) CarveGetIndex( CarveSymbol,
                                                        StartInfo.targ.sym );
        }
        hdr->startoff = StartInfo.off;
    } else {
        hdr->startmodidx = 0;
    }
}

static void WriteAltData( perm_write_info *info )
/***********************************************/
{
    symbol *    sym;
    unsigned    savepos;

    info->currpos = 0;
    for( sym = HeadSym; sym != NULL; sym = sym->link ) {
        if( sym->info & SYM_IS_ALTDEF && sym->info & SYM_HAS_DATA ) {
            savepos = info->currpos;
            VMemWritePermFile( info, sym->p.seg->data, sym->p.seg->length );
            sym->p.seg->data = savepos;
        }
    }
    FlushPermBuf( info );
}

static unsigned_32 WriteLibList( perm_write_info *info, bool douser )
/*******************************************************************/
{
    unsigned_32 numlibs;
    file_list * file;
    void *      data;

    numlibs = 0;
    for( file = ObjLibFiles; file != NULL; file = file->next_file ) {
        if( !(((file->status & STAT_USER_SPECD) != 0) ^ douser) ) {
            data = GetString( info, file->file->name );
            BufWritePermFile( info, &data, sizeof(void *) );
            numlibs++;
        }
    }
    return numlibs;
}

extern void WritePermData( void )
/*******************************/
{
    inc_file_header     hdr;
    perm_write_info     info;

    if( !(LinkFlags & INC_LINK_FLAG) || LinkState & LINK_ERROR ) return;
    InitStringTable( &info.strtab, FALSE );
    CharStringTable( &info.strtab, '\0' );      // make 0 idx not valid
    info.incfhdl = QOpenRW( IncFileName );
    hdr.flags = 0;
    hdr.exename = (unsigned_32) GetString( &info, Root->outfile->fname );
    QModTime( Root->outfile->fname, &hdr.exemodtime );
    if( SymFileName != NULL ) {
        hdr.symname = (unsigned_32) GetString( &info, SymFileName );
        QModTime( SymFileName, &hdr.symmodtime );
    } else {
        hdr.symname = 0;
    }
    info.currpos = 0;
    BufWritePermFile( &info, &hdr, sizeof(inc_file_header) ); // reserve space
    PrepClasses( &info );
    hdr.numgroups = WriteGroups( &info );
    hdr.numuserlibs = WriteLibList( &info, TRUE );
    hdr.numdeflibs = WriteLibList( &info, FALSE );
    if( FmtData.type & (MK_OS2 | MK_PE) ) {
        PrepNameTable( FmtData.u.os2.mod_ref_list, &info );
        PrepNameTable( FmtData.u.os2.imp_tab_list, &info );
        hdr.numdllsyms = WriteSmallCarve( CarveDLLInfo, MarkDLLInfo,
                                          WriteDLLInfo, &info );
        hdr.numexports = WriteSmallCarve( CarveExportInfo, MarkExportInfo,
                                          WriteExportInfo, &info );
    }
    FlushPermBuf( &info );
    WriteHashPointers( &info );
    hdr.hdrsize = info.currpos;
    WriteAltData( &info );
    hdr.altdefsize = info.currpos;
    PrepFileList( &info );
    info.prepfn = PrepModEntry;
    DumpBlockStruct( CarveModEntry, MarkModEntry, &hdr.mods, &info );
    info.prepfn = PrepSegData;
    DumpBlockStruct( CarveSegData, MarkSegData, &hdr.segdatas, &info );
    info.prepfn = PrepSymbol;
    DumpBlockStruct( CarveSymbol, MarkSymbol, &hdr.symbols, &info );
    FiniStringBlock( &info.strtab, (unsigned *) &hdr.strtabsize, &info, WriteStringBlock );
    QWrite( info.incfhdl, ReadRelocs, SizeRelocs, IncFileName );
    memcpy( hdr.signature, INC_FILE_SIG, INC_FILE_SIG_SIZE );
    hdr.rootmodidx = (unsigned_32) CarveGetIndex( CarveModEntry, Root->mods );
    hdr.headsymidx = (unsigned_32) CarveGetIndex( CarveSymbol, HeadSym );
    hdr.libmodidx = (unsigned_32) CarveGetIndex( CarveModEntry, LibModules );
    hdr.linkstate = LinkState & ~CLEAR_ON_INC;
    hdr.relocsize = SizeRelocs;
    PrepStartValue( &hdr );
    QSeek( info.incfhdl, 0, IncFileName );
    QWrite( info.incfhdl, &hdr, sizeof(inc_file_header), IncFileName );
    QClose( info.incfhdl, IncFileName );
}

typedef struct {
    f_handle    incfhdl;
    unsigned    currpos;
    char *      buffer;
    unsigned    num;
    void        (*cbfn)(void *, void *);
    carve_t     cv;
} perm_read_info;

extern void ReadPermFile( perm_read_info *info, void *data, unsigned len )
/************************************************************************/
{
    QRead( info->incfhdl, data, len, IncFileName );
}

static unsigned_32 BufPeekU32( perm_read_info *info )
/***************************************************/
{
    return *((unsigned_32 *)(info->buffer + info->currpos));
}

static unsigned_32 BufReadU32( perm_read_info *info )
/***************************************************/
{
    unsigned_32 retval;

    retval = *((unsigned_32 *)(info->buffer + info->currpos));
    info->currpos += sizeof(unsigned_32);
    return retval;
}

static void BufRead( perm_read_info *info, void *data, unsigned len )
/*******************************************************************/
{
    memcpy( data, info->buffer + info->currpos, len );
    info->currpos += len;
}

static char * MapString( char *off )
/**********************************/
{
    if( off == 0 ) return NULL;
    return IncStrTab + (unsigned)off;
}

static void ReadGroups( unsigned count, perm_read_info *info )
/************************************************************/
{
    incgroupdef *       def;
    unsigned_32         size;

    while( count > 0 ) {
        size = BufReadU32( info );
        _ChkAlloc( def, sizeof(incgroupdef) + size * 2 * sizeof(char *) );
        RingAppend( &IncGroupDefs, def );
        def->numsegs = size;
        size *= 2;
        BufRead( info, def->names, (size + 1) * sizeof(char *) );
        for(;;) {
            def->names[size] = MapString( def->names[size] );
            if( size == 0 ) break;
            size--;
        }
        count--;
    }
}

static void ReadLibList( unsigned count, libnamelist **head,
                         perm_read_info *info )
/**********************************************************/
{
    libnamelist *       list;
    char *              name;
    unsigned            namelen;
    unsigned_32         nameidx;

    while( count > 0 ) {
        nameidx = BufReadU32( info );
        name = MapString( (char *) nameidx );
        namelen = strlen( name );
        _ChkAlloc( list, sizeof(libnamelist) + namelen );
        memcpy( list->name, name, namelen + 1 );
        list->namelen = namelen;
        LinkList( head, list );
        count--;
    }
}

static void RebuildDLLInfo( dll_sym_info *dll, perm_read_info *info )
/*******************************************************************/
{
    BufRead( info, dll, offsetof(dll_sym_info, iatsym) );
    dll->m.modname = MapString( dll->m.modname );
    if( !dll->isordinal ) {
        dll->u.entname = MapString( dll->u.entname );
    }
}

static void RebuildExportInfo( entry_export *exp, perm_read_info *info )
/**********************************************************************/
{
    BufRead( info, exp, offsetof(entry_export, sym) );
    exp->next = CarveMapIndex( CarveExportInfo, exp->next );
    if( exp->name != NULL ) {
        exp->name = MapString( exp->name );
    }
    exp->impname = NULL;
}

static void RebuildModEntry( mod_entry *mod, perm_read_info *info )
/*****************************************************************/
{
    info = info;
    if( *((unsigned_32 *)mod) == CARVE_INVALID_INDEX ) {
        CarveInsertFree( CarveModEntry, mod );
        return;
    }
    mod->n.next_mod = CarveMapIndex( CarveModEntry, mod->n.next_mod );
    mod->name = MapString( mod->name );
    mod->publist = CarveMapIndex( CarveSymbol, mod->publist );
    mod->segs = CarveMapIndex( CarveSegData, mod->segs );
    mod->f.fname = MapString( mod->f.fname );
}

static void RebuildSegData( segdata *sdata, perm_read_info *info )
/****************************************************************/
{
    info = info;
    if( *((unsigned_32 *)sdata) == CARVE_INVALID_INDEX ) {
        CarveInsertFree( CarveSegData, sdata );
        return;
    }
    sdata->next = CarveMapIndex( CarveSegData, sdata->next );  // dont use this?
    sdata->mod_next = CarveMapIndex( CarveSegData, sdata->mod_next );
    if( !sdata->isdead || sdata->iscdat ) {
        sdata->u.name = MapString( sdata->u.name );
        sdata->o.clname = MapString( sdata->o.clname );
    }
}

static void RebuildSymbol( symbol *sym, perm_read_info *info )
/************************************************************/
{
    info = info;
    if( *((unsigned_32 *)sym) == CARVE_INVALID_INDEX ) {
        CarveInsertFree( CarveSymbol, sym );
        return;
    }
    sym->hash = CarveMapIndex( CarveSymbol, sym->hash );
    sym->link = CarveMapIndex( CarveSymbol, sym->link );
    sym->publink = CarveMapIndex( CarveSymbol, sym->publink );
    sym->name = MapString( sym->name );
    sym->info &= ~SYM_CLEAR_ON_INC;
    sym->mod = CarveMapIndex( CarveModEntry, sym->mod );
    if( IS_SYM_ALIAS( sym ) ) {
        sym->p.alias = MapString( sym->p.alias );
    } else if( IS_SYM_IMPORTED(sym) ) {
        if( FmtData.type & (MK_OS2 | MK_PE) ) {
            sym->p.import = CarveMapIndex( CarveDLLInfo, sym->p.import );
        }
    } else if( !(sym->info & SYM_IS_ALTDEF) || IS_SYM_COMDAT(sym) ) {
        sym->p.seg = CarveMapIndex( CarveSegData, sym->p.seg );
        sym->u.altdefs = CarveMapIndex( CarveSymbol, sym->u.altdefs );
    }
    if( sym->info & SYM_EXPORTED ) {
        if( FmtData.type & (MK_OS2 | MK_PE) ) {
            sym->e.export = CarveMapIndex( CarveExportInfo, sym->e.export );
        }
    } else if( sym->e.def != NULL ) {
        sym->e.def = CarveMapIndex( CarveSymbol, sym->e.def );
    }
}

static void ReadBlockInfo( carve_t cv, void *blk, perm_read_info *info )
/**********************************************************************/
{
    QRead( info->incfhdl, CarveBlockData(blk), CarveBlockSize(cv), IncFileName);
}

static void SmallFreeCheck( void *data, perm_read_info *info )
/************************************************************/
{
    unsigned_32 freeblk;

    if( info->num > 0 ) {
        info->num--;
        freeblk = BufPeekU32( info );
        if( freeblk == CARVE_INVALID_INDEX ) {
            info->currpos += sizeof(unsigned_32);
            CarveInsertFree( info->cv, data );
        } else {
            info->cbfn( data, info );
        }
    }
}

static void RebuildSmallCarve( carve_t cv, unsigned num,
                               void (*fn)(void *,perm_read_info *),
                               perm_read_info * info )
/*****************************************************************/
{
    info->cv = cv;
    info->num = num;
    info->cbfn = fn;
    CarveWalkAll( cv, SmallFreeCheck, info );
}

static void PurgeRead( perm_read_info *info )
/*******************************************/
{
    ClearHashPointers();
    CarvePurge( CarveGroup );
    CarvePurge( CarveModEntry );
    CarvePurge( CarveSegData );
    CarvePurge( CarveSymbol );
    if( FmtData.type & (MK_OS2 | MK_PE) ) {
        CarvePurge( CarveDLLInfo );
        CarvePurge( CarveExportInfo );
    }
    _LnkFree( info->buffer );
    _LnkFree( IncStrTab );
    _LnkFree( ReadRelocs );
    _LnkFree( AltDefData );
    IncStrTab = NULL;
    ReadRelocs = NULL;
    AltDefData = NULL;
    if( OldExe != NULL ) {
        _LnkFree( OldExe );
        OldExe = NULL;
    }
}

static void ReadBinary( char **buf, unsigned_32 nameidx, time_t modtime )
/***********************************************************************/
{
    char *              fname;
    f_handle            hdl;
    unsigned long       size;

    fname = MapString( (char *) nameidx );
    hdl = QObjOpen( fname );
    if( hdl == NIL_HANDLE ) {
        return;
    }
    if( QFModTime(hdl) != modtime ) {
        LnkMsg( WRN+MSG_EXE_DATE_CHANGED, "s", fname );
        return;
    }
    size = QFileSize( hdl );
    _ChkAlloc( *buf, size );
    QRead( hdl, *buf, size, fname );
    QClose( hdl, fname );
}

static void ReadStartInfo( inc_file_header *hdr )
/***********************************************/
{
    if( hdr->startmodidx != 0 ) {
        StartInfo.from_inc = TRUE;
        StartInfo.mod = CarveMapIndex( CarveModEntry, (void *)hdr->startmodidx);
        if( hdr->flags & INC_FLAG_START_SEG ) {
            StartInfo.type = START_IS_SDATA;
            StartInfo.targ.sdata = CarveMapIndex( CarveSegData,
                                                    (void *)hdr->startidx );
        } else {
            StartInfo.type = START_IS_SYM;
            StartInfo.targ.sym = CarveMapIndex( CarveSymbol,
                                                    (void *)hdr->startidx );
        }
        StartInfo.off = hdr->startoff;
    }
}

extern void ReadPermData( void )
/******************************/
{
    perm_read_info      info;
    inc_file_header *   hdr;

    info.incfhdl = QObjOpen( IncFileName );
    if( info.incfhdl == NIL_HANDLE ) return;
    _ChkAlloc( info.buffer, SECTOR_SIZE );
    QRead( info.incfhdl, info.buffer, SECTOR_SIZE, IncFileName );
    hdr = (inc_file_header *) info.buffer;
    if( memcmp( hdr->signature, INC_FILE_SIG, INC_FILE_SIG_SIZE ) != 0 ) {
        LnkMsg( WRN+MSG_INV_INC_FILE, NULL );
        _LnkFree( info.buffer );
        QClose( info.incfhdl, IncFileName );
        return;
    }
    if( hdr->hdrsize > SECTOR_SIZE ) {
        _LnkReAlloc( info.buffer, info.buffer, hdr->hdrsize );
        hdr = (inc_file_header *) info.buffer;  /* in case realloc moved it*/
        QRead( info.incfhdl, info.buffer + SECTOR_SIZE,
                        hdr->hdrsize - SECTOR_SIZE, IncFileName );
    }
    info.currpos = sizeof(inc_file_header);
    CarveRestart( CarveModEntry, hdr->mods.num );
    CarveRestart( CarveSegData, hdr->segdatas.num );
    CarveRestart( CarveSymbol, hdr->symbols.num );
    if( FmtData.type & (MK_OS2 | MK_PE) ) {
        CarveRestart( CarveDLLInfo, hdr->numdllsyms );
        CarveRestart( CarveExportInfo, hdr->numexports );
    }
    ReadHashPointers( &info );
    if( hdr->altdefsize > 0 ) {
        _ChkAlloc( AltDefData, hdr->altdefsize );
        QRead( info.incfhdl, AltDefData, hdr->altdefsize, IncFileName );
    }
    CarveWalkBlocks( CarveModEntry, ReadBlockInfo, &info );
    CarveWalkBlocks( CarveSegData, ReadBlockInfo, &info );
    CarveWalkBlocks( CarveSymbol, ReadBlockInfo, &info );
    _ChkAlloc( IncStrTab, hdr->strtabsize );
    QRead( info.incfhdl, IncStrTab, hdr->strtabsize, IncFileName );
    _ChkAlloc( ReadRelocs, hdr->relocsize );
    QRead( info.incfhdl, ReadRelocs, hdr->relocsize, IncFileName );
    QClose( info.incfhdl, IncFileName );
    ReadBinary( &OldExe, hdr->exename, hdr->exemodtime );
    if( OldExe == NULL ) {
        PurgeRead( &info );
        return;
    }
    if( hdr->symname != 0 ) {
        ReadBinary( &OldSymFile, hdr->symname, hdr->symmodtime );
        if( OldSymFile == NULL ) {
            PurgeRead( &info );
            return;
        }
    }
    ReadGroups( hdr->numgroups, &info );
    ReadLibList( hdr->numuserlibs, &SavedUserLibs, &info );
    ReadLibList( hdr->numdeflibs, &SavedDefLibs, &info );
    if( FmtData.type & (MK_OS2 | MK_PE) ) {
        RebuildSmallCarve(CarveDLLInfo, hdr->numdllsyms, RebuildDLLInfo, &info);
        RebuildSmallCarve( CarveExportInfo, hdr->numexports, RebuildExportInfo,
                                &info );
    }
    CarveWalkAll( CarveModEntry, RebuildModEntry, &info );
    CarveWalkAll( CarveSegData, RebuildSegData, &info );
    CarveWalkAll( CarveSymbol, RebuildSymbol, &info );
    Root->mods = CarveMapIndex( CarveModEntry, (void *) hdr->rootmodidx );
    HeadSym = CarveMapIndex( CarveSymbol, (void *) hdr->headsymidx );
    LibModules = CarveMapIndex( CarveModEntry, (void *) hdr->libmodidx );
    LinkState = hdr->linkstate | GOT_PREV_STRUCTS | (LinkState & CLEAR_ON_INC);
    ReadStartInfo( hdr );
    _LnkFree( info.buffer );
}

extern void PermSaveFixup( void *fix, unsigned size )
/***************************************************/
{
    AddStringTable( &StoredRelocs, (char *) fix, size );
}

extern void IterateModRelocs( unsigned offset, unsigned sizeleft,
                              unsigned (*fn)(void *) )
/***************************************************************/
{
    char *      fixoff;
    unsigned    size;

    fixoff = ReadRelocs + offset;
    while( sizeleft > 0 ) {
        size = fn( fixoff );
        sizeleft -= size;
        fixoff += size;
    }
}

static void SaveRelocData( char **curr, char *data, unsigned size )
/*****************************************************************/
{
    if( ReadRelocs == NULL && SizeRelocs > 0 ) {
        _ChkAlloc( ReadRelocs, SizeRelocs );
        *curr = ReadRelocs;
    }
    memcpy( *curr, data, size );
    *curr += size;
}

extern void IncP2Start( void )
/****************************/
{
    char * spare;

    _LnkFree( ReadRelocs );
    ReadRelocs = NULL;
    _LnkFree( OldExe );
    OldExe = NULL;
    _LnkFree( OldSymFile );
    OldSymFile = NULL;
    _LnkFree( AltDefData );
    AltDefData = NULL;
    FiniStringBlock( &StoredRelocs, &SizeRelocs, &spare, SaveRelocData );
}

extern void PermStartMod( mod_entry *mod )
/****************************************/
{
    mod->relocs = GetStringTableSize( &StoredRelocs );
}

extern void PermEndMod( mod_entry *mod )
/**************************************/
{
    mod->sizerelocs = GetStringTableSize( &StoredRelocs ) - mod->relocs;
}

extern void * GetSegContents( segdata *sdata, virt_mem off )
/**********************************************************/
{
    if( OldSymFile != NULL && sdata->u.leader->dbgtype != NOT_DEBUGGING_INFO ) {
        return OldSymFile + off;
    }
    return OldExe + off;
}

extern void * GetAltdefContents( segdata *sdata )
/***********************************************/
{
    return AltDefData + sdata->data;
}

extern void FreeSavedRelocs( void )
/*********************************/
{
    if( !(LinkFlags & INC_LINK_FLAG) ) {
        _LnkFree( ReadRelocs );
        ReadRelocs = NULL;
    }
}

extern void CleanPermData( void )
/*******************************/
{
#ifndef NDEBUG
    if( !(LinkFlags & INC_LINK_FLAG) ) {
        CarveVerifyAllGone( CarveLeader, "seg_leader" );
        CarveVerifyAllGone( CarveModEntry, "mod_entry" );
        CarveVerifyAllGone( CarveDLLInfo, "dll_sym_info" );
        CarveVerifyAllGone( CarveExportInfo, "entry_export" );
        CarveVerifyAllGone( CarveSymbol, "symbol" );
        CarveVerifyAllGone( CarveSegData, "segdata" );
        CarveVerifyAllGone( CarveClass, "class_entry" );
        CarveVerifyAllGone( CarveGroup, "group_entry" );
    }
#endif
    if( LinkState & LINK_ERROR ) {
        QDelete( IncFileName );
    }
    CarveDestroy( CarveLeader );
    CarveDestroy( CarveModEntry );
    CarveDestroy( CarveDLLInfo );
    CarveDestroy( CarveExportInfo );
    CarveDestroy( CarveSymbol );
    CarveDestroy( CarveSegData );
    CarveDestroy( CarveClass );
    CarveDestroy( CarveGroup );
    FiniStringTable( &PermStrings );
    FiniStringTable( &StoredRelocs );
    _LnkFree( IncFileName );
    _LnkFree( IncStrTab );
    _LnkFree( ReadRelocs );
    _LnkFree( OldExe );
    _LnkFree( OldSymFile );
    _LnkFree( AltDefData );
    RingFree( &IncGroupDefs );
    _LnkFree( IncGroups );
    FreeList( SavedUserLibs );
    FreeList( SavedDefLibs );
}
