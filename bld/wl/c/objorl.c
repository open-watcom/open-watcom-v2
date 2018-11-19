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
* Description:  Object file processing routines specific to ORL.
*
****************************************************************************/


#include <string.h>
#include "wio.h"
#include "linkstd.h"
#include "msg.h"
#include "wlnkmsg.h"
#include "alloc.h"
#include "specials.h"
#include "obj2supp.h"
#include "objnode.h"
#include "objcache.h"
#include "objio.h"
#include "cmdline.h"
#include "dbgall.h"
#include "objpass1.h"
#include "objpass2.h"
#include "objorl.h"
#include "strtab.h"
#include "carve.h"
#include "wcomdef.h"
#include "permdata.h"
#include "command.h"    // NYI: don't want to include this!
#include "impexp.h"
#include "virtmem.h"
#include "loadfile.h"
#include "objstrip.h"
#include "toc.h"

#include "clibext.h"


#define FP2FL( fid )   ((file_list *)(fid))
#define FL2FP( fl )    ((FILE *)(fl))

typedef struct readcache READCACHE;

typedef struct readcache {
    READCACHE   *next;
    void        *data;
} readcache;

static orl_handle               ORLHandle;
static long                     ORLFilePos;
static long                     ORLPos;

static void                     ClearCachedData( file_list *list );

static ORL_STRUCT( orl_reloc )  SavedReloc;
static char                     *ImpExternalName;
static char                     *ImpModName;
static const char               *FirstCodeSymName;
static const char               *FirstDataSymName;
static ordinal_t                ImpOrdinal;

static readcache                *ReadCacheList;

static void *ORLRead( FILE *fp, size_t len )
/******************************************/
{
    void        *result;
    readcache   *cache;

    result = CachePermRead( FP2FL( fp ), ORLFilePos + ORLPos, len );
    ORLPos += len;
    _ChkAlloc( cache, sizeof( readcache ) );
    cache->next = ReadCacheList;
    ReadCacheList = cache;
    cache->data = result;
    return( result );
}

static int ORLSeek( FILE *fp, long pos, int where )
/*************************************************/
{
    if( where == SEEK_SET ) {
        ORLPos = pos;
    } else if( where == SEEK_CUR ) {
        ORLPos += pos;
    } else {
        ORLPos = FP2FL( fp )->file->len - ORLFilePos - pos;
    }
    return( 0 );
}

void InitObjORL( void )
/*********************/
{
    ORLSetFuncs( orl_cli_funcs, ORLRead, ORLSeek, ChkLAlloc, LFree );

    ORLHandle = ORLInit( &orl_cli_funcs );
    ReadCacheList = NULL;
}

void ObjORLFini( void )
/*********************/
{
    ORLFini( ORLHandle );
}

static long ORLFileSeek( file_list *list, long pos, int where )
/*************************************************************/
{
    if( where == SEEK_SET ) {
        ORLFilePos = pos;
        ORLPos = 0;
    } else if( where == SEEK_CUR ) {
        ORLFilePos += pos;
    } else {
        ORLFilePos = list->file->len - pos;
    }
    return( ORLFilePos + ORLPos );
}

bool IsORL( file_list *list, unsigned long loc )
/**********************************************/
// return true if this is can be handled by ORL
{
    orl_file_format     type;
    bool                isOK;

    isOK = true;
    ORLFileSeek( list, loc, SEEK_SET );
    type = ORLFileIdentify( ORLHandle, FL2FP( list ) );
    if( type == ORL_ELF ) {
        ObjFormat |= FMT_ELF;
    } else if( type == ORL_COFF ) {
        ObjFormat |= FMT_COFF;
    } else {
        isOK = false;
    }
    ClearCachedData( list );
    return( isOK );
}

static orl_file_handle InitFile( void )
/*************************************/
{
    orl_file_format     type;

    ImpExternalName = NULL;
    ImpModName = NULL;
    ImpOrdinal = 0;
    FirstCodeSymName = NULL;
    FirstDataSymName = NULL;
    if( IS_FMT_ELF( ObjFormat ) ) {
        type = ORL_ELF;
    } else {
        type = ORL_COFF;
    }
    return( ORLFileInit( ORLHandle, FL2FP( CurrMod->f.source ), type ) );
}

