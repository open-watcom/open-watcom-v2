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
 *  CMDLINE : command line parser
 *
 */

#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "linkstd.h"
#include "exeos2.h"
#include "msg.h"
#include "alloc.h"
#include "command.h"
#include "wlnkmsg.h"
#include "cmdall.h"
#include "cmdos2.h"
#include "cmdqnx.h"
#include "cmdnov.h"
#include "cmdelf.h"
#include "cmdphar.h"
#include "cmddos.h"
#include "cmdline.h"
#include "overlays.h"
#include "fileio.h"
#include "symtrace.h"
#include "reloc.h"
#include "strtab.h"
#include "carve.h"
#include "permdata.h"
#include "dbgall.h"

#ifdef _INT_DEBUG
unsigned int            Debug;
#endif

extern void             Ignite( void );
static void             Crash( bool );
static void             DoCmdParse( void );

static bool             ProcDosHelp( void );
static bool             ProcOS2Help( void );
static bool             ProcPharHelp( void );
static bool             ProcNovellHelp( void );
static bool             ProcQNXHelp( void );
static bool             ProcELFHelp( void );
static bool             ProcWindowsHelp( void );
static bool             ProcNTHelp( void );

#ifdef _INT_DEBUG
extern  bool            ProcXDbg( void );
extern  bool            ProcIntDbg( void );
static  void            PrintSect( section * );
#endif

static  parse_entry   FormatHelp[] = {
    "Dos",          &ProcDosHelp,           MK_ALL,     0,
#ifdef _OS2
    "OS2",          &ProcOS2Help,           MK_ALL,     0,
    "WINdows",      &ProcWindowsHelp,       MK_ALL,     0,
    "NT",           &ProcNTHelp,            MK_ALL,     0,
#endif
#ifdef _PHARLAP
    "PHARlap",      &ProcPharHelp,          MK_ALL,     0,
#endif
#ifdef _NOVELL
    "NOVell",       &ProcNovellHelp,        MK_ALL,     0,
#endif
#ifdef _QNXLOAD
    "QNX",          &ProcQNXHelp,           MK_ALL,     0,
#endif
#ifdef _ELF
    "ELF",          &ProcELFHelp,           MK_ALL,     0,
#endif
    NULL
};

byte            Extension;
file_list **    CurrFList;
tok             Token;
commandflag     CmdFlags;
char *          Name;
sysblock *      SysBlocks;
sysblock *      LinkCommands;

static sysblock *       PrevCommand;

#define INIT_FILE_NAME  "wlink.lnk"

extern void InitCmdFile( void )
/*****************************/
{
    PrevCommand = NULL;
}

static void ResetCmdFile( void )
/******************************/
/* do all the initialization necessary for parsing a command file */
{
    SysBlocks = NULL;
    Extension = E_LOAD;
    Name = NULL;
    CmdFlags = CF_UNNAMED;
    Path = NULL;
    memset( &FmtData, 0, sizeof(FmtData) );
    FmtData.base = NO_BASE_SPEC;
    FmtData.objalign = NO_BASE_SPEC;
    FmtData.type = MK_ALL;
    FmtData.def_seg_flags = SEG_LEVEL_3;
    CurrSect = Root;
    CurrFList = &Root->files;
    DBIFlag = 0;        /*  default is only global information */
}

