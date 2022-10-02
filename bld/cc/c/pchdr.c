/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Code to handle pre-compiled header files.
*
****************************************************************************/


#include "cvars.h"
#if !defined( __UNIX__ )
    #include <direct.h>
#endif
#include "wio.h"
#include "watcom.h"
#include "sopen.h"
#include "cmacadd.h"
#include "toggles.h"

#include "clibext.h"


extern  TAGPTR  TagHash[ID_HASH_SIZE + 1];

#define PH_BUF_SIZE     32768
#define PCH_SIGNATURE   (('H'<<24)|('C'<<16)|('P'<<8)|'W')     /* 'WPCH' */
#define PCH_VERSION     0x0128
#if defined(_M_I86)
#define PCH_VERSION_HOST ( ( 1L << 16 ) | PCH_VERSION )
#elif defined(_M_IX86)
#define PCH_VERSION_HOST ( ( 2L << 16 ) | PCH_VERSION )
#elif defined(__AXP__)
#define PCH_VERSION_HOST ( ( 3L << 16 ) | PCH_VERSION )
#elif defined(__PPC__)
#define PCH_VERSION_HOST ( ( 4L << 16 ) | PCH_VERSION )
#elif defined(__SPARC__)
#define PCH_VERSION_HOST ( ( 5L << 16 ) | PCH_VERSION )
#elif defined(__MIPS__)
#define PCH_VERSION_HOST ( ( 6L << 16 ) | PCH_VERSION )
#elif defined(_M_X64)
#define PCH_VERSION_HOST ( ( 7L << 16 ) | PCH_VERSION )
#else
#define PCH_VERSION_HOST ( ( 128L << 16 ) | PCH_VERSION )
#endif

#define PCHGetUInt(p)           ((unsigned)(pointer_uint)(p))
#define PCHSetUInt(v)           ((void *)(pointer_uint)((unsigned)(v)))

#define PCHWriteVar(m)          PCHWriteUnalign(&(m),sizeof(m))

#define PCHAlign(p)             _RoundUp(p,sizeof(int))

enum pch_aux_info_index {
    PCH_NULL_INDEX,
    PCH_FIRST_INDEX
};

#define PCH_FIRST_USER_INDEX    (PCH_FIRST_INDEX + MAX_BUILTIN_AUXINFO)

static  jmp_buf         PH_jmpbuf;
static  int             PH_handle;
static  char            *PH_Buffer;
static  char            *PH_BufPtr;
static  unsigned        PH_BufSize;
static  MEPTR           *PCHMacroHash;
static  MEPTR           PCHUndefMacroList;
static  TYPEPTR         TypeArray;
static  TAGPTR          *TagArray;
static  FNAMEPTR        FNameList;
static  textsegment     **TextSegArray;
static  unsigned        PH_SymHashCount;
static  unsigned        PH_FileCount;
static  unsigned        PH_RDirCount;
static  unsigned        PH_IAliasCount;
static  unsigned        PH_IncFileCount;
static  unsigned        PH_LibraryCount;
static  unsigned        PH_AliasCount;
static  unsigned        PH_SegCount;
static  unsigned        PH_MacroCount;
static  unsigned        PH_UndefMacroCount;
static  unsigned        PH_TypeCount;
static  unsigned        PH_TagCount;
static  unsigned        PH_PragmaCount;
static  unsigned        PH_PragmaEntryCount;
static  unsigned        PH_size;
static  unsigned        PH_MacroSize;
static  unsigned        PH_cwd_len;
static  char            PH_computing_size;

static  RDIRPTR         PCHRDirNames;       /* list of read-only directories */
static  IALIASPTR       PCHIAliasNames;     /* list of include aliases */
static  INCFILE         *PCHOldIncFileList; /* temporary copy of old include file list */
static  bool            PCHOldIncListValid; /* flag to indicate if PCHOldIncFileList is valid */

typedef struct pheader {
    unsigned        signature;      //  'WPCH'
    unsigned        version;
    unsigned        size_of_header;
    unsigned        size_of_int;
    unsigned        pack_amount;    // PackAmount
    unsigned        gen_switches;   // GenSwitches
    unsigned        target_switches;// TargetSwitches
    pragma_toggles  toggles;        // PragmaToggles
    unsigned        size;
    unsigned        macro_size;
    unsigned        file_count;
    unsigned        rdir_count;
    unsigned        ialias_count;
    unsigned        incfile_count;
    unsigned        incline_count;  // IncLineCount
    unsigned        library_count;  // # of pragma library(s)
    unsigned        alias_count;    // # of pragma alias(s)
    unsigned        seg_count;
    unsigned        macro_count;
    unsigned        undef_macro_count;
    unsigned        type_count;
    unsigned        tag_count;
    unsigned        pragma_count;
    unsigned        pragma_entry_count;
    unsigned        symhash_count;
    unsigned        symbol_count;
    unsigned        specialsyms_count;
    unsigned        cwd_len;        // length of current working directory
    unsigned        msglevel_len;   // length of msg_level array
    unsigned        cpp_ignore_env;
    unsigned        ignore_default_dirs;
} pheader;

static int FixupDataStructures( char *p, pheader *pch );

void InitDebugTypes( void );

//========================================================================
//      This portion of the code creates the pre-compiled header.
//========================================================================

static void InitPHVars( void )
//*****************************
// Set vars to 0
//*****************************
{
    PH_SymHashCount    = 0;
    PH_FileCount       = 0;
    PH_RDirCount       = 0;
    PH_IAliasCount     = 0;
    PH_IncFileCount    = 0;
    PH_LibraryCount    = 0;
    PH_AliasCount      = 0;
    PH_SegCount        = 0;
    PH_MacroCount      = 0;
    PH_UndefMacroCount = 0;
    PH_TypeCount       = 0;
    PH_TagCount        = 0;
    PH_PragmaCount     = 0;
    PH_PragmaEntryCount = 0;
    PH_size            = 0;
    PH_MacroSize       = 0;
    PH_cwd_len         = 0;
    PH_computing_size  = 0;
}

static void CreatePHeader( const char *filename )
{
    PH_handle = _sopen4( filename, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, SH_DENYRW, PMODE_RW );
    if( PH_handle == -1 ) {
        longjmp( PH_jmpbuf, 1 );
    }
}

static bool pch_write( const char *buf, size_t len )
{
    size_t     amt_towrite;

    if( PH_Buffer != NULL ) {
        for( ; len != 0; ) {
            amt_towrite = len;
            if( amt_towrite > PH_BufSize )
                amt_towrite = PH_BufSize;
            memcpy( PH_BufPtr, buf, amt_towrite );
            PH_BufSize -= amt_towrite;
            PH_BufPtr  += amt_towrite;
            buf += amt_towrite;
            len -= amt_towrite;
            if( PH_BufSize == 0 ) {         // if buffer is full
                PH_BufSize = PH_BUF_SIZE;
                PH_BufPtr = PH_Buffer;
                if( write( PH_handle, PH_Buffer, PH_BUF_SIZE ) != PH_BUF_SIZE ) {
                    return( true );
                }
            }
        }
    } else if( len > 0 ) {
        if( write( PH_handle, buf, len ) != len ) {
            return( true );
        }
    }
    return( false );
}

static bool pch_align( size_t len )
{
    len = PCHAlign( len ) - len;
    if( len > 0 ) {
        char    padding[8] = {0};
        return( pch_write( padding, len ) );
    }
    return( false );
}

