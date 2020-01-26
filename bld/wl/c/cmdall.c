/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Commands common to all executable formats
*
****************************************************************************/


#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include "walloca.h"
#include "linkstd.h"
#include "msg.h"
#include "alloc.h"
#include "cmdutils.h"
#include "wlnkmsg.h"
#include "strtab.h"
#include "dbgall.h"
#include "cmddos.h"
#include "cmdnov.h"
#include "cmdos2.h"
#include "cmd16m.h"
#include "cmdphar.h"
#include "cmdrdv.h"
#include "cmdqnx.h"
#include "cmdelf.h"
#include "cmdzdos.h"
#include "cmdraw.h"
#include "cmdxxx.h"
#include "cmdline.h"
#include "symtrace.h"
#include "objio.h"
#include "loadfile.h"
#include "carve.h"
#include "permdata.h"
#include "pathlist.h"
#include "cmdall.h"
#include "library.h"
#include "sysblock.h"

#include "clibext.h"


#define DEFAULT_INC_NAME "__wlink.ilk"

static struct {
    union {
        file_list   *file;
        member_list *module;
    } u;
} LastFile;

static file_list        **LastLibFile;
static ORDER_CLASS      *CurrOClass;
static ORDER_SEGMENT    *CurrOSeg;

void ResetCmdAll( void )
/**********************/
{
    LastFile.u.file = NULL;
    LastLibFile = NULL;
    UsrLibPath = NULL;
}

static bool ProcDosSeg( void )
/*****************************
 * process DOSSEG option
 */
{
    LinkState |= LS_DOSSEG_FLAG;
    DEBUG(( DBG_OLD, "dosseg" ));
    return( true );
}

static bool ProcName( void )
/**************************/
{
    if( !GetToken( SEP_NO, TOK_INCLUDE_DOT | TOK_IS_FILENAME ) )
        return( false );
    CmdFlags &= ~CF_UNNAMED;
    if( Name != NULL ) {
        _LnkFree( Name );
    }
    Name = tostring();   // just keep the name around for now.
    return( true );
}

static bool ProcDwarfDBI( void )
/******************************/
{
    if( LinkFlags & (LF_ANY_DBI_FLAG & ~LF_DWARF_DBI_FLAG) ) {
        LnkMsg( LOC+LINE+WRN+MSG_MULT_DBI_FORMATS, NULL );
        return( true );
    }
    LinkFlags |= LF_DWARF_DBI_FLAG;
    return( true );
}

static bool ProcWatcomDBI( void )
/*******************************/
{
    if( LinkFlags & (LF_ANY_DBI_FLAG & ~LF_OLD_DBI_FLAG) ) {
        LnkMsg( LOC+LINE+WRN+MSG_MULT_DBI_FORMATS, NULL );
        return( true );
    }
    LinkFlags |= LF_OLD_DBI_FLAG;
    return( true );
}

static bool ProcCodeviewDBI( void )
/*********************************/
{
    if( LinkFlags & (LF_ANY_DBI_FLAG & ~LF_CV_DBI_FLAG) ) {
        LnkMsg( LOC+LINE+WRN+MSG_MULT_DBI_FORMATS, NULL );
        return( true );
    }
    LinkFlags |= LF_CV_DBI_FLAG;
    return( true );
}

static bool ProcLine( void )
/**************************/
{
    if( (LinkFlags & LF_ANY_DBI_FLAG) == 0 ) {
        LinkFlags |= LF_DWARF_DBI_FLAG;
    }
    DBIFlag |= DBI_LINE;
    return( true );
}

#if 0
static bool ProcDBIStatic( void )
/*******************************/
{
    if( (LinkFlags & LF_ANY_DBI_FLAG) == 0 ) {
        LinkFlags |= LF_DWARF_DBI_FLAG;
    }
    DBIFlag |= DBI_STATICS;
    return( true );
}
#endif

static bool ProcType( void )
/**************************/
{
    if( (LinkFlags & LF_ANY_DBI_FLAG) == 0 ) {
        LinkFlags |= LF_DWARF_DBI_FLAG;
    }
    DBIFlag |= DBI_TYPE;
    return( true );
}

static bool ProcLocal( void )
/***************************/
{
    if( (LinkFlags & LF_ANY_DBI_FLAG) == 0 ) {
        LinkFlags |= LF_DWARF_DBI_FLAG;
    }
    DBIFlag |= DBI_LOCAL;
    return( true );
}

static bool ProcAll( void )
/*************************/
{
    if( (LinkFlags & LF_ANY_DBI_FLAG) == 0 ) {
        LinkFlags |= LF_DWARF_DBI_FLAG;
    }
    DBIFlag |= DBI_ALL;
    return( true );
}

static bool AddAlias( void )
/***************************
 * add an individual alias
 */
{
    char        *name;
    size_t      namelen;

    DUPBUF_STACK( name, Token.this, Token.len );
    namelen = Token.len;
    if( !GetToken( SEP_EQUALS, TOK_INCLUDE_DOT ) ) {
        return( false );
    }
    MakeSymAlias( name, namelen, Token.this, Token.len );
    return( true );
}

static bool ProcAlias( void )
/***************************/
{
    return( ProcArgList( &AddAlias, TOK_INCLUDE_DOT ) );
}

static bool AddReference( void )
/******************************/
{
    symbol      *sym;

    sym = SymOp( ST_REFERENCE_SYM, Token.this, Token.len );
    sym->info |= SYM_DCE_REF;   /* make sure it stays around */
    return( true );
}

static bool ProcReference( void )
/*******************************/
{
    return( ProcArgList( &AddReference, TOK_INCLUDE_DOT ) );
}

static bool ProcOSName( void )
/****************************/
{
    if( GetToken( SEP_EQUALS, TOK_INCLUDE_DOT ) ) {
        if( FmtData.osname != NULL ) {
            _LnkFree( FmtData.osname );
        }
        FmtData.osname = tostring();
        return( true );
    }
    return( false );
}

static bool ProcEliminate( void )
/********************************
 * turn on dead code elimination
 */
{
    LinkFlags |= LF_STRIP_CODE;
    return( true );
}

static bool ProcMaxErrors( void )
/********************************
 * set a maximum number of errors
 * for the linker to generate
 */
{
    if( !GetLong( &MaxErrors ) )
        return( false );
    LinkFlags |= LF_MAX_ERRORS_FLAG;
    return( true );
}

static bool ProcSymFile( void )
/*****************************/
{
    if( GetToken( SEP_EQUALS, TOK_INCLUDE_DOT | TOK_IS_FILENAME ) ) {
        if( SymFileName != NULL ) {
            _LnkFree( SymFileName );
        }
        SymFileName = FileName( Token.this, Token.len, E_SYM, false );
    }
    return( true );
}

static void *AddObjFile( const char *name, char *member, file_list **filelist )
/*****************************************************************************/
{
    file_list       *new_entry;
    member_list     *new_member;

    new_member = NULL;
    if( member != NULL ) {
        _ChkAlloc( new_member, offsetof( member_list, name ) + strlen( member ) + 1 );
        new_member->flags = DBIFlag;
        strcpy( new_member->name, member );
        new_member->next = NULL;
        _LnkFree( member );
        for( new_entry = CurrSect->files; new_entry != NULL; new_entry = new_entry->next_file ) {
            if( FNAMECMPSTR( new_entry->infile->name.u.ptr, name ) == 0 ) {
                CmdFlags |= CF_MEMBER_ADDED;
                if( new_entry->u.member != NULL ) {
                    LinkList( &new_entry->u.member, new_member );
                    return( new_member );
                } else {
                    _LnkFree( new_member );      // user did a stupid thing.
                    return( new_entry->u.member );
                }
            }
        }
    }
    new_entry = AllocNewFile( new_member );
    if( new_member != NULL ) {
        new_entry->infile = AllocUniqueFileEntry( name, UsrLibPath );
        new_entry->infile->status |= INSTAT_LIBRARY;
    } else {
        new_entry->infile = AllocFileEntry( name, ObjPath );
    }
    *filelist = new_entry;
    return( new_entry );
}

static bool AddLibFile( void )
/****************************/
{
    char        *ptr;
    char        *membname;
    file_list   *entry;

    CmdFlags &= ~CF_MEMBER_ADDED;
    ptr = GetFileName( &membname, false );
    if( membname != NULL ) {
        LnkMsg( LOC+LINE+WRN+MSG_NO_MEMB_IN_LIBFILE, NULL );
        _LnkFree( membname );
        _LnkFree( ptr );
        return( true );
    }
    entry = AllocNewFile( NULL );
    entry->infile = AllocFileEntry( ptr, UsrLibPath );
    entry->next_file = *LastLibFile;
    *LastLibFile = entry;
    LastLibFile = &entry->next_file;
    if( *LastLibFile == NULL ) {    // no file directives found yet
        CurrFList = LastLibFile;
    }
    entry->infile->status |= INSTAT_USE_LIBPATH;
    _LnkFree( ptr );
    return( true );
}