extern void DoCmdFile( char *fname )
/**********************************/
/* start parsing the command */
{
    exe_format  possible;
    f_handle    file;
    size_t      namelen;
    unsigned    extension;

    ResetCmdFile();
    if( fname == NULL || *fname == '\0' ) {
        _ChkAlloc( fname, (10*1024) );  // arbitrarily large buffer that won't
        GetCmdLine( fname );            // be overflowed
        NewCommandSource( NULL, fname, COMMANDLINE );
    } else {
        NewCommandSource( NULL, fname, ENVIRONMENT );
    }
    if( IsStdOutConsole() ) {
        CmdFlags |= CF_TO_STDOUT;
    }
    while( *fname == ' ' ) {
        fname++;
    }
    if( QSysHelp( &Token.next ) ) {
        Help();
    }
    if( *fname == '?' ) {
        Token.next = fname + 1;       // skip question mark.
        Help();
#if _OS == _QNX
    } else if( *fname == '-' ) {
#else
    } else if( *fname == '-' || *fname == '/' ) {
#endif
        if( *(fname + 1) == '?' ) {
            Token.next = fname + 2;     // skip /?
            Help();
        }
    }
    if( *fname == '\0' ) {       // go into interactive mode.
        Token.how = INTERACTIVE;
        Token.where = ENDOFLINE;
        LnkMsg( INF+MSG_PRESS_CTRL_Z, NULL );
    }
#if _DEVELOPMENT == _ON
    file = SearchPath( "nwlink.lnk" );
    if( file == NIL_HANDLE ) {
        file = SearchPath( INIT_FILE_NAME );
    }
#else
    file = SearchPath( INIT_FILE_NAME );
#endif
    if( file != NIL_HANDLE ) {
        _ChkAlloc( fname, sizeof(INIT_FILE_NAME));
        memcpy( fname, INIT_FILE_NAME, sizeof(INIT_FILE_NAME) );
        SetCommandFile( file, fname );
    }
    if( Spawn( &DoCmdParse ) ) {
        Ignite();
        Suicide();
    }
    GetExtraCommands();
    if( !(LinkState & FMT_DECIDED) ) {
        /* restrict set to automatically decided ones */
#if _OS == _QNX
#define LAST_CHANCE MK_QNX|MK_OS2_NE|MK_OS2_LX|MK_OS2_LE
#elif _OS == _NT
#define LAST_CHANCE MK_PE|MK_WINDOWS|MK_OS2_LX|MK_OS2_NE
#else
#define LAST_CHANCE MK_DOS_EXE|MK_PHAR_SIMPLE|MK_OS2_NE|MK_OS2_LX|MK_OS2_LE
#endif
        HintFormat( LAST_CHANCE );
    } else {
        /* restrict to only one unique type */
        for( possible = 1; possible != 0; possible <<= 1 ) {
            if( FmtData.type & possible ) {
                FmtData.type = possible;
                break;
            }
        }
    }
    if( FmtData.type & (MK_NOVELL | MK_DOS) && LinkFlags & INC_LINK_FLAG ) {
        LnkMsg( FTL+MSG_FORMAT_BAD_OPTION, "s", "incremental" );
    }
#ifdef _NOVELL
    if( FmtData.type & MK_NOVELL ) {
        CmdNovFini();
    } else
#endif
           if( FmtData.type & MK_OVERLAYS ) {
        CmdOvlFini();
        AddObjLib( "wovl.lib", 0 );     // add a reference to wovl.lib
    }
    if( Name == NULL || !(CmdFlags & CF_HAVE_FILES) ) {
        Ignite();
        LnkMsg( FTL+MSG_NO_FILES_FOUND, NULL );
    }
    namelen = strlen( Name );
    if( MapFlags & MAP_FLAG ) {
        if( MapFName == NULL ) {
            MapFName = FileName( Name, namelen, E_MAP, TRUE );
        }
    } else {
        MapFlags = 0;   // if main isn't set, don't set anything.
    }
    if( SymFileName == NULL && (CmdFlags & CF_SEPARATE_SYM ||
                   (LinkFlags & OLD_DBI_FLAG && FmtData.type & MK_COM)) ) {
        SymFileName = FileName( Name, namelen, E_SYM, TRUE );
    }
    if( FmtData.make_implib && FmtData.implibname == NULL ) {
        if( FmtData.make_impfile ) {
            extension = E_LBC;
        } else {
            extension = E_LIBRARY;
        }
        FmtData.implibname = FileName( Name, namelen, extension, TRUE );
    }
    CheckTraces();
    BurnUtils();
    PruneSystemList();
    NumberSections();
    DBIInit();
}

extern char * GetNextLink( void )
/*******************************/
{
    char *      cmd;

    cmd = NULL;
    _LnkFree( PrevCommand );
    if( LinkCommands != NULL ) {
        PrevCommand = LinkCommands;
        LinkCommands = LinkCommands->next;
        cmd = PrevCommand->commands;
    }
    return cmd;
}

#define PREFIX_SIZE 8

struct extra_cmd_info {
    unsigned    type;
    char        prefix[PREFIX_SIZE];
    bool        retry;
};

