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
#include "command.h"
#include "wlnkmsg.h"
#include "strtab.h"
#include "dbgall.h"
#include "cmddos.h"
#include "cmdline.h"
#include "symtrace.h"
#include "objio.h"
#include "loadfile.h"
#include "carve.h"
#include "permdata.h"
#include "pathlist.h"
#include "cmdall.h"

#include "clibext.h"

static void         *LastFile;
static file_list    **LastLibFile;

void ResetCmdAll( void )
/**********************/
{
    LastFile = NULL;
    LastLibFile = NULL;
    UsrLibPath = NULL;
}

bool ProcDosSeg( void )
/****************************/
/* process DOSSEG option */
{
    LinkState |= DOSSEG_FLAG;
    DEBUG(( DBG_OLD, "dosseg" ));
    return( true );
}

bool ProcName( void )
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

bool ProcFormat( void )
/****************************/
{
    if( LinkState & FMT_SPECIFIED ) {
        LnkMsg( LOC+LINE+FTL + MSG_MULTIPLE_MODES_FOUND, NULL );
    }
    LinkState |= FMT_SPECIFIED;
    return( ProcOne( Models, SEP_NO, true ) );
}

static bool AddOption( void )
/***************************/
{
    Token.thumb = REJECT;
    if( !ProcOne( MainOptions, SEP_NO, false ) )
        return( false );
    return( true );
}

bool ProcOptions( void )
/*****************************/
{
    return( ProcArgList( AddOption, TOK_INCLUDE_DOT ) );
}

bool ProcDebug( void )
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

bool ProcDwarfDBI( void )
/******************************/
{
    if( LinkFlags & (ANY_DBI_FLAG & ~DWARF_DBI_FLAG) ) {
        LnkMsg( LOC+LINE+WRN+MSG_MULT_DBI_FORMATS, NULL );
        return( true );
    }
    LinkFlags |= DWARF_DBI_FLAG;
    return( true );
}

bool ProcWatcomDBI( void )
/*******************************/
{
    if( LinkFlags & (ANY_DBI_FLAG & ~OLD_DBI_FLAG) ) {
        LnkMsg( LOC+LINE+WRN+MSG_MULT_DBI_FORMATS, NULL );
        return( true );
    }
    LinkFlags |= OLD_DBI_FLAG;
    return( true );
}

bool ProcCodeviewDBI( void )
/*********************************/
{
    if( LinkFlags & (ANY_DBI_FLAG & ~CV_DBI_FLAG) ) {
        LnkMsg( LOC+LINE+WRN+MSG_MULT_DBI_FORMATS, NULL );
        return( true );
    }
    LinkFlags |= CV_DBI_FLAG;
    return( true );
}

bool ProcLine( void )
/**************************/
{
    if( (LinkFlags & ANY_DBI_FLAG) == 0 ) {
        LinkFlags |= DWARF_DBI_FLAG;
    }
    DBIFlag |= DBI_LINE;
    return( true );
}

#if 0
bool ProcDBIStatic( void )
/*******************************/
{
    if( (LinkFlags & ANY_DBI_FLAG) == 0 ) {
        LinkFlags |= DWARF_DBI_FLAG;
    }
    DBIFlag |= DBI_STATICS;
    return( true );
}
#endif

bool ProcType( void )
/**************************/
{
    if( (LinkFlags & ANY_DBI_FLAG) == 0 ) {
        LinkFlags |= DWARF_DBI_FLAG;
    }
    DBIFlag |= DBI_TYPE;
    return( true );
}

bool ProcLocal( void )
/***************************/
{
    if( (LinkFlags & ANY_DBI_FLAG) == 0 ) {
        LinkFlags |= DWARF_DBI_FLAG;
    }
    DBIFlag |= DBI_LOCAL;
    return( true );
}

bool ProcAll( void )
/*************************/
{
    if( (LinkFlags & ANY_DBI_FLAG) == 0 ) {
        LinkFlags |= DWARF_DBI_FLAG;
    }
    DBIFlag |= DBI_ALL;
    return( true );
}

static bool AddAlias( void )
/**************************/
/* add an individual alias */
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