static bool ProcLibFile( void )
/******************************
 * process LIBFILE command
 */
{
    if( (LinkFlags & (LF_DWARF_DBI_FLAG | LF_OLD_DBI_FLAG | LF_NOVELL_DBI_FLAG)) == 0 ) {
        CmdFlags |= CF_FILES_BEFORE_DBI;
    }
    if( LastLibFile == NULL ) {
        LastLibFile = &Root->files;
    }
    return( ProcArgList( &AddLibFile, TOK_INCLUDE_DOT | TOK_IS_FILENAME ) );
}

static bool AddModFile( void )
/****************************/
{
    char        *ptr;
    char        *membname;

    ptr = GetFileName( &membname, false );
    AddHTableElem( Root->modFilesHashed, ptr );
    LinkFlags |= LF_GOT_CHGD_FILES;
    if( membname != NULL ) {
        _LnkFree( membname );
    }
    return( true );
}

static bool AddFile( void )
/*************************/
{
    char        *ptr;
    char        *membname;
    file_list   **temp;

    CmdFlags &= ~CF_MEMBER_ADDED;
#ifdef _EXE
    if( CmdFlags & CF_AUTOSECTION ) {
        if( CmdFlags & CF_SECTION_THERE ) { // is section there already?
            CmdFlags &= ~CF_SECTION_THERE;
        } else {
            MakeNewSection();
        }
    }
#endif
    ptr = GetFileName( &membname, true );
    temp = CurrFList;
    if( *CurrFList != NULL ) {
        CurrFList = &(*CurrFList)->next_file;
    }
    LastFile.u.file = AddObjFile( ptr, membname, CurrFList );
    if( CmdFlags & CF_MEMBER_ADDED ) {
        CurrFList = temp;               // go back to previous entry.
    } else if( membname != NULL ) {     // 1st member added
        LastFile.u.module = LastFile.u.file->u.member;
        CmdFlags |= CF_MEMBER_ADDED;
    }
    _LnkFree( ptr );
    return( true );
}

static bool ProcFiles( void )
/****************************
 * process FILE command
 */
{
    if( (LinkFlags & (LF_DWARF_DBI_FLAG | LF_OLD_DBI_FLAG | LF_NOVELL_DBI_FLAG)) == 0 ) {
        CmdFlags |= CF_FILES_BEFORE_DBI;
    }
    return( ProcArgList( &AddFile, TOK_INCLUDE_DOT | TOK_IS_FILENAME ) );
}

static bool ProcModFiles( void )
/******************************/
{
    return( ProcArgList( &AddModFile, TOK_INCLUDE_DOT | TOK_IS_FILENAME ) );
}


static bool AddLib( void )
/************************/
{
    char        *ptr;
    file_list   *result;

    ptr = FileName( Token.this, Token.len, E_LIBRARY, false );
    result = AddObjLib( ptr, LIB_PRIORITY_MAX );
    result->flags |= STAT_USER_SPECD;
#ifdef _EXE
    if( CmdFlags & CF_SET_SECTION ) {
        result->flags |= STAT_LIB_FIXED;
        result->ovlref = GetOvlRef();
    }
#endif
    if( CmdFlags & CF_DOING_OPTLIB ) {
        result->infile->status |= INSTAT_NO_WARNING;
    }
    DEBUG(( DBG_BASE, "library: %s", ptr ));
    _LnkFree( ptr );
    return( true );
}

bool ProcLibrary( void )
/***********************
 * process LIB command
 */
{
    if( (LinkFlags & (LF_DWARF_DBI_FLAG | LF_OLD_DBI_FLAG | LF_NOVELL_DBI_FLAG)) == 0
        && !IsSystemBlock() ) {
        CmdFlags |= CF_FILES_BEFORE_DBI;
    }
    return( ProcArgList( &AddLib, TOK_INCLUDE_DOT | TOK_IS_FILENAME ) );
}

static bool ProcOptLib( void )
/****************************/
{
    bool    retval;

    CmdFlags |= CF_DOING_OPTLIB;
    retval = ProcLibrary();
    CmdFlags &= ~CF_DOING_OPTLIB;
    return( retval );
}

static bool ProcLibPath( void )
/******************************
 * process libpath command
 */
{
    if( !GetToken( SEP_NO, TOK_INCLUDE_DOT | TOK_IS_FILENAME ) ) {
        LnkMsg( LOC+LINE+WRN+MSG_VALUE_INCORRECT, "s", "LIBPATH" );
        return( true );
    }
    AddLibPaths( Token.this, Token.len, true );  // true == add to front.
    return( true );
}

static bool ProcPath( void )
/***************************
 * process PATH option
 */
{
    path_entry      *new_path;
    char            *p;
    const char      *end;

    if( GetToken( SEP_NO, TOK_INCLUDE_DOT | TOK_IS_FILENAME ) ) {
        _ChkAlloc( new_path, sizeof( path_entry ) + Token.len );
        end = Token.this + Token.len;
        p = new_path->name;
        while( Token.this != end ) {
            if( p != new_path->name )
                *p++ = PATH_LIST_SEP;
            Token.this = GetPathElement( Token.this, end, &p );
        }
        *p = '\0';
        new_path->next = ObjPath;
        ObjPath = new_path;
        DEBUG(( DBG_BASE, "path: %s", new_path->name ));
        return( true );
    }
    return( false );
}

static bool ProcMap( void )
/**************************
 * process MAP option
 */
{
    MapFlags |= MAP_FLAG;
    if( GetToken( SEP_EQUALS, TOK_INCLUDE_DOT | TOK_IS_FILENAME ) ) {
        if( MapFName != NULL ) {
            _LnkFree( MapFName );
        }
        MapFName = FileName( Token.this, Token.len, E_MAP, false );
        DEBUG(( DBG_OLD, "produce map file" ));
    }
    return( true );
}

static bool ProcMapLines( void )
/*******************************
 * process MAPLINES option
 */
{
    MapFlags |= MAP_LINES;
    return( true );
}

static bool ProcStack( void )
/****************************
 * process STACK option
 */
{
    unsigned_32     value;
    bool            ret;

    LinkFlags |= LF_STK_SIZE_FLAG;
    ret = GetLong( &value );
    if( ret ) {
        StackSize = value;
    }
    return( ret );
}

static bool ProcNameLen( void )
/******************************
 * process NAMELEN option
 */
{
    unsigned_32     value;
    bool            ret;

    ret = GetLong( &value );
    if( ret ) {
        if( value == 0 ) {
            LnkMsg( LOC+LINE+WRN+MSG_VALUE_INCORRECT, "s", "NAMELEN" );
        } else {
            NameLen = value;
        }
    }
    return( ret );
}

static bool ProcCase( void )
/***************************
 * process CASE option
 */
{
    LinkFlags |= LF_CASE_FLAG;
    SetSymCase();
    DEBUG(( DBG_OLD, "case" ));
    return( true );
}

static bool ProcNoCaseExact( void )
/**********************************
 * process nocaseexact option
 */
{
    LinkFlags &= ~LF_CASE_FLAG;
    SetSymCase();
    DEBUG(( DBG_OLD, "nocase" ));
    return( true );
}

static bool ProcNoExtension( void )
/*********************************/
{
    CmdFlags |= CF_NO_EXTENSION;
    return( true );
}

static bool ProcNoCache( void )
/*****************************/
{
    LinkFlags &= ~LF_CACHE_FLAG;
    LinkFlags |= LF_NOCACHE_FLAG;
    return( true );
}

static bool ProcCache( void )
/***************************/
{
    LinkFlags &= ~LF_NOCACHE_FLAG;
    LinkFlags |= LF_CACHE_FLAG;
    return( true );
}

static bool AddDisable( void )
/*****************************
 * disable an error message number
 */
{
    unsigned_16     value;

    if( getatoi( &value ) == ST_IS_ORDINAL ) {
        value = value % 1000;
        if( value <= MSG_MAX_ERR_MSG_NUM ) {
            ClearBit( MsgFlags, value );
        } else {
            LnkMsg( LOC+LINE+WRN+MSG_INVALID_MSG_NUM, NULL );
        }
    } else {
        LnkMsg( LOC+LINE+WRN+MSG_INVALID_MSG_NUM, NULL );
    }
    return( true );
}