static struct extra_cmd_info ExtraCmds[] = {
        EXTRA_NAME_DIR, "name    ",     FALSE,
        EXTRA_OBJ_FILE, "file    ",     TRUE,
        EXTRA_LIB_FILE, "lib     ",     TRUE,
        EXTRA_RES_FILE, "opt res=",     FALSE,
        0,              "\0",           FALSE
};

extern void GetExtraCommands( void )
/**********************************/
{
    struct extra_cmd_info *     cmd;
    char                        buff[ _MAX_PATH + PREFIX_SIZE ];

    cmd = ExtraCmds;
    while( cmd->prefix[0] != '\0' ) {
        for(;;) {
            memcpy( buff, cmd->prefix, PREFIX_SIZE );
            if( !GetAddtlCommand( cmd->type, buff + PREFIX_SIZE ) ) break;
            NewCommandSource( NULL, buff, COMMANDLINE );
            if( Spawn( &DoCmdParse ) ) break;
            if( !cmd->retry ) break;
        }
        cmd++;
    }

}

extern void Syntax( void )
/************************/
{
    if( Token.this == NULL ) {
        LnkMsg( LOC+LINE+FTL+MSG_DIRECTIVE_ERR_BEGINNING, NULL );
    } else {
        Token.this[Token.len] = '\0';
        LnkMsg( LOC+LINE+FTL+MSG_DIRECTIVE_ERR, "s", Token.this );
    }
}

static void Crash( bool check_file )
/**********************************/
{
    char        buff[81];
    int         len;
    f_handle    fp;

    if( check_file ) {
        fp = SearchPath( "wlink.hlp" );
        if( fp != NIL_HANDLE ) {
            WLPrtBanner();
            for( ;; ) {
                len = QRead( fp, buff, 80, "wlink.hlp" );
                if( len == 0 ) break;
                buff[len] = '\0';
                WriteStdOut( buff );
            }
            QClose( fp, "wlink.hlp" );
            Ignite();
            Suicide();
        }
    }
    DisplayOptions();
    Ignite();
    Suicide();
}

static void Help( void )
/**********************/
// display help, optionally allowing the user to specifiy the format he/she
// wants the help for.
{
    EatWhite();
    if( *Token.next == '?' ) {
        Crash( FALSE );
    } else if( *Token.next == '\0' || !ProcOne( FormatHelp, SEP_NO, FALSE ) ) {
        Crash( TRUE );
    } else {
        Ignite();
        Suicide();
    }
}

static void DoCmdParse( void )
/****************************/
{
    while( GetToken( SEP_END, TOK_INCLUDE_DOT ) == FALSE ) {
        if( ProcOne( Directives, SEP_NO, TRUE ) == FALSE ) {
            Syntax();
        }
        RestoreParser();
    }
}

static void WriteGenHelp( void )
/******************************/
{
    WLPrtBanner();
    WriteHelp( MSG_GENERAL_HELP_0, MSG_GENERAL_HELP_51, FALSE );
}

static void DisplayOptions( void )
/********************************/
{
    bool    isout;

    isout = FALSE;
    if( CmdFlags & CF_TO_STDOUT ) {
        isout = TRUE;
    }
    WriteGenHelp();
#if defined( _QNXLOAD ) && _OS == _QNX
    WriteHelp( MSG_QNX_HELP_0, MSG_QNX_HELP_15, isout );
#endif
#ifdef _EXE
    WriteHelp( MSG_DOS_HELP_0, MSG_DOS_HELP_15, isout );
#endif
#ifdef _OS2
    WriteHelp( MSG_OS2_HELP_0, MSG_OS2_HELP_31, isout );
    WriteHelp( MSG_WINDOWS_HELP_0, MSG_WINDOWS_HELP_31, isout );
    WriteHelp( MSG_NT_HELP_0, MSG_NT_HELP_31, isout );
#endif
#ifdef _PHARLAP
    WriteHelp( MSG_PHAR_HELP_0, MSG_PHAR_HELP_15, isout );
#endif
#ifdef _NOVELL
    WriteHelp( MSG_NOVELL_HELP_0, MSG_NOVELL_HELP_31, isout );
#endif
#if defined( _QNXLOAD ) && _OS != _QNX
    WriteHelp( MSG_QNX_HELP_0, MSG_QNX_HELP_15, isout );
#endif
#ifdef _ELF
    WriteHelp( MSG_ELF_HELP_0, MSG_ELF_HELP_15, isout );
#endif
}