bool ProcAlias( void )
/***************************/
{
    return( ProcArgList( &AddAlias, TOK_INCLUDE_DOT ) );
}

static bool AddReference( void )
/******************************/
{
    symbol      *sym;

    sym = SymOp( ST_CREATE | ST_REFERENCE, Token.this, Token.len );
    sym->info |= SYM_DCE_REF;           /* make sure it stays around */
    return( true );
}

bool ProcReference( void )
/*******************************/
{
    return( ProcArgList( &AddReference, TOK_INCLUDE_DOT ) );
}

bool ProcOSName( void )
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

bool ProcEliminate( void )
/*******************************/
/* turn on dead code elimination */
{
    LinkFlags |= STRIP_CODE;
    return( true );
}

bool ProcMaxErrors( void )
/*******************************/
/* set a maximum number of errors for the linker to generate */
{
    if( !GetLong( &MaxErrors ) )
        return( false );
    LinkFlags |= MAX_ERRORS_FLAG;
    return( true );
}

bool ProcSymFile( void )
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

static file_list *AllocNewFile( member_list *member )
/****************************************************/
{
    file_list       *new_entry;

    _PermAlloc( new_entry, sizeof(file_list) );
    new_entry->next_file = NULL;
    new_entry->status = DBIFlag;
    new_entry->strtab = NULL;
    new_entry->u.member = member;
    if( member != NULL ) {
        new_entry->status |= STAT_HAS_MEMBER;
    }
    return( new_entry );
}