static bool ProcDisable( void )
/*****************************/
{
    return( ProcArgList( &AddDisable, TOK_NORMAL ) );
}

static bool ProcNoDefLibs( void )
/********************************
 * process CASE option
 */
{
    return( true );
}

static bool ProcVerbose( void )
/*****************************/
{
    MapFlags |= MAP_VERBOSE;
    DEBUG(( DBG_OLD, "verbose map file" ));
    return( true );
}

static bool ProcUndefsOK( void )
/******************************/
{
    LinkFlags |= LF_UNDEFS_ARE_OK;
    DEBUG(( DBG_OLD, "undefined symbols are OK" ));
    return( true );
}

static bool ProcNoUndefsOK( void )
/********************************/
{
    LinkFlags &= ~LF_UNDEFS_ARE_OK;
    return( true );
}

static bool ProcRedefsOK( void )
/******************************/
{
    LinkFlags |= LF_REDEFS_OK;
    return( true );
}

static bool ProcNoRedefs( void )
/******************************/
{
    LinkFlags &= ~LF_REDEFS_OK;
    return( true );
}

static bool ProcCVPack( void )
/*********************/
{
    LinkFlags |= LF_CVPACK_FLAG;
    return( true );
}

static bool ProcIncremental( void )
/*********************************/
{
#if !defined( __DOS__ )
    if( CmdFlags & CF_AFTER_INC ) {
        LnkMsg( LOC+LINE+ERR+MSG_INC_NEAR_START, NULL );
    }
    LinkFlags |= LF_INC_LINK_FLAG;
    if( GetToken( SEP_EQUALS, TOK_INCLUDE_DOT | TOK_IS_FILENAME ) ) {
        IncFileName = FileName( Token.this, Token.len, E_ILK, false );
    } else if( Name != NULL ) {
        IncFileName = FileName( Name, strlen( Name ), E_ILK, true );
    } else {
        IncFileName = ChkStrDup( DEFAULT_INC_NAME );
    }
    ReadPermData();
#endif
    return( true );
}

static bool ProcQuiet( void )
/***************************/
{
    LinkFlags |= LF_QUIET_FLAG;
    return( true );
}

static bool ProcMangledNames( void )
/**********************************/
{
    LinkFlags |= LF_DONT_UNMANGLE;
    return( true );
}

static bool ProcOpResource( void )
/********************************/
{
    if( GetToken( SEP_EQUALS, TOK_INCLUDE_DOT | TOK_IS_FILENAME ) ) {
        FmtData.res_name_only = true;
        FmtData.resource = tostring();
    } else if( GetToken( SEP_NO, TOK_INCLUDE_DOT ) ) {
        FmtData.resource = tostring();
    } else {
        return( false );
    }
    return( true );
}

static bool ProcStatics( void )
/*****************************/
{
    MapFlags |= MAP_STATICS;
    return( true );
}

static bool ProcArtificial( void )
/********************************/
{
    MapFlags |= MAP_ARTIFICIAL;
    return( true );
}

static bool ProcAlphabetical( void )
/**********************************/
{
    MapFlags |= MAP_ALPHA;
    return( true );
}

static bool ProcGlobal( void )
/****************************/
{
    MapFlags |= MAP_GLOBAL;
    return( true );
}

static bool ProcJapanese( void )
/******************************/
{
    CmdFlags |= CF_LANGUAGE_JAPANESE;
    return( true );
}

static bool ProcChinese( void )
/*****************************/
{
    CmdFlags |= CF_LANGUAGE_CHINESE;
    return( true );
}

static bool ProcKorean( void )
/****************************/
{
    CmdFlags |= CF_LANGUAGE_KOREAN;
    return( true );
}

static bool ProcShowDead( void )
/******************************/
{
    LinkFlags |= LF_SHOW_DEAD;
    return( true );
}

static bool ProcVFRemoval( void )
/*******************************/
{
    LinkFlags |= LF_VF_REMOVAL;
    return( true );
}

static bool ProcStart( void )
/***************************/
{
    char        *name;

    if( !GetToken( SEP_EQUALS, TOK_INCLUDE_DOT ) )
        return( false );
    StartInfo.user_specd = true;
    DUPSTR_STACK( name, Token.this, Token.len );
    SetStartSym( name );
    return( true );
}

static bool GetPackValue( unsigned_32 *value, const char *message )
/*****************************************************************/
{
    ord_state   ret;

    if( !HaveEquals( TOK_NORMAL ) )
        return( false );
    ret = getatol( value );
    if( ret != ST_IS_ORDINAL ) {
        LnkMsg( LOC+LINE+WRN+MSG_VALUE_INCORRECT, "s", message );
        return( true );
    }
    if( *value == 0 )
        *value = 1;
    return( true );
}

static bool ProcPackcode( void )
/******************************/
{
    unsigned_32     value;

    if( GetPackValue( &value, "PACKCODE" ) ) {
        PackCodeLimit = value;
        LinkFlags |= LF_PACKCODE_FLAG;
        return( true );
    }
    return( false );
}

static bool ProcPackdata( void )
/******************************/
{
    unsigned_32     value;

    if( GetPackValue( &value, "PACKDATA" ) ) {
        PackDataLimit = value;
        LinkFlags |= LF_PACKDATA_FLAG;
        return( true );
    }
    return( false );
}

#if defined( _OS2 ) || defined( _EXE ) || defined ( _QNX )
static bool ProcNewSegment( void )
/*********************************
 * force the start of a new auto-group
 * after the previous object file.
 */
{
    if( LastFile.u.file == NULL ) {
        LnkMsg( LOC+LINE+WRN+MSG_NEWSEG_BEFORE_OBJ, NULL );
    } else {
        if( CmdFlags & CF_MEMBER_ADDED ) {
            LastFile.u.module->flags |= MOD_LAST_SEG;
        } else {
            LastFile.u.file->flags |= STAT_LAST_SEG;
        }
    }
    return( true );
}
#endif

sysblock *FindSysBlock( const char *name )
/****************************************/
{
    sysblock    *sys;

    for( sys = SysBlocks; sys != NULL; sys = sys->next ) {
        if( sys->name != NULL && stricmp( sys->name, name ) == 0 ) {
            return( sys );
        }
    }
    return( NULL );
}

static sysblock *FindSystemBlock( const char *name )
/**************************************************/
{
    sysblock    *sys;
    sysblock    *tmpblk;
    size_t      len;

    tmpblk = FindSysBlock( name );
    if( tmpblk == NULL ) {
        len = strlen( name );
        for( sys = SysBlocks; sys != NULL; sys = sys->next ) {
            if( sys->name != NULL && strnicmp( sys->name, name, len ) == 0 ) {
                if( tmpblk == NULL ) {
                    tmpblk = sys;
                } else {
                    return( NULL );
                }
            }
        }
    }
    return( tmpblk );
}

static bool ProcSysDelete( void )
/*******************************/
{
    return( true );
}

static void CopyBlocks( void *copyp, const char *data, size_t size )
/******************************************************************/
{
    char    **copyptr = copyp;

    memcpy( *copyptr, data, size );
    *copyptr += size;
}

static void GetCommandBlock( sysblock **hdr, const char *name, parse_entry *endtab )
/**********************************************************************************/
{
    char        *copyptr;
    sysblock    *sys;
    stringtable strtab;

    InitStringTable( &strtab, false );
    AddBufferStringTable( &strtab, &strtab, offsetof( sysblock, commands ) );
    while( !ProcOne( endtab, SEP_SPACE, false ) ) {
        Token.thumb = false;
        AddBufferStringTable( &strtab, Token.this, Token.len );
        AddCharStringTable( &strtab, ' ' );
        RestoreParser();
    }
    AddCharStringTable( &strtab, '\0' );
    _ChkAlloc( copyptr, GetStringTableSize( &strtab ) );
    sys = (sysblock *)copyptr;
    WriteStringTable( &strtab, CopyBlocks, &copyptr );
    FiniStringTable( &strtab );
    if( name != NULL ) {
        sys->name = ChkStrDup( name );
    } else {
        sys->name = NULL;
    }
    LinkList( hdr, sys );
}

static bool ProcSysEnd( void )
/*****************************
 * finished parsing a system block
 */
{
    return( true );
}

static bool ProcEndLink( void )
/******************************
 * finished parsing a link section
 */
{
    return( true );
}

