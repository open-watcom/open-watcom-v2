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
* Description:  Remote File eXchange utility.
*
****************************************************************************/


#include <stdlib.h>
#include <malloc.h>
#include <conio.h>
#include <ctype.h>
#include <process.h>
#include <dos.h>
#include "dbgdefn.h"
#include "rfxdata.h"
#include "fparse.h"
#include "banner.h"
#include "dbgio.h"
#include "dta.h"
#include "tcerr.h"
#include "trprfx.h"
//#include "strutil.h"

#include "local.h"

char _Literal_No_Mem_4_Path[] = { "no memory for PATH" };

enum {
    IO_OK,
    IO_EOF,
    IO_FILE_NOT_FOUND,
    IO_PATH_NOT_FOUND,
    IO_NO_HANDLES,
    IO_NO_ACCESS,
    IO_BAD_HANDLE,
    IO_BAD_MEM,
    IO_NO_MEM,
    IO_BAD_ADDR,
    IO_BAD_ENV,
    IO_BAD_FORMAT,
    IO_BAD_ACCESS,
    IO_BAD_DATA,
    IO_EOR,
    IO_BAD_DRIVE,
    IO_BAD_RMDIR,
    IO_DIFF_DEV,
    IO_NO_FILES,
    IO_CANT_COPY_TO_SELF,
    IO_DUP_OR_NOT_FOUND,
    IO_INTERRUPT,
    IO_DISK_FULL,
    IO_FIND_ERROR          = 2,
    IO_FIND_NO_MORE_FILES  = 18,
};

#define IO_SUBDIRECTORY 0x10
#define BUFF_LEN        4096

typedef struct copyspec {
        struct copyspec *next;
        char            *src;
        int             src_loc;
        char            *dst;
        int             dst_loc;
} COPYSPEC, *COPYPTR;

extern  void            SysFileInit( void );
extern  void            PathInit( void );
extern  bool            InitTrap( const char * );
extern  bool            InitFileSupp( void );
extern  bool            InitRFXSupp( void );
extern  void            FiniTrap( void );
extern  void            InitInt( void );
extern  void            FiniInt( void );
extern  int             CtrlCHit( void );
extern  char            *RealFName( char *, open_access * );
extern  sys_error       GetSystemErrCode( error_idx );
extern  unsigned        RemoteWriteConsole( void *, unsigned );
extern  unsigned        RemoteWriteConsoleNL( void );
extern  error_idx       GetLastErr( void );
extern  sys_handle      GetSystemHandle( handle );

extern  char            *_FileParse( char *name, file_parse *file );
extern  char            *Squish( file_parse *parse, char *into );

extern rc_erridx        RemoteRename( const char *, const char * );
extern rc_erridx        RemoteMkDir( const char * );
extern rc_erridx        RemoteRmDir( const char * );
extern rc_erridx        RemoteSetDrv( int );
extern int              RemoteGetDrv( void );
extern rc_erridx        RemoteSetCWD( const char * );
extern long             RemoteGetFileAttr( const char * );
extern rc_erridx        RemoteSetFileAttr( const char * , long );
extern long             RemoteGetFreeSpace( int );
extern rc_erridx        RemoteDateTime( sys_handle , int *, int *, int );
extern rc_erridx        RemoteGetCwd( int, char * );
extern rc_erridx        RemoteFindFirst( const char *, void *, unsigned , int );
extern int              RemoteFindNext( void *, unsigned );
extern rc_erridx        RemoteFindClose( void );

dbg_switches            DbgSwitches;
char                    *TxtBuff;
char                    Buff[ BUFF_LEN ];
char                    NullStr[] = { '\0' };
int                     MaxOnLine = { 0 };
int                     Typing = { 0 };
error_idx               ErrorStatus = { 0 };
int                     DefaultLocation = { 0 };
static  file_parse      Parse1;
static  file_parse      Parse2;
static  file_parse      Parse3;
static  char            Name1[MAX_DRIVE+MAX_PATH+MAX_NAME+MAX_EXT+2];
static  char            Name2[MAX_DRIVE+MAX_PATH+MAX_NAME+MAX_EXT+2];
static  char            Name3[MAX_DRIVE+MAX_PATH+MAX_NAME+MAX_EXT+2];
static  int             FilesCopied;
static  int             DirectoriesMade;
trap_dta                Info;
system_config           SysConfig;


COPYPTR CopySpecs;

#define REAL_CODE( err ) (GetSystemErrCode(err)&0xffff)

const char *HelpText[] = {
"",
"Commands which accept special file names are:",
"",
"      copy    [/s] src_spec [dst_spec] [/s] - copy files (/s=recursive)",
"      dir     [/w] dir_spec [/w]            - directory (/w=wide)",
"      cd      dir_spec                      - set current directory",
"      chdir   dir_spec                      - set current directory",
"      md      dir_spec                      - create directory",
"      mkdir   dir_spec                      - create directory",
"      rd      [/s] dir_spec [/s]            - delete directory",
"      rmdir   [/s] dir_spec [/s]            - delete directory",
"      type    dir_spec                      - type a file",
"      del     [/s] file_spec [/s]           - erase files (/s= recursive)",
"      erase   [/s] file_spec [/s]           - erase files (/s= recursive)",
"      ren     src_spec dst_spec             - rename files",
"      drive:                                - set drive and XXLcle",
"      exit                                  - leave rfx",
"",
" Special file names are the same as DOS file names except",
" they may be preceded by a XXLcle specifier of the form:",
"      @[XXLcle]",
"      - where: XXLcle = 'l' for XXLcl machine, 'r' for XXRem machine",
"      - if a file name starts with an '@', then double it e.g. '@@file'",
" Note:",
"      If [dst_spec] is omitted from the copy command, it defaults to the",
"      current directory on the machine NOT specified by [src_spec]",
NULL
};

char * ErrMessages[] = {
    "",
    "End of file encountered",
    "File not found",
    "Path not found",
    "Too many open files",
    "Access denied",
    "Invalid operation",
    "Memory control blocks destroyed",
    "Insufficient memory",
    "Invalid memory block address",
    "Invalid environment",
    "Invalid format",
    "Invalid access code",
    "Invalid data",
    "Record truncated",
    "Invalid drive",
    "Attempt to remove current directory",
    "Not same device",
    "No more files",
    "File cannot be copied to itself",
    "Duplicate file name or file not found",
    "Interrupted from keyboard",
    "Disk is full",

/* Start of Critical error messages */

    "Attempt to write on write-protected diskette",
    "Unknown unit",
    "Device not ready",
    "Unknown command",
    "Data error (CRC)",
    "Bad request structure length",
    "Seek error",
    "Unknown media type",
    "Sector not found",
    "Printer out of paper",
    "Write fault",
    "Read fault",
    "General failure"
};


static char * Day[] = {
        "Sun  ",
        "Mon  ",
        "Tue  ",
        "Wed  ",
        "Thu  ",
        "Fri  ",
        "Sat  "
        };


/* Forward declarations */
extern  void    Replace( const char *frum, const char *to, char *into );
extern  void    FinishName( char *fn, file_parse *parse, int loc, int addext );
extern  int     GetFreeSpace( dir_handle *h, int loc );
extern  void    CopyStrMax( const char *src, char *dst, unsigned max_len );