static void ClearCachedData( file_list *list )
/********************************************/
{
    readcache   *cache;
    readcache   *next;

    for( cache = ReadCacheList; cache != NULL; cache = next ) {
        next = cache->next;
        CacheFree( list, cache->data );
        _LnkFree( cache );
    }
    ReadCacheList = NULL;
}

static void FiniFile( orl_file_handle filehdl, file_list *list )
/**************************************************************/
{
    ORLFileFini( filehdl );
    ClearCachedData( list );
    if( ImpModName != NULL ) {
        _LnkFree( ImpModName );
        ImpModName = NULL;
    }
}

void ORLSkipObj( file_list *list, unsigned long *loc )
/****************************************************/
// skip the object file.
// NYI: add an entry point in ORL for a more efficient way of doing this.
{
    orl_file_handle     filehdl;

    ORLFileSeek( list, *loc, SEEK_SET );
    filehdl = InitFile();               // assumes that entire file is read!
    *loc = ORLFileSeek( list, 0, SEEK_CUR );
    FiniFile( filehdl, list );
}

static bool CheckFlags( orl_file_handle filehdl )
/***********************************************/
{
    orl_machine_type    machtype;
    stateflag           typemask;
    stateflag           test;
    orl_file_flags      flags;

    machtype = ORLFileGetMachineType( filehdl );
    switch( machtype ) {
    case ORL_MACHINE_TYPE_I386:
        typemask = HAVE_I86_CODE;
        break;
    case ORL_MACHINE_TYPE_AMD64:
        typemask = HAVE_X64_CODE;
        break;
    case ORL_MACHINE_TYPE_ALPHA:
        typemask = HAVE_ALPHA_CODE;
        break;
    case ORL_MACHINE_TYPE_PPC601:
        typemask = HAVE_PPC_CODE;
        break;
    case ORL_MACHINE_TYPE_R3000:
        typemask = HAVE_MIPS_CODE;
        break;
    case ORL_MACHINE_TYPE_NONE:
        typemask = 0;
        break;
    default:
        typemask = HAVE_MACHTYPE_MASK;  // trigger the error
        break;
    }
    test = (typemask | LinkState) & HAVE_MACHTYPE_MASK;
    test &= test - 1;           // turn off one bit
    if( test != 0 ) {   // multiple bits were turned on.
        LnkMsg( WRN+MSG_MACHTYPE_DIFFERENT, "s", CurrMod->f.source->file->name.u.ptr );
    } else {
        LinkState |= typemask;
    }
    if( ORLFileGetType( filehdl ) != ORL_FILE_TYPE_OBJECT ) {
        BadObject();
        return( false );
    }
    flags = ORLFileGetFlags( filehdl );
#if 0
    if( flags & ORL_FILE_FLAG_BIG_ENDIAN ) {    // MS lies about this.
        LnkMsg( ERR+LOC+MSG_NO_BIG_ENDIAN, NULL );
        return( false );
    }
#endif
    if( flags & ORL_FILE_FLAG_64BIT_MACHINE ) {
        Set64BitMode();
    } else if( flags & ORL_FILE_FLAG_16BIT_MACHINE ) {
        Set16BitMode();
    } else {
        Set32BitMode();
    }
    return( true );
}

static orl_return NullFunc( orl_sec_handle dummy )
/************************************************/
// section type is ignored
{
    /* unused parameters */ (void)dummy;

    return( ORL_OKAY );
}

static orl_return ExportCallback( const char *name, void *dummy )
/***************************************************************/
{
    length_name lname;

    /* unused parameters */ (void)dummy;

    lname.name = name;
    lname.len = strlen( name );
    HandleExport( &lname, &lname, 0, 0 );
    return( ORL_OKAY );
}

static orl_return EntryCallback( const char *name, void *dummy )
/**************************************************************/
{
    /* unused parameters */ (void)dummy;

    if( !StartInfo.user_specd ) {
        SetStartSym( name );
    }
    return( ORL_OKAY );
}

static orl_return DeflibCallback( const char *name, void *dummy )
/***************************************************************/
{
    /* unused parameters */ (void)dummy;

    AddCommentLib( name, strlen( name ), LIB_PRIORITY_MAX - 2 );
    return( ORL_OKAY );
}