static char **getStubNamePtr( void )
/**********************************/
{
#ifdef _OS2
    if( HintFormat( MK_OS2 | MK_PE | MK_WIN_VXD ) ) {
        return( &FmtData.u.os2.stub_file_name );
    }
#endif
#ifdef _DOS16M
    if( HintFormat( MK_DOS16M ) ) {
        return( &FmtData.u.d16m.stub );
    }
#endif
#ifdef _PHARLAP
    if( HintFormat( MK_PHAR_LAP ) ) {
        Extension = E_LOAD;     /* want .exe instead of .exp now */
        return( &FmtData.u.phar.stub );
    }
#endif
    return( NULL );
}

static bool ProcStub( void )
/**************************/
{
    char        *name;
    char        **nameptr;

    nameptr = getStubNamePtr();
    if( !HaveEquals( TOK_INCLUDE_DOT | TOK_IS_FILENAME ) || nameptr == NULL )
        return( false );
    name = FileName( Token.this, Token.len, E_LOAD, false );
    if( *nameptr == NULL ) {
        *nameptr = name;
    } else if( IsSystemBlock() ) {
        /* if we're in a system block, we don't want to override a user specification */
        _LnkFree( name );
    } else {
        _LnkFree( *nameptr );
        *nameptr = name;
    }
    return( true );
}

static bool ProcNoStub( void )
/****************************/
{
    FmtData.u.os2.no_stub = true;
    return( true );
}

static bool ProcVersion( void )
/*****************************/
{
    version_state   result;
    version_block   vb;

    /* set required limits, 0 = no limit */
    vb.major = 100;
    vb.minor = 100;
    vb.revision = 100;
    vb.message = "VERSION";
    result = GetGenVersion( &vb, GENVER_MAJOR | GENVER_MINOR | GENVER_REVISION, true );
    if( result != GENVER_ERROR ) {
        FmtData.major = vb.major;
        FmtData.minor = vb.minor;
        FmtData.revision = vb.revision;
        return( true );
    }
    return( false );
}

static bool ProcImplib( void )
/****************************/
{
    FmtData.make_implib = true;
    FmtData.make_impfile = false;
    _LnkFree( FmtData.implibname );
    if( GetToken( SEP_EQUALS, TOK_IS_FILENAME|TOK_INCLUDE_DOT ) ) {
        FmtData.implibname = tostring();
    }
    return( true );
}

static bool ProcImpFile( void )
/*****************************/
{
    FmtData.make_implib = true;
    FmtData.make_impfile = true;
    _LnkFree( FmtData.implibname );
    if( GetToken( SEP_EQUALS, TOK_IS_FILENAME|TOK_INCLUDE_DOT ) ) {
        FmtData.implibname = tostring();
    }
    return( true );
}

static bool AddSymTrace( void )
/*****************************/
{
    symbol      *sym;

    sym = SymOp( ST_REFERENCE_SYM, Token.this, Token.len );
    sym->info |= SYM_TRACE;
    return( true );
}

static bool ProcSymTrace( void )
/******************************/
{
    LinkFlags |= LF_TRACE_FLAG;
    return( ProcArgList( &AddSymTrace, TOK_INCLUDE_DOT ) );
}

static bool AddModTrace( void )
/*****************************/
{
    char            *membname;
    char            *fname;
    trace_info      *info;

    fname = GetFileName( &membname, false );
    _ChkAlloc( info, sizeof( trace_info ) );
    info->u.name = fname;
    info->member = membname;
    info->found = false;        // used for matching libraries
    info->next = TraceList;
    TraceList = info;
    return( true );
}

static bool ProcModTrace( void )
/******************************/
{
    LinkFlags |= LF_TRACE_FLAG;
    return( ProcArgList( &AddModTrace, TOK_INCLUDE_DOT | TOK_IS_FILENAME ) );
}

static bool ProcFarCalls( void )
/******************************/
{
    LinkFlags |= LF_FAR_CALLS_FLAG;
    DEBUG(( DBG_OLD, "Far Calls optimization" ));
    return( true );
}

static bool ProcNoFarCalls( void )
/********************************/
{
    LinkFlags &= ~LF_FAR_CALLS_FLAG ;
    DEBUG(( DBG_OLD, "No Far Calls optimization" ));
    return( true );
}

#ifdef _RAW
static bool ProcOutputRaw( void )
/*******************************/
{
   FmtData.output_raw = true;
   return( true );
}

static bool ProcOutputHex( void )
/*******************************/
{
   FmtData.output_hex = true;
   return( true );
}

static bool ProcOutputStart( void )
/*********************************/
{
   FmtData.output_start = true;
   return( true );
}

static bool ProcOutputOffset( void )
/**********************************/
{
    ord_state   retval;
    unsigned_32 value;

    if( !GetToken( SEP_EQUALS, TOK_NORMAL ) ) {
        return( false );
    }
    retval = getatol( &value );
    if( retval == ST_IS_ORDINAL && (value <= ( 0xFFFFUL << FmtData.SegShift ) || HintFormat( ~MK_SEGMENTED )) ) {
        FmtData.output_offset = value;
        return( true );
    } else {
        LnkMsg(LOC+LINE+WRN+MSG_VALUE_INCORRECT, "s", "OUTPUT OFFSET");
        return( false );
    }
}

static bool ProcOutputHshift( void )
/**********************************/
{
    ord_state   ret;
    unsigned_16 value;

    if( !GetToken( SEP_EQUALS, TOK_NORMAL ) ) {
        return( false );
    }
    ret = getatoi( &value );
    if( ret == ST_IS_ORDINAL && value < 16) {
        FmtData.HexSegShift = 16 - value;
        FmtData.output_hshift = true;
        return( true );
    } else {
        LnkMsg(LOC+LINE+WRN+MSG_VALUE_TOO_LARGE, "s", "OUTPUT HSHIFT");
        return( false );
    }
}
#endif

static bool ProcHshift( void )
/****************************/
{
    ord_state   ret;
    unsigned_16 value;

    if( !GetToken( SEP_EQUALS, TOK_NORMAL ) ) {
        return( false );
    }
    ret = getatoi( &value );
    if( ret == ST_IS_ORDINAL && value < 16) {
        FmtData.Hshift = value;
        SetSegMask();
        return( true );
    } else {
        LnkMsg(LOC+LINE+WRN+MSG_VALUE_TOO_LARGE, "s", "HSHIFT");
        return( false );
    }
}

static bool ProcFillchar( void )
/******************************/
{
    ord_state   ret;
    unsigned_16 value;

    if( !GetToken( SEP_EQUALS, TOK_NORMAL ) ) {
        return( false );
    }
    ret = getatoi( &value );
    if( ret == ST_IS_ORDINAL && value < 256) {
        FmtData.FillChar = value;
        return( true );
    } else {
        LnkMsg(LOC+LINE+WRN+MSG_VALUE_TOO_LARGE, "s", "FILLCHAR");
        return( false );
    }
}

static bool ProcOrdSegAdr( void )
/*******************************/
{
    ord_state   ret;
    unsigned_16 value;

    if( !GetToken( SEP_EQUALS, TOK_NORMAL ) ) {
        return( false );
    }
    ret = getatoi( &value );
    if( ret == ST_IS_ORDINAL ) {
        CurrOClass->Base.seg = value;
        CurrOClass->FixedAddr = true;
        return( true );
    } else {
        CurrOClass->FixedAddr = false;
        LnkMsg(LOC+LINE+WRN+MSG_ADDR_INFO_TOO_LARGE, NULL);
        return( false );
    }
}

static bool ProcOrdOfsAdr( void )
/*******************************/
{
    ord_state   retval;
    unsigned_32 value;

    if( !GetToken( SEP_EQUALS, TOK_NORMAL ) ) {
        return( false );
    }
    retval = getatol( &value );
    if( retval == ST_IS_ORDINAL && (value <= 0xFFFFL || HintFormat( ~MK_SEGMENTED )) ) {
        CurrOClass->Base.off = value;
        CurrOClass->FixedAddr = true;
        return( true );
    } else {
        CurrOClass->FixedAddr = false;
        LnkMsg(LOC+LINE+WRN+MSG_VALUE_INCORRECT, "s", "ORDER CLNAME OFFSET");
        return( false );
    }
}

static bool ProcOrdCopy( void )
/*****************************/
{
    if( !GetToken( SEP_NO, TOK_INCLUDE_DOT ) ) {
        return( false );
    }
    CurrOClass->SrcName = tostring();
    CurrOClass->Copy = true;
    return( true );
}

static bool ProcOrdNoEmit( void )
/*******************************/
{
    CurrOClass->NoEmit = true;
    return( true );
}

