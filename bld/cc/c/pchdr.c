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
* Description:  Code to handle pre-compiled header files.
*
****************************************************************************/


#include "cvars.h"
#if defined( __WATCOMC__ ) && !defined( __UNIX__ )
    #include <direct.h>
#endif
#include "wio.h"
#include "watcom.h"
#include "pragdefn.h"
#include "autodept.h"
#include "sopen.h"

extern  TAGPTR  TagHash[TAG_HASH_SIZE + 1];

#define PH_BUF_SIZE     32768
#define PCH_SIGNATURE   (('H'<<24)|('C'<<16)|('P'<<8)|'W')     /* 'WPCH' */
#define PCH_VERSION     0x0124
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

#define PCHGetUInt(p)           ((unsigned)(pointer_int)(p))
#define PCHSetUInt(v)           ((void *)(pointer_int)((unsigned)(v)))

#define PCHWriteVar(m)          PCHWrite(&(m),sizeof(m))

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
static  int             PCHOldIncListValid; /* flag to indicate if PCHOldIncFileList is valid */

struct  pheader {
    unsigned long   signature;      //  'WPCH'
    unsigned long   version;
    unsigned        size_of_header;
    unsigned        size_of_int;
    unsigned        pack_amount;    // PackAmount
    unsigned long   gen_switches;   // GenSwitches
    unsigned long   target_switches;// TargetSwitches
    int             toggles;        // Toggles
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
    unsigned        msgflags_len;   // length of MsgFlags array
    unsigned        disable_ialias;
    unsigned        cpp_ignore_env;
    unsigned        ignore_curr_dirs;
};

static int FixupDataStructures( char *p, struct pheader *pch );

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
    PH_handle = sopen4( filename, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, SH_DENYRW, PMODE_RW );
    if( PH_handle == -1 ) {
        longjmp( PH_jmpbuf, 1 );
    }
}

static int _PCHWriteUnalign( const char *buf, unsigned len )
{
    unsigned    amt_written;

    if( PH_Buffer != NULL ) {
        for( ; len != 0; ) {
            amt_written = len;
            if( amt_written > PH_BufSize )
                amt_written = PH_BufSize;
            memcpy( PH_BufPtr, buf, amt_written );
            PH_BufSize -= amt_written;
            PH_BufPtr  += amt_written;
            buf += amt_written;
            len -= amt_written;
            if( PH_BufSize == 0 ) {         // if buffer is full
                PH_BufSize = PH_BUF_SIZE;
                PH_BufPtr  = PH_Buffer;
                amt_written = write( PH_handle, PH_Buffer, PH_BUF_SIZE );
                if( amt_written != PH_BUF_SIZE ) {
                    return( 1 );
                }
            }
        }
    } else {
        if( len != 0 ) {
            amt_written = write( PH_handle, buf, len );
            if( amt_written != len ) {
                return( 1 );
            }
        }
    }
    return( 0 );
}

