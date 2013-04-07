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
#include "command.h"
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

#ifdef _INT_DEBUG
unsigned int            Debug;
#endif

static void             Crash( bool );
static void             Help( void );
static void             DoCmdParse( void );
static void             DisplayOptions( void );

static bool             ProcDosHelp( void );
static bool             ProcOS2Help( void );
static bool             ProcPharHelp( void );
static bool             ProcNovellHelp( void );
static bool             Proc16MHelp( void );
static bool             ProcQNXHelp( void );
static bool             ProcELFHelp( void );
static bool             ProcWindowsHelp( void );
static bool             ProcWinVxdHelp( void );
static bool             ProcNTHelp( void );
static bool             ProcZdosHelp( void );
static bool             ProcRdosHelp( void );
static bool             ProcRawHelp( void );
static void             WriteHelp( unsigned first_ln, unsigned last_ln, bool prompt );

static  parse_entry   FormatHelp[] = {
    "Dos",          ProcDosHelp,            MK_ALL,     0,
#ifdef _OS2
    "OS2",          ProcOS2Help,            MK_ALL,     0,
    "WINdows",      ProcWindowsHelp,        MK_ALL,     0,
    "VXD",          ProcWinVxdHelp,         MK_ALL,     0,
    "NT",           ProcNTHelp,             MK_ALL,     0,
#endif
#ifdef _PHARLAP
    "PHARlap",      ProcPharHelp,           MK_ALL,     0,
#endif
#ifdef _NOVELL
    "NOVell",       ProcNovellHelp,         MK_ALL,     0,
#endif
#ifdef _DOS16M
    "DOS16M",       Proc16MHelp,            MK_ALL,     0,
#endif
#ifdef _QNXLOAD
    "QNX",          ProcQNXHelp,            MK_ALL,     0,
#endif
#ifdef _ELF
    "ELF",          ProcELFHelp,            MK_ALL,     0,
#endif
#ifdef _ZDOS
    "ZDos",         ProcZdosHelp,           MK_ALL,     0,
#endif
#ifdef _RDOS
    "RDos",         ProcRdosHelp,           MK_ALL,     0,
#endif
#ifdef _RAW
    "Raw",          ProcRawHelp,            MK_ALL,     0,
#endif
    NULL
};

file_defext     Extension;
file_list       **CurrFList;
tok             Token;
commandflag     CmdFlags;
char            *Name;
sysblock        *SysBlocks;
sysblock        *LinkCommands;

static sysblock         *PrevCommand;

#ifdef BOOTSTRAP
#define INIT_FILE_NAME  "bwlink.lnk"
#else
#define INIT_FILE_NAME  "wlink.lnk"
#endif
#define INIT_FILE_ENV   "WLINK_LNK"

void InitCmdFile( void )
/******************************/
{
    PrevCommand = NULL;
}

void SetSegMask(void)
/***************************/
{
   FmtData.SegShift = 16 - FmtData.Hshift;
   FmtData.SegMask = (1 << FmtData.SegShift) - 1;
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
    memset( &FmtData, 0, sizeof( FmtData ) );
    FmtData.base = NO_BASE_SPEC;
    FmtData.objalign = NO_BASE_SPEC;
    FmtData.type = MK_ALL;
    FmtData.def_seg_flags = SEG_LEVEL_3;
    FmtData.output_raw = FALSE;
    FmtData.output_hex = FALSE;
    FmtData.Hshift = 12;   // May want different value for some 32 bit segmented modes
    FmtData.FillChar = 0;  // Default fillchar for segment alignment
    SetSegMask();
    CurrSect = Root;
    CurrFList = &Root->files;
    DBIFlag = 0;        /*  default is only global information */
}

