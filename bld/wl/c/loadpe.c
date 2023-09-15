/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Utilities for creation of PE (Win32) executable files.
*
****************************************************************************/


#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>
#include "linkstd.h"
#include "exeos2.h"
#include "loados2.h"
#include "exepe.h"
#include "exedos.h"
#include "reloc.h"
#include "specials.h"
#include "pcobj.h"
#include "msg.h"
#include "wlnkmsg.h"
#include "virtmem.h"
#include "objnode.h"
#include "loadfile.h"
#include "objcalc.h"
#include "fileio.h"
#include "dbgcomm.h"
#include "dbgall.h"
#include "dbgcv.h"
#include "objpass1.h"
#include "ring.h"
#include "strtab.h"
#include "carve.h"
#include "permdata.h"
#include "loadpe.h"
#include "reserr.h"
#include "impexp.h"
#include "toc.h"
#include "objstrip.h"
#include "wressetr.h"
#include "rcrtns.h"
#include "pass2l1.h"
#include "exerespe.h"
#include "sharedio.h"
#include "rctypes.h"
#include "exeutil.h"
#include "newmem.h"
#include "dosstub.h"
#include "posixfp.h"
#include "implcoff.h"

#include "clibext.h"


#ifdef _OS2

#define MINIMUM_SEG_SHIFT       2           /* Corresponds to 2^2 == 4 bytes */
#define DEFAULT_SEG_SHIFT       9           /* Corresponds to 2^9 == 512 bytes */

#define STUB_ALIGN              8           /* for PE format */

#define PE32_OPT_SIZE           sizeof(pe32_opt_header)
#define PE64_OPT_SIZE           sizeof(pe64_opt_header)
#define PE32_SIZE               (PE_HDR_SIZE + PE32_OPT_SIZE)
#define PE64_SIZE               (PE_HDR_SIZE + PE64_OPT_SIZE)

/* RDOS OS default major/minor version numbers */
#define PE_RDOS_OS_MAJOR        8
#define PE_RDOS_OS_MINOR        8
/* RDOS Subsystem default major/minor version numbers */
#define PE_RDOS_SS_MAJOR        1
#define PE_RDOS_SS_MINOR        0

typedef struct local_import {
    struct local_import *next;
    symbol              *iatsym;
    symbol              *locsym;
} local_import;

#define TRANSFER_SEGNAME "TRANSFER CODE"

static module_import    *PEImpList;
static unsigned         NumMods;
static segdata          *XFerSegData;
static local_import     *PELocalImpList;
static unsigned         NumLocalImports;

static struct {
    offset      ilt_off;
    offset      eof_ilt_off;
    offset      iat_off;
    offset      mod_name_off;
    offset      hint_off;
    offset      total_size;
    segdata     *sdata;
} IData;

#define WALK_IMPORT_SYMBOLS(sym) \
    for( (sym) = HeadSym; (sym) != NULL; (sym) = (sym)->link ) \
        if( IS_SYM_IMPORTED(sym) && (sym)->p.import != NULL \
            /*&& ((sym)->info & SYM_DEAD) == 0 */)

static offset CalcIDataSize( void )
/*********************************/
{
    struct module_import        *mod;
    struct import_name          *imp;
    unsigned_32 iatsize;
    unsigned_32 size;

    iatsize = ( NumImports + NumMods ) * sizeof( pe_va );
    if( 0 == iatsize ) {
        return( 0 );
    }
    IData.ilt_off = ( NumMods + 1 ) * sizeof( pe_import_directory );
    IData.eof_ilt_off = IData.ilt_off + iatsize;
    IData.iat_off = IData.eof_ilt_off + TocSize;
    IData.mod_name_off = IData.iat_off + iatsize;
    size = 0;
    for( mod = PEImpList; mod != NULL; mod = mod->next ) {
        size += mod->mod->len + 1;
    }
    IData.hint_off = IData.mod_name_off + size;
    size = IData.hint_off;
    for( mod = PEImpList; mod != NULL; mod = mod->next ) {
        for( imp = mod->imports; imp != NULL; imp = imp->next ) {
            if( imp->imp != NULL ) {
                size = __ROUND_UP_SIZE_EVEN( size );
                size += imp->imp->len + sizeof( unsigned_16 ) + sizeof( unsigned_8 );
            }
        }
    }
    IData.total_size = size;
    return( IData.total_size );
}

void ResetLoadPE( void )
/**********************/
{
    PEImpList = NULL;
    XFerSegData = NULL;
    NumMods = 0;
    NumImports = 0;
    memset( &IData, 0, sizeof( IData ) );
}

static offset CalcIATAbsOffset( void )
/************************************/
{
    return( IDataGroup->linear + FmtData.base + IData.iat_off );
}

static void CalcImpOff( dll_sym_info *dll, offset *off )
/******************************************************/
{
    if( dll != NULL) { // if not end of mod marker
        SET_SYM_ADDR( dll->iatsym, *off, 0 );
    }
    *off += sizeof( pe_va );
}

static void XFerReloc( offset off, group_entry *group, unsigned type, unsigned data )
/***********************************************************************************/
{
    reloc_item  reloc;
    size_t      size;

    size = sizeof( pe_reloc_item );
    reloc.pe = (( off + group->linear ) & OSF_PAGE_MASK) | type;
    if( type == PE_FIX_HIGHADJ ) {
        size = sizeof( high_pe_reloc_item );
        reloc.hpe.low_off = data;
    }
    WriteReloc( group, off, &reloc, size );
}

static int GetTransferGlueSize( void )
/************************************/
{
    switch( LinkState & LS_HAVE_MACHTYPE_MASK ) {
    case LS_HAVE_ALPHA_CODE:    return( sizeof( CoffImportAxpText ) );
    case LS_HAVE_MIPS_CODE:     return( sizeof( CoffImportMipsText ) ); // TODO
    case LS_HAVE_PPC_CODE:      return( sizeof( CoffImportPpcText ) );
    case LS_HAVE_X64_CODE:      return( sizeof( CoffImportX64Text ) ); // TODO
    case LS_HAVE_X86_CODE:      return( sizeof( CoffImportX86Text ) );
    default:                    DbgAssert( 0 ); return( 0 );
    }
}

static void *GetTransferGlueCode( void )
/**************************************/
{
    switch( LinkState & LS_HAVE_MACHTYPE_MASK ) {
    case LS_HAVE_ALPHA_CODE:    return( CoffImportAxpText );
    case LS_HAVE_MIPS_CODE:     return( CoffImportMipsText ); // TODO
    case LS_HAVE_PPC_CODE:      return( CoffImportPpcText );
    case LS_HAVE_X64_CODE:      return( CoffImportX64Text ); // TODO
    case LS_HAVE_X86_CODE:      return( CoffImportX86Text );
    default:                    DbgAssert( 0 ); return( NULL );
    }
}

offset FindIATSymAbsOff( symbol *sym )
/************************************/
{
    dll_sym_info        *dll;

    dll = sym->p.import;
    DbgAssert( IS_SYM_IMPORTED( sym ) && dll != NULL );
    return( dll->iatsym->addr.off );
}

offset FindSymPosInTocv( symbol *sym )
/************************************/
{
    return( FindIATSymAbsOff( sym ) - IDataGroup->linear - FmtData.base - TocShift - IData.eof_ilt_off );
}