static int PCHWrite( const void *bufptr, unsigned len )
{
    char        blank[8] = {0};
    int         rc;

    if( len != 0 ) {
        if( PH_computing_size ) {
            PH_size += PCHAlign( len );
        } else {
            rc = _PCHWriteUnalign( bufptr, len );
            return( rc | _PCHWriteUnalign( blank, PCHAlign( len ) - len ) );
        }
    }
    return( 0 );
}
#if 0
static int PCHWriteUnalign( const void *bufptr, unsigned len )
{

    if( len != 0 ) {
        if( PH_computing_size ) {
            PH_size += len;
        } else {
            return( _PCHWriteUnalign( bufptr, len ) );
        }
    }
    return( 0 );
}
#endif
static void FlushPHeader( void )
{
    unsigned    len;

    if( PH_BufSize != PH_BUF_SIZE ) {   // if buffer has some stuff in it
        len = PH_BUF_SIZE - PH_BufSize;
        if( write( PH_handle, PH_Buffer, len ) != len ) {
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
    int                 rc;
    struct pheader      pch;

    pch.signature         = PCH_SIGNATURE;
    pch.version           = PCH_VERSION_HOST;
    pch.size_of_header    = sizeof( struct pheader );
    pch.size_of_int       = TARGET_INT;
    pch.pack_amount       = PackAmount;
    pch.gen_switches      = GenSwitches;
    pch.target_switches   = TargetSwitches;
    pch.toggles           = Toggles;
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
    pch.symhash_count     = PH_SymHashCount;
    pch.symbol_count      = SymGetNumSyms();
    pch.specialsyms_count = SymGetNumSpecialSyms();
    pch.cwd_len           = PH_cwd_len;
    pch.msgflags_len      = MESSAGE_COUNT;
    pch.disable_ialias    = CompFlags.disable_ialias;
    pch.cpp_ignore_env    = CompFlags.cpp_ignore_env;

    rc  = PCHWriteVar( pch );
    rc |= PCHWrite( PH_Buffer + sizeof( struct pheader ), pch.cwd_len );
    if( rc != 0 ) {
        longjmp( PH_jmpbuf, rc );
    }
}

static void OutPutIncPathList( void )     // output include paths
{
    int         rc;
    unsigned    len;

    if( IncPathList == NULL ) {
        len = 0;
        rc = PCHWrite( &len, 1 );
    } else {
        len = strlen( IncPathList ) + 1;
        rc = PCHWrite( IncPathList, len );
    }
    if( rc != 0 ) {
        longjmp( PH_jmpbuf, rc );
    }
}

static void OutPutIncFileList( void )   // output primary include files
{
    int         rc;
    unsigned    len;
    INCFILE     *ifile;

    for( ifile = IncFileList; ifile != NULL; ifile = ifile->nextfile ) {
        len = sizeof( INCFILE ) + ifile->len;
        rc = PCHWrite( ifile, len );
        if( rc != 0 ) {
            longjmp( PH_jmpbuf, rc );
        }
        PH_IncFileCount++;
    }
}

static void OutPutLibraries( void )
{
    int             rc;
    unsigned        len;
    struct library_list *lib;

    for( lib = HeadLibs; lib != NULL; lib = lib->next ) {
        len = sizeof( struct library_list ) + strlen( lib->libname );
        rc = PCHWrite( lib, len );
        if( rc != 0 ) {
            longjmp( PH_jmpbuf, rc );
        }
        PH_LibraryCount++;
    }
}

static void OutPutAliases( void )
{
    int             rc;
    unsigned        len;
    struct alias_list   *alias;
    struct alias_list   tmp_alias;
    const char      *alias_name;
    const char      *alias_subst;

    for( alias = AliasHead; alias != NULL; alias = alias->next ) {
        memcpy( &tmp_alias, alias, sizeof( tmp_alias ) );
        alias_name = alias->name;
        alias_subst = alias->subst;
        if( alias_name != NULL ) {
            len = PCHAlign( strlen( alias_name ) + 1 );
            tmp_alias.name = (const void *)len;
        }
        if( alias_subst != NULL ) {
            len =  PCHAlign( strlen( alias_subst ) + 1 );
            tmp_alias.subst = (const void *)len;
        }
        rc = PCHWriteVar( tmp_alias );
        if( rc != 0 ) {
            longjmp( PH_jmpbuf, rc );
        }
        if( alias_name != NULL ) {
            rc = PCHWrite( alias_name, (unsigned)tmp_alias.name );
            if( rc != 0 ) {
                longjmp( PH_jmpbuf, rc );
            }
        }
        if( alias_subst != NULL ) {
            rc = PCHWrite( alias_subst, (unsigned)tmp_alias.subst );
        if( rc != 0 ) {
            longjmp( PH_jmpbuf, rc );
        }
        }
        PH_AliasCount++;
    }
}

static void OutPutMsgFlags( void )
{
    int         rc;

    if( MsgFlags != NULL ) {                            /* 06-jul-94 */
        rc = PCHWrite( MsgFlags, (MESSAGE_COUNT + 7) / 8 );
        if( rc != 0 ) {
            longjmp( PH_jmpbuf, rc );
        }
    }
}

static void OutPutIncludes( void )
{
    FNAMEPTR    flist;
    FNAMEPTR    next_flist;
    unsigned    len;
    int         rc;

    // don't want to include the primary source file, skip first item
    for( flist = FNames->next; flist != NULL; flist = next_flist ) {
        next_flist = flist->next;
        len = PCHAlign( strlen( flist->name ) + sizeof( struct fname_list ) );
        flist->fname_len = len;
        rc = PCHWrite( flist, len );
        flist->next = next_flist;
        if( rc != 0 ) {
            longjmp( PH_jmpbuf, rc );
        }
        PH_FileCount++;
    }
}

static void OutPutRoDirList( void )
{
    RDIRPTR     dirlist;
    RDIRPTR     next_dirlist;
    unsigned    len;
    int         rc;

    for( dirlist = PCHRDirNames; dirlist != NULL; dirlist = next_dirlist ) {
        next_dirlist = dirlist->next;
        len = PCHAlign( strlen( dirlist->name ) + sizeof( struct rdir_list ) );
        dirlist->name_len = len;
        rc = PCHWrite( dirlist, len );
        dirlist->next = next_dirlist;
        if( rc != 0 ) {
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
    unsigned    len;
    int         rc;

    for( aliaslist = IAliasNames; aliaslist != NULL; aliaslist = next_aliaslist ) {
        next_aliaslist = aliaslist->next;
        real_name = aliaslist->real_name;
        len = PCHAlign( sizeof( struct ialias_list ) + strlen( aliaslist->alias_name ) + strlen( aliaslist->real_name ) + 1 );
        aliaslist->total_len = len;
        aliaslist->alias_name_len = strlen( aliaslist->alias_name );
        rc = PCHWrite( aliaslist, len );
        aliaslist->real_name = real_name;
        aliaslist->next = next_aliaslist;
        if( rc != 0 ) {
            longjmp( PH_jmpbuf, rc );
        }
        PH_IAliasCount++;
    }
}

static void OutPutSegInfo( void )
{
    textsegment     *seg;
    textsegment     *next;
    int             rc;
    unsigned        len;

    for( seg = TextSegList; seg != NULL; seg = seg->next ) {
        ++PH_SegCount;
        seg->index = PH_SegCount;
        len = strlen( seg->segname );               // segment name
        len += strlen( &seg->segname[len + 1] );    // class name
        len += sizeof( textsegment ) + 1;
        len = PCHAlign( len );
        next = seg->next;                       // save next pointer
        seg->textsegment_len = len;             // replace with len
        rc = PCHWrite( seg, len );
        seg->next = next;                       // restore next pointer
        if( rc != 0 ) {
            longjmp( PH_jmpbuf, rc );
        }
    }
}

static void OutPutEnums( ENUMPTR ep, TAGPTR parent )
{
    int         rc;
    unsigned    len;

    for( ; ep != NULL; ep = ep->thread ) {
        len = PCHAlign( strlen( ep->name ) + sizeof( ENUMDEFN ) );
        ep->enum_len = len;
        rc = PCHWrite( ep, len );
        ep->parent = parent;            // enum_len is union'ed with parent
        if( rc != 0 ) {
            longjmp( PH_jmpbuf, rc );
        }
    }
}

static void OutPutFields( FIELDPTR field )
{
    int         rc;
    int         len;
    FIELDPTR    next_field;
    TYPEPTR     typ;

    for( ; field != NULL; field = next_field ) {
        len = PCHAlign( strlen( field->name ) + sizeof( FIELD_ENTRY ) );
        next_field = field->next_field;         // save pointer
        field->field_len = len;                 // replace with length
        if( next_field == NULL ) {
            field->field_len = - len;           // marker for end of list
        }
        typ = field->field_type;                // save type pointer
        field->field_type_index = typ->type_index;// replace with type index
        rc = PCHWrite( field, len );
        field->next_field = next_field;         // restore pointer
        field->field_type = typ;                // restore type pointer
        if( rc != 0 ) {
            longjmp( PH_jmpbuf, rc );
        }
    }
}

static void OutPutATag( TAGPTR tag )
{
    int         rc;
    TYPEPTR     typ;
    unsigned    len;

    typ = tag->sym_type;
    tag->sym_type_index = typ->type_index;
    len = strlen( tag->name ) + sizeof( TAGDEFN );
    rc = PCHWrite( tag, len );
    tag->sym_type = typ;
    if( rc != 0 ) {
        longjmp( PH_jmpbuf, rc );
    }
    if( typ->decl_type == TYPE_ENUM ) {
        OutPutEnums( tag->u.enum_list, tag );
    } else {
        OutPutFields( tag->u.field_list );
    }
}

static void OutPutTags( void )
{
    WalkTagList( OutPutATag );
}

void SetTypeIndex( TYPEPTR typ )
{
    ++PH_TypeCount;
    typ->type_index = PH_TypeCount;
}

static void SetFuncTypeIndex( TYPEPTR typ, int index )
{
    // index;      /* unused */
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
    tag->tag_index = PH_TagCount;
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

static int WriteType( TYPEPTR typ )
{
    int         rc;
    TYPEPTR     object;

    object = typ->object;
    if( object != NULL ) {
        typ->object_index = object->type_index;
        rc = PCHWriteVar( *typ );
        typ->object = object;
    } else {
        rc = PCHWriteVar( *typ );
    }
    return( rc );
}

struct type_indices {
    int     basetype_index[TYPE_LAST_ENTRY];
    int     stringtype_index;
    int     constchartype_index;
};

static void OutPutTypeIndexes( void )                       /* 02-jan-95 */
{
    TYPEPTR             typ;
    int                 rc;
    int                 i;
    struct type_indices typ_index;

    for( i = TYPE_CHAR; i < TYPE_LAST_ENTRY; i++ ) {
        typ = BaseTypes[i];
        if( typ == NULL ) {
            typ_index.basetype_index[i] = 0;
        } else {
            typ_index.basetype_index[i] = typ->type_index;
        }
    }
    typ_index.stringtype_index = StringType->type_index;
    typ_index.constchartype_index = ConstCharType->type_index;
    rc = PCHWriteVar( typ_index );
    if( rc != 0 ) {
        longjmp( PH_jmpbuf, rc );
    }
}

static void OutPutAType( TYPEPTR typ )
{
    TAGPTR          tag;
    int             rc;
    struct array_info   *array;

    rc = 0;
    switch( typ->decl_type ) {
    case TYPE_ARRAY:
        array = typ->u.array;                   // save pointer
        typ->u.array_dimension = array->dimension;      // replace with dim
        rc = WriteType( typ );
        typ->u.array = array;                   // restore pointer
        break;
    case TYPE_STRUCT:
    case TYPE_UNION:
    case TYPE_ENUM:
        tag = typ->u.tag;                               // save tag
        typ->u.tag_index = tag->tag_index;              // replace with index
        rc = WriteType( typ );
        typ->u.tag = tag;                               // restore tag pointer
        break;
    default:
        if( typ->decl_type != TYPE_FUNCTION ) {
            rc = WriteType( typ );
        }
        break;
    }
    if( rc != 0 ) {
        longjmp( PH_jmpbuf, rc );
    }
}

static void OutPutAFuncType( TYPEPTR typ, int index )
{
    TYPEPTR     *parm_list;
    int         rc;

    parm_list = typ->u.fn.parms;                // save pointer
    typ->u.fn.parm_index = index;               // replace with index
    rc = WriteType( typ );
    typ->u.fn.parms = parm_list;                // restore pointer
    if( rc != 0 ) {
        longjmp( PH_jmpbuf, rc );
    }
}

static void OutPutFuncParmList( TYPEPTR typ, int index )
{
    TYPEPTR     *parm_list;
    int         rc;
    TYPEPTR     parm_typ;

    // index;      /* unused */
    parm_list = typ->u.fn.parms;
    if( parm_list != NULL ) {
        for( ; *parm_list != NULL; ++parm_list ) {
            parm_typ = PCHSetUInt( (*parm_list)->type_index );
            rc = PCHWriteVar( parm_typ );
            if( rc != 0 ) {
                longjmp( PH_jmpbuf, rc );
            }
        }
    }
    parm_typ = PCHSetUInt( -1 );
    rc = PCHWriteVar( parm_typ );
    if( rc != 0 ) {
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
    int                 rc;
    unsigned            len;
    unsigned            objname_len;
    unsigned            code_len;

    info_parms = info->parms;
    info_objname = info->objname;
    info_code = info->code;
    len = sizeof( aux_info );
    if( info_parms != NULL ) {
        len = 0;
        regs = info_parms;
        for( ;; ) {
            len += sizeof( hw_reg_set );
            if( HW_CEqual( *regs, HW_EMPTY ) )
                break;
            ++regs;
        }
        info->parms = PCHSetUInt( len );
    }
    if( info_objname != NULL ) {
        objname_len = strlen( info_objname ) + 1;
        info->objname = PCHSetUInt( PCHAlign( objname_len ) );
    }
    if( info_code != NULL ) {
        code_len = info_code->length + offsetof( byte_seq, data );
        info->code = PCHSetUInt( PCHAlign( code_len ) );
    }
    rc = PCHWriteVar( *info );
    if( info_parms != NULL ) {
        regs = info_parms;
        for( ;; ) {
            rc |= PCHWriteVar( *regs );
            if( HW_CEqual( *regs, HW_EMPTY ) )
                break;
            ++regs;
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
    if( rc != 0 ) {
        longjmp( PH_jmpbuf, rc );
    }
}

static void OutPutPragmaInfo( void )
{
    aux_entry       *ent;
    aux_entry       *ent_next;
    aux_info        *ent_info;
    int             index;
    int             rc;
    unsigned        len;

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
        len = sizeof( aux_entry ) + strlen( ent->name );
        ent->next = PCHSetUInt( PCHAlign( len ) );
        ent->info = PCHSetUInt( ent_info->index - PCH_FIRST_INDEX );
        rc = PCHWrite( ent, len );
        ent->info = ent_info;                   // restore pointer
        ent->next = ent_next;                   // restore pointer
        if( rc != 0 ) {
            longjmp( PH_jmpbuf, rc );
        }
        ++PH_PragmaEntryCount;
    }
}
#endif

static void OutPutMacros( void )
{
    int         i;
    int         rc;
    MEPTR       mentry;
    MEPTR       next_macro;
    unsigned    mentry_len;

    PH_MacroCount = 0;
    PH_MacroSize = PH_size;
    for( i = 0; i < MACRO_HASH_SIZE; ++i ) {
        for( mentry = MacHash[i]; mentry != NULL; mentry = mentry->next_macro ) {
            mentry_len = mentry->macro_len;
            next_macro = mentry->next_macro;        // save pointer
            mentry->macro_index = i;                // replace with hash index
            rc = PCHWrite( mentry, mentry_len );
            mentry->next_macro = next_macro;        // restore pointer
            if( rc != 0 ) {
                longjmp( PH_jmpbuf, rc );
            }
            ++PH_MacroCount;
        }
    }
    /* write out undefined macro list.  26-may-94 */
    PH_UndefMacroCount = 0;
    for( mentry = UndefMacroList; mentry != NULL; mentry = mentry->next_macro ) {
        mentry_len = mentry->macro_len;
        rc = PCHWrite( mentry, mentry_len );
        if( rc != 0 ) {
            longjmp( PH_jmpbuf, rc );
        }
        ++PH_UndefMacroCount;
    }
    PH_MacroSize = PH_size - PH_MacroSize;
}

static void OutPutSymHashTable( void )
{
    SYM_HASHPTR     hsym;
    SYM_HASHPTR     next_hsymptr;
    SYM_HASHPTR     sym_list;
    TYPEPTR         typ;
    int             i;
    int             rc;
    unsigned        len;

    for( i = 0; i < SYM_HASH_SIZE; i++ ) {
        // reverse the list
        sym_list = NULL;
        for( hsym = HashTab[i]; hsym != NULL; hsym = next_hsymptr ) {
            next_hsymptr = hsym->next_sym;
            hsym->next_sym = sym_list;
            sym_list = hsym;
            ++PH_SymHashCount;
        }
        HashTab[i] = NULL;
        rc = 0;
        for( hsym = sym_list; hsym != NULL; hsym = next_hsymptr ) {
            next_hsymptr = hsym->next_sym;
            hsym->hash_index = i;
            typ = hsym->sym_type;               // save type pointer
            if( typ != NULL ) {
                hsym->sym_type_index = typ->type_index; // replace with index
            }
            len = strlen( hsym->name ) + sizeof( struct sym_hash_entry );
            rc |= PCHWrite( hsym, len );
            hsym->sym_type = typ;               // restore type pointer
            hsym->next_sym = HashTab[i];
            HashTab[i] = hsym;
        }
        if( rc != 0 ) {
            longjmp( PH_jmpbuf, rc );
        }
    }
}

static void OutPutSymbols( void )
{
    SYM_ENTRY   sym;
    SYM_HANDLE  sym_handle;
    int         rc;

    if( PH_computing_size ) {
        PH_size += SymGetNumSyms() * sizeof( SYM_ENTRY );
    } else {
        for( sym_handle = SymGetFirst(); sym_handle != SYM_INVALID; sym_handle = SymGetNext( sym_handle ) ) {
            SymGet( &sym, sym_handle );
            if( sym.sym_type != NULL ) {
                sym.sym_type_index = sym.sym_type->type_index;
            }
            if( sym.seginfo != NULL ) {
                sym.seginfo_index = sym.seginfo->index;
            }
            sym.name = NULL; // can't carry a name across
            rc = PCHWriteVar( sym );
            if( rc != 0 ) {
                longjmp( PH_jmpbuf, rc );
            }
        }
    }
}

void OutPutEverything( void )
{
    PH_SymHashCount = 0;
    PH_FileCount    = 0;
    PH_RDirCount    = 0;
    PH_IAliasCount  = 0;
    PH_SegCount     = 0;
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
    OutPutMsgFlags();
    OutPutMacros();
}

void InitBuildPreCompiledHeader( void )
//Save any before info when building pre compiled headers
{
    RDIRPTR     start;

    start = RDirNames;
    if( start != NULL ){
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
    cwd = getcwd( PH_Buffer + sizeof( struct pheader ),
                  PH_BUF_SIZE - sizeof( struct pheader ) );
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
    CompFlags.make_precompiled_header = 0;
}

//========================================================================
//      This portion of the code loads the pre-compiled header and
//      rebuilds the data structures by replacing all the indices with
//      pointers, reconstructing hash tables, etc.
//========================================================================

static char *FixupIncFileList( char *p, unsigned incfile_count )
{
    INCFILE     *ifile;
    unsigned    len;

    PCHOldIncFileList = IncFileList;
    PCHOldIncListValid = 1;
    IncFileList = NULL;
    for( ; incfile_count != 0; --incfile_count ) {
        ifile = (INCFILE *)p;
        AddIncFileList( ifile->filename );
        len = sizeof( INCFILE ) + ifile->len;
        len = _RoundUp( len, sizeof( int ) );
        p += len;
    }
    return( p );
}

static void RestoreIncFileList( void )
{
    if( PCHOldIncListValid ) {
        FreeIncFileList();
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
        PCHOldIncListValid = 0;
    }
}

static char *FixupIncludes( char *p, unsigned file_count )
{
    FNAMEPTR    flist;
    unsigned    len;

    FNameList = NULL;
    if( file_count != 0 ) {
        FNameList = (FNAMEPTR)p;
        do {
        flist = (FNAMEPTR)p;
            len = flist->fname_len;
        flist->fullpath = NULL;
            p += len;
            flist->next = (FNAMEPTR)p;
        } while( --file_count > 0 );
        flist->next = NULL;
    }
    return( p );
}

static char *FixupRoDirList( char *p, unsigned list_count )
{
    RDIRPTR     dirlist;
    RDIRPTR     *lnk;
    unsigned    len;

    for( lnk = &RDirNames; (dirlist = *lnk) != NULL; ) {
        lnk = &dirlist->next;
    }
    for( ; list_count != 0; --list_count ) {
        dirlist = (RDIRPTR)p;
        len = dirlist->name_len;
        p += len;
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
    unsigned    len;

    lnk = &IAliasNames;
    for( ; list_count != 0; --list_count ) {
        aliaslist = (IALIASPTR)p;
        len = aliaslist->total_len;
        p += len;
        *lnk = aliaslist;
        aliaslist->real_name = aliaslist->alias_name + aliaslist->alias_name_len + 1;
        lnk = &aliaslist->next;
    }
    /* Tack pre-existing aliases onto the end (so that they're searched last) */
    *lnk = PCHIAliasNames;
    return( p );
}

static int VerifyIncludes( const char *filename )
{
    FNAMEPTR    flist;
    time_t      mtime;
    FNAMEPTR    fnew;

    // skip the primary source file (first item)
    fnew = FNames->next;
    for( flist = FNameList; flist != NULL; flist = flist->next ) {
        if( fnew != NULL ) {
            if( FNAMECMPSTR( fnew->name, flist->name ) != 0 ) {
                return( TRUE );                
            }
            fnew = fnew->next;
        } else if( filename != NULL ) {
            if( FNAMECMPSTR( filename, flist->name ) != 0 ) {
                return( TRUE );                
            }
            filename = NULL;
        }
        if( flist->rwflag ){
            if( SrcFileInRDir( flist ) ) {
                flist->rwflag = FALSE;
            }
        }
        if( flist->rwflag ) {
            mtime = _getFilenameTimeStamp( flist->name );
            if( flist->mtime != mtime || mtime == 0 ) {
                PCHNote( PCHDR_INCFILE_CHANGED, flist->name  );
#if 0
                printf( "%s old %d new %d\n", flist->name, flist->mtime, mtime );
#endif
                return( TRUE );
            }
        }
    }
    return( FALSE );
}

static char *FixupLibraries( char *p, unsigned library_count )
{
    struct library_list *lib;
    unsigned            len;

    HeadLibs = NULL;
    if( library_count != 0 ) {
        lib = (struct library_list *)p;
        HeadLibs = lib;
        for( ;; ) {
            len = sizeof( struct library_list ) + strlen( lib->libname );
            len = _RoundUp( len, sizeof( int ) );
            p += len;
            lib->next = (struct library_list *)p;
            --library_count;
            if( library_count == 0 )
                break;
            lib = (struct library_list *)p;
        }
        lib->next = NULL;
    }
    return( p );
}

static char *FixupAliases( char *p, unsigned alias_count )
{
    struct alias_list   *alias;
    struct alias_list   *new_alias;
    unsigned        len;
    char                *str;

    AliasHead = NULL;
    if( alias_count != 0 ) {
        new_alias = CMemAlloc( sizeof( struct alias_list ) );
        memcpy( new_alias, p, sizeof( struct alias_list ) );
        AliasHead = alias = new_alias;
        for( ;; ) {
            len = sizeof( struct alias_list );
            p += len;
            len = (unsigned)new_alias->name;
            if( len ) {
                str = CMemAlloc( len );
                memcpy( str, p, len );
                new_alias->name = str;
                p += len;
            }
            len = (unsigned)new_alias->subst;
            if( len ) {
                str = CMemAlloc( len );
                memcpy( str, p, len );
                new_alias->subst = str;
                p += len;
        }
            --alias_count;
            if( alias_count == 0 )
                break;
            new_alias = CMemAlloc( sizeof( struct alias_list ) );
            memcpy( new_alias, p, sizeof( struct alias_list ) );
            alias->next = new_alias;
            alias = new_alias;
        }
        alias->next = NULL;
    }
    return( p );
}

static char *FixupSegInfo( char *p, unsigned seg_count )
{
    textsegment     *seg;
    unsigned        len;

    TextSegArray = (textsegment **)CMemAlloc( (seg_count + 1) * sizeof( textsegment * ) );
    TextSegArray[0] = NULL;
    for( ; seg_count != 0; --seg_count ) {
        seg = (textsegment *)p;
        TextSegArray[seg->index + 1] = seg;
        len = seg->textsegment_len;
        p += len;
        seg->next = (textsegment *)p;
    }
    if( seg_count != 0 )
        seg->next = NULL;
    return( p );
}

static char *FixupMacros( char *p, unsigned macro_count )
{
    int         i;
    MEPTR       mentry;
    unsigned    mentry_len;

    for( ; macro_count != 0; --macro_count ) {
        mentry = (MEPTR)p;
        i = mentry->macro_index;                // get hash index
        mentry->next_macro = PCHMacroHash[i];
        PCHMacroHash[i] = mentry;
        mentry_len = _RoundUp( mentry->macro_len, sizeof( int ) );
        p += mentry_len;
    }
    return( p );
}

static char *FixupUndefMacros( char *p, unsigned undef_macro_count )
{   // Read in as written out
    MEPTR       mentry, *lnk;
    unsigned    mentry_len;

    PCHUndefMacroList = NULL;
    lnk = &PCHUndefMacroList;
    for( ; undef_macro_count != 0; --undef_macro_count ) {
        mentry = (MEPTR)p;
        *lnk = mentry;
        lnk = &mentry->next_macro;
        mentry_len = _RoundUp( mentry->macro_len, sizeof( int ) );
        p += mentry_len;
    }
    *lnk = NULL;
    return( p );
}

static int VerifyMacros( char *p, unsigned macro_count, unsigned undef_count )
{
    int         i;
    MEPTR       mpch;
    MEPTR       mcur;
    int         macro_compare;

    PCHMacroHash = (MEPTR *)CMemAlloc( MACRO_HASH_SIZE * sizeof( MEPTR ) );
    p = FixupMacros( p, macro_count );
    p = FixupUndefMacros( p, undef_count );
    for( i = 0; i < MACRO_HASH_SIZE; ++i ) {
        MEPTR       prev_mpch;

        prev_mpch = NULL;
        for( mpch = PCHMacroHash[i]; mpch != NULL; mpch = mpch->next_macro ) {
            if( mpch->macro_flags & MFLAG_DEFINED_BEFORE_FIRST_INCLUDE ) {
                for( mcur = MacHash[i]; mcur != NULL; mcur = mcur->next_macro ) {
                    if( strcmp( mcur->macro_name, mpch->macro_name ) == 0 ) {
                        macro_compare = MacroCompare( mpch, mcur );
                        if( mpch->macro_flags & MFLAG_REFERENCED ) {
                            if( macro_compare == 0 )
                                break;
                            return( -1 );       // abort: macros different
                        }
                        if( macro_compare != 0 ) { /* if different */
                            /* delete macro from pch, add new one */
                            if( prev_mpch == NULL ) {
                                PCHMacroHash[i] = mpch->next_macro;
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
                        PCHMacroHash[i] = mpch->next_macro;
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
    for( i = 0; i < MACRO_HASH_SIZE; ++i ) {
        for( mcur = MacHash[i]; mcur != NULL; mcur = mcur->next_macro ) {
            for( mpch = PCHMacroHash[i]; mpch != NULL; mpch = mpch->next_macro ) {
                if( strcmp( mpch->macro_name, mcur->macro_name ) == 0 ) {
                    break;
                }
            }
            if( mpch == NULL || !(mpch->macro_flags & MFLAG_DEFINED_BEFORE_FIRST_INCLUDE) ) {
            // macro may either have been undef'd (mpch == NULL ) or undef'd and defined
                if( mcur->macro_flags & MFLAG_USER_DEFINED ) {  //compiler defined macros not saved on undefs
                    for( mpch = PCHUndefMacroList; mpch != NULL; mpch = mpch->next_macro ) {
                        if( strcmp( mpch->macro_name, mcur->macro_name ) == 0 ) {
                            if( MacroCompare( mpch, mcur ) != 0 ) {
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

    for( i = 0; i < MACRO_HASH_SIZE; ++i ) {
        MEPTR       next_mcur;

        for( mcur = MacHash[i]; mcur != NULL; mcur = next_mcur ) {
            for( mpch = PCHMacroHash[i]; mpch != NULL; mpch = mpch->next_macro ) {
                if( strcmp( mpch->macro_name, mcur->macro_name ) == 0 ) {
                    break;
                }
            }
            next_mcur = mcur->next_macro;
            if( mpch == NULL ) {                    // if this macro not found in PCH
                mcur->next_macro = PCHMacroHash[i]; // add it to PCH
                PCHMacroHash[i] = mcur;
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
    int         i;
    unsigned    len;

    for( ; symhash_count != 0; --symhash_count ) {
        hsym = (SYM_HASHPTR)p;
        i = hsym->hash_index;
        hsym->next_sym = HashTab[i];
        HashTab[i] = hsym;
        if( hsym->sym_type_index != 0 ) {
            hsym->sym_type = TypeArray + hsym->sym_type_index;
        }
        len = strlen( hsym->name ) + sizeof( struct sym_hash_entry );
        len = _RoundUp( len, sizeof( int ) );
        p += len;
    }
    return( p );
}

static char *FixupSymbols( char *p, unsigned symbol_count )
{
    SYMPTR      symptr;
    SYM_ENTRY   sym;
    unsigned    sym_handle;     // TODO: don't cheat!

    sym_handle = 0;
    for( ; symbol_count != 0; --symbol_count ) {
        symptr = (SYMPTR)p;
        if( symptr->sym_type_index != 0 ) {
            symptr->sym_type = TypeArray + symptr->sym_type_index;
        }
        symptr->seginfo = TextSegArray[symptr->seginfo_index];
        PCH_SymArray[sym_handle] = symptr;
        p += sizeof( SYM_ENTRY );
        ++sym_handle;
        }
    for( sym_handle = 0; sym_handle < (unsigned)SpecialSyms; sym_handle++ ) {
        SymGet( &sym, (SYM_HANDLE)sym_handle );  // Redo special syms
        symptr  = PCH_SymArray[sym_handle];
        *symptr = sym;
    }
    return( p );
}

static void FixupTypeIndexes( struct type_indices *typ_index ) /* 02-jan-95 */
{
    int         i;
    int         index;

    for( i = TYPE_CHAR; i < TYPE_LAST_ENTRY; i++ ) {
        index = typ_index->basetype_index[i];
        if( index != 0 ) {
            BaseTypes[i] = TypeArray + index;
        }
    }
    VoidParmList[0] = BaseTypes[TYPE_VOID];
    StringType    = TypeArray + typ_index->stringtype_index;
    ConstCharType = TypeArray + typ_index->constchartype_index;
}

static char *FixupTypes( char *p, unsigned type_count )
{
    TYPEPTR         typ;
    int             index;
    struct array_info   *array;
    union parmtype {
        TYPEPTR parm_typ;
        int     type_index;
    } *parm_list;

    InitTypeHashTables();
    typ = (TYPEPTR)p;
    TypeArray = typ - 1;
    for( ; type_count != 0; --type_count ) {
        if( typ->decl_type == TYPE_FUNCTION )
            break;
        if( typ->object_index != 0 ) {
            typ->object = &TypeArray[typ->object_index];
        }
        if( typ->decl_type == TYPE_ARRAY ) {
            array = (struct array_info *)CMemAlloc( sizeof( struct array_info ) );
            array->dimension = typ->u.array_dimension;
            array->unspecified_dim = ( array->dimension == 0 );
            typ->u.array = array;
        }
        AddTypeHash( typ );
        ++typ;
    }
    parm_list = (union parmtype *)(typ + type_count);
    for( ; type_count != 0; --type_count ) {
        index = typ->u.fn.parm_index;
        typ->next_type = FuncTypeHead[index];
        FuncTypeHead[index] = typ;
        if( typ->object_index != 0 ) {
            typ->object = &TypeArray[typ->object_index];
        }
        if( parm_list->type_index == -1 ) {
            typ->u.fn.parms = NULL;
        } else {
            typ->u.fn.parms = (TYPEPTR *)parm_list;
            for( ;; ) {
                index = parm_list->type_index;
                if( index == -1 )
                    break;
                parm_list->parm_typ = TypeArray + index;
                parm_list++;
            }
            parm_list->parm_typ = NULL;
        }
        parm_list++;
        ++typ;
    }
    FixupTypeIndexes( (struct type_indices *)parm_list );
    return( (char *)parm_list + sizeof( struct type_indices ) );
}

static char *FixupEnums( char *p, TAGPTR parent )
{
    ENUMPTR     ep;

    for( ;; ) {
        ep = (ENUMPTR)p;
        p += ep->enum_len;
        ep->parent = parent;            // parent is union'ed with enum_len
        ep->next_enum = EnumTable[ep->hash];
        EnumTable[ep->hash] = ep;
        if( ep->thread == NULL )
            break;
        ep->thread = (ENUMPTR)p;
    }
    return( p );
}

static char *FixupFields( char *p )
{
    FIELDPTR    field;
    int         len;

    for( ;; ) {
        field = (FIELDPTR)p;
        field->field_type = TypeArray + field->field_type_index;
        len = field->field_len;
        p += len;
        field->next_field = (FIELDPTR)p;
        if( len < 0 ) {
            break;
        }
    }
    field->next_field = NULL;
    p -= len + len;
    return( p );
}

static void FixupTag( TYPEPTR typ )
{
    switch( typ->decl_type ) {
    case TYPE_STRUCT:
    case TYPE_UNION:
    case TYPE_ENUM:
        typ->u.tag = TagArray[typ->u.tag_index];
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
    unsigned    len;

    if( tag_count != 0 ) {
        TagArray = (TAGPTR *)CMemAlloc( tag_count * sizeof( TAGPTR ) );
    }
    tag = NULL;
    prevtag = NULL;
    for( ; tag_count != 0; --tag_count ) {
        tag = (TAGPTR)p;
        TagArray[tag->tag_index] = tag;
        typ = TypeArray + tag->sym_type_index;
        tag->sym_type = typ;
        len = strlen( tag->name ) + sizeof( TAGDEFN );
        len = _RoundUp( len, sizeof( int ) );
        p += len;
        if( typ->decl_type == TYPE_ENUM ) {
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
    int             index;

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
#if 0
static char *FixupMsgFlags( char *p, unsigned len )
{
    if( len != 0 ) {
        MsgFlags = (unsigned char *)p;
        len = ( len + 7 ) / 8;
        p += PCHAlign( len );
    }
    return( p );
}
#endif
void FixupFNames( void )
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
        flist->index_db = -1;
    }
}

int ValidHeader( struct pheader *pch )
{
    if( (pch->signature == PCH_SIGNATURE)
      && (pch->version == PCH_VERSION_HOST)
      && (pch->size_of_header == sizeof( struct pheader ))
      && (pch->size_of_int == TARGET_INT)
      && (pch->specialsyms_count == SymGetNumSpecialSyms())
      && (pch->pack_amount == PackAmount) ) {
        return( 1 );
    }
    return( 0 );                // indicate unusable pre-compiled header
}

int LoadPreCompiledHeader( char *p, struct pheader *pch )
{
    int         rc;

    rc = FixupDataStructures( p, pch );
    CMemFree( TagArray );
    CMemFree( TextSegArray );
    CMemFree( PCHMacroHash );
    return( rc );
}

extern void SetNextSymHandle( unsigned val );

static int FixupDataStructures( char *p, struct pheader *pch )
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
    if( pch->msgflags_len != 0 ) {                      /* 06-jul-94 */
        MsgFlags = (unsigned char *)p;
        p += pch->msgflags_len;
    }
    PCH_MaxSymHandle = pch->symbol_count;
    SetNextSymHandle( pch->symbol_count - 1 );
    IncLineCount = pch->incline_count;
    Toggles = pch->toggles;
    FixupFNames();
    InitDebugTypes();
    InitDebugTags();
    return( 0 );
}

int SameCWD( char *p )
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

void AbortPreCompiledHeader( void )
{
    FreePreCompiledHeader();
    CMemFree( TagArray );
    CMemFree( TextSegArray );
    CMemFree( PCHMacroHash );
    PCH_Start        = NULL;
    PCH_End          = NULL;
    PCH_Macros       = NULL;
    PCH_SymArray     = NULL;
    PCH_MaxSymHandle = 0;
    TagArray         = NULL;
    TextSegArray     = NULL;
    FNameList        = NULL;
    PCHMacroHash     = NULL;
    IAliasNames      = PCHIAliasNames;
    RestoreIncFileList();
    CompFlags.make_precompiled_header = 1;      // force new PCH to be created
}

//========================================================================
//      This portion of the code checks to see if we can use the
//      existing pre-compiled header.
//      - all the predefined macros have the same definition
//      - all the include files are the same and have not been modified
//========================================================================

int UsePreCompiledHeader( const char *filename )
{
    int                 handle;
    unsigned            len;
    char                *p;
    struct pheader      pch;

    handle = sopen3( PCH_FileName, O_RDONLY | O_BINARY, SH_DENYWR );
    if( handle == -1 ) {
        CompFlags.make_precompiled_header = 1;
        return( -1 );
    }
    PCH_Start = NULL;
    TextSegArray = NULL;
    TagArray = NULL;
    PCHMacroHash = NULL;
    PCHOldIncFileList = NULL;
    PCHOldIncListValid = 0;
    len = read( handle, &pch, sizeof( struct pheader ) );
    if( len != sizeof( struct pheader ) ) {
        close( handle );
        PCHNote( PCHDR_READ_ERROR );
        AbortPreCompiledHeader();
        return( -1 );
    }
    if( !ValidHeader( &pch ) ) {
        close( handle );
        PCHNote( PCHDR_INVALID_HEADER );
        AbortPreCompiledHeader();
        return( -1 );
    }
    if( pch.gen_switches != GenSwitches
      || pch.target_switches != TargetSwitches ) {
        close( handle );
        PCHNote( PCHDR_DIFFERENT_OPTIONS );
        AbortPreCompiledHeader();
        return( -1 );
    }
    p = FEmalloc( pch.size );                   // allocate big memory block
    PCH_Start = p;
    PCH_End = p + pch.size;
    PH_size = read( handle, p, pch.size );      // read rest of the file
    PCH_Macros = FEmalloc( pch.macro_size );
    len = read( handle, PCH_Macros, pch.macro_size );
    close( handle );
    PCH_SymArray = (SYMPTR *)FEmalloc( pch.symbol_count * sizeof( SYMPTR ) );
    if( PH_size != pch.size || len != pch.macro_size ) {
        PCHNote( PCHDR_READ_ERROR );
        AbortPreCompiledHeader();
        return( -1 );
    }
    if( !SameCWD( p ) ) {
        PCHNote( PCHDR_DIFFERENT_CWD );
        AbortPreCompiledHeader();
        return( -1 );
    }
    if( CompFlags.cpp_ignore_env != pch.cpp_ignore_env ) {
        PCHNote( PCHDR_INCFILE_DIFFERENT );
        AbortPreCompiledHeader();
        return( -1 );
    }
    p = FixupIncludes( p + pch.cwd_len, pch.file_count );
    p = FixupRoDirList( p, pch.rdir_count );
    p = FixupIncAliasList( p, pch.ialias_count );
    if( CompFlags.disable_ialias != pch.disable_ialias || VerifyIncludes( filename ) ) {
        PCHNote( PCHDR_INCFILE_DIFFERENT );
        AbortPreCompiledHeader();
        return( -1 );
    }
    len = PCHAlign( strlen( p ) + 1 );              // get length of saved IncPathList
    if( ((IncPathList == NULL) && (strlen( p ) > 0))
      || ((IncPathList != NULL) && (FNAMECMPSTR( p, IncPathList ) != 0)) ) {
        PCHNote( PCHDR_INCPATH_CHANGED );
        AbortPreCompiledHeader();
        return( -1 );
    }
    p = FixupIncFileList( p + len, pch.incfile_count );
    if( VerifyMacros( PCH_Macros, pch.macro_count, pch.undef_macro_count ) != 0 ) {
        PCHNote( PCHDR_MACRO_CHANGED );
        AbortPreCompiledHeader();
        return( -1 );
    }
    LoadPreCompiledHeader( p, &pch );
    FreeOldIncFileList();
    PCH_FileName = NULL;
    return( 0 );
}

void SetDebugType( TYPEPTR typ )
{
    typ->debug_type = DBG_NIL_TYPE;
}

void SetFuncDebugType( TYPEPTR typ, int index )
{
    // index;      /* unused */
    typ->debug_type = DBG_NIL_TYPE;
}

void InitDebugTypes( void )
{
    WalkTypeList( SetDebugType );
    WalkFuncTypeList( SetFuncDebugType );
}
