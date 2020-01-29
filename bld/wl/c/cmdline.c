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
* Description:  Command line parser.
*
****************************************************************************/


#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "linkstd.h"
#include "exeos2.h"
#include "msg.h"
#include "alloc.h"
#include "cmdutils.h"
#include "wlnkmsg.h"
#include "cmdall.h"
#include "cmdos2.h"
#include "cmdqnx.h"
#include "cmd16m.h"
#include "cmdnov.h"
#include "cmdelf.h"
#include "cmdphar.h"
#include "cmddos.h"
#include "cmdzdos.h"
#include "cmdrdv.h"
#include "cmdraw.h"
#include "cmdline.h"
#include "overlays.h"
#include "fileio.h"
#include "ideentry.h"
#include "symtrace.h"
#include "reloc.h"
// #include "strtab.h"
// #include "carve.h"
// #include "permdata.h"
#include "dbgall.h"
#include "loadfile.h"
#include "pathlist.h"
#include "cmdhelp.h"
#include "library.h"
#include "sysblock.h"


#ifdef BOOTSTRAP
#define INIT_FILE_NAME  "bwlink.lnk"
#else
#define INIT_FILE_NAME  "wlink.lnk"
#endif
#define INIT_FILE_ENV   "WLINK_LNK"

#define HELP_FILE_NAME  "wlink.hlp"

typedef struct {
    exe_format      bits;
    char            *lib_var_name;
    void            (*set_func)(void);
    void            (*free_func)(void);
} select_format;

file_defext             Extension;
file_list               **CurrFList;
tok                     Token;
commandflag             CmdFlags;
char                    *Name;
sysblock                *SysBlocks;
sysblock                *LinkCommands;

static sysblock         *PrevCommand;

static const select_format PossibleFmt[] = {
#ifdef _EXE
    MK_DOS,         "LIBDOS",       SetDosFmt,      FreeDosFmt,
#endif
#ifdef _DOS16M
    MK_DOS16M,      "LIBDOS16M",    SetD16MFmt,     FreeD16MFmt,
#endif
#ifdef _QNX
    MK_QNX,         "LIBQNX",       SetQNXFmt,      FreeQNXFmt,
#endif
#ifdef _ELF
    MK_ELF,         "LIBELF",       SetELFFmt,      FreeELFFmt,
#endif
#ifdef _OS2
    MK_WINDOWS,     "LIBWIN",       SetOS2Fmt,      FreeOS2Fmt,
    MK_OS2_NE,      "LIBOS2",       SetOS2Fmt,      FreeOS2Fmt,
    MK_OS2_LE,      "LIBOS2FLAT",   SetOS2Fmt,      FreeOS2Fmt,
    MK_OS2_LX,      "LIBOS2FLAT",   SetOS2Fmt,      FreeOS2Fmt,
    MK_PE,          "LIBPE",        SetOS2Fmt,      FreeOS2Fmt,
    MK_WIN_VXD,     "LIBVXD",       SetOS2Fmt,      FreeOS2Fmt,
#endif
#ifdef _PHARLAP
    MK_PHAR_LAP,    "LIBPHAR",      SetPharFmt,     FreePharFmt,
#endif
#ifdef _NOVELL
    MK_NOVELL,      "LIBNOV",       SetNovFmt,      FreeNovFmt,
#endif
#ifdef _RDOS
    MK_RDOS,        "LIBRDOS",      SetRdosFmt,     FreeRdosFmt,
#endif
#ifdef _ZDOS
    MK_ZDOS,        "LIBZDOS",      SetZdosFmt,     FreeZdosFmt,
#endif
    0,              NULL,           NULL,           NULL
};

void InitCmdFile( void )
/**********************/
{
    PrevCommand = NULL;
}

void SetSegMask(void)
/*******************/
{
   FmtData.SegShift = 16 - FmtData.Hshift;
   FmtData.SegMask = (1 << FmtData.SegShift) - 1;
}

static void ResetCmdFile( void )
/*******************************
 * do all the initialization necessary for parsing a command file
 */
{
    SysBlocks = NULL;
    Extension = E_LOAD;
    Name = NULL;
    CmdFlags = CF_UNNAMED;
    ObjPath = NULL;
    memset( &FmtData, 0, sizeof( FmtData ) );
    FmtData.base = NO_BASE_SPEC;
    FmtData.objalign = NO_BASE_SPEC;
    FmtData.type = MK_ALL;
    FmtData.def_seg_flags = SEG_LEVEL_3;
    FmtData.Hshift = 12;    /* May want different value for some 32 bit segmented modes */
    FmtData.FillChar = 0;   /* Default fillchar for segment alignment */
    SetSegMask();
    CurrSect = Root;
    CurrFList = &Root->files;
    DBIFlag = 0;            /*  default is only global information */
}