static bool PCHWrite( const void *bufptr, size_t len )
{
    bool    rc = false;

    if( len != 0 ) {
        if( PH_computing_size ) {
            PH_size += PCHAlign( len );
        } else {
            rc = pch_write( bufptr, len );
            rc |= pch_align( len );
        }
    }
    return( rc );
}

static bool PCHWriteUnalign( const void *bufptr, size_t len )
{
    if( len != 0 ) {
        if( PH_computing_size ) {
            PH_size += len;
        } else {
            return( pch_write( bufptr, len ) );
        }
    }
    return( false );
}

static void FlushPHeader( void )
{
    unsigned    len;

    if( PH_BufSize != PH_BUF_SIZE ) {   // if buffer has some stuff in it
        len = (unsigned)( PH_BUF_SIZE - PH_BufSize );
        if( (unsigned)write( PH_handle, PH_Buffer, len ) != len ) {
            longjmp( PH_jmpbuf, 1 );
        }
    }
}

static void ClosePHeader( void )
{
    close( PH_handle );
    PH_handle = -1;
}


static void OutPutHeader( void )
{
    bool            rc;
    pheader         pch;

    pch.signature         = PCH_SIGNATURE;
    pch.version           = PCH_VERSION_HOST;
    pch.size_of_header    = sizeof( pheader );
    pch.size_of_int       = TARGET_INT;
    pch.pack_amount       = PackAmount;
    pch.gen_switches      = GenSwitches;
    pch.target_switches   = TargetSwitches;
    pch.toggles           = PragmaToggles;
    pch.size              = PH_size - PH_MacroSize;
    pch.macro_size        = PH_MacroSize;
    pch.file_count        = PH_FileCount;
    pch.rdir_count        = PH_RDirCount;
    pch.ialias_count      = PH_IAliasCount;
    pch.incfile_count     = PH_IncFileCount;
    pch.incline_count     = IncLineCount;
    pch.library_count     = PH_LibraryCount;
    pch.alias_count       = PH_AliasCount;
    pch.seg_count         = PH_SegCount;
    pch.macro_count       = PH_MacroCount;
    pch.undef_macro_count = PH_UndefMacroCount;
    pch.type_count        = PH_TypeCount;
    pch.tag_count         = PH_TagCount;
    pch.pragma_count      = PH_PragmaCount;
    pch.pragma_entry_count = PH_PragmaEntryCount;
    pch.symhash_count     = PH_SymHashCount;
    pch.symbol_count      = SymGetNumSyms();
    pch.specialsyms_count = SymGetNumSpecialSyms();
    pch.cwd_len           = PH_cwd_len;
    pch.msglevel_len      = sizeof( msg_level );
    pch.cpp_ignore_env    = CompFlags.cpp_ignore_env;
    pch.ignore_default_dirs = CompFlags.ignore_default_dirs;

    rc = PCHWriteVar( pch );
    rc |= PCHWrite( PH_Buffer + sizeof( pch ), pch.cwd_len );
    if( rc ) {
        longjmp( PH_jmpbuf, rc );
    }
}

static void OutPutIncPathList( void )     // output include paths
{
    bool        rc;
    size_t      len;

    len = strlen( IncPathList ) + 1;
    rc = PCHWrite( IncPathList, len );
    if( rc ) {
        longjmp( PH_jmpbuf, rc );
    }
}

static void OutPutIncFileList( void )   // output primary include files
{
    bool        rc;
    size_t      len;
    INCFILE     *ifile;
    INCFILE     *next_file;

    for( ifile = IncFileList; ifile != NULL; ifile = next_file ) {
        next_file = ifile->nextfile;
        len = offsetof( INCFILE, filename ) + strlen( ifile->filename ) + 1;
        ifile->nextfile = PCHSetUInt( PCHAlign( len ) );
        rc = PCHWrite( ifile, len );
        ifile->nextfile = next_file;
        if( rc ) {
            longjmp( PH_jmpbuf, rc );
        }
        PH_IncFileCount++;
    }
}

static void OutPutLibraries( void )
{
    bool                rc;
    size_t              len;
    library_list        *lib;
    library_list        *lib_next;

    for( lib = HeadLibs; lib != NULL; lib = lib_next ) {
        lib_next = lib->next;
        len = offsetof( library_list, libname ) + strlen( lib->libname ) + 1;
        lib->next = PCHSetUInt( PCHAlign( len ) );
        rc = PCHWrite( lib, len );
        lib->next = lib_next;
        if( rc ) {
            longjmp( PH_jmpbuf, rc );
        }
        PH_LibraryCount++;
    }
}

static void OutPutAliases( void )
{
    bool            rc;
    size_t          len;
    size_t          alias_len;
    size_t          subst_len;
    alias_list      *alias;
    alias_list      *alias_next;
    const char      *alias_name;
    const char      *alias_subst;

    for( alias = AliasHead; alias != NULL; alias = alias->next ) {
        alias_next = alias->next;
        alias_name = alias->name;
        alias_subst = alias->subst;
        len = offsetof( alias_list, names );
        if( alias_name != NULL ) {
            alias_len = strlen( alias_name ) + 1;
            alias->name = PCHSetUInt( alias_len );
            len += alias_len;
        }
        if( alias_subst != NULL ) {
            subst_len =  strlen( alias_subst ) + 1;
            alias->subst = PCHSetUInt( subst_len );
            len += subst_len;
        }
        alias->next = PCHSetUInt( PCHAlign( len ) );
        rc = PCHWrite( alias, len );
        alias->subst = alias_subst;
        alias->name = alias_name;
        alias->next = alias_next;
        if( rc ) {
            longjmp( PH_jmpbuf, rc );
        }
        PH_AliasCount++;
    }
}

static void OutPutMsgLevels( void )
{
    bool        rc;

    rc = PCHWrite( msg_level, sizeof( msg_level ) );
    if( rc ) {
        longjmp( PH_jmpbuf, rc );
    }
}

static void OutPutIncludes( void )
{
    FNAMEPTR    flist;
    FNAMEPTR    next_flist;
    size_t      len;
    bool        rc;

    // don't want to include the primary source file, skip first item
    for( flist = FNames->next; flist != NULL; flist = next_flist ) {
        next_flist = flist->next;
        len = offsetof( fname_list, name ) + strlen( flist->name ) + 1;
        flist->next = PCHSetUInt( PCHAlign( len ) );
        rc = PCHWrite( flist, len );
        flist->next = next_flist;
        if( rc ) {
            longjmp( PH_jmpbuf, rc );
        }
        PH_FileCount++;
    }
}

static void OutPutRoDirList( void )
{
    RDIRPTR     dirlist;
    RDIRPTR     next_dirlist;
    size_t      len;
    bool        rc;

    for( dirlist = PCHRDirNames; dirlist != NULL; dirlist = next_dirlist ) {
        next_dirlist = dirlist->next;
        len = offsetof( rdir_list, name ) + strlen( dirlist->name ) + 1;
        dirlist->next = PCHSetUInt( PCHAlign( len ) );
        rc = PCHWrite( dirlist, len );
        dirlist->next = next_dirlist;
        if( rc ) {
            longjmp( PH_jmpbuf, rc );
        }
        PH_RDirCount++;
    }
}