static bool ProcOrdSegSegAdr( void )
/**********************************/
{
    ord_state   ret;
    unsigned_16 value;

    if( !GetToken( SEP_EQUALS, TOK_NORMAL ) ) {
        return( false );
    }
    ret = getatoi( &value );
    if( ret == ST_IS_ORDINAL ) {
        CurrOSeg->Base.seg = value;
        CurrOSeg->FixedAddr = true;
        return( true );
    } else {
        CurrOSeg->FixedAddr = false;
        LnkMsg(LOC+LINE+WRN+MSG_ADDR_INFO_TOO_LARGE, NULL);
        return( false );
    }
}

static bool ProcOrdSegOfsAdr( void )
/**********************************/
{
    ord_state   retval;
    unsigned_32 value;

    if( !GetToken( SEP_EQUALS, TOK_NORMAL ) ) {
        return( false );
    }
    retval = getatol( &value );
    if( retval == ST_IS_ORDINAL && (value <= 0xFFFFL || HintFormat( ~MK_SEGMENTED )) ) {
        CurrOSeg->Base.off = value;
        CurrOSeg->FixedAddr = true;
        return( true );
    } else {
        CurrOSeg->FixedAddr = false;
        LnkMsg(LOC+LINE+WRN+MSG_VALUE_INCORRECT, "s", "ORDER CLNAME SEGMENT OFFSET");
        return( false );
    }
}

static bool ProcOrdSegNoEmit( void )
/**********************************/
{
    CurrOSeg->NoEmit = true;
    return( true );
}

static bool ProcObjAlign( void )
/*******************************
 * process ObjAlign option
 */
{
    ord_state           ret;
    unsigned_32         value;

    if( !HaveEquals( TOK_NORMAL ) )
        return( false );
    ret = getatol( &value );
    if( ret != ST_IS_ORDINAL || value == 0 ) {
        return( false );
    }                                            /* value not a power of 2 */
    if( value < 16 || value > _256MB || (value & (value - 1)) ) {
        LnkMsg( LOC+LINE+WRN+MSG_VALUE_INCORRECT, "s", "OBJALIGN" );
        value = _64KB;
    }
    FmtData.objalign = value;
    ChkBase( value );
    return( true );
}

static bool ProcDescription( void )
/*********************************/
{
    if( !GetToken( SEP_NO, TOK_INCLUDE_DOT ) ) {
        return( false );
    }
    if( FmtData.description != NULL ) {
        _LnkFree( FmtData.description );
    }
    FmtData.description = tostring();
    return( true );
}

void ChkBase( offset align )
/***************************
 * Note: align must be a power of 2
 */
{
    if( FmtData.objalign != NO_BASE_SPEC && FmtData.objalign > align ) {
        align = FmtData.objalign;
    }
    if( FmtData.base != NO_BASE_SPEC && (FmtData.base & (align - 1)) != 0 ) {
        LnkMsg( LOC+LINE+WRN+MSG_OFFSET_MUST_BE_ALIGNED, "l", align );
        FmtData.base = ROUND_UP( FmtData.base, align );
    }
}

#ifdef _INT_DEBUG
static bool ProcXDbg( void )
/***************************
 * process DEBUG command
 */
{
    char        value[7];

    if( GetToken( SEP_EQUALS, TOK_INCLUDE_DOT ) ) {
        if( Token.len > 6 ) {
            return( false );
        } else {
            memcpy( value, Token.this, Token.len );
            value[Token.len] = '\0';
            Debug = strtoul( value, NULL, 0 );
            DEBUG(( DBG_BASE, "debugging info type = %x", Debug ));
        }
        return( true );
    } else {
        return( false );
    }
}

static bool ProcIntDbg( void )
/****************************/
{
    LinkState |= LS_INTERNAL_DEBUG;
    return( true );
}
#endif

static parse_entry  OrderSegOpts[] = {
    "SEGAddr",      ProcOrdSegSegAdr,   MK_ALL, 0,
    "OFFset",       ProcOrdSegOfsAdr,   MK_ALL, 0,
    "NOEmit",       ProcOrdSegNoEmit,   MK_ALL, 0,
    NULL
};

static bool ProcOrdSeg( void )
/****************************/
{
    if( !GetToken( SEP_NO, TOK_INCLUDE_DOT ) ) {
        return( false );
    }
    _ChkAlloc( CurrOSeg, sizeof( ORDER_SEGMENT ) );
    CurrOSeg->NextSeg = CurrOClass->SegList;
    CurrOClass->SegList = CurrOSeg;
    CurrOSeg->Name = tostring();
    CurrOSeg->FixedAddr = false;
    CurrOSeg->NoEmit = false;
    while( ProcOne( OrderSegOpts, SEP_NO, false ) )
        {};
    return( true );
}

static parse_entry  OrderClassOpts[] = {
    "SEGAddr",      ProcOrdSegAdr,      MK_ALL, 0,
    "OFFset",       ProcOrdOfsAdr,      MK_ALL, 0,
    "COpy",         ProcOrdCopy,        MK_ALL, 0,
    "NOEmit",       ProcOrdNoEmit,      MK_ALL, 0,
    "SEGMent",      ProcOrdSeg,         MK_ALL, 0,
    NULL
};

static bool ProcOrdClass( void )
/******************************/
{
    ORDER_CLASS *LastOClass;

    if( !GetToken( SEP_NO, TOK_INCLUDE_DOT ) ) {
        return( false );
    }
    LinkState |= LS_SPEC_ORDER_FLAG;
    LastOClass = CurrOClass;
    _ChkAlloc( CurrOClass, sizeof( ORDER_CLASS ) );
    if( LastOClass == NULL ) {
        CurrSect->orderlist = CurrOClass;
    } else {
        LastOClass->NextClass = CurrOClass;
    }
    CurrOClass->Name = tostring();
    CurrOClass->NextClass = NULL;
    CurrOClass->Ring = NULL;
    CurrOClass->SegList = NULL;
    CurrOSeg = NULL;
    CurrOClass->FixedAddr = false;
    CurrOClass->Copy = false;
    CurrOClass->NoEmit = false;
    while( ProcOne( OrderClassOpts, SEP_NO, false ) )
        {};
    return( true );
}

static parse_entry  OrderOpts[] = {
    "CLName",       ProcOrdClass,       MK_ALL, 0,
    NULL
};

static bool ProcOrder( void )
/***************************/
{
    bool    ret;

    ret = false;
    CurrOClass = CurrSect->orderlist;
    if( CurrOClass != NULL) {
        LnkMsg(LOC+LINE+WRN+MSG_DUP_DIRECTIVE, "s", "OPTION");
    }
    while( ProcOne( OrderOpts, SEP_NO, false ) ) {
        ret = true;
    }
    return( ret );
}


#ifdef _RAW
static parse_entry  OutputOpts[] = {
    "RAW",          ProcOutputRaw,      MK_ALL, 0,
    "HEX",          ProcOutputHex,      MK_ALL, 0,
    "OFFset",       ProcOutputOffset,   MK_ALL, 0,
    "HSHIFT",       ProcOutputHshift,   MK_ALL, 0,
    "STartrec",     ProcOutputStart,    MK_ALL, 0,
    NULL
};

static bool ProcOutput( void )
/****************************/
{
   bool     ret;

   FmtData.output_offset = 0L;
   FmtData.output_hshift = false;
   FmtData.output_start = false;
   ret = false;
   while( ProcOne( OutputOpts, SEP_NO, false ) ) {
       ret = true;
   }
   return( ret );
}
#endif