void    FreeCopySpec( COPYPTR junk );
void    ProcCD( int argc, char **argv, int crlf );
int     ProcessCmd( const char * cmd );
int     ProcessArgv( int argc, char **argv, const char *cmd );
void    ProcCopy( int argc, char **argv );
void    ProcDir( int argc, char **argv );
void    ProcMakeDir( int argc, char **argv );
void    ProcErase( int argc, char **argv );
void    ProcDelDir( int argc, char **argv );
void    ProcRename( int argc, char **argv );
void    ProcType( int argc, char **argv );
int     ProcDrive( int argc, char **argv );

static void    FormatDTA( char *buff, trap_dta *dir, bool wide );

/**************************************************************************/
/* UTILITIES                                                              */
/**************************************************************************/

static void Help( void )
{
    const char  **txts;
    const char  *txt;

    for( txts = HelpText; (txt = *txts) != NULL; ++txts ) {
        WriteText( STD_ERR, txt, strlen( txt ) );
    }
}


static void WhatDidYouSay( void )
{
#define whadjasay "Not understood: Type '?' for help"
    WriteText( STD_ERR, whadjasay, sizeof( whadjasay ) - 1 );
}


static void Error( const char *err )
{
    WriteText( STD_ERR, err, strlen( err ) );
}


static void Usage( void )
{
    Error( "Usage: rfx trap_file[;trap_parm] [rfx_command]" );
}

char *StrCopy( const char *src, char *dest )
{
    while( (*dest = *src++) != 0 ) {
        ++dest;
    }
    return( dest );
}

static char *RealRFXName( char *name, int *loc )
{
    open_access op;

    name = RealFName( name, &op );
    if( op & OP_LOCAL ) {
        *loc = -1;
    } else if( op & OP_REMOTE ) {
        *loc = 1;
    } else {
        *loc = 0;
    }
    return( name );
}

static open_access RFX2Acc( int loc )
{
    if( loc == 0 )
        loc = DefaultLocation;
    if( loc < 0 )
        return( OP_LOCAL );
    return( OP_REMOTE );
}

static char *RealName( char *name, int * loc )
{
    name = RealRFXName( name, loc );
    if( *loc == 0 ) {
        *loc = DefaultLocation;
    }
    if( *name == '\0' )
        return( "." );
    return( name );
}

void StartupErr( const char *err )
{
    Error( err );
    exit( 2 );
}

void RestoreHandlers( void )
{
}

void GrabHandlers( void )
{
}

void CheckError( void )
{
    char    *p;
    char    buff[ 256 ];

    if( ErrorStatus != 0 ) {
        p = SysErrMsg( ErrorStatus, buff );
        WriteText( STD_ERR, buff, p - buff );
    }
    while( CopySpecs ) {
        FreeCopySpec( CopySpecs );
    }
}

void *DbgAlloc( size_t size )
{
    return( malloc( size ) );
}

void DbgFree( void * chunk )
{
    free( chunk );
}

void FreeRing( void )
{
}

error_idx TransSetErr( error_idx err )
{
    ErrorStatus = err;
    return( err );
}

error_idx SysSetErr( sys_error err )
{
    return( TransSetErr( StashErrCode( err, OP_LOCAL ) ) );
}

char *MyStrDup( const char *str ) {

    char *new;

    new = DbgAlloc( strlen( str ) + 1 );
    strcpy( new, str );
    return( new );
}

extern  char    *Copy( const void *s, void *d, unsigned len ) {

    char        *dst = d;
    const char  *src = s;

    while( len-- > 0 ) {
        *dst++ = *src++;
    }
    return( dst );
}

extern  char    *Fill( void *d, int len, char filler ) {

    char *dst = d;
    while( len-- ) {
        *dst++ = filler;
    }
    return( dst );
}

extern char *CopyStr( const char *src, char *dst )
{
    while( (*dst = *src) != '\0' ) {
        ++src;
        ++dst;
    }
    return( dst );
}

extern  void    DItoD( long s, char *d ) {

    if( s == 0 ) {
        *d = '0';
    } else {
        do {
            *d-- = s % 10 + '0';
            s /= 10;
        } while( s != 0 );
    }
}

extern  void    ItoD( unsigned int i, char *b ) {

    b[ 1 ] = i % 10 + '0';
    i /= 10;
    b[ 0 ] = i % 10 + '0';
}


/**************************************************************************/
/* ACTUAL OS CALLS                                                        */
/**************************************************************************/

char GetDrv( int loc )
/********************/
{
    if( loc == 1 ) {
        return( RemoteGetDrv() + 'A' );
    } else {
        return( LocalGetDrv() + 'A' );
    }
}

void SetDrv( int drive, int loc )
/*******************************/
{
    drive = toupper( drive );
    if( loc == 1 ) {
        RemoteSetDrv( drive - 'A' );
    } else {
        LocalSetDrv( drive - 'A' );
    }
    if( GetDrv( loc ) != drive ) {
        SysSetErr( IO_BAD_DRIVE );
    }
}

rc_erridx RemoveDir( const char *name, int loc )
/**********************************************/
{
    if( loc == 1 ) {
        return( RemoteRmDir( name ) );
    } else {
        return( LocalRmDir( name ) );
    }
}

rc_erridx SetDir( const char *name, int loc )
/*******************************************/
{
    if( loc == 1 ) {
        return( RemoteSetCWD( name ) );
    } else {
        return( LocalSetCWD( name ) );
    }
}

rc_erridx GetDir( int drive, char *name, int loc )
/************************************************/
{
    /* drive=0 means current drive A:=1, B:=2, etc. */
    if( loc == 1 ) {
        return( RemoteGetCwd( drive, name ) );
    } else {
        return( LocalGetCwd( drive, name ) );
    }
}

rc_erridx Erase( const char *name, int loc )
/******************************************/
{
    return( FileRemove( name, RFX2Acc( loc ) ) );
}

rc_erridx MakeDir( const char *name, int loc )
/********************************************/
{
    if( loc == 1 ) {
        return( RemoteMkDir( name ) );
    } else {
        return( LocalMkDir( name ) );
    }
}


long GetAttrs( const char *fn, int loc )
/**************************************/
{
    if( loc == 1 ) {
        return( RemoteGetFileAttr( fn ) );
    } else {
        return( LocalGetFileAttr( fn ) );
    }
}

int IsDevice( const char *fn, int loc )
/*************************************/
{
    unsigned rc;

    if( GetAttrs( fn, loc ) >= 0 )
        return( FALSE );
    rc = FileOpen( fn, OP_READ | RFX2Acc( loc ) );
    if( rc == NIL_HANDLE )
        return( FALSE );
    FileClose( rc );
    return( TRUE );
}


rc_erridx FindFirst( const char *name, int loc, int attr )
/********************************************************/
{
    if( loc == 1 ) {
        return( RemoteFindFirst( name, &Info, sizeof( Info ), attr ) );
    } else {
        return( LocalFindFirst( name, &Info, sizeof( Info ), attr ) );
    }
}

int FindNext( int loc )
/*********************/
{
    if( loc == 1 ) {
        return( RemoteFindNext( &Info, sizeof( Info ) ) );
    } else {
        return( LocalFindNext( &Info, sizeof( Info ) ) );
    }
}