static void OutPutIncAliasList( void )
{
    IALIASPTR   aliaslist;
    IALIASPTR   next_aliaslist;
    char        *real_name;
    size_t      len;
    size_t      alias_len;
    bool        rc;

    for( aliaslist = IAliasNames; aliaslist != NULL; aliaslist = next_aliaslist ) {
        next_aliaslist = aliaslist->next;
        real_name = aliaslist->real_name;
        alias_len = aliaslist->real_name - aliaslist->alias_name;
        len = offsetof( ialias_list, alias_name ) + alias_len + strlen( aliaslist->real_name ) + 1;
        aliaslist->next = PCHSetUInt( PCHAlign( len ) );
        aliaslist->real_name = PCHSetUInt( alias_len );
        rc = PCHWrite( aliaslist, len );
        aliaslist->real_name = real_name;
        aliaslist->next = next_aliaslist;
        if( rc ) {
            longjmp( PH_jmpbuf, rc );
        }
        PH_IAliasCount++;
    }
}

static void OutPutSegInfo( void )
{
    textsegment     *tseg;
    textsegment     *tseg_next;
    bool            rc;
    size_t          len;

    for( tseg = TextSegList; tseg != NULL; tseg = tseg_next ) {
        tseg_next = tseg->next;             // save next pointer
        ++PH_SegCount;
        tseg->index = PH_SegCount;
        len = offsetof( textsegment, segname ) + tseg->class + strlen( tseg->segname + tseg->class ) + 1;
        tseg->next = PCHSetUInt( PCHAlign( len ) );
        rc = PCHWrite( tseg, len );
        tseg->next = tseg_next;             // restore next pointer
        if( rc ) {
            longjmp( PH_jmpbuf, rc );
        }
    }
}

static void OutPutEnums( ENUMPTR ep, TAGPTR parent )
{
    bool        rc;
    size_t      len;

    for( ; ep != NULL; ep = ep->thread ) {
        len = offsetof( ENUMDEFN, name ) + strlen( ep->name ) + 1;
        ep->parent = PCHSetUInt( PCHAlign( len ) );
        rc = PCHWrite( ep, len );
        ep->parent = parent;
        if( rc ) {
            longjmp( PH_jmpbuf, rc );
        }
    }
}

static void OutPutFields( FIELDPTR field )
{
    bool        rc;
    size_t      len;
    TYPEPTR     field_type;

    for( ; field != NULL; field = field->next_field ) {
        field_type = field->field_type;         // save pointer
        len = offsetof( FIELD_ENTRY, name ) + strlen( field->name ) + 1;
        field->next_field_same_hash = PCHSetUInt( PCHAlign( len ) );
        field->field_type = PCHSetUInt( field_type->u1.type_index );
        rc = PCHWrite( field, len );
        field->field_type = field_type;         // restore pointer
        if( rc ) {
            longjmp( PH_jmpbuf, rc );
        }
    }
}

static void OutPutATag( TAGPTR tag )
{
    bool        rc;
    TYPEPTR     sym_type;
    TAGPTR      next_tag;
    size_t      len;

    next_tag = tag->next_tag;
    sym_type = tag->sym_type;
    len = offsetof( TAGDEFN, name ) + strlen( tag->name ) + 1;
    tag->next_tag = PCHSetUInt( PCHAlign( len ) );
    tag->sym_type = PCHSetUInt( sym_type->u1.type_index );
    rc = PCHWrite( tag, len );
    tag->sym_type = sym_type;
    tag->next_tag = next_tag;
    if( rc ) {
        longjmp( PH_jmpbuf, rc );
    }
    if( sym_type->decl_type == TYP_ENUM ) {
        OutPutEnums( tag->u.enum_list, tag );
    } else {
        OutPutFields( tag->u.field_list );
    }
}

static void OutPutTags( void )
{
    WalkTagList( OutPutATag );
}

static void SetTypeIndex( TYPEPTR typ )
{
    ++PH_TypeCount;
    typ->u1.type_index = PH_TypeCount;
}

static void SetFuncTypeIndex( TYPEPTR typ, int index )
{
    /* unused parameters */ (void)index;

    SetTypeIndex( typ );
}

static void NumberTypes( void )
{
    PH_TypeCount = 0;
    WalkTypeList( SetTypeIndex );
    WalkFuncTypeList( SetFuncTypeIndex );
}

static void SetTagIndex( TAGPTR tag )
{
    tag->refno = PH_TagCount;
    ++PH_TagCount;
}

static void SetDebugTag( TAGPTR tag )
{
    tag->refno = 0;
}

static void NumberTags( void )
{
    PH_TagCount = 0;
    WalkTagList( SetTagIndex );
}

static void InitDebugTags( void )
{
    WalkTagList( SetDebugTag );
}

static bool WriteType( TYPEPTR typ )
{
    bool        rc;
    TYPEPTR     object;

    object = typ->object;
    if( object != NULL ) {
        typ->object = PCHSetUInt( object->u1.type_index );
        rc = PCHWriteVar( *typ );
        typ->object = object;
    } else {
        rc = PCHWriteVar( *typ );
    }
    return( rc );
}

typedef struct type_indices {
    int     basetype_index[TYP_LAST_ENTRY];
    int     stringtype_index;
    int     constchartype_index;
} type_indices;

static void OutPutTypeIndexes( void )
{
    TYPEPTR             typ;
    bool                rc;
    int                 i;
    type_indices        typ_index;

    for( i = TYP_BOOL; i < TYP_LAST_ENTRY; i++ ) {
        typ = BaseTypes[i];
        if( typ == NULL ) {
            typ_index.basetype_index[i] = 0;
        } else {
            typ_index.basetype_index[i] = typ->u1.type_index;
        }
    }
    typ_index.stringtype_index = StringType->u1.type_index;
    typ_index.constchartype_index = ConstCharType->u1.type_index;
    rc = PCHWriteVar( typ_index );
    if( rc ) {
        longjmp( PH_jmpbuf, rc );
    }
}

static void OutPutAType( TYPEPTR typ )
{
    TAGPTR          tag;
    bool            rc;
    array_info      *array;

    rc = false;
    switch( typ->decl_type ) {
    case TYP_ARRAY:
        array = typ->u.array;                   // save pointer
        typ->u.array = PCHSetUInt( array->dimension );  // replace with dim
        rc = WriteType( typ );
        typ->u.array = array;                   // restore pointer
        break;
    case TYP_STRUCT:
    case TYP_UNION:
    case TYP_ENUM:
        tag = typ->u.tag;                       // save tag
        typ->u.tag = PCHSetUInt( tag->refno );  // replace with index
        rc = WriteType( typ );
        typ->u.tag = tag;                       // restore tag pointer
        break;
    default:
        if( typ->decl_type != TYP_FUNCTION ) {
            rc = WriteType( typ );
        }
        break;
    }
    if( rc ) {
        longjmp( PH_jmpbuf, rc );
    }
}

static void OutPutAFuncType( TYPEPTR typ, int index )
{
    TYPEPTR     *parm_types;
    bool        rc;

    parm_types = typ->u.fn.parms;           // save pointer
    typ->u.fn.parms = PCHSetUInt( index );  // replace with index
    rc = WriteType( typ );
    typ->u.fn.parms = parm_types;           // restore pointer
    if( rc ) {
        longjmp( PH_jmpbuf, rc );
    }
}

static void OutPutFuncParmList( TYPEPTR typ, int index )
{
    TYPEPTR     *parm_types;
    bool        rc;
    TYPEPTR     parm_type;

    /* unused parameters */ (void)index;

    if( typ->u.fn.parms != NULL ) {
        for( parm_types = typ->u.fn.parms; (parm_type = *parm_types) != NULL; ++parm_types ) {
            parm_type = PCHSetUInt( parm_type->u1.type_index );
            rc = PCHWriteVar( parm_type );
            if( rc ) {
                longjmp( PH_jmpbuf, rc );
            }
        }
    }
    parm_type = PCHSetUInt( -1 );
    rc = PCHWriteVar( parm_type );
    if( rc ) {
        longjmp( PH_jmpbuf, rc );
    }
}