#if defined( _PHARLAP ) || defined( _DOS16M ) || defined( _OS2 ) || defined( _ELF )
static parse_entry  RunOptions[] = {
  #ifdef _DOS16M
    "KEYboard",     ProcKeyboard,       MK_DOS16M, 0,
    "OVERload",     ProcOverload,       MK_DOS16M, 0,
    "INIT00",       ProcInit00,         MK_DOS16M, 0,
    "INITFF",       ProcInitFF,         MK_DOS16M, 0,
    "ROTate",       ProcRotate,         MK_DOS16M, 0,
    "AUTO",         ProcAuto,           MK_DOS16M, 0,
    "SELectors",    ProcSelectors,      MK_DOS16M, 0,
    "INT10",        ProcInt10,          MK_DOS16M, 0,
  #endif
  #ifdef _PHARLAP
    "MINReal",      ProcMinReal,        MK_PHAR_FLAT, 0,
    "MAXReal",      ProcMaxReal,        MK_PHAR_FLAT, 0,
    "REALBreak",    ProcRealBreak,      MK_PHAR_FLAT, CF_HAVE_REALBREAK,
    "CALLBufs",     ProcCallBufs,       MK_PHAR_FLAT, 0,
    "MINIBuf",      ProcMiniBuf,        MK_PHAR_FLAT, 0,
    "MAXIBuf",      ProcMaxiBuf,        MK_PHAR_FLAT, 0,
    "NISTack",      ProcNIStack,        MK_PHAR_FLAT, 0,
    "ISTKsize",     ProcIStkSize,       MK_PHAR_FLAT, 0,
    "UNPRIVileged", ProcUnpriv,         MK_PHAR_FLAT, 0,
    "PRIVileged",   ProcPriv,           MK_PHAR_FLAT, 0,
    /* WARNING: do not document the following directive -- for internal use only */
    "FLAGs",        ProcFlags,          MK_PHAR_FLAT, 0,
  #endif
  #ifdef _OS2
    "NATive",       ProcRunNative,      MK_PE, 0,
    "WINdows",      ProcRunWindows,     MK_PE, 0,
    "CONsole",      ProcRunConsole,     MK_PE, 0,
    "POSix",        ProcRunPosix,       MK_PE, 0,
    "OS2",          ProcRunOS2,         MK_PE, 0,
    "DOSstyle",     ProcRunDosstyle,    MK_PE, 0,
  #endif
  #ifdef _ELF
    "ABIver",       ProcELFRNumber,     MK_ELF, 0,
    "SVR4",         ProcELFRSVR4,       MK_ELF, 0,
    "NETbsd",       ProcELFRNetBSD,     MK_ELF, 0,
    "LINux",        ProcELFRLinux,      MK_ELF, 0,
    "FREebsd",      ProcELFRFBSD,       MK_ELF, 0,
    "SOLaris",      ProcELFRSolrs,      MK_ELF, 0,
  #endif
    NULL
};

static bool AddRunTime( void )
/****************************/
{
    Token.thumb = true;         // reparse last token.
    return( ProcOne( RunOptions, SEP_NO, false ) );
}

static bool ProcRuntime( void )
/*****************************/
{
    return( ProcArgList( AddRunTime, TOK_INCLUDE_DOT ) );
}
#endif

static parse_entry  EndLinkOpt[] = {
    "ENDLink",      ProcEndLink,        MK_ALL, 0,
    NULL
};

static bool ProcStartLink( void )
/********************************
 * save up list of commands to process later
 */
{
    GetCommandBlock( &LinkCommands, NULL, EndLinkOpt );
    return( true );
}

static parse_entry  SysEndOptions[] = {
    "End",          ProcSysEnd,         MK_ALL, 0,
    NULL
};

static bool ProcSysBegin( void )
/*******************************
 * parse a system begin block
 * and store it somewhere
 */
{
    char        *sysname;
    sysblock    *sys;

    if( !GetToken( SEP_NO, TOK_INCLUDE_DOT ) ) {
        LnkMsg( LOC+LINE+WRN+MSG_VALUE_INCORRECT, "s", "SYSTEM BEGIN" );
        return( true );
    }
    sysname = tostring();
    sys = FindSysBlock( sysname );
    if( sys != NULL ) {
        LnkMsg( LOC+LINE+WRN+MSG_SYSTEM_ALREADY_DEFINED, "s", sys->name );
        while( !ProcOne( SysEndOptions, SEP_SPACE, false ) ) {
            Token.thumb = false;
            RestoreParser();
        }
    } else {
        GetCommandBlock( &SysBlocks, sysname, SysEndOptions );
    }
    _LnkFree( sysname );
    return( true );
}

static parse_entry  SysBeginOptions[] = {
    "Begin",        ProcSysBegin,       MK_ALL, 0,
    NULL
};

static parse_entry  SysDeleteOptions[] = {
    "DELete",       ProcSysDelete,      MK_ALL, 0,
    NULL
};

static bool ProcSystem( void )
/*****************************
 * process the system directive
 */
{
    char        *sysname;
    sysblock    *sys;
    sysblock    **prev;
    bool        dodelete;

    if( ProcOne( SysBeginOptions, SEP_NO, false ) )
        return( true );
    dodelete = ProcOne( SysDeleteOptions, SEP_NO, false );
    if( dodelete ) {
        if( !GetToken( SEP_NO, TOK_INCLUDE_DOT ) ) {
            return( false );
        }
    } else {
        Token.thumb = false;
    }
    sysname = tostring();
    sys = FindSystemBlock( sysname );
    if( sys == NULL ) {
        LnkMsg( LOC+LINE+WRN+MSG_SYSTEM_UNDEFINED, "s", sysname );
    } else {
        if( dodelete ) {
            for( prev = &SysBlocks; *prev != sys; ) {
                prev = &((*prev)->next);
            }
            *prev = sys->next;
            if( sys->name != NULL ) {
                _LnkFree( sys->name );
            }
            _LnkFree( sys );
        } else {
            RestoreParser();
            NewCommandSource( sys->name, sys->commands, SYSTEM );
        }
    }
    _LnkFree( sysname );
    return( true );
}

static parse_entry  Languages[] = {
    "JApanese",     ProcJapanese,       MK_ALL, 0,
    "CHinese",      ProcChinese,        MK_ALL, 0,
    "KOrean",       ProcKorean,         MK_ALL, 0,
    NULL
};

static bool ProcLanguage( void )
/******************************/
{
    CmdFlags &= ~CF_LANGUAGE_MASK;
    return( ProcOne( Languages, SEP_NO, false ) );
}

static parse_entry  SortOptions[] = {
    "ALPhabetical", ProcAlphabetical,   MK_ALL, 0,
    "GLobal",       ProcGlobal,         MK_ALL, 0,
    NULL
};

static bool ProcSort( void )
/**************************/
{
    MapFlags |= MAP_SORT;
    if( !ProcOne( SortOptions, SEP_NO, false ) )
        return( true );
    ProcOne( SortOptions, SEP_NO, false );
    return( true );
}

static parse_entry  PosDbgMods[] = {
    "LInes",        ProcLine,           MK_ALL, 0,
    "Types",        ProcType,           MK_ALL, 0,
    "LOcals",       ProcLocal,          MK_ALL, 0,
//    "STatic",       ProcDBIStatic,      MK_ALL, 0,
    "All",          ProcAll,            MK_ALL, 0,
#ifdef _NOVELL
    "ONLyexports",  ProcExportsDBI,     MK_NOVELL, 0,
#endif
    NULL
};

static parse_entry  DbgMods[] = {
    "Watcom",       ProcWatcomDBI,      MK_ALL, 0,
    "Dwarf",        ProcDwarfDBI,       MK_ALL, 0,
    "Codeview",     ProcCodeviewDBI,    MK_ALL, 0,
#ifdef _NOVELL
    "Novell",       ProcNovDBI,         MK_NOVELL, 0,
#endif
    NULL
};

static bool ProcDebug( void )
/***************************/
{
    bool        gotmod;

    if( CmdFlags & CF_FILES_BEFORE_DBI ) {
        LnkMsg( LOC+LINE+WRN+MSG_DEBUG_AFTER_FILES, NULL );
    }
    gotmod = ProcOne( DbgMods, SEP_NO, false );
    DBIFlag &= ~DBI_MASK;
    if( ProcOne( PosDbgMods, SEP_NO, false ) ) {
        while( ProcOne( PosDbgMods, SEP_COMMA, false ) ) {
            ; /*null loop*/
        }
    } else {
        DBIFlag |= DBI_ALL; //DBI_MASK;
        if( !gotmod ) {
            return( false );
        }
    }
    return( true );
}