rc_erridx Rename( const char *f1, const char *f2, int loc )
/*********************************************************/
{
    if( loc == 1 ) {
        return( RemoteRename( f1, f2 ) );
    } else {
        return( LocalRename( f1, f2 ) );
    }
}

long FreeSpace( char drive, int loc )
/***********************************/
{
    if( loc == 1 ) {
        return( RemoteGetFreeSpace( drive ) );
    } else {
        return( LocalGetFreeSpace( drive ) );
    }
}

void SameDate( handle src, int src_loc, handle dst, int dst_loc )
/***************************************************************/
{
    int     time, date;

    if( src_loc == 1 ) {
        RemoteDateTime( GetSystemHandle( src ), &time, &date, 0 );
    } else {
        LocalDateTime( GetSystemHandle( src ), &time, &date, 0 );
    }
    if( dst_loc == 1 ) {
        RemoteDateTime( GetSystemHandle( dst ), &time, &date, 1 );
    } else {
        LocalDateTime( GetSystemHandle( dst ), &time, &date, 1 );
    }
}


/**************************************************************************/
/* MAIN LINEISH                                                           */
/**************************************************************************/

static void OutName( void )
{
    static char Name[] = { "[RFX] " };

    WriteStream( STD_ERR, Name, sizeof( Name ) - 1 );
}

void Prompt( void )
{
    char        *prompt;
    char        drv;
    int         hour, min, sec, hundredths;
    int         year, month, day, weekday;

    prompt = getenv( "PROMPT" );
    if( !prompt )
        prompt = "$n$g";
    OutName();
    while( *prompt ) {
        if( *prompt == '$' ) {
            ++prompt;
            switch( *prompt ) {
            case '$':
                WriteStream( STD_ERR, "$", 1 );
                break;
            case 't':
                LocalTime( &hour, &min, &sec, &hundredths );
                ItoD( hour, Buff+0 );
                Buff[2] = ':';
                ItoD( min, Buff+3 );
                Buff[5] = ':';
                ItoD( sec, Buff+6 );
                Buff[8] = '.';
                ItoD( hundredths, Buff+9 );
                WriteStream( STD_ERR, Buff, 11 );
                break;
            case 'd':
                LocalDate( &year, &month, &day, &weekday );
                strcpy( Buff, Day[ weekday ] );
                ItoD( month, Buff+5 );
                Buff[7] = '-';
                ItoD( day, Buff+8 );
                Buff[10] = '-';
                DItoD( year, Buff + 14 );
                WriteStream( STD_ERR, Buff, 15 );
                break;
            case 'p':
                ProcCD( 0, NULL, FALSE );
                break;
            case 'v':
                #define RFX "RFX Version " _RFX_VERSION_
                WriteStream( STD_ERR, RFX, sizeof( RFX ) - 1 );
                break;
            case 'n':
                drv = GetDrv( DefaultLocation );
                WriteStream( STD_ERR, &drv, 1 );
                break;
            case 'g':
                WriteStream( STD_ERR, ">", 1 );
                break;
            case 'l':
                WriteStream( STD_ERR, "<", 1 );
                break;
            case 'b':
                WriteStream( STD_ERR, "|", 1 );
                break;
            case 'q':
                WriteStream( STD_ERR, "=", 1 );
                break;
            case 'h':
                WriteStream( STD_ERR, "\x7c", 1 );
                break;
            case 'e':
                WriteStream( STD_ERR, "\x1b", 1 );
                break;
            case '_':
                WriteNL( STD_ERR );
                OutName();
                break;
            default:
                break;
            }
        } else {
            WriteStream( STD_ERR, prompt, 1 );
        }
        ++prompt;
    }
}


static void Interactive( void )
{
    char *p;
    int interactive;

    interactive = LocalInteractive( STD_IN );
    if( interactive ) {
        Error( banner1w( "Remote File eXchange program", _RFX_VERSION_ ) );
        Error( banner2 );
        Error( banner2a( "1990" ) );
        Error( banner3 );
        Error( banner3a );
        Error( "" );
    }
    for( ;; ) {
        if( interactive ) {
            Prompt();
            LocalGetBuff( Buff, 254 );
        } else {
            p = Buff;
            for(;;) {
                if( ReadStream( STD_IN, p, 1 ) != 1 )
                    return;
                if( *p == '\n' )
                    break;
                ++p;
            }
            p = Buff;
            while( *p != '\r' )
                ++p;
            *p = '\0';
        }
        CtrlCHit();
        if( ProcessCmd( Buff ) != 0 ) {
            break;
        }
    }
}

int main( int argc, char **argv )
{

    TxtBuff = DbgAlloc( 512 );
    SysFileInit();
    if( argc < 2 || argv[1][0] == '?' ) {
        Usage();
        return( 1 );
    }
    PathInit();
    InitTrap( argv[1] );
    if( !InitFileSupp() || !InitRFXSupp() ) {
        FiniTrap();
        StartupErr( "no remote file system support" );
    }
    InitInt();
    CopySpecs = NULL;
    MaxOnLine = 0;
    DefaultLocation = -1;
    if( argc == 2 ) {
        Interactive();
    } else {
        ErrorStatus = 0;
        ProcessArgv( argc - 2, argv + 2, NULL );
        CheckError();
    }
    FiniTrap();
    FiniInt();
    return( 0 );
}

int Option( const char * str, char opt )
{
    if( *str == '/' || *str == '-' ) {
        if( tolower( str[1] ) == opt ) {
            return( 1 );
        }
    }
    return( 0 );
}

/**************************************************************************/
/* PROCESS COMMANDS                                                       */
/**************************************************************************/

void CopyCmd( const char *src, char *dst )
{
    for( ;; ) {
        if( *src == '/' ) {
            *dst++ = ' ';
        }
        *dst = *src;
        if( *dst == '\0' )
            break;
        ++src; ++dst;
    }
}


int ProcessCmd( const char *cmd ) {

    int         argc;
    char        *argv[20];
    char        *p;
    char        tokens[255];

    CopyCmd( cmd, tokens );
    WriteNL( STD_ERR );
    if( tokens[0] == '\0' )
        return( 0 );
    argc = 0;
    p = strtok( tokens, " " );
    while( p != NULL ) {
        argv[ argc ] = p;
        ++argc;
        p = strtok( NULL, " " );
    }
    if( argc ) {
        return( ProcessArgv( argc, argv, cmd ) );
    } else {
        return( 0 );    /* No useful input. */
    }
}


