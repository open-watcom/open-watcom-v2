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
 *  CMDALL : commands common to all executable formats
 *
*/

#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
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
#include "cmdall.h"

static void *       LastFile;
static file_list ** LastLibFile;

void ResetCmdAll( void )
/**********************/
{
    LastFile = NULL;
    LastLibFile = NULL;
    LibPath = NULL;
}

extern bool ProcDosSeg( void )
/****************************/
/* process DOSSEG option */
{
    LinkState |= DOSSEG_FLAG;
    DEBUG(( DBG_OLD, "dosseg" ));
    return( TRUE );
}

extern bool ProcName( void )
/**************************/
{
    if( !GetToken( SEP_NO, TOK_INCLUDE_DOT | TOK_IS_FILENAME ) ) return( FALSE );
    CmdFlags &= ~CF_UNNAMED;
    if( Name != NULL ) {
        _LnkFree( Name );
    }
    Name = tostring();   // just keep the name around for now.
    return( TRUE );
}

extern bool ProcFormat( void )
/****************************/
{
    if( LinkState & FMT_SPECIFIED ) {
        LnkMsg( LOC+LINE+FTL + MSG_MULTIPLE_MODES_FOUND, NULL );
    }
    LinkState |= FMT_SPECIFIED;
    return( ProcOne( Models, SEP_NO, TRUE ) );
}

static bool AddOption( void )
/***************************/
{
    Token.thumb = REJECT;
    if( ProcOne( MainOptions, SEP_NO, FALSE ) == FALSE ) return( FALSE );
    return TRUE;
}

extern bool ProcOptions( void )
/*****************************/
{
    return ProcArgList( AddOption, TOK_INCLUDE_DOT );
}

extern bool ProcDebug( void )
/***************************/
{
    bool        gotmod;

    if( CmdFlags & CF_FILES_BEFORE_DBI ) {
        LnkMsg( LOC+LINE+WRN+MSG_DEBUG_AFTER_FILES, NULL );
    }
    gotmod = ProcOne( DbgMods, SEP_NO, FALSE );
    DBIFlag &= ~DBI_MASK;
    if( ProcOne( PosDbgMods, SEP_NO, FALSE ) ) {
        while( ProcOne( PosDbgMods, SEP_COMMA, FALSE ) != FALSE ); /*null loop*/
    } else {
        DBIFlag |= DBI_ALL; //DBI_MASK;
        if( !gotmod ) {
            return FALSE;
        }
    }
    return TRUE;
}

extern bool ProcDwarfDBI( void )
/******************************/
{
    if( LinkFlags & (ANY_DBI_FLAG & ~DWARF_DBI_FLAG) ) {
        LnkMsg( LOC+LINE+WRN+MSG_MULT_DBI_FORMATS, NULL );
        return TRUE;
    }
    LinkFlags |= DWARF_DBI_FLAG;
    return( TRUE );
}

extern bool ProcWatcomDBI( void )
/*******************************/
{
    if( LinkFlags & (ANY_DBI_FLAG & ~OLD_DBI_FLAG) ) {
        LnkMsg( LOC+LINE+WRN+MSG_MULT_DBI_FORMATS, NULL );
        return TRUE;
    }
    LinkFlags |= OLD_DBI_FLAG;
    return( TRUE );
}

extern bool ProcCodeviewDBI( void )
/*********************************/
{
    if( LinkFlags & (ANY_DBI_FLAG & ~CV_DBI_FLAG) ) {
        LnkMsg( LOC+LINE+WRN+MSG_MULT_DBI_FORMATS, NULL );
        return TRUE;
    }
    LinkFlags |= CV_DBI_FLAG;
    return TRUE;
}

extern bool ProcLine( void )
/**************************/
{
    if( !(LinkFlags & ANY_DBI_FLAG) ) {
        LinkFlags |= DWARF_DBI_FLAG;
    }
    DBIFlag |= DBI_LINE;
    return( TRUE );
}