static orl_return P1Note( orl_sec_handle sec )
/********************************************/
// handle extra object file information records
{
    orl_note_callbacks cb;

    cb.export_fn = ExportCallback;
    cb.deflib_fn = DeflibCallback;
    cb.entry_fn = EntryCallback;
    ORLNoteSecScan( sec, &cb, NULL );
    return( ORL_OKAY );
}

static orl_return Unsupported( orl_sec_handle dummy )
/***************************************************/
// NYI
{
    /* unused parameters */ (void)dummy;

    return( ORL_OKAY );
}

static void AllocSeg( void *_snode, void *dummy )
/***********************************************/
{
    segnode             *snode = _snode;
    segdata             *sdata;
    char                *clname;
    const char          *sname;
    group_entry         *group;
    bool                isdbi;

    /* unused parameters */ (void)dummy;

    sdata = snode->entry;
    if( sdata == NULL )
        return;
    sname = sdata->u.name.u.ptr;
    if( CurrMod->modinfo & MOD_IMPORT_LIB ) {
        if( sdata->isidata || sdata->iscode ) {
            if( sdata->iscode ) {
                snode->info |= SEG_CODE;
            }
            snode->info |= SEG_DEAD;
            snode->entry = NULL;
            FreeSegData( sdata );
            return;
        }
    }
    isdbi = false;
    if( memicmp( CoffDebugPrefix, sdata->u.name.u.ptr, sizeof( CoffDebugPrefix ) - 1 ) == 0 ) {
        if( CurrMod->modinfo & MOD_IMPORT_LIB ) {
            snode->info |= SEG_DEAD;
            snode->entry = NULL;
            FreeSegData( sdata );
            return;
        }
        isdbi = true;
        if( stricmp( CoffDebugSymName, sdata->u.name.u.ptr ) == 0 ) {
            clname = _MSLocalClass;
        } else if( stricmp( CoffDebugTypeName, sdata->u.name.u.ptr ) == 0 ) {
            clname = _MSTypeClass;
        } else {
            clname = _DwarfClass;
        }
    } else if( memicmp( TLSSegPrefix, sdata->u.name.u.ptr, sizeof( TLSSegPrefix ) - 1 ) == 0 ) {
        clname = TLSClassName;
    } else if( sdata->iscode ) {
        clname = CodeClassName;
    } else if( sdata->isuninit ) {
        clname = BSSClassName;
    } else {
        clname = DataClassName;
        if( memcmp( sname, CoffPDataSegName, sizeof( CoffPDataSegName ) ) == 0 ) {
            sdata->ispdata = true;
        } else if( memcmp( sname, CoffReldataSegName, sizeof( CoffReldataSegName ) ) == 0 ) {
            sdata->isreldata = true;
        }
    }
    AllocateSegment( snode, clname );
    if( clname == TLSClassName ) {
        group = GetGroup( TLSGrpName );
        AddToGroup( group, snode->entry->u.leader );
    } else if( !sdata->iscode && !isdbi ) {
        group = GetGroup( DataGrpName );
        AddToGroup( group, snode->entry->u.leader );
    }
    if( sdata->isuninit ) {
        snode->contents = NULL;
    } else {
        snode->entry->u.leader->info |= SEG_LXDATA_SEEN;
        if( !sdata->isdead ) {
            ORLSecGetContents( snode->handle, &snode->contents );
            if( !sdata->iscdat && ( snode->contents != NULL ) ) {
                PutInfo( sdata->u1.vm_ptr, snode->contents, sdata->length );
            }
        }
    }
}

static void DefNosymComdats( void *_snode, void *dummy )
/******************************************************/
{
    segnode             *snode = _snode;
    segdata             *sdata;

    /* unused parameters */ (void)dummy;

    sdata = snode->entry;
    if( sdata == NULL || (snode->info & SEG_DEAD) )
        return;
    if( sdata->iscdat && !sdata->hascdatsym && ( snode->contents != NULL )) {
        sdata->u1.vm_ptr = AllocStg( sdata->length );
        PutInfo( sdata->u1.vm_ptr, snode->contents, sdata->length );
    }
}