static void *AddObjFile( const char *name, char *member, file_list **filelist )
/*****************************************************************************/
{
    file_list       *new_entry;
    member_list     *new_member;

    new_member = NULL;
    if( member != NULL ) {
        _ChkAlloc( new_member, offsetof(member_list,name) + strlen( member ) + 1 );
        new_member->flags = DBIFlag;
        strcpy( new_member->name, member );
        new_member->next = NULL;
        _LnkFree( member );
        for( new_entry = CurrSect->files; new_entry != NULL; new_entry = new_entry->next_file ) {
            if( FNAMECMPSTR( new_entry->file->name, name ) == 0 ) {
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
        new_entry->file = AllocUniqueFileEntry( name, UsrLibPath );
        new_entry->file->flags |= INSTAT_LIBRARY;
    } else {
        new_entry->file = AllocFileEntry( name, ObjPath );
    }
    *filelist = new_entry;
    return( new_entry );
}

file_list *AddObjLib( const char *name, lib_priority priority )
/***************************************************************/

 {
    file_list   **owner;
    file_list   **new_owner;
    file_list   *lib;

    DEBUG(( DBG_OLD, "Adding Object library name %s", name ));
    /* search for new library position in linked list */
    for( owner = &ObjLibFiles; (lib = *owner) != NULL; owner = &lib->next_file ) {
        if( lib->priority < priority )
            break;
        /* end search if library already exists with same or a higher priority */
        if( FNAMECMPSTR( lib->file->name, name ) == 0 ) {
            return( lib );
        }
    }
    new_owner = owner;
    /* search for library definition with a lower priority */
    for( ; (lib = *owner) != NULL; owner = &lib->next_file ) {
        if( FNAMECMPSTR( lib->file->name, name ) == 0 ) {
            /* remove library entry from linked list */
            *owner = lib->next_file;
            break;
        }
    }
    /* if we need to add one */
    if( lib == NULL ) {
        lib = AllocNewFile( NULL );
        lib->file = AllocUniqueFileEntry( name, UsrLibPath );
        lib->file->flags |= INSTAT_LIBRARY | INSTAT_OPEN_WARNING;
        LinkState |= LIBRARIES_ADDED;
    }
    /* put it to new position and setup priority */
    lib->next_file = *new_owner;
    *new_owner = lib;
    lib->priority = priority;

    return( lib );
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
    entry->file = AllocFileEntry( ptr, UsrLibPath );
    entry->next_file = *LastLibFile;
    *LastLibFile = entry;
    LastLibFile = &entry->next_file;
    if( *LastLibFile == NULL ) {        // no file directives found yet
        CurrFList = LastLibFile;
    }
    entry->file->flags |= INSTAT_USE_LIBPATH;
    _LnkFree( ptr );
    return( true );
}

bool ProcLibFile( void )
/*****************************/
/* process FILE command */
{
    if( (LinkFlags & (DWARF_DBI_FLAG |OLD_DBI_FLAG | NOVELL_DBI_FLAG)) == 0 ) {
        CmdFlags |= CF_FILES_BEFORE_DBI;
    }
    if( LastLibFile == NULL ) {
        LastLibFile = &Root->files;
    }
    return( ProcArgList( &AddLibFile, TOK_INCLUDE_DOT | TOK_IS_FILENAME ) );
}

static bool AddModFile( void )
/*****************************/
{
    char        *ptr;
    char        *membname;

    ptr = GetFileName( &membname, false );
    AddHTableElem(Root->modFilesHashed, ptr);
    LinkFlags |= GOT_CHGD_FILES;
    return( true );
}

static bool AddFile( void )
/*************************/
{
    char        *ptr;
    char        *membname;
    file_list   **temp;

    CmdFlags &= ~CF_MEMBER_ADDED;
    if( CmdFlags & CF_AUTOSECTION ) {
        if( CmdFlags & CF_SECTION_THERE ) {     // is section there already?
            CmdFlags &= ~CF_SECTION_THERE;
        } else {
            MakeNewSection();
        }
    }
    ptr = GetFileName( &membname, true );
    temp = CurrFList;
    if( *CurrFList != NULL ) {
        CurrFList = &(*CurrFList)->next_file;
    }
    LastFile = AddObjFile( ptr, membname, CurrFList );
    if( CmdFlags & CF_MEMBER_ADDED ) {
        CurrFList = temp;   // go back to previous entry.
    } else if( membname != NULL ) {     // 1st member added
        LastFile = ((file_list *)LastFile)->u.member;
        CmdFlags |= CF_MEMBER_ADDED;
    }
    _LnkFree( ptr );
    return( true );
}

bool ProcFiles( void )
/***************************/
/* process FILE command */
{
    if( (LinkFlags & (DWARF_DBI_FLAG|OLD_DBI_FLAG | NOVELL_DBI_FLAG)) == 0 ) {
        CmdFlags |= CF_FILES_BEFORE_DBI;
    }
    return( ProcArgList( &AddFile, TOK_INCLUDE_DOT | TOK_IS_FILENAME ) );
}

bool ProcModFiles( void )
/***************************/
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
    result->status |= STAT_USER_SPECD;
    if( CmdFlags & CF_SET_SECTION ) {
        result->status |= STAT_LIB_FIXED;
        if( OvlLevel == 0 ) {
            result->ovlref = 0;
        } else {
            result->ovlref = OvlNum - 1;
        }
    }
    if( CmdFlags & CF_DOING_OPTLIB ) {
        result->file->flags |= INSTAT_NO_WARNING;
    }
    DEBUG(( DBG_BASE, "library: %s", ptr ));
    _LnkFree( ptr );
    return( true );
}

bool ProcLibrary( void )
/*****************************/
/* process LIB command */
{
    if( (LinkFlags & (DWARF_DBI_FLAG|OLD_DBI_FLAG | NOVELL_DBI_FLAG)) == 0
        && !IsSystemBlock() ) {
        CmdFlags |= CF_FILES_BEFORE_DBI;
    }
    return( ProcArgList( &AddLib, TOK_INCLUDE_DOT | TOK_IS_FILENAME ) );
}

bool ProcOptLib( void )
/****************************/
{
    bool    retval;

    CmdFlags |= CF_DOING_OPTLIB;
    retval = ProcLibrary();
    CmdFlags &= ~CF_DOING_OPTLIB;
    return( retval );
}

bool ProcLibPath( void )
/*****************************/
/* process libpath command */
{
    if( !GetToken( SEP_NO, TOK_INCLUDE_DOT | TOK_IS_FILENAME ) ) {
        LnkMsg( LOC+LINE+WRN+MSG_VALUE_INCORRECT, "s", "libpath" );
        return( true );
    }
    AddLibPaths( Token.this, Token.len, true );  // true == add to front.
    return( true );
}

bool ProcPath( void )
/**************************/
/* process PATH option */
{
    path_entry      *new_path;
    char            *p;
    char            *end;

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
    }
    return( false );
}