void DoCmdFile( char *fname )
/**********************************/
/* start parsing the command */
{
    exe_format  possible;
    f_handle    file;
    size_t      namelen;
    file_defext extension;
    char        *namelnk;

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
#if defined( __UNIX__ )
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
    namelnk = GetEnvString( INIT_FILE_ENV );
    file = ( namelnk != NULL ) ? SearchPath( namelnk ) : NIL_FHANDLE;
    if( file == NIL_FHANDLE ) {
        namelnk = INIT_FILE_NAME;
        file = SearchPath( namelnk );
    }
    if( file != NIL_FHANDLE ) {
        fname = ChkStrDup( namelnk );
        SetCommandFile( file, fname );
    }
    if( Spawn( DoCmdParse ) ) {
        Ignite();
        Suicide();
    }
    GetExtraCommands();
    if( !(LinkState & FMT_DECIDED) ) {
        /* restrict set to automatically decided ones */
#if defined( __QNX__ )
#define LAST_CHANCE ( MK_OS2_LX | MK_OS2_LE | MK_OS2_NE | MK_QNX )
#elif defined( __LINUX__ )
#define LAST_CHANCE ( MK_OS2_LX | MK_OS2_LE | MK_OS2_NE | MK_ELF )
#elif defined( __NT__ )
#define LAST_CHANCE ( MK_OS2_LX             | MK_OS2_NE | MK_WINDOWS | MK_PE | MK_DOS_EXE | MK_WIN_VXD )
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
    if( (FmtData.type & (MK_NOVELL | MK_DOS)) && (LinkFlags & INC_LINK_FLAG) ) {
        LnkMsg( FTL+MSG_FORMAT_BAD_OPTION, "s", "incremental" );
    }
#ifdef _NOVELL
    if( FmtData.type & MK_NOVELL ) {
        CmdNovFini();
    } else
#endif
    if( FmtData.type & MK_OVERLAYS ) {
        CmdOvlFini();
        AddObjLib( "wovl.lib", LIB_PRIORITY_MIN );     // add a reference to wovl.lib
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
    if( SymFileName == NULL && ( (CmdFlags & CF_SEPARATE_SYM) ||
                   (LinkFlags & OLD_DBI_FLAG) && (FmtData.type & MK_COM) ) ) {
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

char *GetNextLink( void )
/***********************/
{
    char        *cmd;

    cmd = NULL;
    _LnkFree( PrevCommand );
    if( LinkCommands != NULL ) {
        PrevCommand = LinkCommands;
        LinkCommands = LinkCommands->next;
        cmd = PrevCommand->commands;
    }
    return( cmd );
}

void Syntax( void )
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
    unsigned    len;
    f_handle    fp;

    if( check_file ) {
        fp = SearchPath( "wlink.hlp" );
        if( fp != NIL_FHANDLE ) {
            WLPrtBanner();
            for( ; (len = QRead( fp, buff, 80, "wlink.hlp" )) != 0; ) {
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

int DoBuffCmdParse( char *cmd )
/*****************************/
{
    NewCommandSource( NULL, cmd, COMMANDLINE );
    return( Spawn( DoCmdParse ) );
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
#if defined( _QNXLOAD ) && defined( __QNX__ )
    WriteHelp( MSG_QNX_HELP_0, MSG_QNX_HELP_15, isout );
#endif
#ifdef _EXE
    WriteHelp( MSG_DOS_HELP_0, MSG_DOS_HELP_15, isout );
#endif
#ifdef _OS2
    WriteHelp( MSG_OS2_HELP_0, MSG_OS2_HELP_31, isout );
    WriteHelp( MSG_WINDOWS_HELP_0, MSG_WINDOWS_HELP_31, isout );
    WriteHelp( MSG_WIN_VXD_HELP_0, MSG_WIN_VXD_HELP_31, isout );
    WriteHelp( MSG_NT_HELP_0, MSG_NT_HELP_31, isout );
#endif
#ifdef _PHARLAP
    WriteHelp( MSG_PHAR_HELP_0, MSG_PHAR_HELP_15, isout );
#endif
#ifdef _NOVELL
    WriteHelp( MSG_NOVELL_HELP_0, MSG_NOVELL_HELP_31, isout );
#endif
#ifdef _DOS16M
    WriteHelp( MSG_DOS16_HELP_0, MSG_DOS16_HELP_15, isout );
#endif
#if defined( _QNXLOAD ) && !defined( __QNX__ )
    WriteHelp( MSG_QNX_HELP_0, MSG_QNX_HELP_15, isout );
#endif
#ifdef _ELF
    WriteHelp( MSG_ELF_HELP_0, MSG_ELF_HELP_15, isout );
#endif
#ifdef _ZDOS
    WriteHelp( MSG_ZDOS_HELP_0, MSG_ZDOS_HELP_15, isout );
#endif
#ifdef _RDOS
    WriteHelp( MSG_RDOS_HELP_0, MSG_RDOS_HELP_15, isout );
#endif
#ifdef _RAW
    WriteHelp( MSG_RAW_HELP_0, MSG_RAW_HELP_15, isout );
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

static bool ProcWinVxdHelp( void )
/*********************************/
{
    WriteGenHelp();
    WriteHelp( MSG_WIN_VXD_HELP_0, MSG_WIN_VXD_HELP_31,
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
#ifdef _DOS16M
static bool Proc16MHelp( void )
/*****************************/
{
    WriteGenHelp();
    WriteHelp( MSG_DOS16_HELP_0, MSG_DOS16_HELP_15, CmdFlags & CF_TO_STDOUT );
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

#ifdef _ZDOS
static bool ProcZdosHelp( void )
/*****************************/
{
    WriteGenHelp();
    WriteHelp( MSG_ZDOS_HELP_0, MSG_ZDOS_HELP_15, CmdFlags & CF_TO_STDOUT );
    return( TRUE );
}
#endif

#ifdef _RDOS
static bool ProcRdosHelp( void )
/*****************************/
{
    WriteGenHelp();
    WriteHelp( MSG_RDOS_HELP_0, MSG_RDOS_HELP_15, CmdFlags & CF_TO_STDOUT );
    return( TRUE );
}
#endif

#ifdef _RAW
static bool ProcRawHelp( void )
/*****************************/
{
    WriteGenHelp();
    WriteHelp( MSG_RAW_HELP_0, MSG_RAW_HELP_15, CmdFlags & CF_TO_STDOUT );
    return( TRUE );
}
#endif

static void PressKey( void );
static void WriteMsg( char msg_buffer[] );

static void WriteHelp( unsigned first_ln, unsigned last_ln, bool prompt )
/***********************************************************************/
{
    char        msg_buffer[RESOURCE_MAX_SIZE];
    int         previous_null = 0;

    if( prompt ) {
        PressKey();
    }
    for( ; first_ln <= last_ln; first_ln++ ) {
        Msg_Get( (int) first_ln, msg_buffer );
        if( previous_null ) {
            if( msg_buffer[0] != '\0' ) {
                PressKey();
                WriteMsg( msg_buffer );
                previous_null = 0;
            } else {
                break;
            }
        } else if( msg_buffer[0] == '\0' ) {
            previous_null = 1;
        } else {
            WriteMsg( msg_buffer );
        }
    }
}

static void PressKey( void )
/**************************/
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

static void WriteMsg( char msg_buffer[] )
/***************************************/
{
    WriteStdOut( msg_buffer );
    WriteStdOutNL();
}

void FreePaths( void )
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

void Burn( void )
/**********************/
// necessary to split this out from Ignite() for the workframe options
// processor.
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
/************************/
/* free local structures */
{
    BurnSystemList();
    Burn();
}

void SetFormat( void )
/***************************/
// do final processing now that the executable format has been decided.
{
    char        *fname;

    if( CmdFlags & CF_NO_EXTENSION ) {
        fname = Name;
    } else {
        unsigned    len = strlen( Name );

        if( FmtData.output_hex ) {  // override default extension if hex or raw (bin)
            Extension = E_HEX;       //   has been specified
        } else if( FmtData.output_raw ) {
            Extension = E_BIN;
        }
        fname = FileName( Name, len, Extension, CmdFlags & CF_UNNAMED);
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
#ifdef _DOS16M
    MK_DOS16M,      "LIBDOS16M",    SetD16MFmt,     FreeD16MFmt,
#endif
#ifdef _QNXLOAD
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
    0,              NULL,           NULL,           NULL
};


void AddFmtLibPaths( void )
/********************************/
{
    struct select_format const *check;
    exe_format                  possible;

    if( !(LinkState & FMT_DECIDED) )
        return;
    for( check = PossibleFmt; (possible = check->bits) != 0; ++check ) {
        if( (~possible & FmtData.type) == 0 ) {
            break;
        }
    }
    if( possible != 0 ) {
        AddEnvPaths( check->lib_var_name );
    }
}

static void InitFmt( void (*set)(void) )
/**************************************/
{
    if( LinkState & FMT_INITIALIZED )
        return;
    if( set != NULL )
        set();
    LinkState |= FMT_INITIALIZED;
}

bool HintFormat( exe_format hint )
/***************************************/
{
    struct select_format const *check;
    exe_format                  possible;

    if( !(hint & FmtData.type) )
        return( FALSE );
    FmtData.type &= hint;
    if( LinkState & FMT_DECIDED )
        return( TRUE );

    for( check = PossibleFmt; (possible = check->bits) != 0; ++check ) {
        if( (~possible & FmtData.type) == 0 ) {
            break;
        }
    }
    if( possible == 0 ) {
#ifdef _OS2
        if( (~(MK_OS2|MK_PE|MK_WIN_VXD) & FmtData.type) == 0 ) {
            /* Windows, OS/2 V1.x, OS/2 V2.x, PE, VxD all
                want the same structure */
            InitFmt( SetOS2Fmt );
        }
#endif
        return( TRUE );
    }
    InitFmt( check->set_func );
    LinkState |= FMT_DECIDED;
    if( LinkState & SEARCHING_LIBRARIES )
        AddFmtLibPaths();
    return( TRUE );
}

void DecideFormat( void )
/******************************/
{
    exe_format  possible;
    exe_format  allowed;
    char        rc_buff[RESOURCE_MAX_SIZE];

    if( !(LinkState & FMT_DECIDED) ) {
        possible = FmtData.type;
        allowed = MK_OS2_NE | MK_OS2_LX;
        if( !(LinkState & FMT_SEEN_IMPORT_CMT) )
            allowed = ~allowed;
        if( (possible & allowed) != 0 )
            possible &= allowed;
        HintFormat( possible );
        if( !(LinkState & FMT_DECIDED) ) {
            Msg_Get( MSG_FORMAT_NOT_DECIDED, rc_buff );
            LnkMsg( FTL+MSG_INTERNAL, "s", rc_buff );
        }
    }
}

void FreeFormatStuff( void )
/***************************/
{
    struct select_format const *check;
    exe_format                  possible;

    if( !(LinkState & FMT_DECIDED) )
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

void AddCommentLib( char *ptr, unsigned len, lib_priority priority )
/*********************************************************************/
//  Add a library from a comment record.
{
    file_list   *result;

    if( CmdFlags & CF_NO_DEF_LIBS )
        return;
    ptr = FileName( ptr, len, E_LIBRARY, FALSE );
    result = AddObjLib( ptr, priority );
    CheckLibTrace( result );
    DEBUG(( DBG_BASE, "library: %s", ptr ));
    _LnkFree( ptr );
}

void AddLibPaths( char *name, unsigned len, bool add_to_front )
/***************************************************************/
{
    path_entry         *newpath;
    file_list const    *libfiles;

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
        for( libfiles = ObjLibFiles; libfiles != NULL; libfiles = libfiles->next_file ) {
            libfiles->file->path_list = LibPath;
        }
        for( libfiles = Root->files; libfiles != NULL; libfiles = libfiles->next_file ) {
            if( libfiles->file->flags & INSTAT_USE_LIBPATH ) {
                libfiles->file->path_list = LibPath;
            }
        }
    }
}

void AddEnvPaths( char *envname )
/**************************************/
{
    char * const        val = GetEnvString( envname );
    unsigned            len;

    if( val == NULL )
        return;
    len = strlen( val );
    AddLibPaths( val, len, FALSE );
}

void ExecSystem( char *name )
/**********************************/
/* run a system block with the given name (only called once!)
 * (this is called after the parser has already been stopped */
{
    sysblock    *sys;

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
    sysblock    **sys;
    sysblock    *next;
    char        *name;

    sys = &SysBlocks;
    while( *sys != NULL ) {
        name = (*sys)->name;
        if( !check || memcmp( "286", name, 4 ) != 0 && memcmp( "386", name, 4) != 0 ) {
            next = (*sys)->next;
            _LnkFree( name );
            _LnkFree( *sys );
            *sys = next;
        } else {
            sys = &(*sys)->next;
        }
    }
}

void PruneSystemList( void )
/*********************************/
/* delete all system blocks except for the "286" and "386" records */
{
    CleanSystemList( TRUE );
}

void BurnSystemList( void )
/********************************/
/* delete everything in the system list */
{
    CleanSystemList( FALSE );
}

bool ProcImport( void )
/****************************/
{
#if defined( _OS2) || defined( _ELF ) || defined( _NOVELL )
    if( HintFormat( MK_OS2 | MK_PE ) ) {
        return( ProcOS2Import() );
    } else if( HintFormat( MK_WIN_VXD ) ) {
        return( FALSE );
    } else if( HintFormat( MK_ELF ) ) {
        return( ProcELFImport() );
    } else {
        return( ProcNovImport() );
    }
#else
    return( FALSE );
#endif
}

#if defined(_OS2) || defined(_NOVELL)
bool ProcExport( void )
/****************************/
{
#ifdef _OS2
    if( HintFormat( ( MK_OS2 | MK_PE | MK_WIN_VXD ) ) ) {
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

#if defined( _DOS16M ) || defined( _QNXLOAD ) || defined( _OS2 ) || defined( _ELF )
bool ProcNoRelocs( void )
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
#ifdef _DOS16M
    if( HintFormat( MK_DOS16M ) ) {
        return( Proc16MNoRelocs() );
    }
#endif
#if defined( _ELF )
    if( HintFormat( MK_ELF ) ) {
        return( ProcELFNoRelocs() );
    }
#endif
    return( TRUE );
}
#endif

#if defined(_OS2) || defined(_QNXLOAD)
bool ProcSegment( void )
/*****************************/
{
#ifdef _OS2
    if( HintFormat( MK_OS2 | MK_PE | MK_WIN_VXD ) ) {
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

bool ProcAlignment( void )
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

bool ProcHeapSize( void )
/******************************/
{
#if defined( __QNX__ )
    if( HintFormat( MK_QNX ) ) {
        return( ProcQNXHeapSize() );
    }
#endif
#ifdef _OS2
    if( HintFormat( MK_OS2 | MK_PE ) ) {
        return( ProcOS2HeapSize() );
    }
#endif
#if defined( _QNXLOAD ) && !defined( __QNX__ )
    if( HintFormat( MK_QNX ) ) {
        return( ProcQNXHeapSize() );
    }
#endif
    return( TRUE );
}

#if defined(_PHARLAP) || defined(_QNXLOAD) || defined(_OS2) || defined(_RAW)
bool ProcOffset( void )
/****************************/
{
    if( !GetLong( &FmtData.base ) )
        return( FALSE );
    if( !(FmtData.type & (MK_PHAR_LAP|MK_QNX_FLAT|MK_RAW)) ) {
        ChkBase( 64 * 1024 );
    } else if( !(FmtData.type & (MK_OS2_FLAT|MK_PE)) ) {
        ChkBase( 4 * 1024 );
    }
    return( TRUE );
}
#endif

#ifdef _INT_DEBUG
bool ProcXDbg( void )
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

bool ProcIntDbg( void )
/****************************/
{
    LinkState |= INTERNAL_DEBUG;
    return( TRUE );
}
#endif