static void GenPETransferTable( void )
/************************************/
{
    offset          off;
    offset          base;
    symbol          *sym;
    void*           data;
    size_t          datalen;
    group_entry     *group;
    local_import    *loc_imp;
    pe_va           addr;

    if( XFerSegData == NULL )
        return;
    group = XFerSegData->u.leader->group;
    base = XFerSegData->u.leader->seg_addr.off + XFerSegData->a.delta;
    if( IDataGroup != NULL ) {
        datalen = GetTransferGlueSize();
        data = GetTransferGlueCode();
        WALK_IMPORT_SYMBOLS( sym ) {
            switch( LinkState & LS_HAVE_MACHTYPE_MASK ) {
            case LS_HAVE_ALPHA_CODE:
              {
                offset dest = FindIATSymAbsOff( sym );
                CoffImportAxpText[0] |= ( dest >> 16 );     // hioff
                CoffImportAxpText[1] |= ( dest & 0xFFFF );  // looff
                if( LinkState & LS_MAKE_RELOCS ) {
                    if( (FmtData.objalign & 0xFFFF) == 0 ) {
                        XFerReloc( sym->addr.off + 0, group, PE_FIX_HIGH, 0 );
                    } else {
                        XFerReloc( sym->addr.off + 4, group, PE_FIX_LOW, 0 );
                        XFerReloc( sym->addr.off + 0, group, PE_FIX_HIGHADJ, CoffImportAxpText[0] );
                    }
                }
              }
                break;
            case LS_HAVE_MIPS_CODE:
              {
                offset dest = FindIATSymAbsOff( sym );
                CoffImportMipsText[0] |= ( dest >> 16 );     // hioff
                CoffImportMipsText[1] |= ( dest & 0xFFFF );  // looff
                if( LinkState & LS_MAKE_RELOCS ) {
                    if( (FmtData.objalign & 0xFFFF) == 0 ) {
                        XFerReloc( sym->addr.off + 0, group, PE_FIX_HIGH, 0 );
                    } else {
                        XFerReloc( sym->addr.off + 4, group, PE_FIX_LOW, 0 );
                        XFerReloc( sym->addr.off + 0, group, PE_FIX_HIGHADJ, CoffImportMipsText[0] );
                    }
                }
              }
                break;
            case LS_HAVE_PPC_CODE:
                CoffImportPpcText[0] = (CoffImportPpcText[0] & 0xffff0000) | (FindSymPosInTocv( sym ) & 0x0000ffff);
                break;
            case LS_HAVE_X64_CODE:
                // TODO
              {
                offset dest = FindIATSymAbsOff( sym );
                ((unsigned_64 *)(CoffImportX64Text + 2))->u._32[0] = dest;
                ((unsigned_64 *)(CoffImportX64Text + 2))->u._32[1] = 0;
                if( LinkState & LS_MAKE_RELOCS ) {
                    XFerReloc( sym->addr.off + 2, group, PE_FIX_HIGHLOW, 0 );
                }
              }
                break;
            case LS_HAVE_X86_CODE:
              {
                offset dest = FindIATSymAbsOff( sym );

                *(unsigned_32 *)(CoffImportX86Text + 2) = dest;
                if( LinkState & LS_MAKE_RELOCS ) {
                    XFerReloc( sym->addr.off + 2, group, PE_FIX_HIGHLOW, 0 );
                }
              }
                break;
            default:
                break;
            }
            off = sym->addr.off - base;
            PutInfo( XFerSegData->u1.vm_ptr + off, data, datalen );
        }
    }
    /* dump the local addresses table */
    for( loc_imp = PELocalImpList; loc_imp != NULL; loc_imp = loc_imp->next ) {
        off = loc_imp->iatsym->addr.off - base;
        addr = FindLinearAddr( &loc_imp->locsym->addr ) + FmtData.base;
        PutInfo( XFerSegData->u1.vm_ptr + off, &addr, sizeof( addr ) );
    }
    if( LinkState & LS_MAKE_RELOCS ) {
        for( loc_imp = PELocalImpList; loc_imp != NULL; loc_imp = loc_imp->next ) {
            XFerReloc( loc_imp->iatsym->addr.off, group, PE_FIX_HIGHLOW, 0 );
        }
    }
    group->size = group->totalsize;
}

unsigned_32 DefStackSizePE( void )
/********************************/
{
    return( PE_DEF_STACK_SIZE );
}

static unsigned_32 WriteDataPages( pe_exe_header *pehdr, pe_object *object, unsigned_32 file_align )
/***************************************************************************************************
 * write the enumerated data pages
 */
{
    group_entry *group;
    char        *name;
    unsigned_32 linear;
    seg_leader  *leader;
    unsigned_32 size_v;
    unsigned_32 size_ph;
    unsigned_32 code_base;
    unsigned_32 data_base;
    unsigned_32 entry_rva;
    unsigned_32 init_data_size;
    unsigned_32 code_size;

    linear = 0;
    entry_rva = 0;
    init_data_size = 0;
    code_size = 0;
    code_base = 0xFFFFFFFFUL;
    data_base = 0xFFFFFFFFUL;
    for( group = Groups; group != NULL; group = group->next_group) {
        if( group->totalsize == 0 )
            continue;   // DANGER DANGER DANGER <--!!!
        name = group->sym->name.u.ptr;
        if( name == NULL || name[0] == 0 ) {
            leader = Ring2First( group->leaders );
            name = leader->segname.u.ptr;
            if( name == NULL ) {
                name = "";
            }
        }
        strncpy( object->name, name, PE_OBJ_NAME_LEN );
        size_ph = CalcGroupSize( group );
        if( group == DataGroup && FmtData.dgroupsplitseg != NULL ) {
            size_v = group->totalsize;
            if( StackSegPtr != NULL ) {
                size_v -= StackSize;
            }
        } else {
            size_v = size_ph;
        }
        linear = __ROUND_UP_SIZE( size_v, FmtData.objalign );
        linear += group->linear;
        if( StartInfo.addr.seg == group->grp_addr.seg ) {
            entry_rva = group->linear + StartInfo.addr.off;
        }
        object->rva = group->linear;
        /*
        //  Why weren't we filling in this field? MS do!
        */
        object->virtual_size = size_v;
        object->physical_size = __ROUND_UP_SIZE( size_ph, file_align );

        object->flags = 0;
        /* segflags are in OS/2 V1.x format, we have to translate them
            into the appropriate PE bits */
        if( group->segflags & SEG_DATA ) {
            object->flags |= PE_OBJ_INIT_DATA | PE_OBJ_READABLE;
            if( (group->segflags & SEG_READ_ONLY) == 0 ) {
                object->flags |= PE_OBJ_WRITABLE;
            }
            init_data_size += object->physical_size;
            if( object->rva < data_base ) {
                data_base = object->rva;
            }
        } else {
            object->flags |= PE_OBJ_CODE | PE_OBJ_EXECUTABLE;
            if( (group->segflags & SEG_READ_ONLY) == 0 ) {
                object->flags |= PE_OBJ_READABLE;
            }
            if( group->segflags & SEG_NOPAGE) {
                object->flags |= PE_OBJ_NOT_PAGABLE;
            }
            code_size += object->physical_size;
            if( object->rva < code_base ) {
                code_base = object->rva;
            }
        }
        if( group->segflags & SEG_PURE ) {
            object->flags |= PE_OBJ_SHARED;
        }
        if( size_ph != 0 ) {
            object->physical_offset = NullAlign( file_align );
            WriteGroupLoad( group, false );
            PadLoad( size_ph - group->size );
        }
        ++object;
    }
    if( code_base == 0xFFFFFFFFUL ) {
        code_base = 0;
    }
    if( data_base == 0xFFFFFFFFUL ) {
        data_base = 0;
    }
    PE( *pehdr, code_base ) = code_base;
    PE( *pehdr, entry_rva ) = entry_rva;
    PE( *pehdr, init_data_size ) = init_data_size;
    PE( *pehdr, code_size ) = code_size;
    if( (LinkState & LS_HAVE_X64_CODE) == 0 ) {
        PE32( *pehdr ).data_base = data_base;
    }
    return( linear );
}

static void WalkImportsMods( void (*action)(dll_sym_info *, offset *),
                                                        offset *cookie )
/**********************************************************************/
{
    struct module_import        *mod;
    struct import_name          *imp;

    for( mod = PEImpList; mod != NULL; mod = mod->next ) {
        for( imp = mod->imports; imp != NULL; imp = imp->next ) {
            action( imp->dll, cookie );
        }
        action( NULL, cookie );
    }
}

static void WriteIAT( virt_mem buf, offset linear )
/*************************************************/
{
    struct module_import    *mod;
    struct import_name      *imp;
    pe_va                   iat;
    offset                  pos;
    offset                  hint_rva ;

    pos = 0;
    hint_rva = IData.hint_off + linear;
    for( mod = PEImpList; mod != NULL; mod = mod->next ) {
        for( imp = mod->imports; imp != NULL; imp = imp->next ) {
            if( imp->imp != NULL ) {
                hint_rva = __ROUND_UP_SIZE_EVEN( hint_rva );
                iat = PE_IMPORT_BY_NAME | hint_rva;
                hint_rva += imp->imp->len + ( sizeof( unsigned_16 ) + sizeof( unsigned_8 ) );
            } else {
                iat = PE_IMPORT_BY_ORDINAL | imp->dll->u.ordinal;
            }
            PutInfo( buf + pos, &iat, sizeof( iat ) );
            pos += sizeof( iat );
        }
        /* NULL entry marks end of list */
        PutInfoNulls( buf + pos, sizeof( iat ) );
        pos += sizeof( iat );
    }
}