#ifdef _EXE
static bool ProcDosHelp( void )
/*****************************/
{
    WriteGenHelp();
    WriteHelp( MSG_DOS_HELP_0, MSG_DOS_HELP_15, CmdFlags & CF_TO_STDOUT );
    return( TRUE );
}
#endif
#ifdef _OS2
static bool ProcOS2Help( void )
/*****************************/
{
    WriteGenHelp();
    WriteHelp( MSG_OS2_HELP_0, MSG_OS2_HELP_31, CmdFlags & CF_TO_STDOUT );
    return( TRUE );
}

static bool ProcWindowsHelp( void )
/*********************************/
{
    WriteGenHelp();
    WriteHelp( MSG_WINDOWS_HELP_0, MSG_WINDOWS_HELP_31,
                                                CmdFlags & CF_TO_STDOUT );
    return( TRUE );
}

static bool ProcNTHelp( void )
/****************************/
{
    WriteGenHelp();
    WriteHelp( MSG_NT_HELP_0, MSG_NT_HELP_31, CmdFlags & CF_TO_STDOUT );
    return( TRUE );
}
#endif
#ifdef _PHARLAP
static bool ProcPharHelp( void )
/******************************/
{
    WriteGenHelp();
    WriteHelp( MSG_PHAR_HELP_0, MSG_PHAR_HELP_15, CmdFlags & CF_TO_STDOUT );
    return( TRUE );
}
#endif
#ifdef _NOVELL
static bool ProcNovellHelp( void )
/********************************/
{
    WriteGenHelp();
    WriteHelp( MSG_NOVELL_HELP_0, MSG_NOVELL_HELP_31,
                                                CmdFlags & CF_TO_STDOUT );
    return( TRUE );
}
#endif
#ifdef _QNXLOAD
static bool ProcQNXHelp( void )
/*******************************/
{
    WriteGenHelp();
    WriteHelp( MSG_QNX_HELP_0, MSG_QNX_HELP_15, CmdFlags & CF_TO_STDOUT );
    return( TRUE );
}
#endif

#ifdef _ELF
static bool ProcELFHelp( void )
/*******************************/
{
    WriteGenHelp();
    WriteHelp( MSG_ELF_HELP_0, MSG_ELF_HELP_15, CmdFlags & CF_TO_STDOUT );
    return( TRUE );
}
#endif

static void WriteHelp( unsigned first_ln, unsigned last_ln, bool prompt )
/***********************************************************************/
{
    char        msg_buffer[RESOURCE_MAX_SIZE];
    int         previous_null = 0;

    if( prompt ) {
        PressKey();
    }
    for( ; first_ln <= last_ln; first_ln++ ) {
        Msg_Get( first_ln, msg_buffer );
        if( previous_null ) {
            if( msg_buffer[0] != '\0' ) {
                PressKey();
                WriteMsg( msg_buffer );
                previous_null = 0;
            } else break;
        } else if( msg_buffer[0] == '\0' ) {
            previous_null = 1;
        } else {
            WriteMsg( msg_buffer );
        }
    }
}

static void PressKey()
/********************/
{
    char        msg_buffer[RESOURCE_MAX_SIZE];
    char        result;

    Msg_Get( MSG_PRESS_KEY, msg_buffer );
    WriteStdOut( msg_buffer );
    result = WaitForKey();
    WriteNLStdOut();
    if( tolower(result) == 'q' ) {
        Ignite();
        Suicide();
    }
}

static void WriteMsg( char msg_buffer[] )
/***************************************/
{
    WriteStdOut( msg_buffer );
    WriteNLStdOut();
}

extern void FreePaths( void )
/***************************/
// Free paths & filenames.
{
    FreeList( Path );
    Path = NULL;
    if( Name != NULL ) {
        _LnkFree( Name );
        Name = NULL;
    }
}

extern void Burn( void )
/**********************/
// necessary to split this out from Ignite() for the workframe options
// processor.
{
    outfilelist *   fnode;

    FreePaths();
    if( MapFName != NULL ) {
        _LnkFree( MapFName );
        MapFName = NULL;
    }
    fnode = OutFiles;
    while( fnode != NULL ) {
        _LnkFree( fnode->fname );
        OutFiles = fnode->next;
        _LnkFree( fnode );
        fnode = OutFiles;
    }
    BurnUtils();
}