int ProcessArgv( int argc, char **argv, const char *cmd ) {

    ErrorStatus = 0;
    strlwr( argv[ 0 ] );
    if( strcmp( argv[ 0 ], "copy" ) == 0 ) {
        ProcCopy( argc - 1, argv + 1 );
    } else if( strcmp( argv[ 0 ], "dir" ) == 0 ) {
        ProcDir( argc - 1, argv + 1 );
    } else if( strcmp( argv[ 0 ], "md"    ) == 0
            || strcmp( argv[ 0 ], "mkdir" ) == 0 ) {
        ProcMakeDir( argc - 1, argv + 1 );
    } else if( strcmp( argv[ 0 ], "del"    ) == 0
            || strcmp( argv[ 0 ], "erase" ) == 0 ) {
        ProcErase( argc - 1, argv + 1 );
    } else if( strcmp( argv[ 0 ], "rd"    ) == 0
            || strcmp( argv[ 0 ], "rmdir" ) == 0 ) {
        ProcDelDir( argc - 1, argv + 1 );
    } else if( strcmp( argv[ 0 ], "exit" ) == 0 ) {
        return( -1 );
    } else if( strcmp( argv[ 0 ], "cd"    ) == 0
            || strcmp( argv[ 0 ], "chdir" ) == 0 ) {
        ProcCD( argc - 1, argv + 1, TRUE );
    } else if( strcmp( argv[ 0 ], "ren" ) == 0 ) {
        ProcRename( argc - 1, argv + 1 );
    } else if( strcmp( argv[ 0 ], "type" ) == 0 ) {
        ProcType( argc - 1, argv + 1 );
    } else if( strcmp( argv[ 0 ], "?" ) == 0 ) {
        Help();
    } else if( strcmp( argv[ 0 ], "" ) == 0 ) {
    } else if( ProcDrive( argc, argv ) == 0 ) {
        if( cmd != NULL ) {
            system( cmd );
        }
    }
    CheckError();
    return( 0 );
}

/**************************************************************************/
/* RENAME                                                                 */
/**************************************************************************/

rc_erridx   Renamef( char *fn1, int f1loc, char *fn2, int f2loc )
{
    rc_erridx   retc;
    rc_erridx   err;
    char        *endpath;

    fn1 = _FileParse( fn1, &Parse1 );
    fn2 = _FileParse( fn2, &Parse2 );
    Copy( &Parse1, &Parse3, sizeof( file_parse ) );
    if( Parse1.drive[ 0 ] == '\0' ) {
        Parse1.drive[ 0 ] = GetDrv( f1loc );
        Parse1.drive[ 1 ] = ':';
    }
    if( Parse2.drive[ 0 ] == '\0' ) {
        CopyStr( Parse1.drive, Parse2.drive );
    }
    if( Parse2.path[ 0 ] == '\0' ) {
        CopyStr( Parse1.path, Parse2.path );
    }
    if( f1loc != f2loc ) {
        SysSetErr( IO_DIFF_DEV );
        return( 0 );
    }
    if( toupper( Parse1.drive[ 0 ] ) != toupper( Parse2.drive[ 0 ] ) ) {
        SysSetErr( IO_DIFF_DEV );
        return( 0 );
    }
    if( Parse2.name[ 0 ] == '\0' ) {
        SysSetErr( IO_DUP_OR_NOT_FOUND );
        return( 0 );
    }
    endpath = Squish( &Parse1, Name1 );
    retc = FindFirst( Name1, f1loc, 0 );
    if( retc != 0 ) {
        SysSetErr( IO_FILE_NOT_FOUND );
        return( retc );
    }
    for(;;) {
        if( CtrlCHit() )
            return( IO_INTERRUPT );
        CopyStr( Info.name, endpath );
        _FileParse( Name1, &Parse3 );
        Replace( Parse1.name, Parse2.name, Parse3.name );
        Replace( Parse1.ext, Parse2.ext, Parse3.ext );
        CopyStr( Parse2.path, Parse3.path );
        Squish( &Parse3, Name2 );
        retc = Rename( Name1, Name2, f1loc );
        if( retc != 0 ) {
            err = retc;
            if( REAL_CODE( err ) == IO_FILE_NOT_FOUND ) {
                SysSetErr( IO_DUP_OR_NOT_FOUND );
            } else {
                TransSetErr( err );
            }
            return( retc );
        }
        if( FindNext( f1loc ) != 0 ) {
            break;
        }
    }
    return( retc );
}

void ProcRename( int argc, char **argv )
{
    int         src_loc, dst_loc;
    char        *src, *dst;
    int         i;

    dst = NULL;
    src = NULL;
    for( i = 0; i < argc; ++i ) {
        if( src == NULL ) {
            src = argv[ i ];
        } else if( dst == NULL ) {
            dst = argv[ i ];
        } else {
            WhatDidYouSay();
            return;
        }
    }
    if( src == NULL || dst == NULL ) {
        WhatDidYouSay();
        return;
    }
    src = RealName( src, &src_loc );
    dst = RealRFXName( dst, &dst_loc );
    if( dst_loc == 0 )
        dst_loc = src_loc;
    if( *dst == '\0' )
        dst = ".";
    Renamef( src, src_loc, dst, dst_loc );
}

/**************************************************************************/
/* COPY                                                                   */
/**************************************************************************/

void AddCopySpec( const char *src, const char *dst, int src_loc, int dst_loc )
{
    COPYPTR     new;

    new = DbgAlloc( sizeof( COPYSPEC ) );
    new->next = CopySpecs;
    CopySpecs = new;
    new->src = MyStrDup( src );
    new->src_loc = src_loc;
    new->dst = MyStrDup( dst );
    new->dst_loc = dst_loc;
}

void FreeCopySpec( COPYPTR junk )
{
    COPYPTR     *owner;

    owner = &CopySpecs;
    while( *owner != junk ) {
        owner = &((*owner)->next);
    }
    *owner = junk->next;
    DbgFree( junk->src );
    DbgFree( junk->dst );
    DbgFree( junk );
}

static int HasWildCards( const char * src )
{
    if( strchr( src, '?' ) != NULL )
        return( 1 );
    if( strchr( src, '*' ) != NULL )
        return( 1 );
    return( 0 );
}


static int IsDir( const char *src, int src_loc )
{
    long rc;

    rc = GetAttrs( src, src_loc );
    if( rc < 0 ) {
        return( 0 );
    }
    return( ( rc & IO_SUBDIRECTORY ) != 0 );
}


void WrtCopy( const char *src, const char *dst, int src_loc, int dst_loc )
{
    int         len;

    memset( Buff, ' ', 80 );
    Buff[ 0 ] = '\r';
    len = strlen( src );
    strncpy( Buff + 1, src, len );
    len += 2;
    if( len > MaxOnLine ) {
        MaxOnLine = len;
    }
    Buff[ MaxOnLine ] = '\r';
    if( src_loc == 1 ) {
        RemoteWriteConsole( Buff, MaxOnLine );
    } else if( !Typing ) {
        WriteStream( STD_ERR, Buff, MaxOnLine );
    }
    memset( Buff, ' ', 80 );
    Buff[ 0 ] = '\r';
    len = strlen( dst );
    strncpy( Buff + 1, dst, len );
    len += 2;
    if( len > MaxOnLine ) {
        MaxOnLine = len;
    }
    Buff[ MaxOnLine ] = '\r';
    if( dst_loc == 1 ) {
        RemoteWriteConsole( Buff, MaxOnLine );
    } else if( !Typing ) {
        WriteStream( STD_ERR, Buff, MaxOnLine );
    }
}

void FiniCopy( handle in, const char *src_name, int src_loc,
               handle out, const char *dst_name, int dst_loc )
{
    SameDate( in, src_loc, out, dst_loc );
    FileClose( in );
    FileClose( out );
    if( dst_loc == -1 ) {
        LocalSetFileAttr( dst_name, GetAttrs( src_name, src_loc ) );
    }
    ++FilesCopied;
}