static bool sysHelp( void )
/*************************/
{
    const char  *p;
    bool        help;

    help = true;
    p = Token.next;
    while( *p == ' ' ) {
        p++;
    }
    if( p[0] == '?' ) {
        p++;            // skip '?'
#if defined( __UNIX__ )
    } else if( p[0] == '-' && p[1] == '?' ) {
#else
    } else if( ( p[0] == '-' || p[0] == '/' ) && p[1] == '?' ) {
#endif
        p += 2;         // skip '-?' or '/?'
    } else {
        help = false;
    }
    Token.next = p;
    return( help );
}

char *GetNextLink( void )
/***********************/
{
    char        *cmd;

    cmd = NULL;
    if( PrevCommand != NULL )
        _LnkFree( PrevCommand );
    if( LinkCommands != NULL ) {
        PrevCommand = LinkCommands;
        LinkCommands = LinkCommands->next;
        cmd = PrevCommand->commands;
    }
    return( cmd );
}

void Syntax( void )
/*****************/
{
    if( Token.this == NULL ) {
        LnkMsg( LOC+LINE+FTL+MSG_DIRECTIVE_ERR_BEGINNING, NULL );
    } else {
        ((char *)Token.this)[Token.len] = '\0';
        LnkMsg( LOC+LINE+FTL+MSG_DIRECTIVE_ERR, "s", Token.this );
    }
}

void PressKey( void )
/*******************/
{
    char        msg_buffer[RESOURCE_MAX_SIZE];
    int         result;

    Msg_Get( MSG_PRESS_KEY, msg_buffer );
    WriteStdOut( msg_buffer );
    result = WaitForKey();
    WriteStdOutNL();
    if( result == 'q' || result == 'Q' ) {
        Ignite();
        Suicide();
    }
}

#define HELPLINE_SIZE   80

static void Crash( bool check_file )
/**********************************/
{
    char        buff[HELPLINE_SIZE + 1];
    size_t      len;
    f_handle    fp;

    if( check_file ) {
        fp = FindPath( HELP_FILE_NAME, NULL );
        if( fp != NIL_FHANDLE ) {
            WLPrtBanner();
            len = QRead( fp, buff, HELPLINE_SIZE, HELP_FILE_NAME );
            for( ; len != 0 && len != IOERROR; ) {
                buff[len] = '\0';
                WriteStdOut( buff );
                len = QRead( fp, buff, HELPLINE_SIZE, HELP_FILE_NAME );
            }
            QClose( fp, HELP_FILE_NAME );
            Ignite();
            Suicide();
        }
    }
    DisplayOptions();
    Ignite();
    Suicide();
}

static void DoCmdParse( void )
/****************************/
{
    while( !GetToken( SEP_END, TOK_INCLUDE_DOT ) ) {
        if( !DoParseDirectiveSuicide() ) {
            Syntax();
        }
        RestoreParser();
    }
}

int DoBuffCmdParse( const char *cmd )
/***********************************/
{
    NewCommandSource( NULL, cmd, COMMANDLINE );
    return( Spawn( DoCmdParse ) );
}

void FreePaths( void )
/*********************
 * Free paths & filenames.
 */
{
    FreeList( ObjPath );
    ObjPath = NULL;
    if( Name != NULL ) {
        _LnkFree( Name );
        Name = NULL;
    }
}

void Burn( void )
/****************
 * necessary to split this out from Ignite() for the workframe options
 * processor.
 */
{
    FreePaths();
    if( MapFName != NULL ) {
        _LnkFree( MapFName );
        MapFName = NULL;
    }
    FreeOutFiles();
    BurnUtils();
}

void Ignite( void )
/******************
 * free local structures
 */
{
    BurnSystemList();
    Burn();
}

static void Help( void )
/***********************
 * display help, optionally allowing the user to specifiy the format he/she
 * wants the help for.
 */
{
    EatWhite();
    if( *Token.next == '?' ) {
        Crash( false );
    } else if( *Token.next == '\0' || !DoHelp() ) {
        Crash( true );
    } else {
        Ignite();
        Suicide();
    }
}