static void WriteImportInfo( void )
/*********************************/
{
    pe_import_directory         dir;
    unsigned_16                 hint;
    virt_mem                    buf;
    offset                      pos;
    group_entry                 *group;
    struct module_import        *mod;
    struct import_name          *imp;
    unsigned_32                 size;
    unsigned_32                 mod_name_rva;
    offset                      linear;

    if( IDataGroup == NULL ) {
        return;
    }
    group = IDataGroup;
    linear = group->linear;
    group->size = IData.total_size;
    buf = IData.sdata->u1.vm_ptr;
    pos = 0;
    dir.time_stamp = 0;
    dir.major = 0;
    dir.minor = 0;
    /* dump the directory table */
    size = 0;
    mod_name_rva = IData.mod_name_off + linear;
    for( mod = PEImpList; mod != NULL; mod = mod->next ) {
        dir.name_rva = mod_name_rva;
        dir.import_lookup_table_rva = IData.ilt_off + size + linear;
        dir.import_address_table_rva = IData.iat_off + size + linear;
        size += ( mod->num_entries + 1 ) * sizeof( pe_va );
        mod_name_rva += mod->mod->len + 1;
        PutInfo( buf+pos, &dir, sizeof( dir ) );
        pos += sizeof( dir );
    }
    PutInfoNulls( buf + pos, sizeof( dir ) );    /* NULL entry marks end of table */
    pos += sizeof( dir );
    WriteIAT( buf + IData.ilt_off, linear ); // Import Lookup table
    WriteToc( buf + IData.eof_ilt_off );
    for( pos = IData.eof_ilt_off; pos < IData.iat_off; pos += sizeof( pe_va ) ) {
        XFerReloc( pos, group, PE_FIX_HIGHLOW, 0 );
    }
    WriteIAT( buf + IData.iat_off, linear ); // Import Address table
    pos = IData.mod_name_off;            /* write the module names */
    for( mod = PEImpList; mod != NULL; mod = mod->next ) {
        int name_size = mod->mod->len + 1;
        PutInfo( buf + pos, mod->mod->name.u.ptr, name_size );
        pos += name_size;
    }
    pos = IData.hint_off;        /* write out the import names */
    for( mod = PEImpList; mod != NULL; mod = mod->next ) {
        hint = 1;
        for( imp = mod->imports; imp != NULL; imp = imp->next ) {
            if( imp->imp != NULL ) {
                if( pos & 1 ) {         /* round up */
                    PutInfoNulls( buf + pos, 1 );
                    pos++;
                }
                PutInfo( buf + pos, &hint, sizeof( hint ) );
                pos += sizeof( hint );
                size = imp->imp->len;
                PutInfo( buf + pos, imp->imp->name.u.ptr, size );
                pos += size;
                PutInfoNulls( buf + pos, 1 );
                pos++;
                hint++;
            }
        }
    }
}

static int namecmp_exp( const void *pn1, const void *pn2 )
/********************************************************/
{
    return( strcmp( (*(entry_export **)pn1)->name.u.ptr, (*(entry_export **)pn2)->name.u.ptr ) );
}


static unsigned_32 WriteExportInfo( pe_object *object, unsigned_32 file_align, pe_dir_entry *entry )
/**************************************************************************************************/
{
    unsigned_32         size;
    pe_export_directory dir;
    const char          *name;
    size_t              namelen;
    entry_export        **sort;
    entry_export        *exp;
    size_t              i;
    pe_va               eat;
    ordinal_t           next_ord;
    ordinal_t           high_ord = 0;
    size_t              num_entries;

    strncpy( object->name, ".edata", PE_OBJ_NAME_LEN );
    object->physical_offset = NullAlign( file_align );
    object->flags = PE_OBJ_INIT_DATA | PE_OBJ_READABLE;
    dir.flags = 0;
    dir.time_stamp = 0;
    dir.major = 0;
    dir.minor = 0;
    dir.name_rva = object->rva + sizeof( dir );
    dir.ordinal_base = FmtData.u.os2fam.exports->ordinal;
    if( FmtData.u.os2fam.module_name != NULL ) {
        name = FmtData.u.os2fam.module_name;
    } else {
        name = GetBaseName( Root->outfile->fname, 0, &namelen );
    }
    /* GetBaseName strips the extension, which we actually need to keep!
     * Always recalculate the len including the extension.
     */
    namelen = strlen( name ) + 1;
    dir.address_table_rva = __ROUND_UP_SIZE( dir.name_rva + namelen, sizeof( pe_va ) );
    num_entries = 0;
    for( exp = FmtData.u.os2fam.exports; exp != NULL; exp = exp->next ) {
        high_ord = exp->ordinal;
        ++num_entries;
        if( !exp->isprivate ) {
            if( exp->impname != NULL ) {
                AddImpLibEntry( exp->impname, exp->name.u.ptr, 0, true );
            } else {
                AddImpLibEntry( exp->sym->name.u.ptr, exp->name.u.ptr, 0, true );
            }
        }
    }
    dir.num_eat_entries = high_ord - dir.ordinal_base + 1;
    dir.num_name_ptrs = num_entries;
    dir.name_ptr_table_rva = dir.address_table_rva + dir.num_eat_entries * sizeof( pe_va );
    dir.ordinal_table_rva = dir.name_ptr_table_rva + num_entries * sizeof( pe_va );
    _ChkAlloc( sort, sizeof( entry_export * ) * num_entries );
    /* write the export directory table */
    WriteLoad( &dir, sizeof( dir ) );
    /* write the module name (includes null terminator) */
    WriteLoad( name, namelen );
    NullAlign( sizeof( pe_va ) );
    /* write the export address table */
    i = 0;
    next_ord = dir.ordinal_base;
    for( exp = FmtData.u.os2fam.exports; exp != NULL; exp = exp->next ) {
        sort[i++] = exp;
        eat = exp->addr.off;
        if( next_ord < exp->ordinal ) {
            PadLoad( ( exp->ordinal - next_ord ) * sizeof( pe_va ) );
        }
        next_ord = exp->ordinal + 1;
        WriteLoadU32( eat );
    }
    qsort( sort, num_entries, sizeof( entry_export * ), &namecmp_exp );
    /* write out the export name ptr table */
    eat = dir.ordinal_table_rva + num_entries * sizeof( unsigned_16 );
    for( i = 0; i < num_entries; ++i ) {
        exp = sort[i];
        WriteLoadU32( eat );
        eat += strlen( exp->name.u.ptr ) + 1;
    }
    /* write out the export ordinal table */
    for( i = 0; i < num_entries; ++i ) {
        WriteLoadU16( sort[i]->ordinal - dir.ordinal_base );
    }
    /* write out the export name table */
    for( i = 0; i < num_entries; ++i ) {
        exp = sort[i];
        WriteLoad( exp->name.u.ptr, strlen( exp->name.u.ptr ) + 1 );
    }
    _LnkFree( sort );
    size = eat - object->rva;
    object->physical_size = __ROUND_UP_SIZE( size, file_align );
    entry->size = size;
    entry->rva = object->rva;
    return( size );
}

#define PAGE_COUNT( size )  (((size)+(0x1000-1))>>0xC)

static unsigned_32 WriteRelocList( void **reloclist, unsigned_32 size,
                                   unsigned_32 pagerva, unsigned limit )
/**********************************************************************/
{
    unsigned_32 pagesize;
    bool        padme;

    for( ; limit > 0; --limit ) {
        pagesize = RelocSize( *reloclist );
        if( pagesize != 0 ) {
            padme = false;
            if( ( pagesize / sizeof( pe_reloc_item ) ) & 0x1 ) {
                pagesize += sizeof( pe_reloc_item );
                padme = true;
            }
            pagesize += 2 * sizeof( unsigned_32 );
            WriteLoadU32( pagerva );
            WriteLoadU32( pagesize );
            DumpRelocList( *reloclist );
            if( padme ) {
                PadLoad( sizeof( pe_reloc_item ) );
            }
            size += pagesize;
        }
        pagerva += OSF_PAGE_SIZE;
        reloclist++;
    }
    return( size );
}

static unsigned_32 WriteFixupInfo( pe_object *object, unsigned_32 file_align, pe_dir_entry *entry )
/*************************************************************************************************/
/* dump the fixup table */
{
    unsigned_32         numpages;
    unsigned_32         highidx;
    unsigned_32         pagerva;
    group_entry         *group;
    void ***            reloclist;
    unsigned long       size;

    strncpy( object->name, ".reloc", PE_OBJ_NAME_LEN );
    object->physical_offset = NullAlign( file_align );
    object->flags = PE_OBJ_INIT_DATA | PE_OBJ_READABLE | PE_OBJ_DISCARDABLE;
    size = 0;
    /* When using non-default object alignment, groups and pages need
     * not be in sync at all.
     */
    for( group = Groups; group != NULL; group = group->next_group ) {
        reloclist = group->g.grp_relocs;
        if( reloclist != NULL ) {
            pagerva = group->linear;
            numpages = PAGE_COUNT( group->size );
            for( highidx = OSF_RLIDX_HIGH( numpages ); highidx > 0; --highidx ) {
                size = WriteRelocList( *reloclist, size, pagerva, OSF_RLIDX_MAX );
                reloclist++;
                pagerva += OSF_PAGE_SIZE * ((unsigned_32) OSF_RLIDX_MAX);
            }
            size = WriteRelocList( *reloclist, size, pagerva, OSF_RLIDX_LOW(numpages) );
        }
    }
    PadLoad( sizeof( pe_fixup_header ) );
    size += sizeof( pe_fixup_header );
    object->physical_size = __ROUND_UP_SIZE( size, file_align );
    entry->size = size - sizeof( pe_fixup_header );
    entry->rva = object->rva;
    return( size );
}