rc_erridx DoCopy( const char *src, const char *dst, int src_loc, int dst_loc )
{
    handle      in, out;
    unsigned    len;
    unsigned    written;
    rc_erridx   retc;

    WrtCopy( src, dst, src_loc, dst_loc );
    in = FileOpen( src, OP_READ | RFX2Acc( src_loc ) );
    if( in == NIL_HANDLE )
        return( IO_FILE_NOT_FOUND );
    out = FileOpen( dst, OP_WRITE | OP_CREATE | RFX2Acc( dst_loc ) );
    if( out == NIL_HANDLE ) {
        FileClose( in );
        return( StashErrCode( IO_NO_ACCESS, OP_LOCAL ) );
    }
    for( ;; ) {
        if( CtrlCHit() ) {
            FiniCopy( in, src, src_loc, out, dst, dst_loc );
            return( StashErrCode( IO_INTERRUPT, OP_LOCAL ) );
        }
        len = ReadStream( in, Buff, BUFF_LEN );
        if( len == ERR_RETURN ) {
            retc = GetLastErr();
            FiniCopy( in, src, src_loc, out, dst, dst_loc );
            return( retc );
        }
        if( len == 0 )
            break;
        written = WriteStream( out, Buff, len );
        if( written == ERR_RETURN ) {
            retc = GetLastErr();
            FiniCopy( in, src, src_loc, out, dst, dst_loc );
            return( retc );
        }
        if( written != len ) {
            if( (written == (len - 1)) && (Buff[written] == 0x1A) )
                break;
            FiniCopy( in, src, src_loc, out, dst, dst_loc );
            return( StashErrCode( IO_DISK_FULL, OP_LOCAL ) );
        }
    }
    FiniCopy( in, src, src_loc, out, dst, dst_loc );
    return( StashErrCode( IO_OK, OP_LOCAL ) );
}

extern  void    RRecurse( char *f1, char *f2, int f1loc, int f2loc )
{
    rc_erridx   retc;
    long        retl;
    char        *endptr;
    char        *endpath;
    char        ch;

    f1 = _FileParse( f1, &Parse1 );
    FinishName( f1, &Parse1, f1loc, 1 );
    Copy( &Parse1, &Parse3, sizeof( file_parse ) );
    CopyStr( "*", Parse3.name );
    CopyStr( ".*", Parse3.ext );
    endpath = Squish( &Parse3, Name1 );
    f2 = _FileParse( f2, &Parse2 );
    FinishName( f2, &Parse2, f2loc, 1 );
    retc = FindFirst( Name1, f1loc, IO_SUBDIRECTORY );
    if( retc == 0 ) {
        endpath = Squish( &Parse1, Name1 );
        for(;;) {
            if( Info.attr & IO_SUBDIRECTORY ) {
                if( Info.name[ 0 ] != '.' ) {
                    CopyStr( endpath, Name3 );
                    CopyStr( Name3, CopyStr( "\\", CopyStr( Info.name,endpath ) ) );
                    endpath = Squish( &Parse2, Name2 );
                    CopyStr( endpath, Name3 );
                    endptr = CopyStr( Info.name, endpath );
                    CopyStr( Name3, CopyStr( "\\", endptr ) );
                    ch = *endptr;
                    *endptr = '\0';
                    retl = GetAttrs( Name2, f2loc );
                    if( retl < 0 || ( retl & IO_SUBDIRECTORY ) == 0 ) {
                        retc = MakeDir( Name2, f2loc );
                        if( retc != 0 ) {
                            Error( "Unable to make directory" );
                            SysSetErr( IO_NO_ACCESS );
                            return;
                        }
                        ++DirectoriesMade;
                    }
                    AddCopySpec( Name1, Name2, f1loc, f2loc );
                    endpath = Squish( &Parse1, Name1 );
                }
            }
            if( FindNext( f1loc ) != 0 ) break;
        }
    }
}

rc_erridx   CopyASpec( char *f1, char *f2, int f1loc, int f2loc )
{
    rc_erridx   retc;
    char        *endptr;
    char        *endpath;
    unsigned    dst_cluster;
    unsigned    src_cluster;

    f1 = _FileParse( f1, &Parse1 );
    FinishName( f1, &Parse1, f1loc, 1 );
    f2 = _FileParse( f2, &Parse2 );
    FinishName( f2, &Parse2, f2loc, 1 );
    Copy( &Parse1, &Parse3, sizeof( file_parse ) );
    if( Parse2.name[ 0 ] == '\0' )
        return( IO_FILE_NOT_FOUND );
    dst_cluster = 0xFFFF;
    if( ( f1loc == f2loc ) && ( Parse1.drive[ 0 ] == Parse2.drive[ 0 ] ) ) {
        Squish( &Parse2, Name2 );
        retc = FindFirst( Name2, f2loc, IO_SUBDIRECTORY );
        if( retc == 0 ) {
            dst_cluster = Info.dos.cluster;
        }
    }
    endpath = Squish( &Parse1, Name1 );
    Squish( &Parse2, Name2 );
    WrtCopy( Name1, Name2, f1loc, f2loc );
    retc = FindFirst( Name1, f1loc, 0 );
    if( retc == 0 ) {
        src_cluster = Info.dos.cluster;
        for(;;) {
            CopyStr( Info.name, endpath );
            if( Parse2.device ) {
                Squish( &Parse2, Name2 );
                retc = DoCopy( Name1, Name2, f1loc, f2loc );
            } else {
                _FileParse( Name1, &Parse3 );
                Replace( Parse1.name, Parse2.name, Parse3.name );
                Replace( Parse1.ext, Parse2.ext, Parse3.ext );
                CopyStr( Parse2.path, Parse3.path );
                CopyStr( Parse2.drive, Parse3.drive );
                endptr = Squish( &Parse3, Name2 );
                if( src_cluster == dst_cluster && strcmp( endptr, endpath ) == 0 ) {
                    retc = StashErrCode( IO_CANT_COPY_TO_SELF, OP_LOCAL );
                } else {
                    retc = DoCopy( Name1, Name2, f1loc, f2loc );
                }
            }
            if( retc != 0 )
                break;
            if( FindNext( f1loc ) != 0 ) {
                break;
            }
        }
    }
    return( retc );
}


static void WildCopy( int recursive )
{
    COPYPTR     list;
    int         first;
    rc_erridx   errcod;
    int         none_in_root;

    first = 1;
    none_in_root = FALSE;
    while( (list = CopySpecs) != NULL ) { /* Careful. List shifts underfoot */
        errcod = CopyASpec( list->src, list->dst, list->src_loc, list->dst_loc );
        WriteNL( STD_ERR );
        RemoteWriteConsoleNL();
        if( errcod != 0 ) {
            if( REAL_CODE( errcod ) == 0x02 ||
                REAL_CODE( errcod ) == 0x12 ) { /* File not found */
                if( first ) { /* only crap out if top of tree */
                    none_in_root = TRUE;
                }
            } else {
                TransSetErr( errcod );
                return;
            }
        }
        if( recursive ) {
            RRecurse( list->src, list->dst, list->src_loc, list->dst_loc );
            if( ErrorStatus != 0 ) return;
        }
        if( list == NULL && first && none_in_root ) {
            TransSetErr( errcod );
            return;
        }
        first = 0;
        FreeCopySpec( list );
        MaxOnLine = 0;
    }
}