#if 0
extern bool ProcDBIStatic( void )
/*******************************/
{
    if( !(LinkFlags & ANY_DBI_FLAG) ) {
        LinkFlags |= DWARF_DBI_FLAG;
    }
    DBIFlag |= DBI_STATICS;
    return( TRUE );
}
#endif

extern bool ProcType( void )
/**************************/
{
    if( !(LinkFlags & ANY_DBI_FLAG) ) {
        LinkFlags |= DWARF_DBI_FLAG;
    }
    DBIFlag |= DBI_TYPE;
    return( TRUE );
}

extern bool ProcLocal( void )
/***************************/
{
    if( !(LinkFlags & ANY_DBI_FLAG) ) {
        LinkFlags |= DWARF_DBI_FLAG;
    }
    DBIFlag |= DBI_LOCAL;
    return( TRUE );
}

extern bool ProcAll( void )
/*************************/
{
    if( !(LinkFlags & ANY_DBI_FLAG) ) {
        LinkFlags |= DWARF_DBI_FLAG;
    }
    DBIFlag |= DBI_ALL;
    return( TRUE );
}

static bool AddAlias( void )
/**************************/
/* add an individual alias */
{
    char *      name;
    int         namelen;

    namelen = Token.len;
    name = alloca( namelen );
    memcpy( name, Token.this, namelen );
    if( !GetToken( SEP_EQUALS, TOK_INCLUDE_DOT ) ) {
        return( FALSE );
    }
    MakeSymAlias( name, namelen, tostring(), Token.len );
    return( TRUE );
}

extern bool ProcAlias( void )
/***************************/
{
    return( ProcArgList( &AddAlias, TOK_INCLUDE_DOT ) );
}

static bool AddReference( void )
/******************************/
{
    symbol *    sym;

    sym = SymXOp( ST_REFERENCE | ST_CREATE, Token.this, Token.len );
    sym->info |= SYM_DCE_REF;           /* make sure it stays around */
    return( TRUE );
}

extern bool ProcReference( void )
/*******************************/
{
    return( ProcArgList( &AddReference, TOK_INCLUDE_DOT ) );
}

extern bool ProcOSName( void )
/****************************/
{
    if( GetToken( SEP_EQUALS, TOK_INCLUDE_DOT ) ) {
        if( FmtData.osname != NULL ) {
            _LnkFree( FmtData.osname );
        }
        FmtData.osname = tostring();
        return( TRUE );
    }
    return( FALSE );
}

extern bool ProcEliminate( void )
/*******************************/
/* turn on dead code elimination */
{
    LinkFlags |= STRIP_CODE;
    return( TRUE );
}

extern bool ProcMaxErrors( void )
/*******************************/
/* set a maximum number of errors for the linker to generate */
{
    if( !GetLong( &MaxErrors ) ) return( FALSE );
    LinkFlags |= MAX_ERRORS_FLAG;
    return( TRUE );
}

extern bool ProcSymFile( void )
/*****************************/
{
    if( GetToken( SEP_EQUALS, TOK_INCLUDE_DOT | TOK_IS_FILENAME ) != FALSE ) {
        if( SymFileName != NULL ) {
            _LnkFree( SymFileName );
        }
        SymFileName = FileName( Token.this, Token.len, E_SYM, FALSE );

    }
    return( TRUE );
}