static orl_return DeclareSegment( orl_sec_handle sec )
/****************************************************/
// declare the "segment"
{
    segdata                 *sdata;
    segnode                 *snode;
    const char              *name;
    unsigned_32 _WCUNALIGNED *contents;
    size_t                  len;
    orl_sec_flags           flags;
    orl_sec_type            type;
    size_t                  numlines;
    unsigned                segidx;

    type = ORLSecGetType( sec );
    if( type != ORL_SEC_TYPE_NO_BITS && type != ORL_SEC_TYPE_PROG_BITS ) {
         return( ORL_OKAY );
    }
    flags = ORLSecGetFlags( sec );
    name = ORLSecGetName( sec );
    sdata = AllocSegData();
    segidx = ORLCvtSecHdlToIdx( sec );
    snode = AllocNodeIdx( SegNodes, segidx );
    snode->entry = sdata;
    snode->handle = sec;
    sdata->iscdat = ( (flags & ORL_SEC_FLAG_COMDAT) != 0 );
    len = sizeof( CoffIDataSegName ) - 1;
    if( strnicmp( CoffIDataSegName, name, len ) == 0 ) {
        SeenDLLRecord();
        CurrMod->modinfo |= MOD_IMPORT_LIB;
        if( name[len + 1] == '6' ) {    // it is the segment containg the name
            ORLSecGetContents( sec, (unsigned_8 **)&ImpExternalName );
            ImpExternalName += 2;
        } else if( name[len + 1] == '4' ) {     // it is an import by ordinal
            ORLSecGetContents( sec, (void *)&contents );
            ImpOrdinal = *contents;
        }
        sdata->isdead = true;
        sdata->isidata = true;
    }
    sdata->combine = COMBINE_ADD;
    if( flags & ORL_SEC_FLAG_NO_PADDING ) {
        sdata->align = 0;
    } else {
        sdata->align = ORLSecGetAlignment( sec );
    }
    sdata->is32bit = true;
    sdata->length = ORLSecGetSize( sec );
    sdata->u.name.u.ptr = (char *)name;
    if( flags & ORL_SEC_FLAG_EXEC ) {
        sdata->iscode = true;
    } else if( flags & ORL_SEC_FLAG_UNINITIALIZED_DATA ) {
        sdata->isuninit = true;
#ifdef _DEVELOPMENT
    } else {
        unsigned namelen;

        namelen = strlen( name );
        if( namelen >= 3 && memicmp( name + namelen - 3, "bss", 3 ) == 0 ) {
            LnkMsg( ERR+MSG_INTERNAL, "s", "Initialized BSS found" );
        }
#endif
    }
    numlines = ORLSecGetNumLines( sec );
    if( numlines > 0 ) {
        DBIAddLines( sdata, (const void *)ORLSecGetLines( sec ), numlines * ORL_STRUCT_SIZEOF( orl_linnum ), true );
    }
    return( ORL_OKAY );
}

static segnode *FindSegNode( orl_sec_handle sechdl )
/***************************************************/
{
    orl_table_index     idx;

    if( sechdl == ORL_NULL_HANDLE )
        return( NULL );
    idx = ORLCvtSecHdlToIdx( sechdl );
    if( idx == 0 ) {
        return( NULL );
    } else {
        return( FindNode( SegNodes, idx ) );
    }
}

#define PREFIX_LEN (sizeof( ImportSymPrefix ) - 1)

static void ImpProcSymbol( segnode *snode, orl_symbol_type type, const char *name, size_t namelen )
/*************************************************************************************************/
{
    if( type & ORL_SYM_TYPE_UNDEFINED ) {
        if( namelen > sizeof( CoffImportRefName ) - 1 ) {
            namelen -= sizeof( CoffImportRefName ) - 1;
            if( memicmp( name + namelen, CoffImportRefName, sizeof( CoffImportRefName ) - 1 ) == 0 ) {
                _ChkAlloc( ImpModName, namelen + 5 );
                memcpy( ImpModName, name, namelen );
                if( memicmp( CurrMod->name.u.ptr + strlen( CurrMod->name.u.ptr ) - 4, ".drv", 4 ) == 0 ) { //KLUDGE!!
                    memcpy( ImpModName + namelen, ".drv", 5 );
                } else {
                    memcpy( ImpModName + namelen, ".dll", 5 );
                }
            }
        }
    } else if( snode != NULL && (snode->info & SEG_CODE) ) {
        if( FirstCodeSymName == NULL ) {
            FirstCodeSymName = name;
        }
    } else {
        if( FirstDataSymName == NULL && memcmp( name, ImportSymPrefix, PREFIX_LEN ) == 0 ) {
            FirstDataSymName = name + PREFIX_LEN;
        }
    }
}