void ProcCopy( int argc, char **argv )
{
    int         recursive;
    int         src_loc, dst_loc;
    char        *src, *dst;
    int         i;
    char        name[80];
    char        *endp;

    recursive = 0;
    dst = NULL;
    src = NULL;
    FilesCopied = 0;
    DirectoriesMade = 0;
    for( i = 0; i < argc; ++i ) {
        if( Option( argv[ i ], 's' ) ) {
            recursive = 1;
        } else if( src == NULL ) {
            src = argv[ i ];
        } else if( dst == NULL ) {
            dst = argv[ i ];
        } else {
            WhatDidYouSay();
            return;
        }
    }
    if( src == NULL ) {
        WhatDidYouSay();
        return;
    }
    src = RealName( src, &src_loc );
    if( dst == NULL ) {
        dst = ".";
        dst_loc = -src_loc;
    } else {
        dst = RealName( dst, &dst_loc );
    }
    if( src != NULL && dst != NULL ) {
        strcpy( name, dst );
        dst = name;
        endp = name + strlen( name );
        if( endp[-1] == ':' || endp[-1] == '\\' ) {
            *endp = '.';
            *++endp = '\0';
        }
        if( HasWildCards( dst ) == 0 ) {
            i = 0;
            if( HasWildCards( src ) || IsDir( src, src_loc ) ) {
                if( IsDir( dst, dst_loc ) == 0 ) {
                    MakeDir( dst, dst_loc );
                    i = 1;
                }
                if( IsDir( dst, dst_loc ) ) {
                    DirectoriesMade += i;
                    strcpy( endp, "\\*.*" );
                }
            }
        }
        AddCopySpec( src, dst, src_loc, dst_loc );
        WildCopy( recursive );
        #define CPYMSG "        x Files copied        x Directories created"
        strcpy( Buff, CPYMSG );
        DItoD( FilesCopied, Buff + 8 );
        DItoD( DirectoriesMade, Buff + 30 );
        WriteText( STD_OUT, Buff, sizeof( CPYMSG ) - 1 );
    } else {
        WhatDidYouSay();
    }
}

/**************************************************************************/
/* TYPE                                                                   */
/**************************************************************************/

void ProcType( int argc, char **argv )
{
    int         src_loc;
    char        *src;

    src = NULL;
    if( argc != 1 ) {
        WhatDidYouSay();
        return;
    }
    src = RealName( argv[ 0 ], &src_loc );
    AddCopySpec( src, "con", src_loc, -1 );
    Typing = 1;
    WildCopy( 0 );
    Typing = 0;
}

/**************************************************************************/
/* DIR                                                                    */
/**************************************************************************/

static  void    DirClosef( dir_handle *h )
{
    DbgFree( h );
}

extern  dir_handle      *DirOpenf( char *fspec, int fnloc )
{
    dir_handle  *h;
    rc_erridx   retc;
    long        retl;
    char        *append;
    file_parse  parse;

    h = (dir_handle *)DbgAlloc( sizeof( dir_handle ) );
    if( h == NULL ) {
        SysSetErr( IO_FIND_ERROR );
        return( NULL );
    }
    h->status = RFX_OK;
    h->location = fnloc;
    fspec = _FileParse( fspec, &parse );
    append = NULL;
    if( parse.name[ 0 ] == '\0' ) {
        if( parse.ext[ 0 ] == '\0' ) {
            if( parse.slash || parse.path[ 0 ] == '\0' ) {
                append = "*.*";
            } else {
                append = "\\*.*";
            }
        } else {
            parse.name[ 0 ] = '*';
            parse.name[ 1 ] = '\0';
        }
    } else {
        retl = GetAttrs( fspec, fnloc );
        if( retl >= 0 && ( retl & IO_SUBDIRECTORY ) != 0 ) {    /* 11-jun-90 */
            append = "\\*.*";
        } else if( parse.ext[ 0 ] == '\0' ) {
            CopyStr( ".*", parse.ext );
        }
    }
    if( append != NULL ) {
        CopyStr( append, CopyStr( fspec, &h->path ) );
    } else {
        append = CopyStr( parse.drive, &h->path );
        append = CopyStr( parse.path, append );
        append = CopyStr( parse.name, append );
        append = CopyStr( parse.ext, append );
    }
    if( GetFreeSpace( h, fnloc ) ) {
        retc = FindFirst( &h->path, h->location, IO_SUBDIRECTORY );
        if( retc != 0 ) {
            SysSetErr( IO_FIND_ERROR );
            DirClosef( h );
            return( NULL );
        } else {
            return( h );
        }
    } else {
        SysSetErr( IO_BAD_DRIVE );
        DirClosef( h );
        return( NULL );
    }
}


extern  void    DirReadf( dir_handle *h, char *buff, bool wide )
{
    if( h->status == RFX_EOF ) {
        *buff = '\0';
    } else {
        FormatDTA( buff, &Info, wide );
        if( FindNext( h->location ) != 0 ) {
            h->status = RFX_EOF;
        }
    }
}

void    FormatDTA( char *buff, trap_dta *dir, bool wide )
{
    char                *b;
    char                *d;
    long                s;
    unsigned int        date;
    unsigned int        time;
    unsigned int        hour;

    Fill( buff, 39, ' ' );
    buff[ 39 ] = '\0';
    if( dir->attr & IO_SUBDIRECTORY ) {
        *CopyStr( dir->name, buff ) = ' ';
        if( wide ) {                    /* 11-jun-90 */
            Copy( "     ", buff + 13, 5 );
        } else {
            Copy( "<DIR>", buff + 13, 5 );
        }
    } else {
        b = buff;
        d = dir->name;
        while( *d != '.' && *d != '\0' ) {
            *b++ = *d++;
        }
        if( *d == '.' ) {
            d++;
            b = buff + 9;
            while( *d != '\0' ) {
                *b++ = *d++;
            }
        }
        s = dir->size;
        d = buff + 20;
        if( s == 0 ) {
            *d = '0';
        } else {
            DItoD( s, d );
        }
    }
    date = dir->date;
    ItoD( ( date >> 5 ) & 0x000F, buff + 23 ); /* month */
    ItoD( date & 0x0001F, buff + 26 );         /* day */
    ItoD( ( date >> 9 ) + 1980, buff + 29 );
    buff[ 28 ] = '-';
    buff[ 25 ] = '-';
    time = dir->time;
    hour = time >> 11;
    if( hour <= 11 ) {
        buff[ 38 ] = 'a';
    } else {
        hour -= 12;
        buff[ 38 ] = 'p';
    }
    if( hour == 0 ) hour = 12;
    ItoD( hour, buff + 33 );
    buff[ 35 ] = ':';
    ItoD( ( time >> 5 ) & 0x003F, buff + 36 );
}

int     GetFreeSpace( dir_handle *h, int loc )
{
    char                *path;
    char                drive;
    extern  long        FreeSpace();

    path = &h->path;
    if( path[ 1 ] == ':' ) {
        drive = tolower( path[ 0 ] ) - 'a' + 1;
    } else {
        drive = 0;
    }
    h->free = FreeSpace( drive, loc );
    return( 1 );
}