static file_list * AllocNewFile( member_list *member )
/****************************************************/
{
    file_list *     new_entry;

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

static void * AddObjFile( char *name, char *member, file_list **filelist )
/************************************************************************/
{
    file_list *     new_entry;
    member_list *   new_member;

    new_member = NULL;
    if( member != NULL ) {
        _ChkAlloc( new_member, offsetof(member_list,name) + strlen( member ) + 1 );
        new_member->flags = DBIFlag;
        strcpy( new_member->name, member );
        new_member->next = NULL;
        _LnkFree( member );
        new_entry = CurrSect->files;
        while( new_entry != NULL ) {
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
            new_entry = new_entry->next_file;
        }
    }
    new_entry = AllocNewFile( new_member );
    if( new_member != NULL ) {
        new_entry->file = AllocUniqueFileEntry( name, LibPath );
        new_entry->file->flags |= INSTAT_LIBRARY;
    } else {
        new_entry->file = AllocFileEntry( name, Path );
    }
    *filelist = new_entry;
    return( new_entry );
}

extern file_list *AddObjLib( char *name, unsigned char priority )
/***************************************************************/

 {
    file_list **next_owner;
    file_list **proc_owner;
    file_list * proc_curr;
    file_list * newproc;
    bool        added;

    DEBUG(( DBG_OLD, "Adding Object library name %s", name ));
    proc_owner = &ObjLibFiles;
    for(;;) {
        proc_curr = *proc_owner;
        if( proc_curr == NULL ) break;
        if( proc_curr->priority < priority ) break;
        /* if library already exists with a higher priority */
        if( FNAMECMPSTR( proc_curr->file->name, name ) == 0 ) return(proc_curr);
        proc_owner = &proc_curr->next_file;
    }

    added = TRUE;
    next_owner = proc_owner;            /* replace library if it exists */
    for(;;) {                           /* with a lower priority       */
        if( proc_curr == NULL ) break;
        if( FNAMECMPSTR( proc_curr->file->name, name ) == 0 ) {
            *next_owner = proc_curr->next_file;    /* move entry up */
            proc_curr->next_file = *proc_owner;
            *proc_owner = proc_curr;
            proc_curr->priority = priority;
            newproc = proc_curr;
            added = FALSE;
            break;
        }
        next_owner = &proc_curr->next_file;
        proc_curr = *next_owner;
    }
    if( added ) {   /* if we need to add one */
        newproc = AllocNewFile( NULL );
        newproc->file = AllocUniqueFileEntry( name, LibPath );
        newproc->file->flags |= INSTAT_LIBRARY | INSTAT_OPEN_WARNING;
        newproc->priority = priority;
        newproc->next_file = *proc_owner;
        *proc_owner = newproc;
        LinkState |= LIBRARIES_ADDED;
    }
    return( newproc );
}

static bool AddLibFile( void )
/****************************/
{
    char *      ptr;
    char *      membname;
    file_list * entry;

    CmdFlags &= ~CF_MEMBER_ADDED;
    ptr = GetFileName( &membname, FALSE );
    if( membname != NULL ) {
        LnkMsg( LOC+LINE+WRN+MSG_NO_MEMB_IN_LIBFILE, NULL );
        _LnkFree( membname );
        _LnkFree( ptr );
        return TRUE;
    }
    entry = AllocNewFile( NULL );
    entry->file = AllocFileEntry( ptr, LibPath );
    entry->next_file = *LastLibFile;
    *LastLibFile = entry;
    LastLibFile = &entry->next_file;
    if( *LastLibFile == NULL ) {        // no file directives found yet
        CurrFList = LastLibFile;
    }
    entry->file->flags |= INSTAT_USE_LIBPATH;
    _LnkFree( ptr );
    return( TRUE );
}

extern bool ProcLibFile( void )
/*****************************/
/* process FILE command */
{
    if( (LinkFlags & (DWARF_DBI_FLAG |OLD_DBI_FLAG | NOVELL_DBI_FLAG) ) == 0 ) {
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
    char *      ptr;
    char *      membname;

    ptr = GetFileName( &membname, FALSE );
    AddHTableElem(Root->modFilesHashed, ptr);
    LinkFlags |= GOT_CHGD_FILES;
    return( TRUE );
}

static bool AddFile( void )
/*************************/
{
    char *          ptr;
    char *          membname;
    file_list **    temp;

    CmdFlags &= ~CF_MEMBER_ADDED;
    if( CmdFlags & CF_AUTOSECTION ) {
        if( CmdFlags & CF_SECTION_THERE ) {     // is section there already?
            CmdFlags &= ~CF_SECTION_THERE;
        } else {
            MakeNewSection();
        }
    }
    ptr = GetFileName( &membname, TRUE );
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
    return( TRUE );
}

extern bool ProcFiles( void )
/***************************/
/* process FILE command */
{
    if( (LinkFlags & (DWARF_DBI_FLAG|OLD_DBI_FLAG | NOVELL_DBI_FLAG) ) == 0 ) {
        CmdFlags |= CF_FILES_BEFORE_DBI;
    }
    return( ProcArgList( &AddFile, TOK_INCLUDE_DOT | TOK_IS_FILENAME ) );
}

extern bool ProcModFiles( void )
/***************************/
{
    return( ProcArgList( &AddModFile, TOK_INCLUDE_DOT | TOK_IS_FILENAME ) );
}


#define MAX_PRIORITY    255

static bool AddLib( void )
/************************/
{
    char *      ptr;
    file_list * result;

    ptr = FileName( Token.this, Token.len, E_LIBRARY, FALSE );
    result = AddObjLib( ptr, MAX_PRIORITY );
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
    return( TRUE );
}

extern bool ProcLibrary( void )
/*****************************/
/* process LIB command */
{
    if( (LinkFlags & (DWARF_DBI_FLAG|OLD_DBI_FLAG | NOVELL_DBI_FLAG) ) == 0
        && !IsSystemBlock() ) {
        CmdFlags |= CF_FILES_BEFORE_DBI;
    }
    return( ProcArgList( &AddLib, TOK_INCLUDE_DOT | TOK_IS_FILENAME ) );
}

extern bool ProcOptLib( void )
/****************************/
{
    bool retval;

    CmdFlags |= CF_DOING_OPTLIB;
    retval = ProcLibrary();
    CmdFlags &= ~CF_DOING_OPTLIB;
    return retval;
}

extern bool ProcLibPath( void )
/*****************************/
/* process libpath command */
{
    if( !GetToken( SEP_NO, TOK_INCLUDE_DOT | TOK_IS_FILENAME ) ) {
        LnkMsg( LOC+LINE+WRN+MSG_VALUE_INCORRECT, "s", "libpath" );
        return( TRUE );
    }
    AddLibPaths( Token.this, Token.len, TRUE );  // TRUE == add to front.
    return( TRUE );
}

extern bool ProcPath( void )
/**************************/
/* process PATH option */
{
    bool                ret;
    path_entry          *new_path;
    char                *ptr;

    ret = GetToken( SEP_NO, TOK_INCLUDE_DOT | TOK_IS_FILENAME  );
    if( ret != FALSE ) {
        _ChkAlloc( new_path, sizeof( path_entry ) + Token.len );
        ptr = &new_path->name;
        memcpy( ptr, Token.this, Token.len );
        ptr[ Token.len ] = '\0';
        new_path->next = Path;
        Path = new_path;
        DEBUG(( DBG_BASE, "path: %s", ptr ));
    }
    return( ret );
}

extern bool ProcMap( void )
/*************************/
/* process MAP option */
{
    MapFlags |= MAP_FLAG;
    if( GetToken( SEP_EQUALS, TOK_INCLUDE_DOT | TOK_IS_FILENAME ) ) {
        if( MapFName != NULL ) {
            _LnkFree( MapFName );
        }
        MapFName = FileName( Token.this, Token.len, E_MAP, FALSE );
        DEBUG(( DBG_OLD, "produce map file" ));
    }
    return( TRUE );
}

extern bool ProcStack( void )
/***************************/
/* process STACK option */
{
    LinkFlags |= STK_SIZE_FLAG;
    return( GetLong( &StackSize ) );
}

extern bool ProcNameLen( void )
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

extern bool ProcCase( void )
/**************************/
/* process CASE option */
{
    LinkFlags |= CASE_FLAG;
    SetSymCase();
    DEBUG(( DBG_OLD, "case" ));
    return( TRUE );
}

extern bool ProcNoCaseExact( void )
/*********************************/
/* process nocaseexact option */
{
    LinkFlags &= ~CASE_FLAG;
    SetSymCase();
    DEBUG(( DBG_OLD, "nocase" ));
    return( TRUE );
}

extern bool ProcNoExtension( void )
/*********************************/
{
    CmdFlags |= CF_NO_EXTENSION;
    return TRUE;
}

extern bool ProcNoCache( void )
/*****************************/
{
    LinkFlags &= ~CACHE_FLAG;
    LinkFlags |= NOCACHE_FLAG;
    return( TRUE );
}

extern bool ProcCache( void )
/***************************/
{
    LinkFlags &= ~NOCACHE_FLAG;
    LinkFlags |= CACHE_FLAG;
    return( TRUE );
}

static bool AddDisable( void )
/****************************/
/* disable an error message number */
{
    unsigned_16 value;

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
    return( TRUE );
}

extern bool ProcDisable( void )
/*****************************/
{
    return( ProcArgList( &AddDisable, 0 ) );
}

extern bool ProcNoDefLibs( void )
/*******************************/
/* process CASE option */
{
    return( TRUE );
}

extern bool ProcVerbose( void )
/*****************************/
{
    MapFlags |= MAP_VERBOSE;
    DEBUG(( DBG_OLD, "verbose map file" ));
    return TRUE;
}

extern bool ProcUndefsOK( void )
/******************************/
{
    LinkFlags |= UNDEFS_ARE_OK;
    DEBUG(( DBG_OLD, "undefined symbols are OK" ));
    return( TRUE );
}

extern bool ProcNoUndefsOK( void )
/********************************/
{
    LinkFlags &= ~UNDEFS_ARE_OK;
    return( TRUE );
}

extern bool ProcRedefsOK( void )
/******************************/
{
    LinkFlags |= REDEFS_OK;
    return( TRUE );
}

extern bool ProcNoRedefs( void )
/******************************/
{
    LinkFlags &= ~REDEFS_OK;
    return( TRUE );
}

extern bool ProcCVPack( void )
/****************************/
{
    LinkFlags |= CVPACK_FLAG;
    return TRUE;
}

#define DEFAULT_INC_NAME "__wlink.ilk"

extern bool ProcIncremental( void )
/*********************************/
{
#if _OS != _PHAROS
    if( CmdFlags & CF_AFTER_INC ) {
        LnkMsg( LOC+LINE+ERR+MSG_INC_NEAR_START, NULL );
    }
    LinkFlags |= INC_LINK_FLAG;
    if( GetToken( SEP_EQUALS, TOK_INCLUDE_DOT | TOK_IS_FILENAME ) ) {
        IncFileName = FileName( Token.this, Token.len, E_ILK, FALSE );
    } else if( Name != NULL ) {
        IncFileName = FileName( Name, strlen(Name), E_ILK, TRUE );
    } else {
        IncFileName = ChkStrDup( DEFAULT_INC_NAME );
    }
    ReadPermData();
#endif
    return TRUE;
}

extern bool ProcQuiet( void )
/***************************/
{
    LinkFlags |= QUIET_FLAG;
    return( TRUE );
}

extern bool ProcMangledNames( void )
/**********************************/
{
    LinkFlags |= DONT_UNMANGLE;
    return( TRUE );
}

extern bool ProcOpResource( void )
/********************************/
{
    if( GetToken( SEP_EQUALS, TOK_INCLUDE_DOT | TOK_IS_FILENAME ) ) {
        FmtData.res_name_only = 1;
        FmtData.resource = tostring();
    } else if( GetToken( SEP_NO, TOK_INCLUDE_DOT ) ) {
        FmtData.resource = tostring();
    } else {
        return( FALSE );
    }
    return( TRUE );
}

extern bool ProcStatics( void )
/*****************************/
{
    MapFlags |= MAP_STATICS;
    return( TRUE );
}

#if _LINKER != _WATFOR77
extern bool ProcArtificial( void )
/********************************/
{
    MapFlags |= MAP_ARTIFICIAL;
    return( TRUE );
}
#endif

extern bool ProcAlphabetical( void )
/**********************************/
{
    MapFlags |= MAP_ALPHA;
    return( TRUE );
}

extern bool ProcGlobal( void )
/****************************/
{
    MapFlags |= MAP_GLOBAL;
    return( TRUE );
}

extern bool ProcSort( void )
/**************************/
{
    MapFlags |= MAP_SORT;
    if( !ProcOne( SortOptions, SEP_NO, FALSE ) ) return( TRUE );
    ProcOne( SortOptions, SEP_NO, FALSE );
    return( TRUE );
}

extern bool ProcLanguage( void )
/******************************/
{
    CmdFlags &= ~CF_LANGUAGE_MASK;
    return ProcOne( Languages, SEP_NO, FALSE );
}

extern bool ProcJapanese( void )
/******************************/
{
    CmdFlags |= CF_LANGUAGE_JAPANESE;
    return TRUE;
}

extern bool ProcChinese( void )
/*****************************/
{
    CmdFlags |= CF_LANGUAGE_CHINESE;
    return TRUE;
}

extern bool ProcKorean( void )
/****************************/
{
    CmdFlags |= CF_LANGUAGE_KOREAN;
    return TRUE;
}

extern bool ProcShowDead( void )
/******************************/
{
    LinkFlags |= SHOW_DEAD;
    return( TRUE );
}

extern bool ProcVFRemoval( void )
/*******************************/
{
    LinkFlags |= VF_REMOVAL;
    return( TRUE );
}

extern bool ProcStart( void )
/***************************/
{
    char *name;

    if( !GetToken( SEP_EQUALS, TOK_INCLUDE_DOT ) ) return FALSE;
    StartInfo.user_specd = TRUE;
    name = tostring();
    SetStartSym( name );
    _LnkFree( name );
    return TRUE;
}

static bool GetPackValue( unsigned_32 *value, char *name )
/********************************************************/
{
    ord_state           ret;

    if( !HaveEquals(0) ) return( FALSE );
    ret = getatol( value );
    if( ret != ST_IS_ORDINAL ) {
        LnkMsg( LOC+LINE+WRN+MSG_VALUE_INCORRECT, "s", name );
        return( TRUE );
    }
    if( *value == 0 ) *value = 1;
    return TRUE;
}

extern bool ProcPackcode( void )
/******************************/
{
    unsigned_32         value;

    if( GetPackValue( &value, "packcode" ) ) {
        PackCodeLimit = value;
        LinkFlags |= PACKCODE_FLAG;
        return TRUE;
    }
    return FALSE;
}

extern bool ProcPackdata( void )
/******************************/
{
    unsigned_32         value;

    if( GetPackValue( &value, "packdata" ) ) {
        PackDataLimit = value;
        LinkFlags |= PACKDATA_FLAG;
        return TRUE;
    }
    return FALSE;
}

extern bool ProcNewSegment( void )
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
    return( TRUE );
}

extern sysblock * FindSysBlock( char *name )
/******************************************/
{
    sysblock *  sys;

    for( sys = SysBlocks; sys != NULL; sys = sys->next ) {
        if( stricmp( sys->name, name ) == 0 ) {
            return( sys );
        }
    }
    return( NULL );
}

static sysblock * FindSystemBlock( char *name )
/*********************************************/
{
    sysblock *  sys;
    sysblock *  tmpblk;
    unsigned    len;

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

extern bool ProcSysDelete( void )
/*******************************/
{
    return TRUE;
}

extern bool ProcSystem( void )
/****************************/
/* process the system directive */
{
    char *      sysname;
    sysblock *  sys;
    sysblock ** prev;
    bool        dodelete;

    if( ProcOne( SysBeginOptions, SEP_NO, FALSE ) ) return( TRUE );
    dodelete = ProcOne( SysDeleteOptions, SEP_NO, FALSE );
    if( dodelete ) {
        if( !GetToken( SEP_NO, TOK_INCLUDE_DOT ) ) return FALSE;
    } else {
        Token.thumb = OK;
    }
    sysname = tostring();
    sys = FindSystemBlock( sysname );
    if( sys == NULL ) {
        LnkMsg( LOC+LINE+WRN+MSG_SYSTEM_UNDEFINED, "s", sysname );
        _LnkFree( sysname );
        return( TRUE );
    }
    if( dodelete ) {
        prev = &SysBlocks;
        while( *prev != sys ) {
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
    return( TRUE );
}

static void CopyBlocks( char **copyptr, char *data, unsigned size )
/*****************************************************************/
{
    memcpy( *copyptr, data, size );
    *copyptr += size;
}

static void GetCommandBlock( sysblock **hdr, char *name, parse_entry *endtab )
/****************************************************************************/
{
    char *      copyptr;
    sysblock *  sys;
    stringtable strtab;

    InitStringTable( &strtab, FALSE );
    AddStringTable( &strtab, (char *) &strtab, offsetof(sysblock,commands)  );
    while( !ProcOne( endtab, SEP_SPACE, FALSE ) ) {
        Token.thumb = OK;
        AddStringTable( &strtab, Token.this, Token.len );
        CharStringTable( &strtab, ' ' );
        RestoreParser();
    }
    CharStringTable( &strtab, '\0' );
    _ChkAlloc( copyptr, GetStringTableSize( &strtab ) );
    sys = (sysblock *) copyptr;
    WriteStringTable( &strtab, CopyBlocks, &copyptr );
    FiniStringTable( &strtab );
    sys->name = name;
    LinkList( hdr, sys );
}

extern bool ProcSysBegin( void )
/******************************/
/* parse a system begin block and store it somewhere */
{
    char *      sysname;
    sysblock *  sys;

    if( !GetToken( SEP_NO, TOK_INCLUDE_DOT ) ) {
        LnkMsg( LOC+LINE+WRN+MSG_VALUE_INCORRECT, "s", "system begin" );
        return( TRUE );
    }
    sysname = tostring();
    sys = FindSysBlock( sysname );
    if( sys != NULL ) {
        LnkMsg( LOC+LINE+WRN+MSG_SYSTEM_ALREADY_DEFINED, "s", sysname );
        while( !ProcOne( SysEndOptions, SEP_SPACE, FALSE ) ) {
            Token.thumb = OK;
            RestoreParser();
        }
    } else {
        GetCommandBlock( &SysBlocks, sysname, SysEndOptions );
    }
    return TRUE;
}

extern bool ProcSysEnd( void )
/****************************/
/* finished parsing a system block */
{
    return TRUE;
}

extern bool ProcStartLink( void )
/*******************************/
/* save up list of commands to process later */
{
    GetCommandBlock( &LinkCommands, NULL, EndLinkOpt );
    return TRUE;
}

extern bool ProcEndLink( void )
/*****************************/
/* finished parsing a link section */
{
    return TRUE;
}

extern bool ProcStub( void )
/**************************/
{
    char *      name;
    char **     nameptr;

    if( HintFormat( MK_OS2 | MK_PE ) ) {
        nameptr = &FmtData.u.os2.stub_file_name;
    } else {    /* it must be pharlap */
        nameptr = &FmtData.u.phar.stub;
        Extension = E_LOAD;     /* want .exe instead of .exp now */
    }
    if( !HaveEquals( TOK_INCLUDE_DOT | TOK_IS_FILENAME ) ) return( FALSE );
    name = FileName( Token.this, Token.len, E_LOAD, FALSE );
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
    return( TRUE );
}

extern bool ProcVersion( void )
/*****************************/
{
    ord_state   retval;
    unsigned_32 value;

    if( !GetToken( SEP_EQUALS, 0 ) ) {
        return( FALSE );
    }
    FmtData.minor = 0;
    FmtData.revision = 0;
    retval = getatol( &value );
    if( retval != ST_IS_ORDINAL ) {
        LnkMsg( LOC+LINE+WRN+MSG_VALUE_INCORRECT, "s", "version" );
        return( TRUE );
    }
    FmtData.major = value;
    FmtData.ver_specified = TRUE;
    if( !GetToken( SEP_PERIOD, 0 ) ) {  /*if we don't get a minor number*/
       return( TRUE );                      /* that's OK */
    }
    retval = getatol( &value );
    if( retval != ST_IS_ORDINAL || value >= 100 ) {
        LnkMsg( LOC+LINE+WRN+MSG_VALUE_INCORRECT, "s", "version" );
        return( TRUE );
    } else {
        FmtData.minor = value;
    }
    if( !GetToken( SEP_PERIOD, 0 ) ) {  /* if we don't get a revision*/
        return( TRUE );                 /* that's all right */
    }
    retval = getatol( &value );
    if( retval == ST_NOT_ORDINAL && Token.len == 1 ) {
        FmtData.revision = tolower( *Token.this ) - 'a' + 1;
    } else if( retval == ST_IS_ORDINAL && value < 27 ) {
        FmtData.revision = value;
    } else {
        LnkMsg( LOC+LINE+WRN+MSG_VALUE_INCORRECT, "s", "version" );
        return( TRUE );
    }
    return( TRUE );
}

extern bool ProcImplib( void )
/****************************/
{
    FmtData.make_implib = TRUE;
    FmtData.make_impfile = FALSE;
    _LnkFree( FmtData.implibname );
    if( GetToken( SEP_EQUALS, TOK_IS_FILENAME|TOK_INCLUDE_DOT ) ) {
        FmtData.implibname = tostring();
    }
    return TRUE;
}

extern bool ProcImpFile( void )
/*****************************/
{
    FmtData.make_implib = TRUE;
    FmtData.make_impfile = TRUE;
    _LnkFree( FmtData.implibname );
    if( GetToken( SEP_EQUALS, TOK_IS_FILENAME|TOK_INCLUDE_DOT ) ) {
        FmtData.implibname = tostring();
    }
    return TRUE;
}

static bool AddRunTime( void )
/****************************/
{
    Token.thumb = REJECT;       // reparse last token.
    return( ProcOne( RunOptions, SEP_NO, FALSE ) );
}

extern bool ProcRuntime( void )
/*****************************/
{
    return( ProcArgList( AddRunTime, TOK_INCLUDE_DOT ) );
}

static bool AddSymTrace( void )
/*****************************/
{
    symbol *    sym;

    sym = SymXOp( ST_REFERENCE | ST_CREATE, Token.this, Token.len );
    sym->info |= SYM_TRACE;
    return( TRUE );
}

extern bool ProcSymTrace( void )
/******************************/
{
    LinkFlags |= TRACE_FLAG;
    return ProcArgList( &AddSymTrace, TOK_INCLUDE_DOT );
}

static bool AddModTrace( void )
/*****************************/
{
    char *          membname;
    char *          fname;
    trace_info *    info;

    fname = GetFileName( &membname, FALSE );
    _ChkAlloc( info, sizeof( trace_info ) );
    info->u.name = fname;
    info->member = membname;
    info->found = FALSE;        // used for matching libraries
    info->next = TraceList;
    TraceList = info;
    return( TRUE );
}

extern bool ProcModTrace( void )
/******************************/
{
    LinkFlags |= TRACE_FLAG;
    return ProcArgList( &AddModTrace, TOK_INCLUDE_DOT | TOK_IS_FILENAME );
}