static void DefineComdatSym( segnode *seg, symbol *sym, unsigned_32 value )
/*************************************************************************/
{
    unsigned    select;
    sym_info    sym_type;
    segdata     *sdata;

    sdata = seg->entry;
    sdata->hascdatsym = true;
    select = sdata->select;
    if( select == 0 ) {
        sym_type = SYM_CDAT_SEL_ANY;
    } else {
        sym_type = (select - 1) << SYM_CDAT_SEL_SHIFT;
    }
    DefineComdat( sdata, sym, value, sym_type, seg->contents );
}

static orl_return ProcSymbol( orl_symbol_handle symhdl )
/******************************************************/
{
    orl_symbol_type     type;
    const char          *name;
    unsigned_64         val64;
    orl_sec_handle      sechdl;
    symbol              *sym;
    size_t              namelen;
    sym_flags           symop;
    extnode             *newnode;
    segnode             *snode;
    bool                isweak;
    orl_symbol_handle   assocsymhdl;
    symbol              *assocsym;
    orl_symbol_binding  binding;

    sechdl = ORLSymbolGetSecHandle( symhdl );
    snode = FindSegNode( sechdl );
    type = ORLSymbolGetType( symhdl );
    name = ORLSymbolGetName( symhdl );
    if( type & ORL_SYM_TYPE_FILE ) {
        if( (CurrMod->modinfo & MOD_GOT_NAME) == 0 ) {
            CurrMod->modinfo |= MOD_GOT_NAME;
            _LnkFree( CurrMod->name.u.ptr );
            CurrMod->name.u.ptr = AddStringStringTable( &PermStrings, name );
        }
        return( ORL_OKAY );
    }
    if( type & ORL_SYM_TYPE_DEBUG )
        return( ORL_OKAY );
    if( (type & (ORL_SYM_TYPE_OBJECT | ORL_SYM_TYPE_FUNCTION))
      || ((type & (ORL_SYM_TYPE_NOTYPE | ORL_SYM_TYPE_UNDEFINED)) && name != NULL ) ) {
        namelen = strlen( name );
        if( namelen == 0 ) {
            BadObject();
        }
        if( CurrMod->modinfo & MOD_IMPORT_LIB ) {
            ImpProcSymbol( snode, type, name, namelen );
            return( ORL_OKAY );
        }
        newnode = AllocNode( ExtNodes );
        newnode->handle = symhdl;
        binding = ORLSymbolGetBinding( symhdl );
        symop = ST_CREATE;
        if( binding == ORL_SYM_BINDING_LOCAL ) {
            symop |= ST_STATIC | ST_NONUNIQUE;
        }
        if( (type & ORL_SYM_TYPE_UNDEFINED) && binding != ORL_SYM_BINDING_ALIAS ) {
            symop |= ST_REFERENCE;
        } else {
            symop |= ST_NOALIAS;
        }
        sym = SymOp( symop, name, namelen );
        CheckIfTocSym( sym );
        if( type & ORL_SYM_TYPE_COMMON ) {
            ORLSymbolGetValue( symhdl, &val64 );
            sym = MakeCommunalSym( sym, val64.u._32[I64LO32], false, true );
        } else if( type & ORL_SYM_TYPE_UNDEFINED ) {
            DefineReference( sym );
            isweak = false;
            switch( binding ) {
            case ORL_SYM_BINDING_WEAK:
                isweak = true;
                /* fall through */
            case ORL_SYM_BINDING_ALIAS:
            case ORL_SYM_BINDING_LAZY:
                assocsymhdl = ORLSymbolGetAssociated( symhdl );
                name = ORLSymbolGetName( assocsymhdl );
                namelen = strlen(name);
                if( binding == ORL_SYM_BINDING_ALIAS ) {
                    MakeSymAlias( sym->name.u.ptr, strlen( sym->name.u.ptr ), name, namelen );
                } else {
                    assocsym = SymOp( ST_CREATE | ST_REFERENCE, name, namelen );
                    DefineLazyExtdef( sym, assocsym, isweak );
                    newnode->isweak = true;
                }
            }
        } else {
            newnode->isdefd = true;
            ORLSymbolGetValue( symhdl, &val64 );
            if( (type & ORL_SYM_TYPE_COMMON) && (type & ORL_SYM_TYPE_OBJECT) && sechdl == ORL_NULL_HANDLE ) {
                sym = MakeCommunalSym( sym, val64.u._32[I64LO32], false, true );
            } else if( snode != NULL && snode->entry != NULL && snode->entry->iscdat ) {
                DefineComdatSym( snode, sym, val64.u._32[I64LO32] );
            } else {
                sym->info |= SYM_DEFINED;
                DefineSymbol( sym, snode, val64.u._32[I64LO32], 0 );
            }
        }
        newnode->entry = sym;
    } else if( (type & ORL_SYM_TYPE_SECTION) && (type & ORL_SYM_CDAT_MASK)
                            && snode != NULL && (snode->info & SEG_DEAD) == 0 ) {
        snode->entry->select = (type & ORL_SYM_CDAT_MASK) >> ORL_SYM_CDAT_SHIFT;
    }
    return( ORL_OKAY );
}