static parse_entry  MainOptions[] = {
    "Map",          ProcMap,            MK_ALL, 0,
    "MAPLines",     ProcMapLines,       MK_ALL, 0,
    "STack",        ProcStack,          MK_ALL, 0,
    "NODefaultlibs",ProcNoDefLibs,      MK_ALL, CF_NO_DEF_LIBS,
    "Quiet",        ProcQuiet,          MK_ALL, 0,
    "Dosseg",       ProcDosSeg,         MK_ALL, 0,
    "Caseexact",    ProcCase,           MK_ALL, 0,
    "Verbose",      ProcVerbose,        MK_ALL, 0,
    "Undefsok",     ProcUndefsOK,       MK_ALL, 0,
    "NOUndefsok",   ProcNoUndefsOK,     MK_ALL, 0,
    "NAMELen",      ProcNameLen,        MK_ALL, 0,
    "SYMFile",      ProcSymFile,        MK_ALL, CF_SEPARATE_SYM,
    "OSName",       ProcOSName,         MK_ALL, 0,
    "ELiminate",    ProcEliminate,      MK_ALL, 0,
    "FARcalls",     ProcFarCalls,       MK_ALL, 0,
    "MAXErrors",    ProcMaxErrors,      MK_ALL, 0,
    "NOCASEexact",  ProcNoCaseExact,    MK_ALL, 0,
    "NOCAChe",      ProcNoCache,        MK_ALL, 0,
    "NOEXTension",  ProcNoExtension,    MK_ALL, 0,
    "NOFARcalls",   ProcNoFarCalls,     MK_ALL, 0,
    "CAChe",        ProcCache,          MK_ALL, 0,
    "MANGlednames", ProcMangledNames,   MK_ALL, 0,
    "OBJAlign",     ProcObjAlign,       MK_ALL, 0,
    "RESource",     ProcOpResource,     MK_ALL, 0,
    "STATics",      ProcStatics,        MK_ALL, 0,
    "START",        ProcStart,          MK_ALL, 0,
    "ARTificial",   ProcArtificial,     MK_ALL, 0,
    "SHOwdead",     ProcShowDead,       MK_ALL, 0,
    "VFRemoval",    ProcVFRemoval,      MK_ALL, 0,
    "REDefsok",     ProcRedefsOK,       MK_ALL, 0,
    "NOREDefsok",   ProcNoRedefs,       MK_ALL, 0,
    "CVPack",       ProcCVPack,         MK_ALL, 0,
    "INCremental",  ProcIncremental,    MK_ALL, 0,
    "FILLchar",     ProcFillchar,       MK_ALL, 0,
#ifdef _EXE
    "SMall",        ProcSmall,          MK_OVERLAYS, 0,
    "DIStribute",   ProcDistribute,     MK_OVERLAYS, 0,
    "DYNamic",      ProcDynamic,        MK_OVERLAYS, 0,
    "STANdard",     ProcStandard,       MK_OVERLAYS, 0,
    "NOIndirect",   ProcNoIndirect,     MK_OVERLAYS, 0,
    "ARea",         ProcArea,           MK_OVERLAYS, 0,
    "PADSections",  ProcPadSections,    MK_OVERLAYS, 0,
    "FULLHeader",   ProcFullHeader,     MK_DOS, 0,
#endif
#if defined( _OS2 ) || defined( _EXE ) || defined( _DOS16M ) || defined( _QNX )
    "PACKCode",     ProcPackcode,       (MK_OS2_16BIT | MK_DOS | MK_QNX | MK_DOS16M), 0,
    "PACKData",     ProcPackdata,       (MK_OS2_16BIT | MK_DOS | MK_QNX | MK_DOS16M), 0,
#endif
#if defined( _OS2 ) || defined( _ELF )
    "Alignment",    ProcAlignment,      (MK_OS2_16BIT | MK_OS2_LX | MK_PE | MK_ELF), 0,
#endif
#if defined( _OS2 ) || defined( _PHARLAP ) || defined( _DOS16M )
    "STUB",         ProcStub,           (MK_OS2 | MK_PE | MK_WIN_VXD | MK_PHAR_LAP | MK_DOS16M), 0,
#endif
#ifdef _OS2
    "NOSTUB",       ProcNoStub,         MK_OS2 | MK_PE | MK_WIN_VXD, 0,
    "ONEautodata",  ProcSingle,         MK_OS2, CF_AUTO_SEG_FLAG,
    "MANYautodata", ProcMultiple,       MK_OS2, CF_AUTO_SEG_FLAG,
    "NOAutodata",   ProcNone,           MK_OS2_16BIT, CF_AUTO_SEG_FLAG,
    "OLDlibrary",   ProcOldLibrary,     MK_OS2 | MK_PE, 0,
    "MODName",      ProcModName,        MK_OS2 | MK_PE | MK_WIN_VXD, 0,
    "NEWFiles",     ProcNewFiles,       MK_ONLY_OS2_16, 0,
    "PROTmode",     ProcProtMode,       MK_OS2_16BIT, 0,
#endif
#if defined( _OS2 ) || defined( _NOVELL )
    "DEscription",  ProcDescription,    MK_OS2 | MK_PE | MK_WIN_VXD | MK_NOVELL, 0,
#endif
#ifdef _OS2
    "NOSTDCall",    ProcNoStdCall,      MK_PE, 0,
    "RWReloccheck", ProcRWRelocCheck,   MK_WINDOWS, 0,
    "SELFrelative", ProcSelfRelative,   MK_OS2_LX, 0,
    "INTernalrelocs",ProcInternalRelocs,MK_OS2_LX, 0,
    "TOGglerelocsflag",ProcToggleRelocsFlag,MK_OS2_LX, 0,
#endif
#if defined( _OS2 ) || defined( _QNX )
    "Heapsize",     ProcHeapSize,       (MK_OS2 | MK_QNX | MK_PE), 0,
#endif
#ifdef _PHARLAP
//    "PACKExp",      ProcPackExp,        MK_PHAR_FLAT, 0,
    "MINData",      ProcMinData,        MK_PHAR_LAP, 0,
    "MAXData",      ProcMaxData,        MK_PHAR_LAP, 0,
#endif
#if defined(_PHARLAP) || defined(_QNX) || defined(_OS2) || defined(_RAW)
    "OFFset",       ProcOffset,         MK_PHAR_FLAT | MK_OS2_FLAT | MK_PE | MK_QNX_FLAT | MK_ELF | MK_RAW, 0,
#endif
#ifdef _NOVELL
    "SCReenname",   ProcScreenName,     MK_NOVELL, 0,
    "CHeck",        ProcCheck,          MK_NOVELL, 0,
    "MULTILoad",    ProcMultiLoad,      MK_NOVELL, 0,
    "AUTOUNload",   ProcAutoUnload,     MK_NOVELL, 0,
    "REentrant",    ProcReentrant,      MK_NOVELL, 0,
    "SYnchronize",  ProcSynch,          MK_NOVELL, 0,
    "CUSTom",       ProcCustom,         MK_NOVELL, 0,
    "EXit",         ProcExit,           MK_NOVELL, 0,
    "THReadname",   ProcThreadName,     MK_NOVELL, 0,
    "PSeudopreemption", ProcPseudoPreemption, MK_NOVELL, 0,
    "COPYRight",    ProcCopyright,      MK_NOVELL, 0,
    "MESsages",     ProcMessages,       MK_NOVELL, 0,
    "HElp",         ProcHelp,           MK_NOVELL, 0,
    "XDCdata",      ProcXDCData,        MK_NOVELL, 0,
    "SHArelib",     ProcSharelib,       MK_NOVELL, 0,
    "OSDomain",     ProcOSDomain,       MK_NOVELL, 0,
    "NLMFlags",     ProcNLMFlags,       MK_NOVELL, 0,
#endif
#if defined( _OS2 ) || defined( _NOVELL )
    "VERSion",      ProcVersion,        MK_NOVELL | MK_OS2_FLAT | MK_PE | MK_WINDOWS, 0,
    "IMPLib",       ProcImplib,         MK_NOVELL | MK_OS2 | MK_PE, 0,
    "IMPFile",      ProcImpFile,        MK_NOVELL | MK_OS2 | MK_PE, 0,
#endif
#ifdef _DOS16M
    "BUFfer",       ProcBuffer,         MK_DOS16M, 0,
    "GDTsize",      ProcGDTSize,        MK_DOS16M, 0,
    "RELocs",       ProcRelocs,         MK_DOS16M, 0,
    "SELstart",     ProcSelStart,       MK_DOS16M, 0,
    "DATASize",     ProcDataSize,       MK_DOS16M, 0,
    "EXTended",     ProcExtended,       MK_DOS16M, 0,
    "EXPName",      ProcExpName,        MK_DOS16M, 0,
#endif
#ifdef _RDOS
    "CODESelector", ProcRdosCodeSel,    MK_RDOS, 0,
    "DATASelector", ProcRdosDataSel,    MK_RDOS, 0,
#endif
#if defined( _DOS16M ) || defined( _QNX ) || defined( _OS2 ) || defined( _ELF )
    "NORelocs",     ProcNoRelocs,       (MK_QNX | MK_DOS16M  | MK_PE | MK_ELF), 0,
#endif
#ifdef _QNX
    "LOnglived",    ProcLongLived,      MK_QNX, 0,
    "PRIVilege",    ProcQNXPrivilege,   MK_QNX, 0,
    "LInearrelocs", ProcLinearRelocs,   MK_QNX, 0,
#endif
#ifdef _ELF
    "EXTRASections",ProcExtraSections,  MK_ELF, 0,
    "EXPORTAll",    ProcExportAll,      MK_ELF, 0,
#endif
#ifdef _OS2
    "LINKVersion",  ProcLinkVersion,    MK_PE,  0,
    "OSVersion",    ProcOsVersion,      MK_PE,  0,
    "CHECKSUM",     ProcChecksum,       MK_PE,  0,
    "LARGEaddressaware",ProcLargeAddressAware, MK_PE, 0,
    "NOLARGEaddressaware",ProcNoLargeAddressAware, MK_PE, 0,
#endif
    "HSHIFT",       ProcHshift,         (MK_DOS | MK_ALLOW_16),  0,
#ifdef _OS2
    "MIXed1632",    ProcMixed1632,      MK_OS2_FLAT, 0,
#endif
    NULL
};