static void OutPutTypes( void )
{
    NumberTypes();
    NumberTags();
    WalkTypeList( OutPutAType );
    WalkFuncTypeList( OutPutAFuncType );
    WalkFuncTypeList( OutPutFuncParmList );
    OutPutTypeIndexes();
}

#if ( _CPU == 8086 ) || ( _CPU == 386 )
static void OutPutAuxInfo( aux_info *info )
{
    hw_reg_set          *regs;
    hw_reg_set          *info_parms;
    char                *info_objname;
    byte_seq            *info_code;
    bool                rc;
    size_t              len;
    size_t              objname_len;
    size_t              code_len;

    objname_len = 0;
    code_len = 0;
    info_parms = info->parms;
    info_objname = info->objname;
    info_code = info->code;
    if( info_parms != NULL ) {
        len = 0;
        for( regs = info_parms; ; ++regs ) {
            len += sizeof( hw_reg_set );
            if( HW_CEqual( *regs, HW_EMPTY ) ) {
                break;
            }
        }
        info->parms = PCHSetUInt( len );
    }
    if( info_objname != NULL ) {
        objname_len = strlen( info_objname ) + 1;
        info->objname = PCHSetUInt( PCHAlign( objname_len ) );
    }
    if( info_code != NULL ) {
        code_len = offsetof( byte_seq, data ) + info_code->length;
        info->code = PCHSetUInt( PCHAlign( code_len ) );
    }
    rc = PCHWriteVar( *info );
    if( info_parms != NULL ) {
        for( regs = info_parms; ; ++regs ) {
            rc |= PCHWriteVar( *regs );
            if( HW_CEqual( *regs, HW_EMPTY ) ) {
                break;
            }
        }
    }
    if( info_objname != NULL ) {
        rc |= PCHWrite( info_objname, objname_len );
    }
    if( info_code != NULL ) {
        rc |= PCHWrite( info_code, code_len );
    }
    info->parms = info_parms;
    info->objname = info_objname;
    info->code = info_code;
    if( rc ) {
        longjmp( PH_jmpbuf, rc );
    }
}

static void OutPutPragmaInfo( void )
{
    aux_entry       *ent;
    aux_entry       *ent_next;
    aux_info        *ent_info;
    int             index;
    bool            rc;
    size_t          len;

    // write built-in aux_info
    for( index = PCH_FIRST_INDEX; index < PCH_FIRST_USER_INDEX; ++index ) {
        ent_info = BuiltinAuxInfo + index - PCH_FIRST_INDEX;
        ent_info->index = index;
        OutPutAuxInfo( ent_info );      // write out the aux_info struct
    }
    // write user aux_info
    for( ent = AuxList; ent != NULL; ent = ent->next ) {
        ent->info->index = PCH_NULL_INDEX;
    }
    for( ent = AuxList; ent != NULL; ent = ent->next ) {
        ent_info = ent->info;
        if( ent_info->index == PCH_NULL_INDEX ) {
            ent_info->index = index++;
            OutPutAuxInfo( ent_info );      // write out the aux_info struct
        }
    }
    PH_PragmaCount = index - PCH_FIRST_INDEX;
    PH_PragmaEntryCount = 0;
    for( ent = AuxList; ent != NULL; ent = ent_next ) {
        ent_next = ent->next;                   // save pointer
        ent_info = ent->info;                   // save pointer
        len = offsetof( aux_entry, name ) + strlen( ent->name ) + 1;
        ent->next = PCHSetUInt( PCHAlign( len ) );
        ent->info = PCHSetUInt( ent_info->index - PCH_FIRST_INDEX );
        rc = PCHWrite( ent, len );
        ent->info = ent_info;                   // restore pointer
        ent->next = ent_next;                   // restore pointer
        if( rc ) {
            longjmp( PH_jmpbuf, rc );
        }
        ++PH_PragmaEntryCount;
    }
}
#endif

static void OutPutMacros( void )
{
    mac_hash_idx    hash;
    bool            rc;
    MEPTR           mentry;
    MEPTR           mentry_next_macro;

    PH_MacroCount = 0;
    PH_MacroSize = PH_size;
    for( hash = 0; hash < MACRO_HASH_SIZE; hash++ ) {
        for( mentry = MacHash[hash]; mentry != NULL; mentry = mentry_next_macro ) {
            mentry_next_macro = mentry->next_macro;        // save pointer
            mentry->next_macro = PCHSetUInt( hash );
            rc = PCHWrite( mentry, mentry->macro_len );
            mentry->next_macro = mentry_next_macro;        // restore pointer
            if( rc ) {
                longjmp( PH_jmpbuf, rc );
            }
            ++PH_MacroCount;
        }
    }
    /* write out undefined macro list. */
    PH_UndefMacroCount = 0;
    for( mentry = UndefMacroList; mentry != NULL; mentry = mentry->next_macro ) {
        rc = PCHWrite( mentry, mentry->macro_len );
        if( rc ) {
            longjmp( PH_jmpbuf, rc );
        }
        ++PH_UndefMacroCount;
    }
    PH_MacroSize = PH_size - PH_MacroSize;
}

static void OutPutSymHashTable( void )
{
    SYM_HASHPTR     hsym;
    SYM_HASHPTR     hsym_next_sym;
    TYPEPTR         hsym_sym_typ;
    SYM_HASHPTR     sym_list;
    id_hash_idx     hash;
    bool            rc;
    size_t          len;

    for( hash = 0; hash < ID_HASH_SIZE; hash++ ) {
        // reverse the list
        sym_list = NULL;
        for( hsym = HashTab[hash]; hsym != NULL; hsym = hsym_next_sym ) {
            hsym_next_sym = hsym->next_sym;
            hsym->next_sym = sym_list;
            sym_list = hsym;
            ++PH_SymHashCount;
        }
        HashTab[hash] = NULL;
        rc = false;
        for( hsym = sym_list; hsym != NULL; hsym = hsym_next_sym ) {
            hsym_next_sym = hsym->next_sym;
            hsym->next_sym = PCHSetUInt( hash );
            hsym_sym_typ = hsym->sym_type;               // save type pointer
            if( hsym_sym_typ != NULL ) {
                hsym->sym_type = PCHSetUInt( hsym_sym_typ->u1.type_index ); // replace with index
            }
            len = offsetof( id_hash_entry, name ) + strlen( hsym->name ) + 1;
            rc |= PCHWrite( hsym, len );
            hsym->sym_type = hsym_sym_typ;               // restore type pointer
            hsym->next_sym = HashTab[hash];
            HashTab[hash] = hsym;
        }
        if( rc ) {
            longjmp( PH_jmpbuf, rc );
        }
    }
}

static void OutPutSymbols( void )
{
    SYM_ENTRY   sym;
    SYM_HANDLE  sym_handle;
    bool        rc;

    if( PH_computing_size ) {
        PH_size += SymGetNumSyms() * sizeof( SYM_ENTRY );
    } else {
        for( sym_handle = SymGetFirst(); sym_handle != SYM_INVALID; sym_handle = SymGetNext( sym_handle ) ) {
            SymGet( &sym, sym_handle );
            if( sym.sym_type != NULL ) {
                sym.sym_type = PCHSetUInt( sym.sym_type->u1.type_index );
            }
            if( sym.seginfo != NULL ) {
                sym.seginfo = PCHSetUInt( sym.seginfo->index );
            }
            sym.name = NULL; // can't carry a name across
            rc = PCHWriteVar( sym );
            if( rc ) {
                longjmp( PH_jmpbuf, rc );
            }
        }
    }
}