static orl_return SymTable( orl_sec_handle sec )
/**********************************************/
{
    return( ORLSymbolSecScan( sec, ProcSymbol ) );
}

static orl_return DoReloc( orl_reloc reloc )
/******************************************/
{
    fix_type    type;
    frame_spec  frame;
    target_spec target;
    offset      addend;
    segnode     *seg;
    segnode     *symseg;
    extnode     *ext;
    bool        istoc;

    istoc = false;
    type = 0;
    switch( reloc->type ) {
    case ORL_RELOC_TYPE_PAIR:
        return( ORL_OKAY );
    case ORL_RELOC_TYPE_ABSOLUTE:
        type = FIX_OFFSET_32 | FIX_ABS;
        break;
    case ORL_RELOC_TYPE_WORD_16:
        type = FIX_OFFSET_16;
        break;
    case ORL_RELOC_TYPE_WORD_26:
        type = FIX_OFFSET_26 | FIX_SHIFT;
        break;
    case ORL_RELOC_TYPE_TOCREL_14:  // relative ref to 14-bit offset from TOC base.
        type = FIX_SHIFT;
        /* fall through */
    case ORL_RELOC_TYPE_TOCREL_16:  // relative ref to 16-bit offset from TOC base.
    case ORL_RELOC_TYPE_GOT_16:     // relative ref to 16-bit offset from TOC base.
        type |= FIX_TOC | FIX_OFFSET_16;
        istoc = true;
        break;
    case ORL_RELOC_TYPE_TOCVREL_14: // relative ref to 14-bit offset from TOC base.
        type = FIX_SHIFT;
        /* fall through */
    case ORL_RELOC_TYPE_TOCVREL_16: // relative ref to 16-bit offset from TOC base.
        type |= FIX_TOCV | FIX_OFFSET_16;
        break;
    case ORL_RELOC_TYPE_IFGLUE:
        type = FIX_IFGLUE | FIX_OFFSET_32;
        break;
    case ORL_RELOC_TYPE_IMGLUE:
        // NYI: do we need this?
        return( ORL_OKAY );
    case ORL_RELOC_TYPE_JUMP:
        type = FIX_OFFSET_32 | FIX_REL;
        break;
    case ORL_RELOC_TYPE_REL_21_SH:
        type = FIX_OFFSET_21 | FIX_REL | FIX_SHIFT;
        break;
    case ORL_RELOC_TYPE_REL_24:
        type = FIX_OFFSET_24 | FIX_REL;
        break;
    case ORL_RELOC_TYPE_REL_32:
        type = FIX_OFFSET_32 | FIX_REL;
        break;
    case ORL_RELOC_TYPE_REL_32_NOADJ:
        type = FIX_OFFSET_32 | FIX_REL | FIX_NOADJ;
        break;
    case ORL_RELOC_TYPE_SEGMENT:
        type = FIX_BASE;
        break;
    case ORL_RELOC_TYPE_WORD_32_NB:
        type = FIX_OFFSET_32 | FIX_NO_BASE;
        break;
    case ORL_RELOC_TYPE_SEC_REL:
        type = FIX_OFFSET_32 | FIX_SEC_REL;
        break;
    case ORL_RELOC_TYPE_SECTION:
        type = FIX_BASE;
        break;
    case ORL_RELOC_TYPE_WORD_32:
        type = FIX_OFFSET_32;
        break;
    case ORL_RELOC_TYPE_GOT_32:     // relative ref to 32-bit offset from TOC base.
        type = FIX_OFFSET_32 | FIX_TOC;
        break;
    case ORL_RELOC_TYPE_HALF_HI:
    case ORL_RELOC_TYPE_HALF_HA:
        SavedReloc = *reloc;
        return( ORL_OKAY );
    case ORL_RELOC_TYPE_HALF_LO:
        if( SavedReloc.type == ORL_RELOC_TYPE_NONE ) {  // we recursed
            type = FIX_OFFSET_16 | FIX_SIGNED;
        } else {
            SavedReloc.type = ORL_RELOC_TYPE_NONE;      // flag recursion
            DoReloc( reloc );
            reloc = &SavedReloc;
            type = FIX_HIGH_OFFSET_16;
        }
        break;
    case ORL_RELOC_TYPE_NONE:
    default:
        LnkMsg( LOC+ERR+MSG_BAD_RELOC_TYPE, NULL );
        break;
    }
    seg = FindSegNode( reloc->section );
    if( seg != NULL && (seg->info & SEG_DEAD) == 0 && seg->entry != NULL && !seg->entry->isdead ) {
        addend = 0;
        SetCurrSeg( seg->entry, 0, seg->contents );
        frame.type = FIX_FRAME_TARG;
        ext = FindExtHandle( reloc->symbol );
        if( ext == NULL ) {
            symseg = FindSegNode( ORLSymbolGetSecHandle( reloc->symbol ) );
            if( symseg == NULL ) {
                return( ORL_OKAY );
            } else {
                unsigned_64 val64;

                ORLSymbolGetValue( reloc->symbol, &val64 );
                addend = val64.u._32[I64LO32];
                target.u.sdata = symseg->entry;
                target.type = FIX_TARGET_SEG;
                if( istoc ) {
                    AddSdataOffToToc( symseg->entry, addend );
                }
            }
        } else {
            target.u.sym = ext->entry;
            target.type = FIX_TARGET_EXT;
            if( istoc ) {
                AddSymToToc( target.u.sym );
            }
        }
        StoreFixup( reloc->offset, type, &frame, &target, addend );
    }
    return( ORL_OKAY );
}