static unsigned_32 WriteDescription( pe_object *object, unsigned_32 file_align )
/******************************************************************************/
{
    size_t      desc_len;

    desc_len = strlen( FmtData.description );
    strncpy( object->name, ".desc", PE_OBJ_NAME_LEN );
    object->physical_offset = NullAlign( file_align );
    object->flags = PE_OBJ_INIT_DATA | PE_OBJ_READABLE;
    object->physical_size = __ROUND_UP_SIZE( desc_len, file_align );
    WriteLoad( FmtData.description, desc_len );
    return( desc_len );
}

bool RcPadFile( FILE *fp, size_t pad )
{
    DbgAssert( FP2POSIX( fp ) == Root->outfile->handle );

    /* unused parameters */ (void)fp;

    PadLoad( pad );
    return( false );
}

void CheckDebugOffset( ExeFileInfo *info )
{
    /* unused parameters */ (void)info;
}

RcStatus CopyExeData( FILE *in_fp, FILE *out_fp, unsigned_32 length )
/*******************************************************************/
{
    /* unused parameters */ (void)out_fp;

    for( ; length > MAX_HEADROOM; length -= MAX_HEADROOM ) {
        QRead( FP2POSIX( in_fp ), TokBuff, MAX_HEADROOM, "resource file" );
        WriteLoad( TokBuff, MAX_HEADROOM );
    }
    if( length > 0 ) {
        QRead( FP2POSIX( in_fp ), TokBuff, length, "resource file" );
        WriteLoad( TokBuff, length );
    }
    return( RS_OK );
}

void DoAddResource( char *name )
/*************************************/
{
    list_of_names       *info;
    unsigned            len;

    len = strlen( name );
    _PermAlloc( info, sizeof( list_of_names ) + len );
    memcpy( info->name, name, len + 1 );
    info->next_name = FmtData.u.pe.resources;
    FmtData.u.pe.resources = info;
}

static unsigned_32 WritePEResources( pe_exe_header *pehdr, pe_object *object, unsigned_32 file_align )
/****************************************************************************************************/
{
    ExeFileInfo einfo;
    ResFileInfo *rinfo;
    bool        allopen;
    bool        status;

    memset( &einfo, 0, sizeof( einfo ) );

    if( FmtData.resource != NULL ) {
        DoAddResource( FmtData.resource );
        FmtData.resource = NULL;
    }
    status = OpenResFiles( (ExtraRes *)FmtData.u.pe.resources, &rinfo, &allopen, EXE_TYPE_PE, Root->outfile->fname );
    if( !status )               // we had a problem opening
        return( 0 );
    einfo.fp = POSIX2FP( Root->outfile->handle );
    einfo.name = Root->outfile->fname;
    einfo.u.PEInfo.WinHead = pehdr;
    einfo.Type = EXE_TYPE_PE;
    status = BuildPEResourceObject( &einfo, rinfo, object, object->rva, NullAlign( file_align ), !allopen );
    CloseResFiles( rinfo );
    return( object->physical_size );
}

static unsigned_32 WriteDebugTable( pe_object *object, const char *symfilename,
                unsigned_32 file_align, unsigned_32 time_stamp, pe_dir_entry *entry )
/***********************************************************************************/
{
    debug_directory     dir;
    unsigned_32         size;

    if( symfilename == NULL ) {
        /* two entries */
        size = 2 * sizeof( debug_directory );
    } else {
        /* one entry */
        size = sizeof( debug_directory );
    }
    strncpy( object->name, ".rdata", PE_OBJ_NAME_LEN );
    object->physical_offset = NullAlign( file_align );
    object->flags = PE_OBJ_INIT_DATA | PE_OBJ_READABLE;
    object->physical_size = __ROUND_UP_SIZE( size, file_align );

    /* write debug dir entry for DEBUG_TYPE_MISC */
    dir.flags = 0;
    dir.time_stamp = time_stamp;
    dir.major = 0;
    dir.minor = 0;
    dir.debug_type = DEBUG_TYPE_MISC;
    dir.debug_size = sizeof( debug_misc_dbgdata );
    dir.data_rva = 0;
    dir.data_seek = object->physical_offset + object->physical_size;
    WriteLoad( &dir, sizeof( debug_directory ) );

    /* remember current file offset of this directory entry for later use */
    CVDebugDirEntryPos = PosLoad();

    if( symfilename == NULL ) {
        /* write debug dir entry for DEBUG_TYPE_CODEVIEW */
        dir.flags = 0;
        dir.time_stamp = time_stamp;
        dir.major = 0;
        dir.minor = 0;
        dir.debug_type = DEBUG_TYPE_CODEVIEW;
        dir.debug_size = CVSize;
        dir.data_rva = 0;
        dir.data_seek = object->physical_offset + object->physical_size + sizeof( debug_misc_dbgdata );
        WriteLoad( &dir, sizeof( debug_directory ) );
    }

    entry->size = size;
    entry->rva = object->rva;
    return( size );
}

static void CheckNumRelocs( void )
/********************************/
// don't want to generate a .reloc section if we don't have any relocs
{
    group_entry *group;
    symbol      *sym;

    if( (LinkState & LS_MAKE_RELOCS) == 0 )
        return;
    for( group = Groups; group != NULL; group = group->next_group ) {
        if( group->g.grp_relocs != NULL ) {
            return;
        }
    }
    WALK_IMPORT_SYMBOLS( sym ) {
        if( LinkState & LS_HAVE_MACHTYPE_MASK ) {
            return;
        }
    }
    LinkState &= ~LS_MAKE_RELOCS;
}

static seg_leader *SetLeaderTable( const char *name, pe_dir_entry *entry )
/************************************************************************/
{
    seg_leader *leader;

    leader = FindSegment( Root, name );
    if( leader != NULL ) {
        entry->rva =  leader->group->linear + SEG_GROUP_DELTA( leader );
        entry->size = leader->size;
    }
    return( leader );
}

static int CmpDesc( virt_mem a, virt_mem b )
/******************************************/
{
    unsigned_32 a32;
    unsigned_32 b32;

    GET32INFO( *((virt_mem *)a), a32 );
    GET32INFO( *((virt_mem *)b), b32 );
    return( (signed_32)a32 - b32 );
}

static void SwapDesc( virt_mem a, virt_mem b )
/********************************************/
{
    procedure_descriptor        tmp;

    a = *((virt_mem *)a);
    b = *((virt_mem *)b);
    ReadInfo( a, &tmp, sizeof( procedure_descriptor ) );
    CopyInfo( a, b, sizeof( procedure_descriptor ) );
    PutInfo( b, &tmp, sizeof( procedure_descriptor ) );
}

static bool SetPDataArray( void *_sdata, void *_array )
/*****************************************************/
{
    segdata    *sdata = _sdata;
    virt_mem  **array = _array;
    offset      size;
    virt_mem    data;

    if( !sdata->isdead ) {
        data = sdata->u1.vm_ptr;
        for( size = sdata->length; size >= sizeof( procedure_descriptor ); size -= sizeof( procedure_descriptor ) ) {
            **array = data;
            *array += 1;
            data += sizeof( procedure_descriptor );
        }
    }
    return( false );
}

static void SetMiscTableEntries( pe_exe_header *pehdr )
/*****************************************************/
{
    seg_leader  *leader;
    virt_mem    *sortarray;
    virt_mem    *temp;
    unsigned    numpdatas;
    symbol      *sym;

    SetLeaderTable( IDataGrpName, &PE_DIRECTORY( *pehdr, PE_TBL_IMPORT ) );
    sym = FindISymbol( TLSSym );
    if( sym != NULL ) {
        PE_DIRECTORY( *pehdr, PE_TBL_THREAD ).rva = FindLinearAddr( &sym->addr );
        PE_DIRECTORY( *pehdr, PE_TBL_THREAD ).size = sym->p.seg->length;
    }
    leader = SetLeaderTable( CoffPDataSegName, &PE_DIRECTORY( *pehdr, PE_TBL_EXCEPTION ) );
    /* The .pdata section may end up being empty if the symbols got optimized out */
    if( leader != NULL && leader->size ) {
        numpdatas = leader->size / sizeof( procedure_descriptor );
        _ChkAlloc( sortarray, numpdatas * sizeof( virt_mem * ) );
        temp = sortarray;
        RingLookup( leader->pieces, SetPDataArray, &temp );
        VMemQSort( (virt_mem)sortarray, numpdatas, sizeof( virt_mem * ), SwapDesc, CmpDesc );
        _LnkFree( sortarray );
    }
}