static bool AddOption( void )
/***************************/
{
    Token.thumb = true;
    if( !ProcOne( MainOptions, SEP_NO, false ) )
        return( false );
    return( true );
}

static bool ProcOptions( void )
/*****************************/
{
    return( ProcArgList( AddOption, TOK_INCLUDE_DOT ) );
}

static parse_entry  Models[] = {
#ifdef _EXE
    "Dos",          ProcDos,            MK_DOS, 0,
#endif
#ifdef _OS2
    "OS2",          ProcOS2,            MK_ONLY_OS2, 0,
    "WINdows",      ProcWindows,        MK_WINDOWS | MK_PE | MK_WIN_VXD, 0,
#endif
#ifdef _PHARLAP
    "PHARlap",      ProcPharLap,        MK_PHAR_LAP, 0,
#endif
#ifdef _NOVELL
    "NOVell",       ProcNovell,         MK_NOVELL, 0,
#endif
#ifdef _QNX
    "QNX",          ProcQNX,            MK_QNX, 0,
#endif
#ifdef _DOS16M
    "DOS16M",       Proc16M,            MK_DOS16M, 0,
#endif
#ifdef _ELF
    "ELF",          ProcELF,            MK_ELF, 0,
#endif
#ifdef _ZDOS
    "ZDos",         ProcZdos,           MK_ZDOS, 0,
#endif
#ifdef _RDOS
    "RDos",         ProcRdos,           MK_RDOS, 0,
#endif
#ifdef _RAW
    "Raw",          ProcRaw,            MK_RAW, 0,
#endif
    NULL
};

static bool ProcFormat( void )
/****************************/
{
    if( LinkState & LS_FMT_SPECIFIED ) {
        LnkMsg( LOC+LINE+FTL + MSG_MULTIPLE_MODES_FOUND, NULL );
    }
    LinkState |= LS_FMT_SPECIFIED;
    return( ProcOne( Models, SEP_NO, true ) );
}

/* these directives are the only ones that are harmless to run after the files
 * have been processed in pass 1 */

static parse_entry  SysDirectives[] = {
    "Library",      ProcLibrary,        MK_ALL, 0,
    "Name",         ProcName,           MK_ALL, 0,
    "OPtion",       ProcOptions,        MK_ALL, 0,
    "LIBPath",      ProcLibPath,        MK_ALL, 0,
    "FORMat",       ProcFormat,         MK_ALL, 0,
    "DISAble",      ProcDisable,        MK_ALL, 0,
    "SOrt",         ProcSort,           MK_ALL, 0,
    "ORDer",        ProcOrder,          MK_ALL, 0,
#ifdef _RAW
    "OUTput",       ProcOutput,         MK_ALL, 0,
#endif
#if defined( _PHARLAP ) || defined( _DOS16M ) || defined( _OS2 ) || defined( _ELF )
    "RUntime",      ProcRuntime,        (MK_PHAR_LAP | MK_DOS16M | MK_PE | MK_ELF), 0,
#endif
#if defined( _OS2 ) || defined( _QNX )
    "SEGment",      ProcSegment,        (MK_QNX | MK_OS2 | MK_PE | MK_WIN_VXD ), 0,
#endif
#ifdef _DOS16M
    "MEMory",       ProcMemory16M,      MK_DOS16M, 0,
    "TRansparent",  ProcTransparent,    MK_DOS16M, 0,
#endif
    NULL
};

bool DoParseSysDirective( bool suicide )
/**************************************/
{
    return( ProcOne( SysDirectives, SEP_NO, suicide ) );
}

static parse_entry  Directives[] = {
    "File",         ProcFiles,          MK_ALL, CF_HAVE_FILES,
    "MODFile",      ProcModFiles,       MK_ALL, 0,
    "Library",      ProcLibrary,        MK_ALL, 0,
    "Name",         ProcName,           MK_ALL, 0,
    "OPtion",       ProcOptions,        MK_ALL, 0,
    "Debug",        ProcDebug,          MK_ALL, 0,
    "SYStem",       ProcSystem,         MK_ALL, 0,
    "LIBPath",      ProcLibPath,        MK_ALL, 0,
    "LIBFile",      ProcLibFile,        MK_ALL, CF_HAVE_FILES,
    "Path",         ProcPath,           MK_ALL, 0,
    "FORMat",       ProcFormat,         MK_ALL, 0,
    "MODTrace",     ProcModTrace,       MK_ALL, 0,
    "SYMTrace",     ProcSymTrace,       MK_ALL, CF_AFTER_INC,
    "Alias",        ProcAlias,          MK_ALL, CF_AFTER_INC,
    "REFerence",    ProcReference,      MK_ALL, CF_AFTER_INC,
    "DISAble",      ProcDisable,        MK_ALL, 0,
    "SOrt",         ProcSort,           MK_ALL, 0,
    "LANGuage",     ProcLanguage,       MK_ALL, 0,
    "STARTLink",    ProcStartLink,      MK_ALL, 0,
    "OPTLIB",       ProcOptLib,         MK_ALL, 0,
    "ORDer",        ProcOrder,          MK_ALL, 0,
#ifdef _RAW
    "OUTput",       ProcOutput,         MK_ALL, 0,
#endif
#ifdef _OS2
    "RESource",     ProcResource,       MK_PE, 0,
    "COMmit",       ProcCommit,         MK_PE, 0,
    "ANONymousexport",ProcAnonExport,   MK_OS2, CF_AFTER_INC,
#endif
#if defined( _NOVELL ) || defined( _OS2 ) || defined( _ELF )
    "IMPort",       ProcImport,         (MK_NOVELL | MK_ELF | MK_OS2 | MK_PE), CF_AFTER_INC,
    "EXPort",       ProcExport,         (MK_NOVELL | MK_ELF | MK_OS2 | MK_PE | MK_WIN_VXD), CF_AFTER_INC,
#endif
#if defined( _OS2 ) || defined( _QNX )
    "SEGment",      ProcSegment,        (MK_QNX | MK_OS2 | MK_PE | MK_WIN_VXD ), 0,
#endif
#ifdef _EXE
    "OVerlay",      ProcOverlay,        MK_OVERLAYS, 0,
    "Begin",        ProcBegin,          MK_OVERLAYS, 0,
    "FIXedlib",     ProcFixedLib,       MK_OVERLAYS, 0,
    "NOVector",     ProcNoVector,       MK_OVERLAYS, CF_AFTER_INC,
    "VEctor",       ProcVector,         MK_OVERLAYS, CF_AFTER_INC,
    "FORCEVEctor",  ProcForceVector,    MK_OVERLAYS, CF_AFTER_INC,
#endif
#if defined( _PHARLAP ) || defined( _DOS16M ) || defined( _OS2 ) || defined( _ELF )
    "RUntime",      ProcRuntime,        (MK_PHAR_LAP | MK_DOS16M | MK_PE | MK_ELF), 0,
#endif
#if defined( _NOVELL ) || defined( _ELF )
    "MODUle",       ProcModule,         MK_NOVELL | MK_ELF, 0,
#endif
#ifdef _DOS16M
    "MEMory",       ProcMemory16M,      MK_DOS16M, 0,
    "TRansparent",  ProcTransparent,    MK_DOS16M, 0,
#endif
#if defined( _OS2 ) || defined( _EXE ) || defined ( _QNX )
    "NEWsegment",   ProcNewSegment,     (MK_OS2_16BIT | MK_DOS | MK_QNX), 0,
#endif
#ifdef _INT_DEBUG
    "Xdbg",         ProcXDbg,           MK_ALL, 0,
    "INTDBG",       ProcIntDbg,         MK_ALL, 0,
#endif
    NULL
};

bool DoParseDirective( bool suicide )
/***********************************/
{
    return( ProcOne( Directives, SEP_NO, suicide ) );
}

bool DoMatchDirective( const char *parse, size_t len )
/****************************************************/
{
    return( MatchOne( Directives, SEP_NO, parse, len ) );
}
