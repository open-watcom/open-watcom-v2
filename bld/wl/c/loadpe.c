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
#include "exerespe.h"
#include "impexp.h"
#include "toc.h"
#include "objstrip.h"
#include "rcrtns.h"
#include "pass2l1.h"
#include "sharedio.h"
#include "exeutil.h"
#include "newmem.h"

#include "clibext.h"

#define I386_TRANSFER_OP1       0xff    /* first byte of a "JMP [FOO]" */
#define I386_TRANSFER_OP2       0x25    /* second byte of a "JMP [FOO]" */

#define MINIMUM_SEG_SHIFT       2       /* Corresponds to 2^2 == 4 bytes */
#define DEFAULT_SEG_SHIFT       9       /* Corresponds to 2^9 == 512 bytes */

#define STUB_ALIGN 8    /* for PE format */

#include "pushpck1.h"

typedef struct {
    unsigned_8  op1;
    unsigned_8  op2;
    unsigned_32 dest;
} i386_transfer;

static i386_transfer    I386Jump = { I386_TRANSFER_OP1, I386_TRANSFER_OP2, 0 };

#define I386_TRANSFER_SIZE (sizeof(i386_transfer))

#define ALPHA_TRANSFER_OP1      0x277F
#define ALPHA_TRANSFER_OP2      0xA37B
#define ALPHA_TRANSFER_OP3      0x6BFB

typedef struct {
    unsigned_16 high;
    unsigned_16 op1;
    unsigned_16 low;
    unsigned_16 op2;
    unsigned_16 zero;
    unsigned_16 op3;
} alpha_transfer;

#include "poppck.h"

typedef struct local_import {
    struct local_import *next;
    symbol              *iatsym;
    symbol              *locsym;
} local_import;

static alpha_transfer   AlphaJump = {   0, ALPHA_TRANSFER_OP1,
                                        0, ALPHA_TRANSFER_OP2,
                                        0, ALPHA_TRANSFER_OP3 };

#define ALPHA_TRANSFER_SIZE (sizeof(alpha_transfer))

static unsigned_32 PPCJump[]= {
    0x81620000,         //   lwz        r11,[tocv]__imp_RtlMoveMemory(rtoc)
    0x818B0000,         //   lwz        r12,(r11)
    0x90410004,         //   stw        rtoc,0x4(sp)
    0x7D8903A6,         //   mtctr      r12
    0x804B0004,         //   lwz        rtoc,0x4(r11)
    0x4E800420          //   bctr
};

#define PPC_TRANSFER_SIZE (sizeof(PPCJump))

#define X64_TRANSFER_OP1    0xff    /* first byte of a "JMP [FOO]" */
#define X64_TRANSFER_OP2    0x25    /* second byte of a "JMP [FOO]" */

#include "pushpck1.h"

typedef struct {
    unsigned_8  op1;
    unsigned_8  op2;
    unsigned_64 dest;
} x64_transfer;

static x64_transfer    X64Jump = { X64_TRANSFER_OP1, X64_TRANSFER_OP2, {0} };

#include "poppck.h"

#define X64_TRANSFER_SIZE (sizeof(x64_transfer))

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
    IData.ilt_off = (NumMods + 1) * sizeof( pe_import_directory );
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
                size += (size & 1); /* round up */
                size += imp->imp->len + sizeof( unsigned_16 ) + sizeof( unsigned_8 );
            }
        }
    }
    IData.total_size = size;
    return( IData.total_size );
}

void ResetLoadPE( void )
/*****************************/
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
/********************************************************/
{
    if( dll != NULL) { // if not end of mod marker
        dll->iatsym->addr.off = *off;
        dll->iatsym->addr.seg = 0;
    }
    *off += sizeof( pe_va );
}

static void XFerReloc( offset off, group_entry *group, unsigned type )
/********************************************************************/
{
    reloc_item  reloc;
    size_t      size;

    size = sizeof( pe_reloc_item );
    reloc.pe = (( off + group->linear ) & OSF_PAGE_MASK) | type;
    if( type == PE_FIX_HIGHADJ ) {
        size = sizeof( high_pe_reloc_item );
        reloc.hpe.low_off = AlphaJump.low;
    }
    WriteReloc( group, off, &reloc, size );
}

static int GetTransferGlueSize( int lnk_state )
/*********************************************/
{
    switch( lnk_state & HAVE_MACHTYPE_MASK ) {
    case HAVE_ALPHA_CODE:   return( ALPHA_TRANSFER_SIZE );
    case HAVE_I86_CODE:     return( I386_TRANSFER_SIZE );
    case HAVE_X64_CODE:     return( X64_TRANSFER_SIZE ); // TODO
    case HAVE_PPC_CODE:     return( PPC_TRANSFER_SIZE );
    default:                DbgAssert( 0 ); return( 0 );
    }
}

static void *GetTransferGlueCode( int lnk_state )
/************************************************/
{
    switch( lnk_state & HAVE_MACHTYPE_MASK ) {
    case HAVE_ALPHA_CODE:   return( &AlphaJump );
    case HAVE_I86_CODE:     return( &I386Jump );
    case HAVE_X64_CODE:     return( &X64Jump ); // TODO
    case HAVE_PPC_CODE:     return( &PPCJump );
    default:                DbgAssert( 0 ); return( NULL );
    }
}