static unsigned FindNumObjects( void )
/************************************/
{
    unsigned            num_objects;

    num_objects = NumGroups;
    if( LinkState & LS_MAKE_RELOCS )
        ++num_objects;
    if( FmtData.u.os2fam.exports != NULL )
        ++num_objects;
    if( LinkFlags & LF_CV_DBI_FLAG )
        ++num_objects;
    if( FmtData.description != NULL )
        ++num_objects;
    if( FmtData.resource != NULL || FmtData.u.pe.resources != NULL )
        ++num_objects;
    return( num_objects );
}

static unsigned long CalcPEChecksum( unsigned long dwInitialCount, unsigned short *pwBuffer, unsigned long dwWordCount )
/**********************************************************************************************************************/
{
    unsigned long      __wCrc      = dwInitialCount;
    unsigned short     *__pwBuffer = pwBuffer;
    unsigned long      __dwCount   = dwWordCount;

    while( 0 != __dwCount-- ) {
        __wCrc += *__pwBuffer++;

        __wCrc = ( __wCrc & 0x0000FFFF ) + ( __wCrc >> 16 );
    }

    __wCrc = ( ( __wCrc >> 16 ) + __wCrc );

    return( __wCrc & 0x0000FFFF );
}

void FiniPELoadFile( void )
/*************************/
/* make a PE executable file */
{
    pe_exe_header   pehdr;
    unsigned_32     stub_len;
    pe_object       *objects;
    unsigned        num_objects;
    pe_object       *tbl_obj;
    unsigned        head_size;
    unsigned_32     file_align;
    unsigned_32     size;
    unsigned_32     image_size;

    file_align = 1UL << FmtData.u.os2fam.segment_shift;
    CheckNumRelocs();
    num_objects = FindNumObjects();
    memset( &pehdr, 0, sizeof( pehdr ) ); /* zero all header fields */
    if( LinkState & LS_HAVE_X64_CODE ) {
        pehdr.signature = EXESIGN_PE;
        head_size = PE64_SIZE;
        PE64( pehdr ).magic = 0x20b;
        pehdr.fheader.cpu_type = PE_CPU_AMD64;
        pehdr.fheader.num_objects = num_objects;
        pehdr.fheader.time_stamp = (unsigned_32)time( NULL );
        pehdr.fheader.opt_hdr_size = PE64_OPT_SIZE;
        pehdr.fheader.flags = PE_FLG_REVERSE_BYTE_LO | PE_FLG_32BIT_MACHINE | PE_FLG_LARGE_ADDRESS_AWARE;
        if( FmtData.u.pe.nolargeaddressaware ) {
            pehdr.fheader.flags &= ~PE_FLG_LARGE_ADDRESS_AWARE;
        }
        if( (LinkState & LS_MAKE_RELOCS) == 0 ) {
            pehdr.fheader.flags |= PE_FLG_RELOCS_STRIPPED;
        }
        if( (LinkState & LS_LINK_ERROR) == 0 ) {
            pehdr.fheader.flags |= PE_FLG_IS_EXECUTABLE;
        }
        if( FmtData.dll ) {
            pehdr.fheader.flags |= PE_FLG_LIBRARY;
            if( FmtData.u.os2fam.flags & INIT_INSTANCE_FLAG ) {
                PE64( pehdr ).dll_flags |= PE_DLL_PERPROC_INIT;
            } else if( FmtData.u.os2fam.flags & INIT_THREAD_FLAG ) {
                PE64( pehdr ).dll_flags |= PE_DLL_PERTHRD_INIT;
            }
            if( FmtData.u.os2fam.flags & TERM_INSTANCE_FLAG ) {
                PE64( pehdr ).dll_flags |= PE_DLL_PERPROC_TERM;
            } else if( FmtData.u.os2fam.flags & TERM_THREAD_FLAG ) {
                PE64( pehdr ).dll_flags |= PE_DLL_PERTHRD_TERM;
            }
        }

        if( FmtData.u.pe.lnk_specd ) {
            PE64( pehdr ).lnk_major = FmtData.u.pe.linkmajor;
            PE64( pehdr ).lnk_minor = FmtData.u.pe.linkminor;
        } else {
            PE64( pehdr ).lnk_major = PE_LNK_MAJOR;
            PE64( pehdr ).lnk_minor = PE_LNK_MINOR;
        }
        PE64( pehdr ).image_base.u._32[0] = FmtData.base;
        PE64( pehdr ).image_base.u._32[1] = 0;
        PE64( pehdr ).object_align = FmtData.objalign;

        /*
         *  I have changed this to allow programmers to control this shift. MS has 0x20 byte segments
         *  in some drivers! Who are we to argue? Never mind it's against the PE spec.
         */
        if( FmtData.u.os2fam.segment_shift < MINIMUM_SEG_SHIFT ) {
            LnkMsg( WRN+MSG_VALUE_INCORRECT, "s", "ALIGNMENT" );
            FmtData.u.os2fam.segment_shift = DEFAULT_SEG_SHIFT;
        }

        PE64( pehdr ).file_align = file_align;

        if( FmtData.u.pe.osv_specd ) {
            PE64( pehdr ).os_major = FmtData.u.pe.osmajor;
            PE64( pehdr ).os_minor = FmtData.u.pe.osminor;
        } else {
            PE64( pehdr ).os_major = PE_OS_MAJOR;
            PE64( pehdr ).os_minor = PE_OS_MINOR + 0xb;      // KLUDGE!
        }

        PE64( pehdr ).user_major = FmtData.major;
        PE64( pehdr ).user_minor = FmtData.minor;
        if( FmtData.u.pe.sub_specd ) {
            PE64( pehdr ).subsys_major = FmtData.u.pe.submajor;
            PE64( pehdr ).subsys_minor = FmtData.u.pe.subminor;
        } else {
            PE64( pehdr ).subsys_major = 3;
            PE64( pehdr ).subsys_minor = 0xa;
        }
        if( FmtData.u.pe.subsystem != PE_SS_UNKNOWN ) {
            PE64( pehdr ).subsystem = FmtData.u.pe.subsystem;
        } else {
            PE64( pehdr ).subsystem = PE_SS_WINDOWS_GUI;
        }
        /*
         * set stack reserved and committed size for executable
         * zero for DLL (StackSize is already set to zero)
         */
        size = StackSize;
        PE64( pehdr ).stack_reserve_size.u._32[0] = size;
        PE64( pehdr ).stack_reserve_size.u._32[1] = 0;
        if( FmtData.u.pe.stackcommit == DEF_VALUE ) {
            if( size > PE_DEF_STACK_COMMIT ) {
                size = PE_DEF_STACK_COMMIT;
            }
        } else if( size > FmtData.u.pe.stackcommit ) {
            size = FmtData.u.pe.stackcommit;
        }
        PE64( pehdr ).stack_commit_size.u._32[0] = size;
        PE64( pehdr ).stack_commit_size.u._32[1] = 0;
        /*
         * set heap reserved and committed size for executable
         * zero for DLL
         */
        size = FmtData.u.os2fam.heapsize;
        if( FmtData.dll ) {
            size = 0;
        }
        PE64( pehdr ).heap_reserve_size.u._32[0] = size;
        PE64( pehdr ).heap_reserve_size.u._32[1] = 0;
        if( size > FmtData.u.pe.heapcommit ) {
            size = FmtData.u.pe.heapcommit;
        }
        PE64( pehdr ).heap_commit_size.u._32[0] = size;
        PE64( pehdr ).heap_commit_size.u._32[1] = 0;

        PE64( pehdr ).num_tables = PE_TBL_NUMBER;
        CurrSect = Root;
        SeekLoad( 0 );
        stub_len = WriteStubFile( STUB_ALIGN );
        _ChkAlloc( objects, num_objects * sizeof( pe_object ) );
        memset( objects, 0, num_objects * sizeof( pe_object ) );
        /* leave space for the header and object table */
        PadLoad( head_size + num_objects * sizeof( pe_object ) );
        GenPETransferTable();
        WriteImportInfo();
        SetMiscTableEntries( &pehdr );
        image_size = WriteDataPages( &pehdr, objects, file_align );
        tbl_obj = &objects[NumGroups];
        if( FmtData.u.os2fam.exports != NULL ) {
            tbl_obj->rva = image_size;
            size = WriteExportInfo( tbl_obj, file_align, &PE_DIRECTORY( pehdr, PE_TBL_EXPORT ) );
            image_size += __ROUND_UP_SIZE( size, FmtData.objalign );
            ++tbl_obj;
        }
        if( LinkState & LS_MAKE_RELOCS ) {
            tbl_obj->rva = image_size;
            size = WriteFixupInfo( tbl_obj, file_align, &PE_DIRECTORY( pehdr, PE_TBL_FIXUP ) );
            image_size += __ROUND_UP_SIZE( size, FmtData.objalign );
            ++tbl_obj;
        }
        if( FmtData.description != NULL ) {
            tbl_obj->rva = image_size;
            size = WriteDescription( tbl_obj, file_align );
            image_size += __ROUND_UP_SIZE( size, FmtData.objalign );
            ++tbl_obj;
        }
        if( FmtData.resource != NULL || FmtData.u.pe.resources != NULL ) {
            tbl_obj->rva = image_size;
            size = WritePEResources( &pehdr, tbl_obj, file_align );
            image_size += __ROUND_UP_SIZE( size, FmtData.objalign );
            ++tbl_obj;
        }
        if( LinkFlags & LF_CV_DBI_FLAG ) {
            tbl_obj->rva = image_size;
            size = WriteDebugTable( tbl_obj, SymFileName, file_align, pehdr.fheader.time_stamp, &PE_DIRECTORY( pehdr, PE_TBL_DEBUG ) );
            image_size += __ROUND_UP_SIZE( size, FmtData.objalign );
            ++tbl_obj;
        }
        NullAlign( file_align ); /* pad out last page */
        PE64( pehdr ).image_size = image_size;
        PE64( pehdr ).headers_size = objects[0].physical_offset;
    } else {
        if( FmtData.u.pe.tnt || FmtData.u.pe.subsystem == PE_SS_PL_DOSSTYLE ) {
            pehdr.signature = EXESIGN_PL;
        } else {
            pehdr.signature = EXESIGN_PE;
        }
        head_size = PE32_SIZE;
        PE32( pehdr ).magic = 0x10b;
        switch( LinkState & LS_HAVE_MACHTYPE_MASK ) {
        case LS_HAVE_ALPHA_CODE:
            pehdr.fheader.cpu_type = PE_CPU_ALPHA;
            break;
        case LS_HAVE_MIPS_CODE:     // TODO
//            pehdr.fheader.cpu_type = PE_CPU_MIPS_R3000;
            pehdr.fheader.cpu_type = PE_CPU_MIPS_R4000;
            break;
        case LS_HAVE_PPC_CODE:
            pehdr.fheader.cpu_type = PE_CPU_POWERPC;
            break;
        case LS_HAVE_X86_CODE:
            pehdr.fheader.cpu_type = PE_CPU_386;
            break;
        default:
            break;
        }
        pehdr.fheader.num_objects = num_objects;
        pehdr.fheader.time_stamp = (unsigned_32)time( NULL );
        pehdr.fheader.opt_hdr_size = PE32_OPT_SIZE;
        pehdr.fheader.flags = PE_FLG_REVERSE_BYTE_LO | PE_FLG_32BIT_MACHINE;
        if( FmtData.u.pe.largeaddressaware ) {
            pehdr.fheader.flags |= PE_FLG_LARGE_ADDRESS_AWARE;
        }
        if( (LinkState & LS_MAKE_RELOCS) == 0 ) {
            pehdr.fheader.flags |= PE_FLG_RELOCS_STRIPPED;
        }
        if( (LinkState & LS_LINK_ERROR) == 0 ) {
            pehdr.fheader.flags |= PE_FLG_IS_EXECUTABLE;
        }
        if( FmtData.dll ) {
            pehdr.fheader.flags |= PE_FLG_LIBRARY;
            if( FmtData.u.os2fam.flags & INIT_INSTANCE_FLAG ) {
                PE32( pehdr ).dll_flags |= PE_DLL_PERPROC_INIT;
            } else if( FmtData.u.os2fam.flags & INIT_THREAD_FLAG ) {
                PE32( pehdr ).dll_flags |= PE_DLL_PERTHRD_INIT;
            }
            if( FmtData.u.os2fam.flags & TERM_INSTANCE_FLAG ) {
                PE32( pehdr ).dll_flags |= PE_DLL_PERPROC_TERM;
            } else if( FmtData.u.os2fam.flags & TERM_THREAD_FLAG ) {
                PE32( pehdr ).dll_flags |= PE_DLL_PERTHRD_TERM;
            }
        }

        if( FmtData.u.pe.lnk_specd ) {
            PE32( pehdr ).lnk_major = FmtData.u.pe.linkmajor;
            PE32( pehdr ).lnk_minor = FmtData.u.pe.linkminor;
        } else {
            PE32( pehdr ).lnk_major = PE_LNK_MAJOR;
            PE32( pehdr ).lnk_minor = PE_LNK_MINOR;
        }
        PE32( pehdr ).image_base = FmtData.base;
        PE32( pehdr ).object_align = FmtData.objalign;

        /*
         *  I have changed this to allow programmers to control this shift. MS has 0x20 byte segments
         *  in some drivers! Who are we to argue? Never mind it's against the PE spec.
         */
        if( FmtData.u.os2fam.segment_shift < MINIMUM_SEG_SHIFT ) {
            LnkMsg( WRN+MSG_VALUE_INCORRECT, "s", "ALIGNMENT" );
            FmtData.u.os2fam.segment_shift = DEFAULT_SEG_SHIFT;
        }

        file_align = 1UL << FmtData.u.os2fam.segment_shift;
        PE32( pehdr ).file_align = file_align;

        if( FmtData.u.pe.osv_specd ) {
            PE32( pehdr ).os_major = FmtData.u.pe.osmajor;
            PE32( pehdr ).os_minor = FmtData.u.pe.osminor;
        } else if( FmtData.u.pe.subsystem == PE_SS_RDOS ) {
            // RDOS default
            PE32( pehdr ).os_major = PE_RDOS_OS_MAJOR;
            PE32( pehdr ).os_minor = PE_RDOS_OS_MINOR;
        } else {
            PE32( pehdr ).os_major = PE_OS_MAJOR;
            PE32( pehdr ).os_minor = PE_OS_MINOR + 0xb;      // KLUDGE!
        }

        PE32( pehdr ).user_major = FmtData.major;
        PE32( pehdr ).user_minor = FmtData.minor;
        if( FmtData.u.pe.sub_specd ) {
            PE32( pehdr ).subsys_major = FmtData.u.pe.submajor;
            PE32( pehdr ).subsys_minor = FmtData.u.pe.subminor;
        } else if( FmtData.u.pe.subsystem == PE_SS_RDOS ) {
            // RDOS default
            PE32( pehdr ).subsys_major = PE_RDOS_SS_MAJOR;
            PE32( pehdr ).subsys_minor = PE_RDOS_SS_MINOR;
        } else {
            PE32( pehdr ).subsys_major = 3;
            PE32( pehdr ).subsys_minor = 0xa;
        }
        if( FmtData.u.pe.subsystem != PE_SS_UNKNOWN ) {
            PE32( pehdr ).subsystem = FmtData.u.pe.subsystem;
        } else {
            PE32( pehdr ).subsystem = PE_SS_WINDOWS_GUI;
        }
        /*
         * set stack reserved and committed size for executable
         * zero for DLL (StackSize is already set to zero)
         */
        size = StackSize;
        PE32( pehdr ).stack_reserve_size = size;
        if( FmtData.u.pe.stackcommit == DEF_VALUE ) {
            if( size > PE_DEF_STACK_COMMIT ) {
                size = PE_DEF_STACK_COMMIT;
            }
        } else if( size > FmtData.u.pe.stackcommit ) {
            size = FmtData.u.pe.stackcommit;
        }
        PE32( pehdr ).stack_commit_size = size;
        /*
         * set heap reserved and committed size for executable
         * zero for DLL
         */
        size = FmtData.u.os2fam.heapsize;
        if( FmtData.dll ) {
            size = 0;
        }
        if( size > FmtData.u.pe.heapcommit ) {
            size = FmtData.u.pe.heapcommit;
        }
        PE32( pehdr ).heap_commit_size = size;

        PE32( pehdr ).num_tables = PE_TBL_NUMBER;
        CurrSect = Root;
        SeekLoad( 0 );
        stub_len = WriteStubFile( STUB_ALIGN );
        _ChkAlloc( objects, num_objects * sizeof( pe_object ) );
        memset( objects, 0, num_objects * sizeof( pe_object ) );
        /* leave space for the header and object table */
        PadLoad( head_size + num_objects * sizeof( pe_object ) );
        GenPETransferTable();
        WriteImportInfo();
        SetMiscTableEntries( &pehdr );
        image_size = WriteDataPages( &pehdr, objects, file_align );
        tbl_obj = &objects[NumGroups];
        if( FmtData.u.os2fam.exports != NULL ) {
            tbl_obj->rva = image_size;
            size = WriteExportInfo( tbl_obj, file_align, &PE_DIRECTORY( pehdr, PE_TBL_EXPORT ) );
            image_size += __ROUND_UP_SIZE( size, FmtData.objalign );
            ++tbl_obj;
        }
        if( LinkState & LS_MAKE_RELOCS ) {
            tbl_obj->rva = image_size;
            size = WriteFixupInfo( tbl_obj, file_align, &PE_DIRECTORY( pehdr, PE_TBL_FIXUP ) );
            image_size += __ROUND_UP_SIZE( size, FmtData.objalign );
            ++tbl_obj;
        }
        if( FmtData.description != NULL ) {
            tbl_obj->rva = image_size;
            size = WriteDescription( tbl_obj, file_align );
            image_size += __ROUND_UP_SIZE( size, FmtData.objalign );
            ++tbl_obj;
        }
        if( FmtData.resource != NULL || FmtData.u.pe.resources != NULL ) {
            tbl_obj->rva = image_size;
            size = WritePEResources( &pehdr, tbl_obj, file_align );
            image_size += __ROUND_UP_SIZE( size, FmtData.objalign );
            ++tbl_obj;
        }
        if( LinkFlags & LF_CV_DBI_FLAG ) {
            tbl_obj->rva = image_size;
            size = WriteDebugTable( tbl_obj, SymFileName, file_align, pehdr.fheader.time_stamp, &PE_DIRECTORY( pehdr, PE_TBL_DEBUG ) );
            image_size += __ROUND_UP_SIZE( size, FmtData.objalign );
            ++tbl_obj;
        }
        NullAlign( file_align ); /* pad out last page */
        PE32( pehdr ).image_size = image_size;
        PE32( pehdr ).headers_size = objects[0].physical_offset;
    }
    DBIWrite();
    SeekLoad( stub_len );

    if( FmtData.u.pe.checksumfile ) {
        /* Ensure checksum is 0 before we calculate it */
        if( LinkState & LS_HAVE_X64_CODE ) {
            PE64( pehdr ).file_checksum = 0L;
        } else {
            PE32( pehdr ).file_checksum = 0L;
        }
    }

    WriteLoad( &pehdr, head_size );
    WriteLoad( objects, num_objects * sizeof( pe_object ) );

    if( FmtData.u.pe.checksumfile ) {
        unsigned_32     crc = 0L;
        size_t          buffsize;
        unsigned long   currpos = 0L;
        unsigned long   totalsize = 0L;
        outfilelist     *outfile;
        char            *buffer = NULL;

        /*
         *  Checksum required. We have already written the EXE header with a NULL checksum
         *  We need to calculate the checksum over all blocks
         *  We flush the buffers by seeking back to 0, then repeatedly reading all the file back in
         *  and checksumming it. The MS checksum is completed by adding the total file size to the
         *  calculated CRC. We then add this to the header and rewrite the header.
         */
        SeekLoad( 0 ); /* Flush the buffer */
        outfile = CurrSect->outfile;
        DbgAssert( outfile->buffer == NULL );

        totalsize = QFileSize( outfile->handle );

#define CRC_BUFF_SIZE   _16K
        _ChkAlloc( buffer, CRC_BUFF_SIZE );

        if( buffer ) {
            for( ; currpos < totalsize; currpos += buffsize ) {
                memset( buffer, 0, CRC_BUFF_SIZE );
                buffsize = QRead( outfile->handle, buffer, CRC_BUFF_SIZE, outfile->fname );
                DbgAssert( buffsize == IOERROR );
                DbgAssert( (buffsize % 2) != 1 ); /* check for odd length */

                crc = CalcPEChecksum( crc, (unsigned short *)buffer, buffsize / sizeof( unsigned short ) );
            }

            _LnkFree( buffer );
            crc += totalsize;

            if( LinkState & LS_HAVE_X64_CODE ) {
                PE64( pehdr ).file_checksum = crc;
            } else {
                PE32( pehdr ).file_checksum = crc;
            }
            SeekLoad( stub_len );
            WriteLoad( &pehdr, head_size );
        }
    }

    _LnkFree( objects );
}