static void OutPutEverything( void )
{
    PH_SymHashCount = 0;
    PH_FileCount = 0;
    PH_RDirCount = 0;
    PH_IAliasCount = 0;
    PH_SegCount = 0;
    OutPutIncludes();
    OutPutRoDirList();
    OutPutIncAliasList();
    OutPutIncPathList();
    OutPutIncFileList();
    OutPutLibraries();
    OutPutAliases();
    OutPutSegInfo();
    OutPutTypes();
    OutPutTags();
#if ( _CPU == 8086 ) || ( _CPU == 386 )
    OutPutPragmaInfo();
#endif
    OutPutSymHashTable();
    OutPutSymbols();
    OutPutMsgLevels();
    OutPutMacros();
}

void InitBuildPreCompiledHeader( void )
//Save any before info when building pre compiled headers
{
    RDIRPTR     start;

    start = RDirNames;
    if( start != NULL ) {
        start = start->next;
    }
    PCHRDirNames = start;

    PCHIAliasNames = IAliasNames;
}

void BuildPreCompiledHeader( const char *filename )
{
    int         rc;
    char        *cwd;

    if( FirstStmt != 0 || DataQuadsAvailable() ) {
        PCHNote( PCHDR_NO_OBJECT );
        return;
    }
    InitPHVars();
    PH_Buffer = FEmalloc( PH_BUF_SIZE );
    PH_BufPtr = PH_Buffer;
    PH_BufSize = PH_BUF_SIZE;
    cwd = getcwd( PH_Buffer + sizeof( pheader ), PH_BUF_SIZE - sizeof( pheader ) );
    rc = setjmp( PH_jmpbuf );
    if( rc == 0 ) {
        CreatePHeader( filename );
        PH_cwd_len = PCHAlign( strlen( cwd ) + 1 );
        PH_size = PH_cwd_len;
        PH_computing_size = 1;
        OutPutEverything();
        PH_computing_size = 0;
        OutPutHeader();
        OutPutEverything();
        FlushPHeader();
        ClosePHeader();
    } else {                            // error creating pre-compiled header
        if( PH_handle != -1 ) {
            ClosePHeader();             // close the file
            remove( filename );         // delete the pre-compiled header
        }
    }
    InitDebugTypes();
    InitDebugTags();
    FEfree( PH_Buffer );
    PCH_FileName = NULL;
    CompFlags.make_precompiled_header = false;
}

//========================================================================
//      This portion of the code loads the pre-compiled header and
//      rebuilds the data structures by replacing all the indices with
//      pointers, reconstructing hash tables, etc.
//========================================================================

static char *FixupIncFileList( char *p, unsigned incfile_count )
{
    INCFILE     *ifile;

    PCHOldIncFileList = IncFileList;
    PCHOldIncListValid = true;
    IncFileList = NULL;
    for( ; incfile_count != 0; --incfile_count ) {
        ifile = (INCFILE *)p;
        p += PCHGetUInt( ifile->nextfile );
        AddIncFile( ifile );
    }
    return( p );
}

static void RestoreIncFileList( void )
{
    if( PCHOldIncListValid ) {
        IncFileList = PCHOldIncFileList;
        PCHOldIncFileList = NULL;
    }
}

static void FreeOldIncFileList( void )
{
    INCFILE *ilist;

    if( PCHOldIncListValid ) {
        while( (ilist = PCHOldIncFileList) != NULL ) {
            PCHOldIncFileList = ilist->nextfile;
            CMemFree( ilist );
        }
        PCHOldIncFileList = NULL;
        PCHOldIncListValid = false;
    }
}

static char *FixupIncludes( char *p, unsigned file_count )
{
    FNAMEPTR    flist;
    FNAMEPTR    *lnk;

    lnk = &FNameList;
    for( ; file_count != 0; --file_count ) {
        flist = (FNAMEPTR)p;
        p += PCHGetUInt( flist->next );
        flist->fullpath = NULL;
        *lnk = flist;
        lnk = &flist->next;
    }
    *lnk = NULL;
    return( p );
}

static char *FixupRoDirList( char *p, unsigned list_count )
{
    RDIRPTR     dirlist;
    RDIRPTR     *lnk;

    for( lnk = &RDirNames; (dirlist = *lnk) != NULL; ) {
        lnk = &dirlist->next;
    }
    for( ; list_count != 0; --list_count ) {
        dirlist = (RDIRPTR)p;
        p += PCHGetUInt( dirlist->next );
        *lnk = dirlist;
        lnk = &dirlist->next;
    }
    *lnk = NULL;
    return( p );
}

static char *FixupIncAliasList( char *p, unsigned list_count )
{
    IALIASPTR   aliaslist;
    IALIASPTR   *lnk;

    lnk = &IAliasNames;
    for( ; list_count != 0; --list_count ) {
        aliaslist = (IALIASPTR)p;
        p += PCHGetUInt( aliaslist->next );
        aliaslist->real_name = aliaslist->alias_name + PCHGetUInt( aliaslist->real_name );
        *lnk = aliaslist;
        lnk = &aliaslist->next;
    }
    /* Tack pre-existing aliases onto the end (so that they're searched last) */
    *lnk = PCHIAliasNames;
    return( p );
}

static bool VerifyIncludes( const char *filename )
{
    FNAMEPTR    flist;
    time_t      mtime;
    FNAMEPTR    fnew;
    struct stat statbuf;

    // skip the primary source file (first item)
    fnew = FNames->next;
    for( flist = FNameList; flist != NULL; flist = flist->next ) {
        if( fnew != NULL ) {
            if( FNAMECMPSTR( fnew->name, flist->name ) != 0 ) {
                return( true );
            }
            fnew = fnew->next;
        } else if( filename != NULL ) {
            if( FNAMECMPSTR( filename, flist->name ) != 0 ) {
                return( true );
            }
            filename = NULL;
        }
        if( flist->rwflag ) {
            if( SrcFileInRDir( flist ) ) {
                flist->rwflag = false;
            }
        }
        if( flist->rwflag ) {
            mtime = 0;
            if( stat( flist->name, &statbuf ) == 0 ) {
                mtime = statbuf.st_mtime;
            }
            if( flist->mtime != mtime || mtime == 0 ) {
                PCHNote( PCHDR_INCFILE_CHANGED, flist->name  );
#if 0
                printf( "%s old %d new %d\n", flist->name, flist->mtime, mtime );
#endif
                return( true );
            }
        }
    }
    return( false );
}

static char *FixupLibraries( char *p, unsigned library_count )
{
    library_list    *lib;
    library_list    **lnk;

    lnk = &HeadLibs;
    for( ; library_count != 0; --library_count ) {
        lib = (library_list *)p;
        p += PCHGetUInt( lib->next );
        *lnk = lib;
        lnk = &lib->next;
    }
    *lnk = NULL;
    return( p );
}

static char *FixupAliases( char *p, unsigned alias_count )
{
    alias_list  *alias;
    alias_list  **lnk;
    unsigned    len;

    lnk = &AliasHead;
    for( ; alias_count > 0; --alias_count ) {
        alias = (alias_list *)p;
        p += PCHGetUInt( alias->next );
        len = PCHGetUInt( alias->name );
        if( len != 0 ) {
            alias->name = alias->names;
        }
        if( PCHGetUInt( alias->subst ) != 0 ) {
            alias->subst = alias->names + len;
        }
        *lnk = alias;
        lnk = &alias->next;
    }
    *lnk = NULL;
    return( p );
}