void DoCmdFile( const char *fname )
/**********************************
 * start parsing the command
 */
{
    exe_format  possible;
    f_handle    file;
    size_t      namelen;
    file_defext extension;
    const char  *namelnk;

    ResetCmdFile();
    if( fname == NULL || *fname == '\0' ) {
        NewCommandSource( NULL, NULL, COMMANDLINE );
    } else {
        NewCommandSource( NULL, fname, ENVIRONMENT );
    }
    if( IsStdOutConsole() ) {
        CmdFlags |= CF_TO_STDOUT;
    }
    if( sysHelp() ) {
        Help();
    }
    if( *Token.next == '\0' ) {     // go into interactive mode.
        Token.how = INTERACTIVE;
        Token.where = ENDOFLINE;
        LnkMsg( INF+MSG_PRESS_CTRL_Z, NULL );
    }
    file = NIL_FHANDLE;
    namelnk = GetEnvString( INIT_FILE_ENV );
    if( namelnk != NULL ) {
        file = FindPath( namelnk, NULL );
    }
    if( file == NIL_FHANDLE ) {
        namelnk = INIT_FILE_NAME;
        file = FindPath( namelnk, NULL );
    }
    if( file != NIL_FHANDLE ) {
        SetCommandFile( file, namelnk );
    }
    if( Spawn( DoCmdParse ) ) {
        Ignite();
        Suicide();
    }
    GetExtraCommands();
    if( (LinkState & LS_FMT_DECIDED) == 0 ) {
        /* restrict set to automatically decided ones */
#if defined( __QNX__ )
#define LAST_CHANCE ( MK_OS2_LX | MK_OS2_LE | MK_OS2_NE | MK_QNX )
#elif defined( __LINUX__ )
#define LAST_CHANCE ( MK_OS2_LX | MK_OS2_LE | MK_OS2_NE | MK_ELF )
#elif defined( __BSD__ )
#define LAST_CHANCE ( MK_OS2_LX | MK_OS2_LE | MK_OS2_NE | MK_ELF )
#elif defined( __NT__ )
#define LAST_CHANCE ( MK_OS2_LX             | MK_OS2_NE | MK_WINDOWS | MK_PE | MK_DOS_EXE | MK_WIN_VXD )
#elif defined( __RDOS__ )
#define LAST_CHANCE ( MK_RDOS | MK_PE | MK_DOS_EXE)
#else
#define LAST_CHANCE ( MK_OS2_LX | MK_OS2_LE | MK_OS2_NE | MK_DOS_EXE | MK_PHAR_SIMPLE )
#endif
        HintFormat( LAST_CHANCE );
    } else {
        /* restrict to a unique type */
        for( possible = 1; possible != 0; possible *= 2 ) {
            if( FmtData.type & possible ) {
                FmtData.type = possible;
                break;
            }
        }
    }
    if( (FmtData.type & (MK_NOVELL | MK_DOS)) && (LinkFlags & LF_INC_LINK_FLAG) ) {
        LnkMsg( FTL+MSG_FORMAT_BAD_OPTION, "s", "incremental" );
    }
#ifdef _NOVELL
    if( FmtData.type & MK_NOVELL ) {
        CmdNovFini();
    }
#endif
#ifdef _DOS16M
    if( FmtData.type & MK_DOS16M ) {
        CmdD16MFini();
    }
#endif
#ifdef _EXE
    if( FmtData.type & MK_OVERLAYS ) {
        CmdOvlFini();
        AddObjLib( "wovl.lib", LIB_PRIORITY_MIN );     // add a reference to wovl.lib
    }
#endif
    if( Name == NULL || (CmdFlags & CF_HAVE_FILES) == 0 ) {
        Ignite();
        LnkMsg( FTL+MSG_NO_FILES_FOUND, NULL );
    }
    namelen = strlen( Name );
    if( MapFlags & MAP_FLAG ) {
        if( MapFName == NULL ) {
            MapFName = FileName( Name, namelen, E_MAP, true );
        }
    } else {
        MapFlags = 0;   // if main isn't set, don't set anything.
    }
    if( SymFileName == NULL && ( (CmdFlags & CF_SEPARATE_SYM) ||
                   (LinkFlags & LF_OLD_DBI_FLAG) && (FmtData.type & MK_COM) ) ) {
        SymFileName = FileName( Name, namelen, E_SYM, true );
    }
    if( FmtData.make_implib && FmtData.implibname == NULL ) {
        if( FmtData.make_impfile ) {
            extension = E_LBC;
        } else {
            extension = E_LIBRARY;
        }
        FmtData.implibname = FileName( Name, namelen, extension, true );
    }
    CheckTraces();
    BurnUtils();
    PruneSystemList();
#ifdef _EXE
    if( FmtData.type & MK_OVERLAYS ) {
        OvlNumberSections();
    }
#endif
    DBIInit();
}