offset FindIATSymAbsOff( symbol *sym )
/********************************************/
{
    dll_sym_info        *dll;

    dll = sym->p.import;
    DbgAssert( IS_SYM_IMPORTED( sym ) && dll != NULL );
    return( dll->iatsym->addr.off );
}

offset FindSymPosInTocv( symbol *sym )
/***********************************************/
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
        datalen = GetTransferGlueSize( LinkState );
        data = GetTransferGlueCode( LinkState );
        WALK_IMPORT_SYMBOLS( sym ) {
            if( LinkState & HAVE_ALPHA_CODE ) {
                offset dest = FindIATSymAbsOff( sym );
                AlphaJump.high = dest >> 16;
                AlphaJump.low = dest;
                if( LinkState & MAKE_RELOCS ) {
                    if( (FmtData.objalign & 0xFFFF) == 0 ) {
                        XFerReloc( sym->addr.off + offsetof( alpha_transfer, high ),
                                   group, PE_FIX_HIGH );
                    } else {
                        XFerReloc( sym->addr.off + offsetof( alpha_transfer, low ),
                                   group, PE_FIX_LOW );
                        XFerReloc( sym->addr.off + offsetof( alpha_transfer, high ),
                                    group, PE_FIX_HIGHADJ );
                    }
                }
            } else if( LinkState & HAVE_I86_CODE ) {
                offset dest = FindIATSymAbsOff( sym );
                I386Jump.dest = dest;
                if( LinkState & MAKE_RELOCS ) {
                    XFerReloc( sym->addr.off + offsetof( i386_transfer, dest ),
                                group, PE_FIX_HIGHLOW );
                }
            } else if( LinkState & HAVE_X64_CODE ) {
                // TODO
                offset dest = FindIATSymAbsOff( sym );
                X64Jump.dest.u._32[0] = dest;
                X64Jump.dest.u._32[1] = 0;
                if( LinkState & MAKE_RELOCS ) {
                    XFerReloc( sym->addr.off + offsetof( x64_transfer, dest ), group, PE_FIX_HIGHLOW );
                }
            } else {
                PPCJump[0] = (PPCJump[0] & 0xffff0000) | (FindSymPosInTocv( sym ) & 0x0000ffff);
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
    if( LinkState & MAKE_RELOCS ) {
        for( loc_imp = PELocalImpList; loc_imp != NULL; loc_imp = loc_imp->next ) {
            XFerReloc( loc_imp->iatsym->addr.off, group, PE_FIX_HIGHLOW );
        }
    }
    group->size = group->totalsize;
}

unsigned_32 DefStackSizePE( void )
{
    return( PE_DEF_STACK_SIZE );
}

static unsigned_32 WriteDataPages( exe_pe_header *h, pe_object *object, unsigned_32 file_align )
/**********************************************************************************************/
/* write the enumerated data pages */
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
        if( group->totalsize == 0 ) continue;   // DANGER DANGER DANGER <--!!!
        name = group->sym->name;
        if( name == NULL || name[0] == 0 ) {
            leader = Ring2First( group->leaders );
            name = leader->segname;
            if( name == NULL ) name = "";
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
        linear = ROUND_UP( size_v, FmtData.objalign );
        linear += group->linear;
        if( StartInfo.addr.seg == group->grp_addr.seg ) {
            entry_rva = group->linear + StartInfo.addr.off;
        }
        object->rva = group->linear;
        /*
        //  Why weren't we filling in this field? MS do!
        */
        object->virtual_size = size_v;
        object->physical_size = ROUND_UP( size_ph, file_align );

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
            WriteGroupLoad( group );
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
    if( LinkState & HAVE_X64_CODE ) {
        PE64( *h ).code_base = code_base;
        PE64( *h ).entry_rva = entry_rva;
        PE64( *h ).init_data_size = init_data_size;
        PE64( *h ).code_size = code_size;
    } else {
        PE32( *h ).code_base = code_base;
        PE32( *h ).data_base = data_base;
        PE32( *h ).entry_rva = entry_rva;
        PE32( *h ).init_data_size = init_data_size;
        PE32( *h ).code_size = code_size;
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
                hint_rva += (hint_rva & 1); /* round up */
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
        XFerReloc( pos, group, PE_FIX_HIGHLOW );
    }
    WriteIAT( buf + IData.iat_off, linear ); // Import Address table
    pos = IData.mod_name_off;            /* write the module names */
    for( mod = PEImpList; mod != NULL; mod = mod->next ) {
        int name_size = mod->mod->len + 1;
        PutInfo( buf + pos, mod->mod->name, name_size );
        pos += name_size;
    }
    pos = IData.hint_off;        /* write out the import names */
    for( mod = PEImpList; mod != NULL; mod = mod->next ) {
        hint = 1;
        for( imp = mod->imports; imp != NULL; imp = imp->next ) {
            if( imp->imp != NULL ) {
                PutInfoNulls( buf + pos, pos & 1 );
                pos += pos & 1;/* round up */
                PutInfo( buf + pos, &hint, sizeof( hint ) );
                pos += sizeof( hint );
                size = imp->imp->len;
                PutInfo( buf + pos, imp->imp->name, size );
                pos += size;
                PutInfoNulls( buf + pos, 1);
                pos++;
                hint++;
            }
        }
    }
}

static int namecmp( const void *pn1, const void *pn2 )
/****************************************************/
{
    entry_export        *n1;
    entry_export        *n2;

    n1 = *(entry_export **)pn1;
    n2 = *(entry_export **)pn2;
    return( strcmp( n1->name, n2->name ) );
}


static unsigned_32 WriteExportInfo( pe_object *object, unsigned_32 file_align, pe_hdr_table_entry *table )
/********************************************************************************************************/
{
    unsigned_32         size;
    pe_export_directory dir;
    const char          *name;
    size_t              namelen;
    entry_export        **sort;
    entry_export        *exp;
    unsigned            i;
    unsigned_16         ordinal;
    pe_va               eat;
    ordinal_t           next_ord;
    ordinal_t           high_ord = 0;
    unsigned            num_entries;

    strncpy( object->name, ".edata", PE_OBJ_NAME_LEN );
    object->physical_offset = NullAlign( file_align );
    object->flags = PE_OBJ_INIT_DATA | PE_OBJ_READABLE;
    dir.flags = 0;
    dir.time_stamp = 0;
    dir.major = 0;
    dir.minor = 0;
    dir.name_rva = object->rva + sizeof( dir );
    dir.ordinal_base = FmtData.u.os2.exports->ordinal;
    if( FmtData.u.os2.res_module_name != NULL ) {
        name = FmtData.u.os2.res_module_name;
    } else {
        name = GetBaseName( Root->outfile->fname, 0, &namelen );
    }
    /* GetBaseName strips the extension, which we actually need to keep!
     * Always recalculate the len including the extension.
     */
    namelen = strlen( name ) + 1;
    dir.address_table_rva = ROUND_UP( dir.name_rva + namelen, sizeof( pe_va ) );
    num_entries = 0;
    for( exp = FmtData.u.os2.exports; exp != NULL; exp = exp->next ) {
        high_ord = exp->ordinal;
        ++num_entries;
        if( !exp->isprivate ) {
            if( exp->impname != NULL ) {
                AddImpLibEntry( exp->impname, exp->name, NOT_IMP_BY_ORDINAL );
            } else {
                AddImpLibEntry( exp->sym->name, exp->name, NOT_IMP_BY_ORDINAL );
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
    for( exp = FmtData.u.os2.exports; exp != NULL; exp = exp->next ) {
        sort[i++] = exp;
        eat = exp->addr.off;
        if( next_ord < exp->ordinal ) {
            PadLoad( (exp->ordinal - next_ord) * sizeof( pe_va ) );
        }
        next_ord = exp->ordinal + 1;
        WriteLoad( &eat, sizeof( eat ) );
    }
    qsort( sort, num_entries, sizeof( entry_export * ), &namecmp );
    /* write out the export name ptr table */
    eat = dir.ordinal_table_rva + num_entries * sizeof( unsigned_16 );
    for( i = 0; i < num_entries; ++i ) {
        exp = sort[i];
        WriteLoad( &eat, sizeof( eat ) );
        eat += strlen( exp->name ) + 1;
    }
    /* write out the export ordinal table */
    for( i = 0; i < num_entries; ++i ) {
        ordinal = sort[i]->ordinal - dir.ordinal_base;
        WriteLoad( &ordinal, sizeof( ordinal ) );
    }
    /* write out the export name table */
    for( i = 0; i < num_entries; ++i ) {
        exp = sort[i];
        WriteLoad( exp->name, strlen( exp->name ) + 1 );
    }
    _LnkFree( sort );
    size = eat - object->rva;
    object->physical_size = ROUND_UP( size, file_align );
    table[PE_TBL_EXPORT].size = size;
    table[PE_TBL_EXPORT].rva = object->rva;
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
            WriteLoad( &pagerva, sizeof( unsigned_32 ) );
            WriteLoad( &pagesize, sizeof( unsigned_32 ) );
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

static unsigned_32 WriteFixupInfo( pe_object *object, unsigned_32 file_align, pe_hdr_table_entry *table )
/*******************************************************************************************************/
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
    object->physical_size = ROUND_UP( size, file_align );
    table[PE_TBL_FIXUP].size = size - sizeof( pe_fixup_header );
    table[PE_TBL_FIXUP].rva = object->rva;
    return( size );
}

static unsigned_32 WriteDescription( pe_object *object, unsigned_32 file_align )
/******************************************************************************/
{
    size_t      desc_len;

    desc_len = strlen( FmtData.u.os2.description );
    strncpy( object->name, ".desc", PE_OBJ_NAME_LEN );
    object->physical_offset = NullAlign( file_align );
    object->flags = PE_OBJ_INIT_DATA | PE_OBJ_READABLE;
    object->physical_size = ROUND_UP( desc_len, file_align );
    WriteLoad( FmtData.u.os2.description, desc_len );
    return( desc_len );
}

WResFileSSize  RcWrite( WResFileID hdl, const void *buf, WResFileSize len )
{
    hdl = hdl;
    WriteLoad( (void *) buf, len );
    return( len );
}

WResFileOffset RcSeek( WResFileID hdl, WResFileOffset off, int pos )
{
    DbgAssert( pos != SEEK_END );
    DbgAssert( !(pos == SEEK_CUR && off < 0) );

    if( hdl == Root->outfile->handle ) {
        if( pos == SEEK_CUR ) {
            unsigned long   old_pos;

            old_pos = PosLoad();
            PadLoad( off );
            return( old_pos + off );
        } else {
            SeekLoad( off );
            return( off );
        }
    } else {
        return( QLSeek( hdl, off, pos, "resource file" ) );
    }
}

WResFileOffset RcTell( WResFileID hdl )
{
    DbgAssert( hdl == Root->outfile->handle );

    hdl = hdl;
    return( PosLoad() );
}

bool RcPadFile( int handle, long pad )
{
    DbgAssert( handle == Root->outfile->handle );

    handle = handle;
    PadLoad( pad );
    return( false );
}

void CheckDebugOffset( ExeFileInfo *info )
{
    info = info;
}

RcStatus CopyExeData( int inhandle, int outhandle, uint_32 length )
/*****************************************************************/
{
    outhandle = outhandle;
    for( ; length > MAX_HEADROOM; length -= MAX_HEADROOM ) {
        QRead( inhandle, TokBuff, MAX_HEADROOM, "resource file" );
        WriteLoad( TokBuff, MAX_HEADROOM );
    }
    if( length > 0 ) {
        QRead( inhandle, TokBuff, length, "resource file" );
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

static unsigned_32 WritePEResources( exe_pe_header *h, pe_object *object, unsigned_32 file_align )
/************************************************************************************************/
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
    einfo.IsOpen = true;
    einfo.Handle = Root->outfile->handle;
    einfo.name = Root->outfile->fname;
    einfo.u.PEInfo.WinHead = h;
    einfo.Type = EXE_TYPE_PE;
    status = BuildPEResourceObject( &einfo, rinfo, object, object->rva, NullAlign( file_align ), !allopen );
    CloseResFiles( rinfo );
    return( object->physical_size );
}

static unsigned_32 WriteDebugTable( pe_object *object, const char *symfilename,
                unsigned_32 file_align, unsigned_32 time_stamp, pe_hdr_table_entry *table )
/******************************************************************************************/
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
    object->physical_size = ROUND_UP( size, file_align);

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

    table[PE_TBL_DEBUG].size = size;
    table[PE_TBL_DEBUG].rva = object->rva;
    return( size );
}

static void CheckNumRelocs( void )
/********************************/
// don't want to generate a .reloc section if we don't have any relocs
{
    group_entry *group;
    symbol      *sym;

    if( (LinkState & MAKE_RELOCS) == 0 )
        return;
    for( group = Groups; group != NULL; group = group->next_group ) {
        if( group->g.grp_relocs != NULL ) {
            return;
        }
    }
    WALK_IMPORT_SYMBOLS( sym ) {
        if( LinkState & HAVE_MACHTYPE_MASK ) {
            return;
        }
    }
    LinkState &= ~MAKE_RELOCS;
}

static seg_leader *SetLeaderTable( const char *name, pe_hdr_table_entry *entry )
/******************************************************************************/
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

static void SetMiscTableEntries( pe_hdr_table_entry *table )
/**********************************************************/
{
    seg_leader  *leader;
    virt_mem    *sortarray;
    virt_mem    *temp;
    unsigned    numpdatas;
    symbol      *sym;

    SetLeaderTable( IDataGrpName, &table[PE_TBL_IMPORT] );
    sym = FindISymbol( TLSSym );
    if( sym != NULL ) {
        table[PE_TBL_THREAD].rva = FindLinearAddr( &sym->addr );
        table[PE_TBL_THREAD].size = sym->p.seg->length;
    }
    leader = SetLeaderTable( CoffPDataSegName, &table[PE_TBL_EXCEPTION] );
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
    if( LinkState & MAKE_RELOCS )
        ++num_objects;
    if( FmtData.u.os2.exports != NULL )
        ++num_objects;
    if( LinkFlags & CV_DBI_FLAG )
        ++num_objects;
    if( FmtData.u.os2.description != NULL )
        ++num_objects;
    if( FmtData.resource != NULL || FmtData.u.pe.resources != NULL )
        ++num_objects;
    return( num_objects );
}

static unsigned long CalcPEChecksum( unsigned long dwInitialCount, unsigned short *pwBuffer, unsigned long dwWordCount )
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
    exe_pe_header   h;
    unsigned_32     stub_len;
    pe_object       *object;
    unsigned        num_objects;
    pe_object       *tbl_obj;
    unsigned        head_size;
    unsigned_32     file_align;
    unsigned_32     size;
    unsigned_32     image_size;

    file_align = 1UL << FmtData.u.os2.segment_shift;
    CheckNumRelocs();
    num_objects = FindNumObjects();
    memset( &h, 0, sizeof( h ) ); /* zero all header fields */
    if( LinkState & HAVE_X64_CODE ) {
        head_size = sizeof( pe_header64 );
        PE64( h ).magic = 0x20b;
        if( FmtData.u.pe.signature != 0 ) {
            PE64( h ).signature = FmtData.u.pe.signature;
        } else {
            PE64( h ).signature = PE_SIGNATURE;
        }
        PE64( h ).cpu_type = PE_CPU_AMD64;
        PE64( h ).num_objects = num_objects;
        PE64( h ).time_stamp = time( NULL );
        PE64( h ).nt_hdr_size = head_size - offsetof( pe_header64, flags ) - sizeof( PE64( h ).flags );
        PE64( h ).flags = PE_FLG_REVERSE_BYTE_LO | PE_FLG_32BIT_MACHINE | PE_FLG_LARGE_ADDRESS_AWARE;
        if( FmtData.u.pe.nolargeaddressaware ) {
            PE64( h ).flags &= ~PE_FLG_LARGE_ADDRESS_AWARE;
        }
        if( (LinkState & MAKE_RELOCS) == 0 ) {
            PE64( h ).flags |= PE_FLG_RELOCS_STRIPPED;
        }
        if( (LinkState & LINK_ERROR) == 0 ) {
            PE64( h ).flags |= PE_FLG_IS_EXECUTABLE;
        }
        if( FmtData.dll ) {
            PE64( h ).flags |= PE_FLG_LIBRARY;
            if( FmtData.u.os2.flags & INIT_INSTANCE_FLAG ) {
                PE64( h ).dll_flags |= PE_DLL_PERPROC_INIT;
            } else if( FmtData.u.os2.flags & INIT_THREAD_FLAG ) {
                PE64( h ).dll_flags |= PE_DLL_PERTHRD_INIT;
            }
            if( FmtData.u.os2.flags & TERM_INSTANCE_FLAG ) {
                PE64( h ).dll_flags |= PE_DLL_PERPROC_TERM;
            } else if( FmtData.u.os2.flags & TERM_THREAD_FLAG ) {
                PE64( h ).dll_flags |= PE_DLL_PERTHRD_TERM;
            }
        }

        if( FmtData.u.pe.lnk_specd ) {
            PE64( h ).lnk_major = FmtData.u.pe.linkmajor;
            PE64( h ).lnk_minor = FmtData.u.pe.linkminor;
        } else {
            PE64( h ).lnk_major = PE_LNK_MAJOR;
            PE64( h ).lnk_minor = PE_LNK_MINOR;
        }
        PE64( h ).image_base.u._32[0] = FmtData.base;
        PE64( h ).image_base.u._32[1] = 0;
        PE64( h ).object_align = FmtData.objalign;

        /*
         *  I have changed this to allow programmers to control this shift. MS has 0x20 byte segments
         *  in some drivers! Who are we to argue? Never mind it's against the PE spec.
         */
        if( FmtData.u.os2.segment_shift < MINIMUM_SEG_SHIFT ) {
            LnkMsg( WRN+MSG_VALUE_INCORRECT, "s", "alignment" );
            FmtData.u.os2.segment_shift = DEFAULT_SEG_SHIFT;
        }

        PE64( h ).file_align = file_align;

        if( FmtData.u.pe.osv_specd ) {
            PE64( h ).os_major = FmtData.u.pe.osmajor;
            PE64( h ).os_minor = FmtData.u.pe.osminor;
        } else {
            PE64( h ).os_major = PE_OS_MAJOR;
            PE64( h ).os_minor = PE_OS_MINOR + 0xb;      // KLUDGE!
        }

        PE64( h ).user_major = FmtData.major;
        PE64( h ).user_minor = FmtData.minor;
        if( FmtData.u.pe.sub_specd ) {
            PE64( h ).subsys_major = FmtData.u.pe.submajor;
            PE64( h ).subsys_minor = FmtData.u.pe.subminor;
        } else {
            PE64( h ).subsys_major = 3;
            PE64( h ).subsys_minor = 0xa;
        }
        if( FmtData.u.pe.subsystem != PE_SS_UNKNOWN ) {
            PE64( h ).subsystem = FmtData.u.pe.subsystem;
        } else {
            PE64( h ).subsystem = PE_SS_WINDOWS_GUI;
        }
        PE64( h ).stack_reserve_size.u._32[0] = StackSize;
        PE64( h ).stack_reserve_size.u._32[1] = 0;
        if( FmtData.u.pe.stackcommit == DEF_VALUE ) {
            PE64( h ).stack_commit_size.u._32[0] = StackSize;
            PE64( h ).stack_commit_size.u._32[1] = 0;
            if( StackSize > PE_DEF_STACK_COMMIT ) {
                PE64( h ).stack_commit_size.u._32[0] = PE_DEF_STACK_COMMIT;
            }
        } else if( FmtData.u.pe.stackcommit > StackSize ) {
            PE64( h ).stack_commit_size.u._32[0] = StackSize;
            PE64( h ).stack_commit_size.u._32[1] = 0;
        } else {
            PE64( h ).stack_commit_size.u._32[0] = FmtData.u.pe.stackcommit;
            PE64( h ).stack_commit_size.u._32[1] = 0;
        }
        PE64( h ).heap_reserve_size.u._32[0] = FmtData.u.os2.heapsize;
        PE64( h ).heap_reserve_size.u._32[1] = 0;
        if( FmtData.u.pe.heapcommit > FmtData.u.os2.heapsize ) {
            PE64( h ).heap_commit_size.u._32[0] = FmtData.u.os2.heapsize;
            PE64( h ).heap_commit_size.u._32[1] = 0;
        } else {
            PE64( h ).heap_commit_size.u._32[0] = FmtData.u.pe.heapcommit;
            PE64( h ).heap_commit_size.u._32[1] = 0;
        }
        PE64( h ).num_tables = PE_TBL_NUMBER;
        CurrSect = Root;
        SeekLoad( 0 );
        stub_len = Write_Stub_File( STUB_ALIGN );
        _ChkAlloc( object, num_objects * sizeof( pe_object ) );
        memset( object, 0, num_objects * sizeof( pe_object ) );
        /* leave space for the header and object table */
        PadLoad( head_size + num_objects * sizeof( pe_object ) );
        GenPETransferTable();
        WriteImportInfo();
        SetMiscTableEntries( PE64( h ).table );
        image_size = WriteDataPages( &h, object, file_align );
        tbl_obj = &object[NumGroups];
        if( FmtData.u.os2.exports != NULL ) {
            tbl_obj->rva = image_size;
            size = WriteExportInfo( tbl_obj, file_align, PE64( h ).table );
            image_size += ROUND_UP( size, FmtData.objalign );
            ++tbl_obj;
        }
        if( LinkState & MAKE_RELOCS ) {
            tbl_obj->rva = image_size;
            size = WriteFixupInfo( tbl_obj, file_align, PE64( h ).table );
            image_size += ROUND_UP( size, FmtData.objalign );
            ++tbl_obj;
        }
        if( FmtData.u.os2.description != NULL ) {
            tbl_obj->rva = image_size;
            size = WriteDescription( tbl_obj, file_align );
            image_size += ROUND_UP( size, FmtData.objalign );
            ++tbl_obj;
        }
        if( FmtData.resource || FmtData.u.pe.resources != NULL ) {
            tbl_obj->rva = image_size;
            size = WritePEResources( &h, tbl_obj, file_align );
            image_size += ROUND_UP( size, FmtData.objalign );
            ++tbl_obj;
        }
        if( LinkFlags & CV_DBI_FLAG ) {
            tbl_obj->rva = image_size;
            size = WriteDebugTable( tbl_obj, SymFileName, file_align, PE64( h ).time_stamp, PE64( h ).table );
            image_size += ROUND_UP( size, FmtData.objalign );
            ++tbl_obj;
        }
        NullAlign( file_align ); /* pad out last page */
        PE64( h ).image_size = image_size;
        PE64( h ).header_size = object->physical_offset;
    } else {
        head_size = sizeof( pe_header );
        PE32( h ).magic = 0x10b;
        if( FmtData.u.pe.signature != 0 ) {
            PE32( h ).signature = FmtData.u.pe.signature;
        } else {
            PE32( h ).signature = PE_SIGNATURE;
        }
        if( LinkState & HAVE_I86_CODE ) {
            PE32( h ).cpu_type = PE_CPU_386;
        } else if( LinkState & HAVE_ALPHA_CODE ) {
            PE32( h ).cpu_type = PE_CPU_ALPHA;
        } else {
            PE32( h ).cpu_type = PE_CPU_POWERPC;
        }
        PE32( h ).num_objects = num_objects;
        PE32( h ).time_stamp = time( NULL );
        PE32( h ).nt_hdr_size = head_size - offsetof( pe_header, flags ) - sizeof( PE32( h ).flags );
        PE32( h ).flags = PE_FLG_REVERSE_BYTE_LO | PE_FLG_32BIT_MACHINE;
        if( FmtData.u.pe.largeaddressaware == 1 ) {
            PE32( h ).flags |= PE_FLG_LARGE_ADDRESS_AWARE;
        }
        if( (LinkState & MAKE_RELOCS) == 0 ) {
            PE32( h ).flags |= PE_FLG_RELOCS_STRIPPED;
        }
        if( (LinkState & LINK_ERROR) == 0 ) {
            PE32( h ).flags |= PE_FLG_IS_EXECUTABLE;
        }
        if( FmtData.dll ) {
            PE32( h ).flags |= PE_FLG_LIBRARY;
            if( FmtData.u.os2.flags & INIT_INSTANCE_FLAG ) {
                PE32( h ).dll_flags |= PE_DLL_PERPROC_INIT;
            } else if( FmtData.u.os2.flags & INIT_THREAD_FLAG ) {
                PE32( h ).dll_flags |= PE_DLL_PERTHRD_INIT;
            }
            if( FmtData.u.os2.flags & TERM_INSTANCE_FLAG ) {
                PE32( h ).dll_flags |= PE_DLL_PERPROC_TERM;
            } else if( FmtData.u.os2.flags & TERM_THREAD_FLAG ) {
                PE32( h ).dll_flags |= PE_DLL_PERTHRD_TERM;
            }
        }

        if( FmtData.u.pe.lnk_specd ) {
            PE32( h ).lnk_major = FmtData.u.pe.linkmajor;
            PE32( h ).lnk_minor = FmtData.u.pe.linkminor;
        } else {
            PE32( h ).lnk_major = PE_LNK_MAJOR;
            PE32( h ).lnk_minor = PE_LNK_MINOR;
        }
        PE32( h ).image_base = FmtData.base;
        PE32( h ).object_align = FmtData.objalign;

        /*
         *  I have changed this to allow programmers to control this shift. MS has 0x20 byte segments
         *  in some drivers! Who are we to argue? Never mind it's against the PE spec.
         */
        if( FmtData.u.os2.segment_shift < MINIMUM_SEG_SHIFT ) {
            LnkMsg( WRN+MSG_VALUE_INCORRECT, "s", "alignment" );
            FmtData.u.os2.segment_shift = DEFAULT_SEG_SHIFT;
        }

        file_align = 1UL << FmtData.u.os2.segment_shift;
        PE32( h ).file_align = file_align;

        if( FmtData.u.pe.osv_specd ) {
            PE32( h ).os_major = FmtData.u.pe.osmajor;
            PE32( h ).os_minor = FmtData.u.pe.osminor;
        } else {
            PE32( h ).os_major = PE_OS_MAJOR;
            PE32( h ).os_minor = PE_OS_MINOR + 0xb;      // KLUDGE!
        }

        PE32( h ).user_major = FmtData.major;
        PE32( h ).user_minor = FmtData.minor;
        if( FmtData.u.pe.sub_specd ) {
            PE32( h ).subsys_major = FmtData.u.pe.submajor;
            PE32( h ).subsys_minor = FmtData.u.pe.subminor;
        } else {
            PE32( h ).subsys_major = 3;
            PE32( h ).subsys_minor = 0xa;
        }
        if( FmtData.u.pe.subsystem != PE_SS_UNKNOWN ) {
            PE32( h ).subsystem = FmtData.u.pe.subsystem;
        } else {
            PE32( h ).subsystem = PE_SS_WINDOWS_GUI;
        }
        PE32( h ).stack_reserve_size = StackSize;
        if( FmtData.u.pe.stackcommit == DEF_VALUE ) {
            PE32( h ).stack_commit_size = StackSize;
            if( StackSize > PE_DEF_STACK_COMMIT ) {
                PE32( h ).stack_commit_size = PE_DEF_STACK_COMMIT;
            }
        } else if( FmtData.u.pe.stackcommit > StackSize ) {
            PE32( h ).stack_commit_size = StackSize;
        } else {
            PE32( h ).stack_commit_size = FmtData.u.pe.stackcommit;
        }
        PE32( h ).heap_reserve_size = FmtData.u.os2.heapsize;
        if( FmtData.u.pe.heapcommit > FmtData.u.os2.heapsize ) {
            PE32( h ).heap_commit_size = FmtData.u.os2.heapsize;
        } else {
            PE32( h ).heap_commit_size = FmtData.u.pe.heapcommit;
        }
        PE32( h ).num_tables = PE_TBL_NUMBER;
        CurrSect = Root;
        SeekLoad( 0 );
        stub_len = Write_Stub_File( STUB_ALIGN );
        _ChkAlloc( object, num_objects * sizeof( pe_object ) );
        memset( object, 0, num_objects * sizeof( pe_object ) );
        /* leave space for the header and object table */
        PadLoad( head_size + num_objects * sizeof( pe_object ) );
        GenPETransferTable();
        WriteImportInfo();
        SetMiscTableEntries( PE32( h ).table );
        image_size = WriteDataPages( &h, object, file_align );
        tbl_obj = &object[NumGroups];
        if( FmtData.u.os2.exports != NULL ) {
            tbl_obj->rva = image_size;
            size = WriteExportInfo( tbl_obj, file_align, PE32( h ).table );
            image_size += ROUND_UP( size, FmtData.objalign );
            ++tbl_obj;
        }
        if( LinkState & MAKE_RELOCS ) {
            tbl_obj->rva = image_size;
            size = WriteFixupInfo( tbl_obj, file_align, PE32( h ).table );
            image_size += ROUND_UP( size, FmtData.objalign );
            ++tbl_obj;
        }
        if( FmtData.u.os2.description != NULL ) {
            tbl_obj->rva = image_size;
            size = WriteDescription( tbl_obj, file_align );
            image_size += ROUND_UP( size, FmtData.objalign );
            ++tbl_obj;
        }
        if( FmtData.resource || FmtData.u.pe.resources != NULL ) {
            tbl_obj->rva = image_size;
            size = WritePEResources( &h, tbl_obj, file_align );
            image_size += ROUND_UP( size, FmtData.objalign );
            ++tbl_obj;
        }
        if( LinkFlags & CV_DBI_FLAG ) {
            tbl_obj->rva = image_size;
            size = WriteDebugTable( tbl_obj, SymFileName, file_align, PE32( h ).time_stamp, PE32( h ).table );
            image_size += ROUND_UP( size, FmtData.objalign );
            ++tbl_obj;
        }
        NullAlign( file_align ); /* pad out last page */
        PE32( h ).image_size = image_size;
        PE32( h ).header_size = object->physical_offset;
    }
    DBIWrite();
    SeekLoad( stub_len );

    if( FmtData.u.pe.checksumfile ) {
        /* Ensure checksum is 0 before we calculate it */
        if( LinkState & HAVE_X64_CODE ) {
            PE64( h ).file_checksum = 0L;
        } else {
            PE32( h ).file_checksum = 0L;
        }
    }

    WriteLoad( &h, head_size );
    WriteLoad( object, num_objects * sizeof( pe_object ) );

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

#define CRC_BUFF_SIZE   (16 * 1024)
        _ChkAlloc( buffer, CRC_BUFF_SIZE );

        if( buffer ) {
            for( ; currpos < totalsize; currpos += buffsize ) {
                memset( buffer, 0, CRC_BUFF_SIZE );
                buffsize = QRead( outfile->handle, buffer, CRC_BUFF_SIZE, outfile->fname );
                DbgAssert( ( buffsize % 2 ) != 1 ); /* check for odd length */

                crc = CalcPEChecksum( crc, (unsigned short *)buffer, buffsize / sizeof( unsigned short ) );
            }

            _LnkFree( buffer );
            crc += totalsize;

            if( LinkState & HAVE_X64_CODE ) {
                PE64( h ).file_checksum = crc;
            } else {
                PE32( h ).file_checksum = crc;
            }
            SeekLoad( stub_len );
            WriteLoad( &h, head_size );
        }
    }

    _LnkFree( object );
}

unsigned long GetPEHeaderSize( void )
/******************************************/
{
    unsigned long       size;
    unsigned            num_objects;

    num_objects = FindNumObjects();
    size = ROUND_UP( GetStubSize(), STUB_ALIGN ) + num_objects * sizeof( pe_object );
    if( LinkState & HAVE_X64_CODE ) {
        size += sizeof( pe_header64 );
    } else {
        size += sizeof( pe_header );
    }
    return( ROUND_UP( size, FmtData.objalign ) );
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
        CheckExport( nameptr, *ordptr + ord_base, &strcmp );
        while( *nameptr != '\0' )
            nameptr++;
        nameptr++;
        ordptr++;
    }
    _LnkFree( ordbuf );
}

void ReadPEExportTable( f_handle file, pe_hdr_table_entry *base )
/***********************************************************************/
/* read a PE export table, and set ordinal values accordingly. */
{
    pe_export_directory table;
    char                *fname;
    unsigned_32         *nameptrs;
    unsigned            nameptrsize;
    unsigned            numentries;
    unsigned_32         entrystart;
    unsigned_32         *curr;
    unsigned_32         namestart;

    fname = FmtData.u.os2.old_lib_name;
    QRead( file, &table, sizeof( pe_export_directory ), fname );
    nameptrsize = table.num_name_ptrs * sizeof( unsigned_32 );
    if( nameptrsize == 0 )                      /* NOTE: <-- premature return */
        return;
    _ChkAlloc( nameptrs, nameptrsize + sizeof( unsigned_32 ) );
    QSeek( file, table.name_ptr_table_rva - base->rva, fname );
    QRead( file, nameptrs, nameptrsize, fname );
    numentries = 1;
    entrystart = table.ordinal_table_rva - base->rva;
    curr = nameptrs;
    *curr -= base->rva;
    namestart = *curr++;
    for( nameptrsize -= sizeof( unsigned_32 ); nameptrsize > 0; nameptrsize -= sizeof( unsigned_32 ) ) {
        *curr -= base->rva;
        if( *curr - namestart > TokSize ) {
            ReadExports( namestart, *(curr - 1), entrystart, numentries, table.ordinal_base, file, fname );
            entrystart += numentries * sizeof( unsigned_16 );
            numentries = 1;
            namestart = *(curr - 1);
        }
        numentries++;
        curr++;
    }   /* NOTE! this assumes the name table is at the end */
    ReadExports( namestart, base->size + *nameptrs, entrystart, numentries, table.ordinal_base, file, fname );
    _LnkFree( nameptrs );
}

static void CreateIDataSection( void )
/************************************/
{
    segdata     *sdata;
    class_entry *class;

    PrepareToc();
    if( 0 != CalcIDataSize() ) {
        IDataGroup = GetGroup( IDataGrpName );
        class = FindClass( Root, CoffIDataSegName, true, false );
        class->flags |= CLASS_IDATA | CLASS_LXDATA_SEEN;
        sdata = AllocSegData();
        if( LinkState & HAVE_X64_CODE ) {
            sdata->align = 4;
        } else {
            sdata->align = 2;
        }
        sdata->is32bit = true;
        sdata->length = IData.total_size;
        sdata->u.name = CoffIDataSegName;
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
    name_list                   *os2_imp;
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
            if( chk == NULL ) break;
            if( chk->imp != NULL ) {
                len = chk->imp->len;
                if( len > os2_imp->len ) len = os2_imp->len;
                cmp = memcmp( chk->imp->name, os2_imp->name, len );
                if( cmp > 0 ) break;
                if( cmp == 0 && len > chk->imp->len ) break;
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
    glue_size = GetTransferGlueSize( LinkState );
    WALK_IMPORT_SYMBOLS( sym ) {
        size += glue_size;
        RegisterImport( sym->p.import );
        DBIAddGlobal( sym );
    }
    size += NumLocalImports * sizeof( pe_va );
    if( size != 0 ) {
        class->flags |= CLASS_TRANSFER;
        sdata = AllocSegData();
        if( LinkState & HAVE_X64_CODE ) {
            sdata->align = 4;
        } else {
            sdata->align = 2;
        }
        sdata->is32bit = true;
        sdata->length = size;
        sdata->u.name = TRANSFER_SEGNAME;
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

    ChkOS2Data();
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
    ChkOS2Exports();
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
        loc_imp->iatsym->addr.off = off;
        loc_imp->iatsym->addr.seg = seg;
    }
    if( IDataGroup != NULL ) {
        glue_size = GetTransferGlueSize( LinkState );
        WALK_IMPORT_SYMBOLS( sym ) {
            off -= glue_size;
            sym->addr.seg = seg;
            sym->addr.off = off;
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

#define PREFIX_LEN (sizeof(ImportSymPrefix)-1)

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
    char            *name;
    symbol          *locsym;

    name = iatsym->name;
    if( memcmp( name, ImportSymPrefix, PREFIX_LEN ) != 0 )
        return( false );
    locsym = FindISymbol( name + PREFIX_LEN );
    if( locsym == NULL )
        return( false );
    if( IS_SYM_IMPORTED( locsym ) )
        return( false );
    LnkMsg( WRN+MSG_IMPORT_LOCAL, "s", locsym->name );
    iatsym->info |= SYM_DEFINED | SYM_DCE_REF;
    if( LinkFlags & STRIP_CODE ) {
        DefStripImpSym( iatsym );
    }
    AddPEImportLocalSym( locsym, iatsym );
    return( true );
}

void FreePELocalImports( void )
/*****************************/
{
    FreeList( PELocalImpList );
    PELocalImpList = NULL;
    NumLocalImports = 0;
}