static unsigned_32 getStubSize( void )
/************************************/
/* return the size of the stub file (unaligned) */
{
    unsigned_32     stub_len = 0;
    f_handle        the_file;
    dos_exe_header  dosheader;
    unsigned_32     read_len;
    unsigned_32     reloc_size;
    unsigned_32     code_start;
    char            fullname[PATH_MAX];
    size_t          len;

    if( FmtData.u.os2fam.no_stub ) {
        return( 0 );
    }
    stub_len = GetDOSDefStubSize();
    if( FmtData.u.os2fam.stub_file_name != NULL && stricmp( FmtData.u.os2fam.stub_file_name, Root->outfile->fname ) != 0 ) {
        the_file = FindPath( FmtData.u.os2fam.stub_file_name, fullname );
        if( the_file == NIL_FHANDLE ) {
            LnkMsg( WRN+MSG_CANT_OPEN_NO_REASON, "s", FmtData.u.os2fam.stub_file_name );
        } else {
            _LnkFree( FmtData.u.os2fam.stub_file_name );
            len = strlen( fullname ) + 1;
            _ChkAlloc( FmtData.u.os2fam.stub_file_name, len );
            memcpy( FmtData.u.os2fam.stub_file_name, fullname, len );
            QRead( the_file, &dosheader, sizeof( dos_exe_header ), FmtData.u.os2fam.stub_file_name );
            if( dosheader.signature == EXESIGN_DOS ) {
                code_start = dosheader.hdr_size * 16ul;
                read_len = dosheader.file_size * 512ul - (-dosheader.mod_size & 0x1ff) - code_start;
                // make sure reloc_size is a multiple of 16.
                reloc_size = __ROUND_UP_SIZE_PARA( dosheader.num_relocs * 4ul );
                dosheader.hdr_size = 4 + reloc_size / 16;
                stub_len = read_len + dosheader.hdr_size * 16ul;
            }
            QClose( the_file, FmtData.u.os2fam.stub_file_name );
        }
    }
    return( stub_len );
}