bool ProcMap( void )
/*************************/
/* process MAP option */
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

bool ProcMapLines( void )
/*************************/
/* process MAPLINES option */
{
    MapFlags |= MAP_LINES;
    return( true );
}

bool ProcStack( void )
/***************************/
/* process STACK option */
{
    unsigned_32     value;
    bool            ret;

    LinkFlags |= STK_SIZE_FLAG;
    ret = GetLong( &value );
    if( ret ) {
        StackSize = value;
    }
    return( ret );
}

bool ProcNameLen( void )
/*****************************/
/* process NAMELEN option */
{
    unsigned_32     value;
    bool            ret;

    ret = GetLong( &value );
    if( ret ) {
        if( value == 0 ) {
            LnkMsg( LOC+LINE+WRN+MSG_VALUE_INCORRECT, "s", "namelen" );
        } else {
            NameLen = value;
        }
    }
    return( ret );
}

bool ProcCase( void )
/**************************/
/* process CASE option */
{
    LinkFlags |= CASE_FLAG;
    SetSymCase();
    DEBUG(( DBG_OLD, "case" ));
    return( true );
}

bool ProcNoCaseExact( void )
/*********************************/
/* process nocaseexact option */
{
    LinkFlags &= ~CASE_FLAG;
    SetSymCase();
    DEBUG(( DBG_OLD, "nocase" ));
    return( true );
}

bool ProcNoExtension( void )
/*********************************/
{
    CmdFlags |= CF_NO_EXTENSION;
    return( true );
}

bool ProcNoCache( void )
/*****************************/
{
    LinkFlags &= ~CACHE_FLAG;
    LinkFlags |= NOCACHE_FLAG;
    return( true );
}

bool ProcCache( void )
/***************************/
{
    LinkFlags &= ~NOCACHE_FLAG;
    LinkFlags |= CACHE_FLAG;
    return( true );
}

static bool AddDisable( void )
/****************************/
/* disable an error message number */
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

bool ProcDisable( void )
/*****************************/
{
    return( ProcArgList( &AddDisable, TOK_NORMAL ) );
}

bool ProcNoDefLibs( void )
/*******************************/
/* process CASE option */
{
    return( true );
}

bool ProcVerbose( void )
/*****************************/
{
    MapFlags |= MAP_VERBOSE;
    DEBUG(( DBG_OLD, "verbose map file" ));
    return( true );
}

bool ProcUndefsOK( void )
/******************************/
{
    LinkFlags |= UNDEFS_ARE_OK;
    DEBUG(( DBG_OLD, "undefined symbols are OK" ));
    return( true );
}

bool ProcNoUndefsOK( void )
/********************************/
{
    LinkFlags &= ~UNDEFS_ARE_OK;
    return( true );
}

bool ProcRedefsOK( void )
/******************************/
{
    LinkFlags |= REDEFS_OK;
    return( true );
}

bool ProcNoRedefs( void )
/******************************/
{
    LinkFlags &= ~REDEFS_OK;
    return( true );
}

bool ProcCVPack( void )
/****************************/
{
    LinkFlags |= CVPACK_FLAG;
    return( true );
}

#define DEFAULT_INC_NAME "__wlink.ilk"