static char *FixupSegInfo( char *p, unsigned seg_count )
{
    textsegment     *tseg;
    textsegment     **lnk;

    TextSegArray = (textsegment **)CMemAlloc( ( seg_count + 1 ) * sizeof( textsegment * ) );
    TextSegArray[0] = NULL;
    lnk = &tseg;
    for( ; seg_count != 0; --seg_count ) {
        tseg = (textsegment *)p;
        p += PCHGetUInt( tseg->next );
        TextSegArray[tseg->index + 1] = tseg;
        *lnk = tseg;
        lnk = &tseg->next;
    }
    *lnk = NULL;
    return( p );
}

static char *FixupMacros( char *p, unsigned macro_count )
{
    int         i;
    MEPTR       mentry;

    for( ; macro_count != 0; --macro_count ) {
        mentry = (MEPTR)p;
        p += PCHAlign( mentry->macro_len );
        i = PCHGetUInt( mentry->next_macro );       // get hash index
        mentry->next_macro = PCHMacroHash[i];
        PCHMacroHash[i] = mentry;
    }
    return( p );
}

static char *FixupUndefMacros( char *p, unsigned undef_macro_count )
{   // Read in as written out
    MEPTR       mentry;
    MEPTR       *lnk;

    lnk = &PCHUndefMacroList;
    for( ; undef_macro_count != 0; --undef_macro_count ) {
        mentry = (MEPTR)p;
        p += PCHAlign( mentry->macro_len );
        *lnk = mentry;
        lnk = &mentry->next_macro;
    }
    *lnk = NULL;
    return( p );
}

static int VerifyMacros( char *p, unsigned macro_count, unsigned undef_count )
{
    mac_hash_idx    hash;
    MEPTR           mpch;
    MEPTR           mcur;
    bool            macro_compare;

    PCHMacroHash = (MEPTR *)CMemAlloc( MACRO_HASH_SIZE * sizeof( MEPTR ) );
    p = FixupMacros( p, macro_count );
    p = FixupUndefMacros( p, undef_count );
    for( hash = 0; hash < MACRO_HASH_SIZE; hash++ ) {
        MEPTR       prev_mpch;

        prev_mpch = NULL;
        for( mpch = PCHMacroHash[hash]; mpch != NULL; mpch = mpch->next_macro ) {
            if( mpch->macro_flags & MFLAG_DEFINED_BEFORE_FIRST_INCLUDE ) {
                for( mcur = MacHash[hash]; mcur != NULL; mcur = mcur->next_macro ) {
                    if( strcmp( mcur->macro_name, mpch->macro_name ) == 0 ) {
                        macro_compare = MacroCompare( mpch, mcur );
                        if( mpch->macro_flags & MFLAG_REFERENCED ) {
                            if( macro_compare ) {
                                return( -1 );           /* abort: macros different */
                            }
                        } else if( macro_compare ) {    /* if different */
                            /* delete macro from pch, add new one */
                            if( prev_mpch == NULL ) {
                                PCHMacroHash[hash] = mpch->next_macro;
                            } else {
                                prev_mpch->next_macro = mpch->next_macro;
                            }
                        }
                        break;
                    }
                }
                if( mcur == NULL ) {  /* macro not found in current compile */
                    if( mpch->macro_flags & MFLAG_REFERENCED ) {
                        return( -1 );   // abort: macro definition required
                    }
                    // delete macro from PCH list
                    if( prev_mpch == NULL ) {
                        PCHMacroHash[hash] = mpch->next_macro;
                    } else {
                        prev_mpch->next_macro = mpch->next_macro;
                    }
                }
            }
            prev_mpch = mpch;
        }
    }
    // check if any user-defined macros have been defined that were not
    // defined when the pre-compiled header file was created
    //
    // for all macros in current compilation unit
    // - if it is present in the PCH header file set of macros
    // -- if not defined before first include
    // --- abort:
    // -- endif
    // - else macro NOT in PCH
    // -- if the macro is a user-defined macro
    // --- abort:
    // -- endif
    // - endif
    // endloop
    for( hash = 0; hash < MACRO_HASH_SIZE; hash++ ) {
        for( mcur = MacHash[hash]; mcur != NULL; mcur = mcur->next_macro ) {
            for( mpch = PCHMacroHash[hash]; mpch != NULL; mpch = mpch->next_macro ) {
                if( strcmp( mpch->macro_name, mcur->macro_name ) == 0 ) {
                    break;
                }
            }
            if( mpch == NULL || (mpch->macro_flags & MFLAG_DEFINED_BEFORE_FIRST_INCLUDE) == 0 ) {
            // macro may either have been undef'd (mpch == NULL ) or undef'd and defined
                if( mcur->macro_flags & MFLAG_USER_DEFINED ) {  //compiler defined macros not saved on undefs
                    for( mpch = PCHUndefMacroList; mpch != NULL; mpch = mpch->next_macro ) {
                        if( strcmp( mpch->macro_name, mcur->macro_name ) == 0 ) {
                            if( MacroCompare( mpch, mcur ) ) {
                                return( -1 );
                            } else {
                                break;
                            }
                        }
                    }
                    if( mpch == NULL ) {
                        // abort: macro wasn't defined before include
                       return( -1 );
                    }
                }
            }
        }
    }

    for( hash = 0; hash < MACRO_HASH_SIZE; hash++ ) {
        MEPTR       next_mcur;

        for( mcur = MacHash[hash]; mcur != NULL; mcur = next_mcur ) {
            for( mpch = PCHMacroHash[hash]; mpch != NULL; mpch = mpch->next_macro ) {
                if( strcmp( mpch->macro_name, mcur->macro_name ) == 0 ) {
                    break;
                }
            }
            next_mcur = mcur->next_macro;
            if( mpch == NULL ) {                    // if this macro not found in PCH
                mcur->next_macro = PCHMacroHash[h]; // add it to PCH
                PCHMacroHash[hash] = mcur;
            }
        }
    }
    memcpy( MacHash, PCHMacroHash, MACRO_HASH_SIZE * sizeof( MEPTR ) );
    CMemFree( PCHMacroHash );
    PCHMacroHash = NULL;
    UndefMacroList = PCHUndefMacroList;
    return( 0 );
}

static char *FixupSymHashTable( char *p, unsigned symhash_count )
{
    SYM_HASHPTR hsym;
    int         index;

    for( ; symhash_count != 0; --symhash_count ) {
        hsym = (SYM_HASHPTR)p;
        p += PCHAlign( offsetof( id_hash_entry, name ) + strlen( hsym->name ) + 1 );
        index = PCHGetUInt( hsym->next_sym );
        hsym->next_sym = HashTab[index];
        HashTab[index] = hsym;
        if( PCHGetUInt( hsym->sym_type ) != 0 ) {
            hsym->sym_type = TypeArray + PCHGetUInt( hsym->sym_type );
        }
    }
    return( p );
}