static orl_return P1Relocs( orl_sec_handle sec )
/**********************************************/
{
    return( ORLRelocSecScan( sec, DoReloc ) );
}

static void HandleImportSymbol( const char *name )
/************************************************/
{
    length_name intname;
    length_name modname;
    length_name extname;

    intname.name = name;
    intname.len = strlen( name );
    if( ImpModName == NULL ) {
        ImpModName = FileName( CurrMod->name.u.ptr, strlen( CurrMod->name.u.ptr ), E_DLL, false );
    }
    modname.name = ImpModName;
    modname.len = strlen( ImpModName );
    if( ImpExternalName == NULL ) {
        if( ImpOrdinal == 0 ) {
            ImpOrdinal = NOT_IMP_BY_ORDINAL;
        } else {
            ImpOrdinal &= 0x7FFFFFFF;           // get rid of that high bit
        }
        HandleImport( &intname, &modname, &intname, ImpOrdinal );
    } else {
        extname.name = ImpExternalName;
        extname.len = strlen( ImpExternalName );
        HandleImport( &intname, &modname, &extname, NOT_IMP_BY_ORDINAL );
    }
    _LnkFree( ImpModName );
    ImpModName = NULL;
}

static void ScanImported( void )
/******************************/
{
    if( CurrMod->modinfo & MOD_IMPORT_LIB ) {
        if( FirstCodeSymName != NULL ) {
            HandleImportSymbol( FirstCodeSymName );
        } else if( FirstDataSymName != NULL ) {
            HandleImportSymbol( FirstDataSymName );
        }
    }
}

static orl_sec_return_func SegmentJumpTable[] = {
    NullFunc,           // ORL_SEC_TYPE_NONE
    DeclareSegment,     // ORL_SEC_TYPE_NO_BITS (bss)
    DeclareSegment,     // ORL_SEC_TYPE_PROG_BITS
    NullFunc,           // ORL_SEC_TYPE_SYM_TABLE
    NullFunc,           // ORL_SEC_TYPE_DYN_SYM_TABLE
    NullFunc,           // ORL_SEC_TYPE_STR_TABLE
    NullFunc,           // ORL_SEC_TYPE_RELOCS
    NullFunc,           // ORL_SEC_TYPE_RELOCS_EXPAND
    NullFunc,           // ORL_SEC_TYPE_HASH
    NullFunc,           // ORL_SEC_TYPE_DYNAMIC
    NullFunc,           // ORL_SEC_TYPE_NOTE
    NullFunc,           // ORL_SEC_TYPE_LINK_INFO
};