unsigned long GetPEHeaderSize( void )
/******************************************/
{
    unsigned long       size;
    unsigned            num_objects;

    num_objects = FindNumObjects();
    size = __ROUND_UP_SIZE( getStubSize(), STUB_ALIGN ) + num_objects * sizeof( pe_object );
    if( LinkState & LS_HAVE_X64_CODE ) {
        size += PE64_SIZE;
    } else {
        size += PE32_SIZE;
    }
    return( __ROUND_UP_SIZE( size, FmtData.objalign ) );
}

static void ReadExports( unsigned_32 namestart, unsigned_32 nameend,
                         unsigned_32 ordstart, unsigned numords,
                         ordinal_t ord_base, f_handle file, char *fname )
/***********************************************************************/
{
    unsigned_16         *ordbuf;
    unsigned_16         *ordptr;
    char                *nameptr;

    _ChkAlloc( ordbuf, numords * sizeof( unsigned_16 ) );
    QSeek( file, ordstart, fname );
    QRead( file, ordbuf, numords * sizeof( unsigned_16 ), fname );
    QSeek( file, namestart, fname );
    QRead( file, TokBuff, nameend - namestart, fname );
    nameptr = TokBuff,
    ordptr = ordbuf;
    for( ; numords > 0; --numords ) {
        CheckExport( nameptr, *ordptr + ord_base, true );
        while( *nameptr != '\0' )
            nameptr++;
        nameptr++;
        ordptr++;
    }
    _LnkFree( ordbuf );
}