extern void Ignite( void )
/************************/
/* free local structures */
{
    BurnSystemList();
    Burn();
}

extern void SetFormat( void )
/***************************/
// do final processing now that the executable format has been decided.
{
    char *      fname;

    if( CmdFlags & CF_NO_EXTENSION ) {
        fname = Name;
    } else {
        fname = FileName( Name, strlen(Name), Extension, CmdFlags & CF_UNNAMED);
        _LnkFree( Name );
    }
    Root->outfile = NewOutFile( fname );
    Name = NULL;
    FillOutFilePtrs();   // fill in all unspecified outfile pointers.
    if( MapFlags & MAP_FLAG ) {
        LnkMsg( MAP+MSG_EXE_NAME, "s", Root->outfile->fname );
        LnkMsg( MAP+MSG_CREATE_EXE, "f" );
    }
#ifdef _QNXLOAD
    if( FmtData.type & MK_QNX ) {
        CmdQNXFini();
    }
#endif
    SetRelocSize();
}

struct select_format {
    exe_format      bits;
    char            *lib_var_name;
    void            (*set_func)(void);
    void            (*free_func)(void);
};

static struct select_format PossibleFmt[] = {
    MK_DOS,         "LIBDOS",       NULL,           NULL,
#ifdef _QNXLOAD
    MK_QNX,         "LIBQNX",       &SetQNXFmt,     &FreeQNXFmt,
#endif
#ifdef _ELF
    MK_ELF,         "LIBELF",       &SetELFFmt,     &FreeELFFmt,
#endif
#ifdef _OS2
    MK_WINDOWS,     "LIBWIN",       &SetOS2Fmt,     &FreeOS2Fmt,
    MK_OS2_NE,      "LIBOS2",       &SetOS2Fmt,     &FreeOS2Fmt,
    MK_OS2_LE,      "LIBOS2FLAT",   &SetOS2Fmt,     &FreeOS2Fmt,
    MK_OS2_LX,      "LIBOS2FLAT",   &SetOS2Fmt,     &FreeOS2Fmt,
    MK_PE,          "LIBPE",        &SetOS2Fmt,     &FreeOS2Fmt,
#endif
#ifdef _PHARLAP
    MK_PHAR_LAP,    "LIBPHAR",      &SetPharFmt,    &FreePharFmt,
#endif
#ifdef _NOVELL
    MK_NOVELL,      "LIBNOV",       &SetNovFmt,     &FreeNovFmt,
#endif
    0 };


extern void AddFmtLibPaths( void )
/********************************/
{
    struct select_format    *check;
    exe_format              possible;

    if( !(LinkState & FMT_DECIDED) ) return;
    check = PossibleFmt;
    for( ;; ) {
        possible = check->bits;
        if( possible == 0 ) return;
        if( (~possible & FmtData.type) == 0 ) break;
        ++check;
    }
    AddEnvPaths( check->lib_var_name );
}

static void InitFmt( void (*set)(void) )
/**************************************/
{
    if( LinkState & FMT_INITIALIZED ) return;
    if( set != NULL ) set();
    LinkState |= FMT_INITIALIZED;
}

extern bool HintFormat( exe_format hint )
/***************************************/
{
    struct select_format    *check;
    exe_format              possible;

    if( !(hint & FmtData.type) ) return( FALSE );
    FmtData.type &= hint;
    if( LinkState & FMT_DECIDED ) return( TRUE );
    check = PossibleFmt;
    for( ;; ) {
        possible = check->bits;
        if( possible == 0 ) {
#ifdef _OS2
            if( (~(MK_OS2|MK_PE) & FmtData.type) == 0 ) {
                /* Windows, OS/2 V1.x, OS/2 V2.x, PE all
                   want the same structure */
                InitFmt( &SetOS2Fmt );
            }
#endif
            return( TRUE );
        }
        if( (~possible & FmtData.type) == 0 ) break;
        ++check;
    }
    InitFmt( check->set_func );
    LinkState |= FMT_DECIDED;
    if( LinkState & SEARCHING_LIBRARIES ) AddFmtLibPaths();
    return( TRUE );
}