static orl_sec_return_func SymbolJumpTable[] = {
    NullFunc,           // ORL_SEC_TYPE_NONE
    NullFunc,           // ORL_SEC_TYPE_NO_BITS (bss)
    NullFunc,           // ORL_SEC_TYPE_PROG_BITS
    SymTable,           // ORL_SEC_TYPE_SYM_TABLE
    NullFunc,           // ORL_SEC_TYPE_DYN_SYM_TABLE
    NullFunc,           // ORL_SEC_TYPE_STR_TABLE
    NullFunc,           // ORL_SEC_TYPE_RELOCS
    NullFunc,           // ORL_SEC_TYPE_RELOCS_EXPAND
    NullFunc,           // ORL_SEC_TYPE_HASH
    NullFunc,           // ORL_SEC_TYPE_DYNAMIC
    NullFunc,           // ORL_SEC_TYPE_NOTE
    NullFunc            // ORL_SEC_TYPE_LINK_INFO
};

static orl_sec_return_func P1SpecificJumpTable[] = {
    NullFunc,           // ORL_SEC_TYPE_NONE
    NullFunc,           // ORL_SEC_TYPE_NO_BITS (bss)
    NullFunc,           // ORL_SEC_TYPE_PROG_BITS
    NullFunc,           // ORL_SEC_TYPE_SYM_TABLE
    Unsupported,        // ORL_SEC_TYPE_DYN_SYM_TABLE
    NullFunc,           // ORL_SEC_TYPE_STR_TABLE
    P1Relocs,           // ORL_SEC_TYPE_RELOCS
    P1Relocs,           // ORL_SEC_TYPE_RELOCS_EXPAND
    NullFunc,           // ORL_SEC_TYPE_HASH
    Unsupported,        // ORL_SEC_TYPE_DYNAMIC
    P1Note,             // ORL_SEC_TYPE_NOTE
    NullFunc            // ORL_SEC_TYPE_LINK_INFO
};

static orl_return ProcSegments( orl_sec_handle hdl )
/**************************************************/
{
    return( (SegmentJumpTable[ORLSecGetType( hdl )])( hdl ) );
}

static orl_return ProcSymbols( orl_sec_handle hdl )
/*********************************************/
{
    return( (SymbolJumpTable[ORLSecGetType( hdl )])( hdl ) );
}

static orl_return ProcP1Specific( orl_sec_handle hdl )
/****************************************************/
{
    return( (P1SpecificJumpTable[ORLSecGetType( hdl )])( hdl ) );
}

unsigned long ORLPass1( void )
/***********************************/
// do pass 1 for an object file handled by ORL.
{
    orl_file_handle     filehdl;

    LinkState |= DOSSEG_FLAG;
    PermStartMod( CurrMod );
    filehdl = InitFile();
    if( filehdl == NULL ) {
        LnkMsg( FTL+MSG_BAD_OBJECT, "s", CurrMod->f.source->file->name.u.ptr );
        CurrMod->f.source->file->flags |= INSTAT_IOERR;
        return( (unsigned long)-1 );
    }
    if( CheckFlags( filehdl ) ) {
        if( (LinkState & HAVE_PPC_CODE) && !FmtData.toc_initialized ) {
            InitToc();
            FmtData.toc_initialized = true;
        }
        if( LinkFlags & DWARF_DBI_FLAG ) {
            CurrMod->modinfo |= MOD_FLATTEN_DBI;
        }
        CurrMod->modinfo |= MOD_NEED_PASS_2;
        ORLFileScan( filehdl, NULL, ProcSegments );
        IterateNodelist( SegNodes, AllocSeg, NULL );
        ORLFileScan( filehdl, NULL, ProcSymbols );
        ScanImported();
        ORLFileScan( filehdl, NULL, ProcP1Specific );
        IterateNodelist( SegNodes, DefNosymComdats, NULL );
    }
    FiniFile( filehdl, CurrMod->f.source );
    return( ORLFileSeek( CurrMod->f.source, 0, SEEK_CUR ) );
}