void ProcDir( int argc, char **argv )
{
    int         wide;
    int         pause;
    int         src_loc;
    char        *src;
    dir_handle  *io;
    int         count;
    int         i;
    int         line;

    wide = 0;
    pause = 0;
    src = NULL;
    for( i = 0; i < argc; ++i ) {
        if( Option( argv[ i ], 'w' ) ) {
            wide = 1;
        } else if( Option( argv[ i ], 'p' ) ) {
            pause = 1;
        } else if( src == NULL ) {
            src = argv[ i ];
        } else {
            WhatDidYouSay();
            return;
        }
    }
    if( src == NULL ) {
        src = "*.*";
    }
    src = RealName( src, &src_loc );
    io = DirOpenf( src, src_loc );
    count = 0;
    if( io != NULL ) {
        i = 0;
        line = 0;
        for( ;; ) {
            if( CtrlCHit() ) break;
            DirReadf( io, Buff, wide );
            if( Buff[0] == '\0' ) break;
            ++i;
            if( wide ) {
                Buff[ 14 ] = '\0';
            }
            if( wide == 0 ) {
                WriteText( STD_OUT, Buff, strlen( Buff ) );
                line++;
            } else {
                WriteStream( STD_OUT, Buff, strlen( Buff ) );
                if ( ++count == 5 ) {
                    count = 0;
                    line++;
                    WriteNL( STD_OUT );
                } else {
                    WriteStream( STD_OUT, "\t", 1 );
                }
            }
            if( line == 23 ) {
                WriteStream( STD_OUT, "Press any key when ready . . . ", 31 );
                getch();
                WriteNL( STD_OUT );
                line = 0;
            }
        }
        DirClosef( io );
        if( count != 0 ) {
            WriteNL( STD_OUT );
        }
        #define MSG "        x File(s)         x bytes free"
        strcpy( Buff, MSG );
        DItoD( i, Buff + 8 );
        DItoD( io->free, Buff + 26 );
        WriteText( STD_OUT, Buff, sizeof( MSG ) - 1 );
    } else {
        SysSetErr( IO_FILE_NOT_FOUND );
    }
}

/**************************************************************************/
/* CD                                                                     */
/**************************************************************************/

void ProcCD( int argc, char **argv, int crlf )
{
    int         src_loc;
    char        *src;

    if( argc == 1 ) {
        src = RealRFXName( argv[0], &src_loc );
        if( src_loc == 0 ) {
            src_loc = DefaultLocation;
        }
    } else {
        src = "";
        src_loc = DefaultLocation;
        if( argc != 0 ) {
            WhatDidYouSay();
        }
    }
    if( *src == '\0' ) {
        if( src_loc == 1 ) {
            WriteStream( STD_OUT, "@R", 2 );
        } else {
            WriteStream( STD_OUT, "@L", 2 );
        }
        Buff[0] = GetDrv( src_loc );
        Buff[1] = ':';
        Buff[2] = '\\';
        WriteStream( STD_OUT, Buff, 3 );
        GetDir( 0, Buff, src_loc );
        WriteStream( STD_OUT, Buff, strlen( Buff ) );
        if( crlf ) {
            WriteNL( STD_OUT );
        }
    } else if ( ( src[1] == ':' ) && ( src[2] == '\0' ) ) {
        if( src_loc == 1 ) {
            WriteStream( STD_OUT, "@R", 2 );
        } else {
            WriteStream( STD_OUT, "@L", 2 );
        }
        Buff[0] = toupper( *src );
        Buff[1] = ':';
        Buff[2] = '\\';
        WriteStream( STD_OUT, Buff, 3 );
        GetDir( Buff[ 0 ] - 'A' + 1, Buff, src_loc );
        WriteStream( STD_OUT, Buff, strlen( Buff ) );
        if( crlf ) {
            WriteNL( STD_OUT );
        }
    } else {
        SetDir( src, src_loc );
    }
}

/**************************************************************************/
/* MKDIR                                                                  */
/**************************************************************************/

void ProcMakeDir( int argc, char **argv )
{
    int         src_loc;
    char        *src;

    if( argc == 1 ) {
        src = RealName( argv[0], &src_loc );
        MakeDir( src, src_loc );
    } else {
        WhatDidYouSay();
    }
}

/**************************************************************************/
/* ERASE/DELETE                                                           */
/**************************************************************************/

rc_erridx   Scratchf( char *fn, int fnloc )
{
    rc_erridx   retc;
    char        *endptr;

    fn = _FileParse( fn, &Parse1 );
    FinishName( fn, &Parse1, fnloc, 0 );
    Squish( &Parse1, Name1 );
    retc = FindFirst( Name1, fnloc, 0 );
    if( retc != 0 ) {
        SysSetErr( IO_FILE_NOT_FOUND );
    } else {
        endptr = Parse1.drive;
        if( Parse1.drive[ 0 ] != '\0' ) {
            endptr += 2;
        }
        endptr = CopyStr( Parse1.path, endptr );
        for(;;) {
            CopyStr( Info.name, endptr );
            retc = Erase( Parse1.drive, fnloc );
            if( retc != 0 ) {
                TransSetErr( retc );
                return( retc );
            }
            if( FindNext( fnloc ) != 0 ) {
                break;
            }
        }
    }
    return( retc );
}

void BuildDFSList( void )
{
    COPYPTR next_last, last, curr;

    for( next_last = NULL; next_last != CopySpecs; ) {
        last = next_last;
        next_last = CopySpecs;
        for( curr = CopySpecs; curr != last; curr = curr->next ) {
            RRecurse( curr->src, curr->dst, curr->src_loc, curr->dst_loc );
            if( ErrorStatus != 0 ) return;
        }
    }
}

void ProcErase( int argc, char **argv )
{
    int         src_loc;
    char        *src;
    int         recursive;
    int         i;
    int         erased_one;

    recursive = 0;
    src = NULL;
    for( i = 0; i < argc; ++i ) {
        if( Option( argv[ i ], 's' ) ) {
            recursive = 1;
        } else if( src == NULL ) {
            src = argv[ i ];
        } else {
            WhatDidYouSay();
            return;
        }
    }
    if( src == NULL ) {
        WhatDidYouSay();
        return;
    }
    src = RealName( src, &src_loc );
    AddCopySpec( src, src, src_loc, src_loc );
    if( recursive ) BuildDFSList();
    if( ErrorStatus != 0 ) return;
    erased_one = FALSE;
    while( CopySpecs != NULL ) {
        ErrorStatus = 0;
        Scratchf( CopySpecs->src, CopySpecs->src_loc );
        if( ErrorStatus == 0 ) erased_one = TRUE;
        FreeCopySpec( CopySpecs );
    }
    if( erased_one ) {
        SysSetErr( IO_OK );
    } else {
        SysSetErr( IO_FILE_NOT_FOUND );
    }
}

/**************************************************************************/
/* RMDIR                                                                  */
/**************************************************************************/