void ReadPEExportTable( f_handle file, pe_dir_entry *export_dir )
/****************************************************************
 * read a PE export table, and set ordinal values accordingly.
 */
{
    pe_export_directory table;
    char                *fname;
    unsigned_32         *nameptrs;
    unsigned            nameptrsize;
    unsigned            numentries;
    unsigned_32         entrystart;
    unsigned_32         *curr;
    unsigned_32         namestart;

    fname = FmtData.u.os2fam.old_lib_name;
    QRead( file, &table, sizeof( pe_export_directory ), fname );
    nameptrsize = table.num_name_ptrs * sizeof( unsigned_32 );
    if( nameptrsize == 0 )                      /* NOTE: <-- premature return */
        return;
    _ChkAlloc( nameptrs, nameptrsize + sizeof( unsigned_32 ) );
    QSeek( file, table.name_ptr_table_rva - export_dir->rva, fname );
    QRead( file, nameptrs, nameptrsize, fname );
    numentries = 1;
    entrystart = table.ordinal_table_rva - export_dir->rva;
    curr = nameptrs;
    *curr -= export_dir->rva;
    namestart = *curr++;
    for( nameptrsize -= sizeof( unsigned_32 ); nameptrsize > 0; nameptrsize -= sizeof( unsigned_32 ) ) {
        *curr -= export_dir->rva;
        if( *curr - namestart > TokSize ) {
            ReadExports( namestart, *(curr - 1), entrystart, numentries, table.ordinal_base, file, fname );
            entrystart += numentries * sizeof( unsigned_16 );
            numentries = 1;
            namestart = *(curr - 1);
        }
        numentries++;
        curr++;
    }   /* NOTE! this assumes the name table is at the end */
    ReadExports( namestart, export_dir->size + *nameptrs, entrystart, numentries, table.ordinal_base, file, fname );
    _LnkFree( nameptrs );
}

static void CreateIDataSection( void )
/************************************/
{
    segdata     *sdata;
    class_entry *class;
    byte        bits;

    PrepareToc();
    if( 0 != CalcIDataSize() ) {
        IDataGroup = GetGroup( IDataGrpName );
        bits = ( LinkState & LS_HAVE_X64_CODE ) ? BITS_64 : BITS_32;
        class = FindClass( Root, CoffIDataClassName, bits, false );
        class->flags |= CLASS_IDATA | CLASS_LXDATA_SEEN;
        sdata = AllocSegData();
        sdata->align = ( LinkState & LS_HAVE_X64_CODE ) ? 4 : 2;
        sdata->bits = bits;
        sdata->length = IData.total_size;
        sdata->u.name.u.ptr = CoffIDataSegName;
        sdata->combine = COMBINE_ADD;
        sdata->isabs = false;
        AddSegment( sdata, class );
        sdata->u1.vm_ptr = AllocStg( sdata->length );
        IData.sdata = sdata;
        AddToGroup( IDataGroup, sdata->u.leader );
    }
}

static void RegisterImport( dll_sym_info *sym )
/*********************************************/
{
    struct module_import        *mod;
    struct import_name          *imp;
    struct import_name          *chk;
    struct import_name          **owner;
    obj_name_list               *os2_imp;
    int                         cmp;
    unsigned                    len;

    for( mod = PEImpList; mod != NULL; mod = mod->next ) {
        if( mod->mod == sym->m.modnum ) {
            break;
        }
    }
    if( mod == NULL ) {
        ++NumMods;
        _PermAlloc( mod, sizeof( struct module_import ) );
        mod->next = PEImpList;
        PEImpList = mod;
        mod->mod = sym->m.modnum;
        mod->imports = NULL;
        mod->num_entries = 0;
    }
    if( !sym->isordinal ) {
        os2_imp = sym->u.entry;
    } else {
        os2_imp = NULL;
    }
    mod->num_entries++;
    _PermAlloc( imp, sizeof( struct import_name ) );
    imp->dll = sym;
    imp->imp = os2_imp;
    /* keep the list sorted by name for calculating hint values */
    owner = &mod->imports;
    if( os2_imp != NULL ) {
        for( ;; ) {
            chk = *owner;
            if( chk == NULL )
                break;
            if( chk->imp != NULL ) {
                len = chk->imp->len;
                if( len > os2_imp->len )
                    len = os2_imp->len;
                cmp = memcmp( chk->imp->name.u.ptr, os2_imp->name.u.ptr, len );
                if( cmp > 0 )
                    break;
                if( cmp == 0 && len > chk->imp->len ) {
                    break;
                }
            }
            owner = &chk->next;
        }
    } else {
        chk = *owner;
    }
    imp->next = chk;
    *owner = imp;
    ++NumImports;
}

static void CreateTransferSegment( class_entry *class )
{
    offset      size;
    int         glue_size;
    symbol      *sym;
    segdata     *sdata;

    size = 0;
    glue_size = GetTransferGlueSize();
    WALK_IMPORT_SYMBOLS( sym ) {
        size += glue_size;
        RegisterImport( sym->p.import );
        DBIAddGlobal( sym );
    }
    size += NumLocalImports * sizeof( pe_va );
    if( size != 0 ) {
        class->flags |= CLASS_TRANSFER;
        sdata = AllocSegData();
        sdata->align = ( LinkState & LS_HAVE_X64_CODE ) ? 4 : 2;
        sdata->bits = ( LinkState & LS_HAVE_X64_CODE ) ? BITS_64 : BITS_32;
        sdata->length = size;
        sdata->u.name.u.ptr = TRANSFER_SEGNAME;
        sdata->combine = COMBINE_ADD;
        sdata->isabs = false;
        AddSegment( sdata, class );
        sdata->u1.vm_ptr = AllocStg( sdata->length );
        XFerSegData = sdata;
    }
}

void ChkPEData( void )
/***************************/
{
    class_entry *class;

    /* find the last code class in the program */
    for( class = Root->classlist; class != NULL; class = class->next_class ) {
        if( class->flags & CLASS_CODE ) {
            break;
        }
    }
    if( class == NULL ) { // No code -- no need to do transfer stuff
        return;
    }
    CurrMod = FakeModule;
    CreateTransferSegment( class );
    CreateIDataSection();
    CurrMod = NULL;
}

void AllocPETransferTable( void )
/**************************************/
{
    symbol              *sym;
    class_entry         *class;
    group_entry         *group;
    seg_leader          *lead;
    segdata             *piece;
    segdata             *save;
    offset              off;
    segment             seg;
    int                 glue_size;
    local_import        *loc_imp;

    /*
     *  Moved export check here as otherwise flags don't get propagated
     */
    if( XFerSegData == NULL ) {
        return;
    }
    for( class = Root->classlist; class != NULL; class = class->next_class ) {
        if( class->flags & CLASS_TRANSFER ) {
            break;
        }
    }
    if( class == NULL ) {
        return;
    }
    lead = RingLast( class->segs );
    piece = RingLast( lead->pieces );
    CurrMod = FakeModule;
    group = lead->group;
    seg = group->grp_addr.seg;
    off = group->grp_addr.off + group->totalsize;
    // now calc addresses for imported local symbols
    for( loc_imp = PELocalImpList; loc_imp != NULL; loc_imp = loc_imp->next ) {
        off -= sizeof( pe_va );
        SET_SYM_ADDR( loc_imp->iatsym, off, seg );
    }
    if( IDataGroup != NULL ) {
        glue_size = GetTransferGlueSize();
        WALK_IMPORT_SYMBOLS( sym ) {
            off -= glue_size;
            SET_SYM_ADDR( sym, off, seg );
            save = sym->p.seg;
            sym->p.seg = piece;
            DBIGenGlobal( sym, Root );
            sym->p.seg = save;
        }
        off = CalcIATAbsOffset();   // now calc addresses for IAT symbols
        WalkImportsMods( CalcImpOff, &off );
        SetTocAddr( IData.eof_ilt_off, IDataGroup ); // Set toc's address.
    }
    CurrMod = NULL;
}

#define PREFIX_LEN  (sizeof( ImportSymPrefix ) - 1)

void AddPEImportLocalSym( symbol *locsym, symbol *iatsym )
/********************************************************/
{
    local_import    *imp;

    _ChkAlloc( imp, sizeof( local_import ) );
    LinkList( &PELocalImpList, imp );
    imp->iatsym = iatsym;
    imp->locsym = locsym;
    ++NumLocalImports;
}

bool ImportPELocalSym( symbol *iatsym )
/*************************************/
{
    const char      *name;
    symbol          *sym;

    name = iatsym->name.u.ptr;
    if( memcmp( name, ImportSymPrefix, PREFIX_LEN ) != 0 )
        return( false );
    sym = FindISymbol( name + PREFIX_LEN );
    if( sym == NULL )
        return( false );
    if( IS_SYM_IMPORTED( sym ) )
        return( false );
    LnkMsg( WRN+MSG_IMPORT_LOCAL, "s", sym->name.u.ptr );
    iatsym->info |= SYM_DEFINED | SYM_DCE_REF;
    if( LinkFlags & LF_STRIP_CODE ) {
        DefStripImpSym( iatsym );
    }
    AddPEImportLocalSym( sym, iatsym );
    return( true );
}

void FreePELocalImports( void )
/*****************************/
{
    FreeList( PELocalImpList );
    PELocalImpList = NULL;
    NumLocalImports = 0;
}

#endif