void SetFormat( void )
/*********************
 * do final processing now that the executable format has been decided.
 */
{
    char        *fname;

    if( CmdFlags & CF_NO_EXTENSION ) {
        fname = Name;
    } else {
#ifdef _RAW
        if( FmtData.output_hex ) {  // override default extension if hex or raw (bin)
            Extension = E_HEX;      //   has been specified
        } else if( FmtData.output_raw ) {
            Extension = E_BIN;
        }
#endif
        fname = FileName( Name, strlen( Name ), Extension, CmdFlags & CF_UNNAMED );
        _LnkFree( Name );
    }
    Root->outfile = NewOutFile( fname );
    Name = NULL;
#ifdef _EXE
    if( FmtData.type & MK_OVERLAYS ) {
        OvlFillOutFilePtrs();       // fill in all unspecified outfile pointers.
    }
#endif
    if( MapFlags & MAP_FLAG ) {
        LnkMsg( MAP+MSG_EXE_NAME, "s", Root->outfile->fname );
        LnkMsg( MAP+MSG_CREATE_EXE, "f" );
    }
#ifdef _QNX
    if( FmtData.type & MK_QNX ) {
        CmdQNXFini();
    }
#endif
    SetRelocSize();
}

void AddFmtLibPaths( void )
/*************************/
{
    const select_format     *check;
    exe_format              possible;

    if( (LinkState & LS_FMT_DECIDED) == 0 )
        return;
    for( check = PossibleFmt; (possible = check->bits) != 0; ++check ) {
        if( (~possible & FmtData.type) == 0 ) {
            break;
        }
    }
    if( possible != 0 ) {
        AddLibPathsToEnd( GetEnvString( check->lib_var_name ) );
    }
}

static void InitFmt( void (*set)(void) )
/**************************************/
{
    if( LinkState & LS_FMT_INITIALIZED )
        return;
    if( set != NULL )
        set();
    LinkState |= LS_FMT_INITIALIZED;
}

bool HintFormat( exe_format hint )
/********************************/
{
    const select_format     *check;
    exe_format              possible;

    if( (hint & FmtData.type) == 0 )
        return( false );
    FmtData.type &= hint;
    if( LinkState & LS_FMT_DECIDED )
        return( true );

    for( check = PossibleFmt; (possible = check->bits) != 0; ++check ) {
        if( (~possible & FmtData.type) == 0 ) {
            break;
        }
    }
    if( possible == 0 ) {
#ifdef _OS2
        if( (~(MK_OS2 | MK_PE | MK_WIN_VXD) & FmtData.type) == 0 ) {
            /* Windows, OS/2 V1.x, OS/2 V2.x, PE, VxD all
                want the same structure */
            InitFmt( SetOS2Fmt );
        }
#endif
        return( true );
    }
    InitFmt( check->set_func );
    LinkState |= LS_FMT_DECIDED;
    if( LinkState & LS_SEARCHING_LIBRARIES )
        AddFmtLibPaths();
    return( true );
}

void DecideFormat( void )
/***********************/
{
    exe_format  possible;
    exe_format  allowed;
    char        rc_buff[RESOURCE_MAX_SIZE];

    if( (LinkState & LS_FMT_DECIDED) == 0 ) {
        possible = FmtData.type;
        allowed = MK_OS2_NE | MK_OS2_LX;
        if( (LinkState & LS_FMT_SEEN_IMPORT_CMT) == 0 )
            allowed = ~allowed;
        if( (possible & allowed) != 0 )
            possible &= allowed;
        HintFormat( possible );
        if( (LinkState & LS_FMT_DECIDED) == 0 ) {
            Msg_Get( MSG_FORMAT_NOT_DECIDED, rc_buff );
            LnkMsg( FTL+MSG_INTERNAL, "s", rc_buff );
        }
    }
}