void ProcDelDir( int argc, char **argv )
{
    int         src_loc;
    char        *src;
    int         recursive;
    int         i;
    rc_erridx   retc;

    recursive = 0;
    src = NULL;
    for( i = 0; i < argc; ++i ) {
        if( Option( argv[ i ], 's' ) ) {
            recursive = 1;
        } else if( src == NULL ) {
            src = argv[ i ];
        } else {
            WhatDidYouSay();
            return;
        }
    }
    if( src == NULL ) {
        WhatDidYouSay();
        return;
    }
    src = RealName( src, &src_loc );
    CopyStr( "\\*.*", CopyStr( src, Buff ) );
    src = Buff;
    AddCopySpec( Buff, Buff, src_loc, src_loc );
    if( recursive )
        BuildDFSList();
    if( ErrorStatus != 0 )
        return;
    while( CopySpecs != NULL ) {
        src = strstr( CopySpecs->src, "\\*.*" );
        if( src != NULL ) {
            *src = '\0';
            retc = RemoveDir( CopySpecs->src, CopySpecs->src_loc );
            if( retc != 0 ) {
                TransSetErr( retc );
            }
        }
        if( ErrorStatus != 0 )
            break;
        FreeCopySpec( CopySpecs );
    }
}

/**************************************************************************/
/* DRV:                                                                   */
/**************************************************************************/

int ProcDrive( int argc, char **argv )
{
    char        *src;
    int         src_loc;
    int         len;

    if( argc != 1 ) return( 0 );
    src = argv[ 0 ];
    src = RealName( src, &src_loc );
    len = strlen( src ) - 1;
    if( src[ len ] != ':' ) return( 0 );
    if( len == 0 ) {
        DefaultLocation = src_loc;
        return( 1 );
    } else if( len == 1 ) {
        SetDrv( *src, src_loc );
        DefaultLocation = src_loc;
        return( 1 );
    }
    return( 0 );
}

/**************************************************************************/
/* FILE NAME PARSING                                                      */
/**************************************************************************/

extern  char    *CopyMax( const char *src, char *buff, unsigned src_len, unsigned buff_len )
{
    while( src_len > 0 && buff_len > 0 ) {
        *buff++ = *src++;
        --src_len; --buff_len;
    }
    return( buff );
}

extern  char    *_FileParse( char *name, file_parse *file )
{
    char        *curr;
    char        *dosname;
    char        *p;
    char        ch;
    int         extlen;

    Fill( (char *)file, sizeof( file_parse ), 0 );
    dosname = name;
    if( dosname[ 1 ] == ':' ) {
        file->drive[ 0 ] = *dosname;
        file->drive[ 1 ] = ':';
        file->drive[ 2 ] = '\0';
    }
    curr = dosname + strlen( dosname ) - 1;
    extlen = 0;
    p = curr;
    for(;;) {
        if( p < dosname ) break;
        ch = *p;
        if( ch == '/' ) break;
        if( ch == '\\' ) break;
        if( ch == ':' ) break;
        ++extlen;
        if( ch == '.' ) {
            CopyStrMax( p, file->ext, MAX_EXT );
            curr = p - 1;
            break;
        }
        --p;
    }
    file->slash = 0;
    p = curr;
    for(;;) {
        if( p < dosname ) break;
        ch = *p;
        if( ch == '/' || ch == '\\' || ch == ':' ) {
            file->slash = 1;
            break;
        }
        if( ch == ':' ) break;
        if( ch == '.' ) break;
        --p;
    }
    *CopyMax( p + 1, file->name, curr - p, MAX_NAME ) = '\0';
    curr = p;
    p = curr;
    for(;;) {
        if( p < dosname ) break;
        ch = *p;
        if( ch == ':' ) break;
        --p;
    }
    curr = CopyMax( p + 1, file->path, curr - p, MAX_PATH );
    if( extlen == 1 ) {
        if( file->name[ 0 ] == '\0' ) {
            *curr++ = '.';
            file->slash = 0;
            file->ext[ 0 ] = '\0';
        }
    }
    *curr = '\0';
    strupr( file->drive );
    strupr( file->path );
    strupr( file->name );
    strupr( file->ext );
    return( dosname );
}

extern  void    CopyStrMax( const char *src, char *dst, unsigned max_len )
{
    unsigned    len;

    len = strlen( src );
    if( len > max_len ) {
        Copy( src, dst, max_len );
        dst[max_len] = '\0';
    } else {
        StrCopy( src, dst );
    }
}

extern  void    Replace( const char *frum, const char *to, char *into )
{
    while( *to != '\0' ) {
        switch( *to ) {
        case '?':
            if( *frum != '?' && *frum != '*' && *frum != '\0' ) {
                *into++ = *frum;
            } else if( *into != '\0' ) {
                into++;
            }
            break;
        case '*':
            while( *into != '\0' ) {
                ++into;
            }
            return;
        default:
            *into = *to;
            into++;
            break;
        }
        ++to;
        if( *frum != '\0' ) {
            ++frum;
        }
    }
    *into = '\0';
}

extern  void    FinishName( char *fn, file_parse *parse, int loc, int addext )
{
    char        *endptr;
    long        rc;

    endptr = &(parse->path[  strlen( parse->path )  ]);
    parse->device = 0;
    if( parse->name[ 0 ] == '\0' ) {
        if( parse->ext[ 0 ] == '\0' ) {
            if( parse->slash || parse->path[ 0 ] == '\0' ) {
                CopyStr( "*", parse->name );
                CopyStr( ".*", parse->ext );
            } else {
                *endptr++ = '\\';
                *endptr++ = '\0';
                CopyStr( "*", parse->name );
                CopyStr( ".*", parse->ext );
            }
        } else {
            parse->name[ 0 ] = '*';
            parse->name[ 1 ] = '\0';
        }
    } else if( IsDevice( fn, loc ) ) {
        parse->device = 1;
    } else {
        rc = GetAttrs( fn, loc );
        if( rc >= 0 && ( rc & IO_SUBDIRECTORY ) != 0 ) {
            endptr = CopyStr( parse->name, endptr );
            endptr = CopyStr( parse->ext, endptr );
            endptr = CopyStr( "\\", endptr );
            CopyStr( "*", parse->name );
            CopyStr( ".*", parse->ext );
        } else if( ( parse->ext[ 0 ] == '\0' ) && addext ) {
            CopyStr( ".*", parse->ext );
        }
    }
    if( !parse->device && parse->drive[ 0 ] == '\0' ) {
        parse->drive[ 0 ] = GetDrv( loc );
        parse->drive[ 1 ] = ':';
    }
}

extern  char    *Squish( file_parse *parse, char *into )
{
    char        *endptr;
    char        *endpath;

    endptr = CopyStr( parse->drive, into );
    endptr = CopyStr( parse->path, endptr );
    endpath = endptr;
    endptr = CopyStr( parse->name, endptr );
    endptr = CopyStr( parse->ext, endptr );
    return( endpath );
}

char    *TrapClientString( unsigned tc )
{
    switch( tc ) {
        case TC_BAD_TRAP_FILE:      return( "Bad trap file" );
        case TC_CANT_LOAD_TRAP:     return( "Cannot load trap file %s" );
        case TC_WRONG_TRAP_VERSION: return( "Incorrect trap file version" );
        case TC_OUT_OF_DOS_MEMORY:  return( "Out of DOS memory" );
    }
    return( NULL );
}