bool ProcIncremental( void )
/*********************************/
{
#if !defined( __DOS__ )
    if( CmdFlags & CF_AFTER_INC ) {
        LnkMsg( LOC+LINE+ERR+MSG_INC_NEAR_START, NULL );
    }
    LinkFlags |= INC_LINK_FLAG;
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

bool ProcQuiet( void )
/***************************/
{
    LinkFlags |= QUIET_FLAG;
    return( true );
}

bool ProcMangledNames( void )
/**********************************/
{
    LinkFlags |= DONT_UNMANGLE;
    return( true );
}

bool ProcOpResource( void )
/********************************/
{
    if( GetToken( SEP_EQUALS, TOK_INCLUDE_DOT | TOK_IS_FILENAME ) ) {
        FmtData.res_name_only = 1;
        FmtData.resource = tostring();
    } else if( GetToken( SEP_NO, TOK_INCLUDE_DOT ) ) {
        FmtData.resource = tostring();
    } else {
        return( false );
    }
    return( true );
}

bool ProcStatics( void )
/*****************************/
{
    MapFlags |= MAP_STATICS;
    return( true );
}

bool ProcArtificial( void )
/********************************/
{
    MapFlags |= MAP_ARTIFICIAL;
    return( true );
}

bool ProcAlphabetical( void )
/**********************************/
{
    MapFlags |= MAP_ALPHA;
    return( true );
}

bool ProcGlobal( void )
/****************************/
{
    MapFlags |= MAP_GLOBAL;
    return( true );
}

bool ProcSort( void )
/**************************/
{
    MapFlags |= MAP_SORT;
    if( !ProcOne( SortOptions, SEP_NO, false ) )
        return( true );
    ProcOne( SortOptions, SEP_NO, false );
    return( true );
}

bool ProcLanguage( void )
/******************************/
{
    CmdFlags &= ~CF_LANGUAGE_MASK;
    return( ProcOne( Languages, SEP_NO, false ) );
}

bool ProcJapanese( void )
/******************************/
{
    CmdFlags |= CF_LANGUAGE_JAPANESE;
    return( true );
}

bool ProcChinese( void )
/*****************************/
{
    CmdFlags |= CF_LANGUAGE_CHINESE;
    return( true );
}

bool ProcKorean( void )
/****************************/
{
    CmdFlags |= CF_LANGUAGE_KOREAN;
    return( true );
}

bool ProcShowDead( void )
/******************************/
{
    LinkFlags |= SHOW_DEAD;
    return( true );
}

bool ProcVFRemoval( void )
/*******************************/
{
    LinkFlags |= VF_REMOVAL;
    return( true );
}

bool ProcStart( void )
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

static bool GetPackValue( unsigned_32 *value, char *name )
/********************************************************/
{
    ord_state   ret;

    if( !HaveEquals( TOK_NORMAL ) )
        return( false );
    ret = getatol( value );
    if( ret != ST_IS_ORDINAL ) {
        LnkMsg( LOC+LINE+WRN+MSG_VALUE_INCORRECT, "s", name );
        return( true );
    }
    if( *value == 0 )
        *value = 1;
    return( true );
}

bool ProcPackcode( void )
/******************************/
{
    unsigned_32     value;

    if( GetPackValue( &value, "packcode" ) ) {
        PackCodeLimit = value;
        LinkFlags |= PACKCODE_FLAG;
        return( true );
    }
    return( false );
}

bool ProcPackdata( void )
/******************************/
{
    unsigned_32     value;

    if( GetPackValue( &value, "packdata" ) ) {
        PackDataLimit = value;
        LinkFlags |= PACKDATA_FLAG;
        return( true );
    }
    return( false );
}

bool ProcNewSegment( void )
/********************************/
// force the start of a new auto-group after the previous object file.
{
    if( LastFile == NULL ) {
        LnkMsg( LOC+LINE+WRN+MSG_NEWSEG_BEFORE_OBJ, NULL );
    } else {
        if( CmdFlags & CF_MEMBER_ADDED ) {
            ((member_list *)LastFile)->flags |= MOD_LAST_SEG;
        } else {
            ((file_list *)LastFile)->status |= STAT_LAST_SEG;
        }
    }
    return( true );
}

sysblock *FindSysBlock( char *name )
/******************************************/
{
    sysblock    *sys;

    for( sys = SysBlocks; sys != NULL; sys = sys->next ) {
        if( stricmp( sys->name, name ) == 0 ) {
            return( sys );
        }
    }
    return( NULL );
}

static sysblock *FindSystemBlock( char *name )
/*********************************************/
{
    sysblock    *sys;
    sysblock    *tmpblk;
    size_t      len;

    tmpblk = FindSysBlock( name );
    if( tmpblk == NULL ) {
        len = strlen( name );
        for( sys = SysBlocks; sys != NULL; sys = sys->next ) {
            if( strnicmp( sys->name, name, len ) == 0 ) {
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

bool ProcSysDelete( void )
/*******************************/
{
    return( true );
}

bool ProcSystem( void )
/****************************/
/* process the system directive */
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
        Token.thumb = OK;
    }
    sysname = tostring();
    sys = FindSystemBlock( sysname );
    if( sys == NULL ) {
        LnkMsg( LOC+LINE+WRN+MSG_SYSTEM_UNDEFINED, "s", sysname );
        _LnkFree( sysname );
        return( true );
    }
    if( dodelete ) {
        for( prev = &SysBlocks; *prev != sys; ) {
            prev = &((*prev)->next);
        }
        *prev = sys->next;
        _LnkFree( sys->name );
        _LnkFree( sysname );
        _LnkFree( sys );
    } else {
        RestoreParser();
        NewCommandSource( sysname, sys->commands, SYSTEM );
    }
    return( true );
}

static void CopyBlocks( void *copyp, const char *data, size_t size )
/******************************************************************/
{
    char    **copyptr = copyp;

    memcpy( *copyptr, data, size );
    *copyptr += size;
}

static void GetCommandBlock( sysblock **hdr, char *name, parse_entry *endtab )
/****************************************************************************/
{
    char        *copyptr;
    sysblock    *sys;
    stringtable strtab;

    InitStringTable( &strtab, false );
    AddBufferStringTable( &strtab, &strtab, offsetof( sysblock, commands ) );
    while( !ProcOne( endtab, SEP_SPACE, false ) ) {
        Token.thumb = OK;
        AddBufferStringTable( &strtab, Token.this, Token.len );
        AddCharStringTable( &strtab, ' ' );
        RestoreParser();
    }
    AddCharStringTable( &strtab, '\0' );
    _ChkAlloc( copyptr, GetStringTableSize( &strtab ) );
    sys = (sysblock *) copyptr;
    WriteStringTable( &strtab, CopyBlocks, &copyptr );
    FiniStringTable( &strtab );
    sys->name = name;
    LinkList( hdr, sys );
}

bool ProcSysBegin( void )
/******************************/
/* parse a system begin block and store it somewhere */
{
    char        *sysname;
    sysblock    *sys;

    if( !GetToken( SEP_NO, TOK_INCLUDE_DOT ) ) {
        LnkMsg( LOC+LINE+WRN+MSG_VALUE_INCORRECT, "s", "system begin" );
        return( true );
    }
    sysname = tostring();
    sys = FindSysBlock( sysname );
    if( sys != NULL ) {
        LnkMsg( LOC+LINE+WRN+MSG_SYSTEM_ALREADY_DEFINED, "s", sysname );
        while( !ProcOne( SysEndOptions, SEP_SPACE, false ) ) {
            Token.thumb = OK;
            RestoreParser();
        }
    } else {
        GetCommandBlock( &SysBlocks, sysname, SysEndOptions );
    }
    return( true );
}

bool ProcSysEnd( void )
/****************************/
/* finished parsing a system block */
{
    return( true );
}

bool ProcStartLink( void )
/*******************************/
/* save up list of commands to process later */
{
    GetCommandBlock( &LinkCommands, NULL, EndLinkOpt );
    return( true );
}

bool ProcEndLink( void )
/*****************************/
/* finished parsing a link section */
{
    return( true );
}

bool ProcStub( void )
/**************************/
{
    char        *name;
    char        **nameptr;

    if( HintFormat( MK_OS2 | MK_PE | MK_WIN_VXD ) ) {
        nameptr = &FmtData.u.os2.stub_file_name;
    } else if( HintFormat( MK_DOS16M ) ) {
        nameptr = &FmtData.u.d16m.stub;
    } else {    /* it must be pharlap */
        nameptr = &FmtData.u.phar.stub;
        Extension = E_LOAD;     /* want .exe instead of .exp now */
    }
    if( !HaveEquals( TOK_INCLUDE_DOT | TOK_IS_FILENAME ) )
        return( false );
    name = FileName( Token.this, Token.len, E_LOAD, false );
    if( *nameptr == NULL ) {
        *nameptr = name;
    } else if( IsSystemBlock() ) {
        /* if we're in a system block, we don't want to override a user
            specification */
        _LnkFree( name );
    } else {
        _LnkFree( *nameptr );
        *nameptr = name;
    }
    return( true );
}

bool ProcNoStub( void )
/*********************/
{
    FmtData.u.os2.no_stub = true;
    return( true );
}

bool ProcVersion( void )
/*****************************/
{
    ord_state   retval;
    unsigned_32 value;

    if( !GetToken( SEP_EQUALS, TOK_NORMAL ) ) {
        return( false );
    }
    FmtData.minor = 0;
    FmtData.revision = 0;
    retval = getatol( &value );
    if( retval != ST_IS_ORDINAL ) {
        LnkMsg( LOC+LINE+WRN+MSG_VALUE_INCORRECT, "s", "version" );
        return( true );
    }
    FmtData.major = value;
    FmtData.ver_specified = true;
    if( !GetToken( SEP_PERIOD, TOK_NORMAL ) ) {  /*if we don't get a minor number*/
       return( true );                      /* that's OK */
    }
    retval = getatol( &value );
    if( retval != ST_IS_ORDINAL || value >= 100 ) {
        LnkMsg( LOC+LINE+WRN+MSG_VALUE_INCORRECT, "s", "version" );
        return( true );
    } else {
        FmtData.minor = value;
    }
    if( !GetToken( SEP_PERIOD, TOK_NORMAL ) ) {  /* if we don't get a revision*/
        return( true );                 /* that's all right */
    }
    retval = getatol( &value );
    if( retval == ST_NOT_ORDINAL && Token.len == 1 ) {
        FmtData.revision = tolower( *Token.this ) - 'a' + 1;
    } else if( retval == ST_IS_ORDINAL && value < 100 ) {
        FmtData.revision = value;
    } else {
        LnkMsg( LOC+LINE+WRN+MSG_VALUE_INCORRECT, "s", "version" );
        return( true );
    }
    return( true );
}

bool ProcImplib( void )
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

bool ProcImpFile( void )
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

static bool AddRunTime( void )
/****************************/
{
    Token.thumb = REJECT;       // reparse last token.
    return( ProcOne( RunOptions, SEP_NO, false ) );
}

bool ProcRuntime( void )
/*****************************/
{
    return( ProcArgList( AddRunTime, TOK_INCLUDE_DOT ) );
}

static bool AddSymTrace( void )
/*****************************/
{
    symbol      *sym;

    sym = SymOp( ST_CREATE | ST_REFERENCE, Token.this, Token.len );
    sym->info |= SYM_TRACE;
    return( true );
}

bool ProcSymTrace( void )
/******************************/
{
    LinkFlags |= TRACE_FLAG;
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

bool ProcModTrace( void )
/******************************/
{
    LinkFlags |= TRACE_FLAG;
    return( ProcArgList( &AddModTrace, TOK_INCLUDE_DOT | TOK_IS_FILENAME ) );
}

bool ProcFarCalls( void )
/*********************************/
{
    LinkFlags |= FAR_CALLS_FLAG;
    DEBUG(( DBG_OLD, "Far Calls optimization" ));
    return( true );
}

bool ProcNoFarCalls( void )
/***********************************/
{
    LinkFlags &= ~FAR_CALLS_FLAG ;
    DEBUG(( DBG_OLD, "No Far Calls optimization" ));
    return( true );
}

bool ProcOutput( void )
/******************************/
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

bool ProcOutputRaw( void )
/*********************************/
{
   FmtData.output_raw = true;
   return( true );
}

bool ProcOutputHex( void )
/*********************************/
{
   FmtData.output_hex = true;
   return( true );
}

bool ProcOutputStart( void )
/*********************************/
{
   FmtData.output_start = true;
   return( true );
}

bool ProcOutputOfs( void )
/*********************************/
{
    ord_state   retval;
    unsigned_32 value;

    if( !GetToken( SEP_EQUALS, TOK_NORMAL ) ) {
        return( false );
    }
    retval = getatol( &value );
    if( retval == ST_IS_ORDINAL && (value <= 0xFFFFL << FmtData.SegShift || HintFormat( ~(MK_DOS | MK_SEGMENTED)))) {
        FmtData.output_offset = value;
        return( true );
    } else {
        LnkMsg(LOC+LINE+WRN+MSG_VALUE_INCORRECT, "s", "output segment offset");
        return( false );
    }
}

bool ProcOutputHshift( void )
/************************************/
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
        LnkMsg(LOC+LINE+WRN+MSG_VALUE_TOO_LARGE, "s", "Hex HSHIFT");
        return( false );
    }
}

bool ProcHshift( void )
/******************************/
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

bool ProcFillchar( void )
/********************************/
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
        LnkMsg(LOC+LINE+WRN+MSG_VALUE_TOO_LARGE, "s", "FillChar");
        return( false );
    }
}