void FreeFormatStuff( void )
/**************************/
{
    const select_format     *check;
    exe_format              possible;

    if( (LinkState & LS_FMT_DECIDED) == 0 )
        return;
    for( check = PossibleFmt; (possible = check->bits) != 0; ++check ) {
        if( (~possible & FmtData.type) == 0 ) {
            break;
        }
    }
    if( possible != 0 && check->free_func != NULL ) {
        check->free_func();
    }
}

void AddCommentLib( const char *comment, size_t len, lib_priority priority )
/***************************************************************************
 * Add a library from a comment record.
 */
{
    file_list   *result;
    char        *ptr;

    if( CmdFlags & CF_NO_DEF_LIBS )
        return;
    ptr = FileName( comment, len, E_LIBRARY, false );
    result = AddObjLib( ptr, priority );
    CheckLibTrace( result );
    DEBUG(( DBG_BASE, "library: %s", ptr ));
    _LnkFree( ptr );
}

void AddLibPaths( const char *path_list, size_t len, bool add_to_front )
/**********************************************************************/
{
    path_entry      *newpath;
    file_list const *libfiles;
    char            *p;
    const char      *end;

    _ChkAlloc( newpath, sizeof( path_entry ) + len );
    end = path_list + len;
    p = newpath->name;
    while( path_list != end ) {
        if( p != newpath->name )
            *p++ = PATH_LIST_SEP;
        path_list = GetPathElement( path_list, end, &p );
    }
    *p = '\0';
    if( add_to_front ) {
        newpath->next = UsrLibPath;
        UsrLibPath = newpath;
    } else {
        LinkList( &UsrLibPath, newpath );
    }
    if( UsrLibPath == newpath ) {
        for( libfiles = ObjLibFiles; libfiles != NULL; libfiles = libfiles->next_file ) {
            libfiles->infile->path_list = UsrLibPath;
        }
        for( libfiles = Root->files; libfiles != NULL; libfiles = libfiles->next_file ) {
            if( libfiles->infile->status & INSTAT_USE_LIBPATH ) {
                libfiles->infile->path_list = UsrLibPath;
            }
        }
    }
}

void AddLibPathsToEnd( const char *path_list )
/********************************************/
{
    if( path_list != NULL && *path_list != '\0' ) {
        AddLibPaths( path_list, strlen( path_list ), false );
    }
}

void AddLibPathsToEndList( const char *path_list )
/************************************************/
{
    size_t          len;
    path_entry      *newpath;

    if( path_list != NULL && *path_list != '\0' ) {
        len = strlen( path_list );
        _ChkAlloc( newpath, sizeof( path_entry ) + len );
        memcpy( newpath->name, path_list, len + 1 );
        LinkList( &UsrLibPath, newpath );
    }
}

void ExecSystem( const char *name )
/**********************************
 * run a system block with the given name (only called once!)
 * (this is called after the parser has already been stopped
 */
{
    sysblock    *sys;

    sys = FindSysBlock( name );
    if( sys != NULL ) {
        NewCommandSource( NULL, NULL, ENVIRONMENT ); // the "command line"
        Token.where = ENDOFCMD;     // nothing on this command line
        NewCommandSource( sys->name, sys->commands, SYSTEM ); // input file
        _LnkFree( sys->name );
        sys->name = NULL;
        while( !GetToken( SEP_END, TOK_INCLUDE_DOT ) ) {
            if( !DoParseDirectiveSubset() ) {
                LnkMsg( LOC+LINE+WRN+MSG_ERROR_IN_SYSTEM_BLOCK, NULL );
                RestoreCmdLine();
                break;
            }
        }
        BurnSystemList();
        BurnUtils();
    }
}

static void CleanSystemList( bool burn )
/***************************************
 * clean up the list of system blocks
 */
{
    sysblock    **sysown;
    sysblock    *sys;
    char        *name;

    for( sysown = &SysBlocks; (sys = *sysown) != NULL; ) {
        name = sys->name;
        if( burn || name == NULL || memcmp( "286", name, 4 ) != 0 && memcmp( "386", name, 4 ) != 0 ) {
            *sysown = sys->next;
            if( name != NULL ) {
                _LnkFree( name );
            }
            _LnkFree( sys );
        } else {
            sysown = &(sys->next);
        }
    }
}

void PruneSystemList( void )
/***************************
 * delete all system blocks except for the "286" and "386" records
 */
{
    CleanSystemList( false );
}

void BurnSystemList( void )
/**************************
 * delete everything in the system list
 */
{
    CleanSystemList( true );
}