static char *FixupSymbols( char *p, unsigned symbol_count )
{
    SYMPTR      symptr;
    SYM_ENTRY   sym;
    unsigned    handle;     // TODO: don't cheat!

    for( handle = 0; handle < symbol_count; ++handle ) {
        symptr = (SYMPTR)p;
        p += sizeof( SYM_ENTRY );
        if( PCHGetUInt( symptr->sym_type ) != 0 ) {
            symptr->sym_type = TypeArray + PCHGetUInt( symptr->sym_type );
        }
        symptr->seginfo = TextSegArray[PCHGetUInt( symptr->seginfo )];
        PCH_SymArray[handle] = symptr;
    }
    for( handle = 0; handle < (unsigned)(pointer_uint)SpecialSyms; ++handle ) {
        SymGet( &sym, (SYM_HANDLE)(pointer_uint)handle );  // Redo special syms
        symptr = PCH_SymArray[handle];
        *symptr = sym;
    }
    return( p );
}

static void FixupTypeIndexes( type_indices *typ_index )
{
    DATA_TYPE   i;
    int         index;

    for( i = TYP_BOOL; i < TYP_LAST_ENTRY; ++i ) {
        index = typ_index->basetype_index[i];
        if( index != 0 ) {
            BaseTypes[i] = TypeArray + index;
        }
    }
    VoidParmList[0] = BaseTypes[TYP_VOID];
    StringType = TypeArray + typ_index->stringtype_index;
    ConstCharType = TypeArray + typ_index->constchartype_index;
}

static char *FixupTypes( char *p, unsigned type_count )
{
    TYPEPTR         typ;
    parm_hash_idx   hash;
    array_info      *array;
    TYPEPTR         *parm_types;
    int             idx;

    InitTypeHashTables();
    typ = (TYPEPTR)p;
    TypeArray = typ - 1;
    for( ; type_count != 0; --type_count ) {
        if( typ->decl_type == TYP_FUNCTION )
            break;
        if( PCHGetUInt( typ->object ) != 0 ) {
            typ->object = TypeArray + PCHGetUInt( typ->object );
        }
        if( typ->decl_type == TYP_ARRAY ) {
            array = (array_info *)CMemAlloc( sizeof( array_info ) );
            array->dimension = PCHGetUInt( typ->u.array );
            array->unspecified_dim = ( array->dimension == 0 );
            typ->u.array = array;
        }
        AddTypeHash( typ );
        ++typ;
    }
    parm_types = (TYPEPTR *)( typ + type_count );
    for( ; type_count != 0; --type_count ) {
        hash = (parm_hash_idx)PCHGetUInt( typ->u.fn.parms );
        typ->next_type = FuncTypeHead[hash];
        FuncTypeHead[hash] = typ;
        if( PCHGetUInt( typ->object ) != 0 ) {
            typ->object = TypeArray + PCHGetUInt( typ->object );
        }
        if( PCHGetUInt( *parm_types ) == -1 ) {
            typ->u.fn.parms = NULL;
        } else {
            typ->u.fn.parms = parm_types;
            while( (idx = PCHGetUInt( *parm_types )) != -1 ) {
                *parm_types++ = TypeArray + idx;
            }
        }
        *parm_types++ = NULL;
        ++typ;
    }
    FixupTypeIndexes( (type_indices *)parm_types );
    return( (char *)parm_types + sizeof( type_indices ) );
}

static char *FixupEnums( char *p, TAGPTR parent )
{
    ENUMPTR     ep;
    ENUMPTR     *lnk;

    lnk = &ep;
    do {
        ep = (ENUMPTR)p;
        p += PCHGetUInt( ep->parent );
        ep->parent = parent;            // parent is union'ed with enum_len
        ep->next_enum = EnumTable[ep->hash];
        EnumTable[ep->hash] = ep;
        *lnk = ep;
        lnk = &ep->thread;
    } while( ep->thread != NULL );
    *lnk = NULL;
    return( p );
}

static char *FixupFields( char *p )
{
    FIELDPTR    field;
    FIELDPTR    *lnk;

    lnk = &field;
    do {
        field = (FIELDPTR)p;
        p += PCHGetUInt( field->next_field_same_hash );
        field->next_field_same_hash = NULL;
        field->field_type = TypeArray + PCHGetUInt( field->field_type );
        *lnk = field;
        lnk = &field->next_field;
    } while( field->next_field != NULL );
    *lnk = NULL;
    return( p );
}

static void FixupTag( TYPEPTR typ )
{
    switch( typ->decl_type ) {
    case TYP_STRUCT:
    case TYP_UNION:
    case TYP_ENUM:
        typ->u.tag = TagArray[PCHGetUInt( typ->u.tag )];
        break;
    default:
        break;
    }
}

static void FixupTagPointers( void )
{
    WalkTypeList( FixupTag );
}

static char *FixupTags( char *p, unsigned tag_count )
{
    TAGPTR      tag;
    TAGPTR      prevtag;
    TAGPTR      nexttag;
    TYPEPTR     typ;

    if( tag_count != 0 ) {
        TagArray = (TAGPTR *)CMemAlloc( tag_count * sizeof( TAGPTR ) );
    }
    tag = NULL;
    prevtag = NULL;
    for( ; tag_count != 0; --tag_count ) {
        tag = (TAGPTR)p;
        p += PCHGetUInt( tag->next_tag );
        TagArray[tag->refno] = tag;
        typ = TypeArray + PCHGetUInt( tag->sym_type );
        tag->sym_type = typ;
        if( typ->decl_type == TYP_ENUM ) {
            if( tag->u.enum_list != NULL ) {
                tag->u.enum_list = (ENUMPTR)p;
                p = FixupEnums( p, tag );
            }
        } else {
            if( tag->u.field_list != NULL ) {
                tag->u.field_list = (FIELDPTR)p;
                p = FixupFields( p );
            }
        }
        tag->next_tag = prevtag;
        prevtag = tag;
    }
    for( tag = prevtag; tag != NULL; tag = nexttag ) {
        nexttag = tag->next_tag;
        tag->next_tag = TagHash[tag->hash];
        TagHash[tag->hash] = tag;
    }
    FixupTagPointers();
    return( p );
}

#if ( _CPU == 8086 ) || ( _CPU == 386 )
static char *FixupAuxInfo( char *p, aux_info *info )
{
    unsigned            len;

    p += sizeof( aux_info );
    len = PCHGetUInt( info->parms );
    if( len != 0 ) {
        info->parms = (hw_reg_set *)p;
        p += len;
    }
    len = PCHGetUInt( info->objname );
    if( len != 0 ) {
        info->objname = (char *)p;
        p += len;
    }
    len = PCHGetUInt( info->code );
    if( len != 0 ) {
        info->code = (byte_seq *)p;
        p += len;
    }
    return( p );
}

static char *FixupPragmaInfo( char *p, unsigned pragma_count, unsigned entry_count )
{
    aux_entry       *ent;
    aux_entry       **lnk;
    aux_info        *info;
    aux_info        **info_array;
    unsigned        index;

    info_array = (aux_info **)CMemAlloc( pragma_count * sizeof( aux_info * ) );
    for( index = 0; index < pragma_count; ++index ) {
        if( index < PCH_FIRST_USER_INDEX - PCH_FIRST_INDEX ) {
            info = BuiltinAuxInfo + index;
            memcpy( info, p, sizeof( aux_info ) );
        } else {
            info = (aux_info *)p;
        }
        info_array[index] = info;
        p = FixupAuxInfo( p, info );
    }
    lnk = &AuxList;
    for( ; entry_count != 0; --entry_count ) {
        ent = (aux_entry *)p;
        p += PCHGetUInt( ent->next );
        ent->info = info_array[PCHGetUInt( ent->info )];
        *lnk = ent;
        lnk = &ent->next;
    }
    *lnk = NULL;
    CMemFree( info_array );
    return( p );
}
#endif

static char *FixupMsgLevels( char *p, unsigned len )
{
    memcpy( msg_level, p, len );
    p += PCHAlign( len );
    return( p );
}