static ORDER_CLASS      *CurrOClass;
static ORDER_SEGMENT    *CurrOSeg;

bool ProcOrder( void )
/******************************/
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

bool ProcOrdClass( void )
/**********************************/
{
    ORDER_CLASS *LastOClass;

    if( !GetToken( SEP_NO, TOK_INCLUDE_DOT ) ) {
        return( false );
    }
    LinkState |= SPEC_ORDER_FLAG;
    LastOClass = CurrOClass;
    _ChkAlloc( CurrOClass, sizeof(ORDER_CLASS));
    if(LastOClass == NULL) {
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

bool ProcOrdSegAdr( void )
/***********************************/
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

bool ProcOrdOfsAdr( void )
/***********************************/
{
    ord_state   retval;
    unsigned_32 value;

    if( !GetToken( SEP_EQUALS, TOK_NORMAL ) ) {
        return( false );
    }
    retval = getatol( &value );
    if( retval == ST_IS_ORDINAL && (value <= 0xFFFFL || HintFormat( ~(MK_DOS | MK_SEGMENTED)))) {
        CurrOClass->Base.off = value;
        CurrOClass->FixedAddr = true;
        return( true );
    } else {
        CurrOClass->FixedAddr = false;
        LnkMsg(LOC+LINE+WRN+MSG_VALUE_INCORRECT, "s", "output segment offset");
        return( false );
    }
}

bool ProcOrdCopy( void )
/*********************************/
{
    if( !GetToken( SEP_NO, TOK_INCLUDE_DOT ) ) {
        return( false );
    }
    CurrOClass->SrcName = tostring();
    CurrOClass->Copy = true;
    return( true );
}

bool ProcOrdNoEmit( void )
/*********************************/
{
    CurrOClass->NoEmit = true;
    return( true );
}

bool ProcOrdSeg( void )
/**********************************/
{
    if( !GetToken( SEP_NO, TOK_INCLUDE_DOT ) ) {
        return( false );
    }
    _ChkAlloc( CurrOSeg, sizeof(ORDER_SEGMENT));
    CurrOSeg->NextSeg = CurrOClass->SegList;
    CurrOClass->SegList = CurrOSeg;
    CurrOSeg->Name = tostring();
    CurrOSeg->FixedAddr = false;
    CurrOSeg->NoEmit = false;
    while( ProcOne( OrderSegOpts, SEP_NO, false ) )
        {};
    return( true );
}

bool ProcOrdSegSegAdr( void )
/***********************************/
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

bool ProcOrdSegOfsAdr( void )
/***********************************/
{
    ord_state   retval;
    unsigned_32 value;

    if( !GetToken( SEP_EQUALS, TOK_NORMAL ) ) {
        return( false );
    }
    retval = getatol( &value );
    if( retval == ST_IS_ORDINAL && (value <= 0xFFFFL || HintFormat( ~(MK_DOS | MK_SEGMENTED)))) {
        CurrOSeg->Base.off = value;
        CurrOSeg->FixedAddr = true;
        return( true );
    } else {
        CurrOSeg->FixedAddr = false;
        LnkMsg(LOC+LINE+WRN+MSG_VALUE_INCORRECT, "s", "output segment offset");
        return( false );
    }
}

bool ProcOrdSegNoEmit( void )
/*********************************/
{
   CurrOSeg->NoEmit = true;
   return( true );
}