extern void DecideFormat( void )
/******************************/
{
    exe_format  possible;
    exe_format  allowed;
    char        rc_buff[RESOURCE_MAX_SIZE];

    if( !(LinkState & FMT_DECIDED) ) {
        possible = FmtData.type;
        allowed = MK_OS2_NE | MK_OS2_LX;
        if( !(LinkState & FMT_SEEN_IMPORT_CMT) ) allowed = ~allowed;
        if( (possible & allowed) != 0 ) possible &= allowed;
        HintFormat( possible );
        if( !(LinkState & FMT_DECIDED) ) {
            Msg_Get( MSG_FORMAT_NOT_DECIDED, rc_buff );
            LnkMsg( FTL+MSG_INTERNAL, "s", rc_buff );
        }
    }
}

extern void FreeFormatStuff()
/***************************/
{
    struct select_format    *check;
    exe_format              possible;

    if( !(LinkState & FMT_DECIDED) ) return;
    check = PossibleFmt;
    for( ;; ) {
        possible = check->bits;
        if( possible == 0 ) return;
        if( (~possible & FmtData.type) == 0 ) break;
        ++check;
    }
    if( check->free_func != NULL ) check->free_func();
}

extern void AddCommentLib( char *ptr, int len, unsigned char priority )
/*********************************************************************/
//  Add a library from a comment record.
{
    file_list * result;

    if( CmdFlags & CF_NO_DEF_LIBS ) return;
    ptr = FileName( ptr, len, E_LIBRARY, FALSE );
    result = AddObjLib( ptr, priority );
    CheckLibTrace( result );
    DEBUG(( DBG_BASE, "library: %s", ptr ));
    _LnkFree( ptr );
}

// we don't need these next two when under workframe
#ifndef APP

extern void AddLibPaths( char *name, int len, bool add_to_front )
/***************************************************************/
{
    path_entry *    newpath;
    file_list *     libfiles;

    _ChkAlloc( newpath, sizeof( path_entry ) + len );
    memcpy( newpath->name, name, len );
    newpath->name[len] = '\0';
    if( add_to_front ) {
        newpath->next = LibPath;
        LibPath = newpath;
    } else {
        LinkList( &LibPath, newpath );
    }
    if( LibPath == newpath ) {
        libfiles = ObjLibFiles;
        while( libfiles != NULL ) {
            libfiles->file->path_list = LibPath;
            libfiles = libfiles->next_file;
        }
        libfiles = Root->files;
        while( libfiles != NULL && libfiles->file->flags & INSTAT_USE_LIBPATH ){
            libfiles->file->path_list = LibPath;
            libfiles = libfiles->next_file;
        }
    }
}

extern void AddEnvPaths( char *envname )
/**************************************/
{
    char *  val;

    val = GetEnvString( envname );
    if( val == NULL ) return;
    AddLibPaths( val, strlen( val ), FALSE );
}

#endif

extern void ExecSystem( char *name )
/**********************************/
/* run a system block with the given name (only called once!)
 * (this is called after the parser has already been stopped */
{
    sysblock *  sys;

    sys = FindSysBlock( name );
    if( sys != NULL ) {
        NewCommandSource( NULL, NULL, ENVIRONMENT ); // the "command line"
        Token.where = ENDOFCMD;     // nothing on this command line
        NewCommandSource( sys->name, sys->commands, SYSTEM ); // input file
        sys->name = NULL;
        while( GetToken( SEP_END, TOK_INCLUDE_DOT ) == FALSE ) {
            if( ProcOne( SysDirectives, SEP_NO, FALSE ) == FALSE ) {
                LnkMsg( LOC+LINE+WRN+MSG_ERROR_IN_SYSTEM_BLOCK, NULL );
                RestoreCmdLine();
                break;
            }
        }
        BurnSystemList();
        BurnUtils();
    }
}

static void CleanSystemList( bool check )
/***************************************/
/* clean up the list of system blocks */
{
    sysblock ** sys;
    sysblock *  next;
    char *      name;

    sys = &SysBlocks;
    while( *sys != NULL ) {
        name = (*sys)->name;
        if( !check
        || (memcmp( "286", name, 4 ) != 0 && memcmp( "386", name, 4) != 0)){
            next = (*sys)->next;
            _LnkFree( name );
            _LnkFree( *sys );
            *sys = next;
        } else {
            sys = &(*sys)->next;
        }
    }
}