static void FixupFNames( void )
{
    FNAMEPTR    *lnk;
    FNAMEPTR    flist;

    for( lnk = &FNames; (flist = *lnk) != NULL; lnk = &flist->next ) {
        if( FNAMECMPSTR( flist->name, FNameList->name ) == 0 ) {
            break;
        }
    }
    *lnk = FNameList;
    for( flist = FNameList; flist != NULL; flist = flist->next ) {
        flist->index_db = DBFILE_INVALID;
    }
}

static bool ValidHeader( pheader *pch )
{
    if( (pch->signature == PCH_SIGNATURE)
      && (pch->version == PCH_VERSION_HOST)
      && (pch->size_of_header == sizeof( pheader ))
      && (pch->size_of_int == TARGET_INT)
      && (pch->specialsyms_count == SymGetNumSpecialSyms())
      && (pch->pack_amount == PackAmount)
      && (pch->msglevel_len == sizeof( msg_level )) ) {
        return( true );
    }
    return( false );                // indicate unusable pre-compiled header
}

static int LoadPreCompiledHeader( char *p, pheader *pch )
{
    int         rc;

    rc = FixupDataStructures( p, pch );
    CMemFree( TagArray );
    CMemFree( TextSegArray );
    CMemFree( PCHMacroHash );
    return( rc );
}

static int FixupDataStructures( char *p, pheader *pch )
{
    p = FixupLibraries( p, pch->library_count );
    p = FixupAliases( p, pch->alias_count );
    p = FixupSegInfo( p, pch->seg_count );
    p = FixupTypes( p, pch->type_count );
    p = FixupTags( p, pch->tag_count );
#if ( _CPU == 8086 ) || ( _CPU == 386 )
    p = FixupPragmaInfo( p, pch->pragma_count, pch->pragma_entry_count );
#endif
    p = FixupSymHashTable( p, pch->symhash_count );
    p = FixupSymbols( p, pch->symbol_count );
    p = FixupMsgLevels( p, pch->msglevel_len );

    PCH_MaxSymHandle = pch->symbol_count;
    SetNextSymHandle( pch->symbol_count - 1 );
    IncLineCount = pch->incline_count;
    PragmaToggles = pch->toggles;
    FixupFNames();
    InitDebugTypes();
    InitDebugTags();
    return( 0 );
}

static bool SameCWD( const char *p )
{
    char        *cwd;
    int         same;
    char        buf[_MAX_PATH];

    cwd = getcwd( buf, _MAX_PATH );
    same = FNAMECMPSTR( cwd, p );
    return( same == 0 );
}

void FreePreCompiledHeader( void )
{
    FEfree( PCH_Start );
    FEfree( PCH_Macros );
    FEfree( PCH_SymArray );
}

static void AbortPreCompiledHeader( void )
{
    FreePreCompiledHeader();
    CMemFree( TagArray );
    CMemFree( TextSegArray );
    CMemFree( PCHMacroHash );
    PCH_Start = NULL;
    PCH_End = NULL;
    PCH_Macros = NULL;
    PCH_SymArray = NULL;
    PCH_MaxSymHandle = 0;
    TagArray = NULL;
    TextSegArray = NULL;
    FNameList = NULL;
    PCHMacroHash = NULL;
    IAliasNames = PCHIAliasNames;
    RestoreIncFileList();
    CompFlags.make_precompiled_header = true;   // force new PCH to be created
}

//========================================================================
//      This portion of the code checks to see if we can use the
//      existing pre-compiled header.
//      - all the predefined macros have the same definition
//      - all the include files are the same and have not been modified
//========================================================================

bool UsePreCompiledHeader( const char *filename )
{
    int                 handle;
    unsigned            size;
    size_t              len;
    char                *p;
    pheader             pch;

    handle = _sopen3( PCH_FileName, O_RDONLY | O_BINARY, SH_DENYWR );
    if( handle == -1 ) {
        CompFlags.make_precompiled_header = true;
        return( false );
    }
    PCH_Start = NULL;
    TextSegArray = NULL;
    TagArray = NULL;
    PCHMacroHash = NULL;
    PCHOldIncFileList = NULL;
    PCHOldIncListValid = false;
    size = read( handle, &pch, sizeof( pheader ) );
    if( size != sizeof( pheader ) ) {
        close( handle );
        PCHNote( PCHDR_READ_ERROR );
        AbortPreCompiledHeader();
        return( false );
    }
    if( !ValidHeader( &pch ) ) {
        close( handle );
        PCHNote( PCHDR_INVALID_HEADER );
        AbortPreCompiledHeader();
        return( false );
    }
    if( pch.gen_switches != GenSwitches || pch.target_switches != TargetSwitches ) {
        close( handle );
        PCHNote( PCHDR_DIFFERENT_OPTIONS );
        AbortPreCompiledHeader();
        return( false );
    }
    p = FEmalloc( pch.size );                   // allocate big memory block
    PCH_Start = p;
    PCH_End = p + pch.size;
    PH_size = read( handle, p, pch.size );      // read rest of the file
    PCH_Macros = FEmalloc( pch.macro_size );
    size = read( handle, PCH_Macros, pch.macro_size );
    close( handle );
    PCH_SymArray = (SYMPTR *)FEmalloc( pch.symbol_count * sizeof( SYMPTR ) );
    if( PH_size != pch.size || size != pch.macro_size ) {
        PCHNote( PCHDR_READ_ERROR );
        AbortPreCompiledHeader();
        return( false );
    }
    if( !SameCWD( p ) ) {
        PCHNote( PCHDR_DIFFERENT_CWD );
        AbortPreCompiledHeader();
        return( false );
    }
    if( CompFlags.cpp_ignore_env != pch.cpp_ignore_env || CompFlags.ignore_default_dirs != pch.ignore_default_dirs ) {
        PCHNote( PCHDR_INCFILE_DIFFERENT );
        AbortPreCompiledHeader();
        return( false );
    }
    p = FixupIncludes( p + pch.cwd_len, pch.file_count );
    p = FixupRoDirList( p, pch.rdir_count );
    p = FixupIncAliasList( p, pch.ialias_count );
    if( VerifyIncludes( filename ) ) {
        PCHNote( PCHDR_INCFILE_DIFFERENT );
        AbortPreCompiledHeader();
        return( false );
    }
    len = strlen( p ) + 1;              // get length of saved IncPathList
    if( FNAMECMPSTR( p, IncPathList ) != 0 ) {
        PCHNote( PCHDR_INCPATH_CHANGED );
        AbortPreCompiledHeader();
        return( false );
    }
    p += PCHAlign( len );
    p = FixupIncFileList( p, pch.incfile_count );
    if( VerifyMacros( PCH_Macros, pch.macro_count, pch.undef_macro_count ) != 0 ) {
        PCHNote( PCHDR_MACRO_CHANGED );
        AbortPreCompiledHeader();
        return( false );
    }
    LoadPreCompiledHeader( p, &pch );
    FreeOldIncFileList();
    PCH_FileName = NULL;
    return( true );
}

static void SetDebugType( TYPEPTR typ )
{
    typ->u1.debug_type = DBG_NIL_TYPE;
}

static void SetFuncDebugType( TYPEPTR typ, int index )
{
    /* unused parameters */ (void)index;

    typ->u1.debug_type = DBG_NIL_TYPE;
}

void InitDebugTypes( void )
{
    WalkTypeList( SetDebugType );
    WalkFuncTypeList( SetFuncDebugType );
}