static void PruneSystemList( void )
/*********************************/
/* delete all system blocks except for the "286" and "386" records */
{
    CleanSystemList( TRUE );
}

extern void BurnSystemList( void )
/********************************/
/* delete everything in the system list */
{
    CleanSystemList( FALSE );
}

extern bool ProcImport( void )
/****************************/
{
#if defined( _OS2) || defined( _ELF ) || defined( _NOVELL )
    if( HintFormat( MK_OS2 | MK_PE ) ) {
        return ProcOS2Import();
    } else if( HintFormat( MK_ELF ) ) {
        return ProcELFImport();
    } else {
        return ProcNovImport();
    }
#else
    return FALSE;
#endif
}

#if defined(_OS2) || defined(_NOVELL)
extern bool ProcExport( void )
/****************************/
{
#ifdef _OS2
    if( HintFormat( ( MK_OS2 | MK_PE ) ) ) {
        return( ProcOS2Export() );
    } else
#endif
#ifdef _NOVELL
            {
        return( ProcNovExport() );
    }
#endif
}
#endif

#if defined( _QNXLOAD ) || defined( _OS2 )
extern bool ProcNoRelocs( void )
/******************************/
{
#if defined( _QNXLOAD )
    if( HintFormat( MK_QNX ) ) {
        return( ProcQNXNoRelocs() );
    }
#endif
#if defined( _OS2 )
    if( HintFormat( MK_PE ) ) {
        return( ProcPENoRelocs() );
    }
#endif
    return( TRUE );
}
#endif

#if defined(_OS2) || defined(_QNXLOAD)
extern bool ProcSegment( void )
/*****************************/
{
#ifdef _OS2
    if( HintFormat( MK_OS2 | MK_PE ) ) {
        return( ProcOS2Segment() );
    }
#endif
#ifdef _QNXLOAD
    if( HintFormat( MK_QNX ) ) {
        return( ProcQNXSegment() );
    }
#endif
    return( TRUE );
}
#endif

extern bool ProcAlignment( void )
/*******************************/
{
#if defined( _OS2 ) || defined( _ELF )
    if( HintFormat( MK_OS2_16BIT | MK_OS2_LX | MK_PE ) ) {
        return( ProcOS2Alignment() );
    } else if( HintFormat( MK_ELF ) ) {
        return( ProcELFAlignment() );
    }
#endif
    return( TRUE );
}

extern bool ProcHeapSize( void )
/******************************/
{
#if _OS == _QNX
    if( HintFormat( MK_QNX ) ) {
        return( ProcQNXHeapSize() );
    }
#endif
#ifdef _OS2
    if( HintFormat( MK_OS2 | MK_PE ) ) {
        return( ProcOS2HeapSize() );
    }
#endif
#if defined( _QNXLOAD ) && (_OS != _QNX)
    if( HintFormat( MK_QNX ) ) {
        return( ProcQNXHeapSize() );
    }
#endif
    return( TRUE );
}

#ifndef APP
#if defined(_PHARLAP) || defined(_QNXLOAD) || defined(_OS2)
extern bool ProcOffset( void )
/****************************/
{
    if( !GetLong( &FmtData.base ) ) return( FALSE );
    if( !(FmtData.type & (MK_PHAR_LAP|MK_QNX_FLAT)) ) {
        ChkBase( 64 * 1024 );
    } else if( !(FmtData.type & (MK_OS2_FLAT|MK_PE)) ) {
        ChkBase( 4 * 1024 );
    }
    return( TRUE );
}
#endif
#endif

#ifdef _INT_DEBUG
extern bool ProcXDbg( void )
/**************************/
/* process DEBUG command */
{
    char        value[7];

    if( GetToken( SEP_EQUALS, TOK_INCLUDE_DOT ) ) {
        if( Token.len > 6 ) {
            return( FALSE );
        } else {
            memcpy( value, Token.this, Token.len );
            value[Token.len] = '\0';
            Debug = strtoul( value, NULL, 0 );
            DEBUG(( DBG_BASE, "debugging info type = %x", Debug ));
        }
        return( TRUE );
    } else {
        return( FALSE );
    }
}

extern bool ProcIntDbg( void )
/****************************/
{
    LinkState |= INTERNAL_DEBUG;
    return TRUE;
}
#endif
