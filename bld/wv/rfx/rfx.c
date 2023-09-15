/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2023 The Open Watcom Contributors. All Rights Reserved.
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
#include <conio.h>
#include <ctype.h>
#include <process.h>
#include "dbgdefn.h"
#include "rfxdata.h"
#include "fparse.h"
#include "banner.h"
#include "dbgio.h"
#include "objloc.h"
#include "trprfx.h"
#include "local.h"
#include "remote.h"
#include "filermt.h"
#include "remfile.h"
#include "dbginit.h"
#include "rfxacc.h"
#include "rfx.h"


#define REAL_CODE( err ) (GetSystemErrCode( err ) & 0xffff)

#define IO_NORMAL       0
#define IO_SUBDIRECTORY 0x10

#define BUFF_LEN        4096

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

enum {
    RFX_EOR,
    RFX_EOF,
    RFX_OK
};

typedef struct copyspec {
    struct copyspec *next;
    char            *src;
    object_loc      src_loc;
    char            *dst;
    object_loc      dst_loc;
} COPYSPEC, *COPYPTR;

typedef struct dir_handle {
    rfx_find        info;
    char            path[64];
    long            free;
    object_loc      location;
    char            status;
} dir_handle;

extern bool             InitTrap( const char * );
extern bool             InitRFXSupp( void );
extern void             FiniTrap( void );

static char             *TxtBuff;
static char             Buff[BUFF_LEN];
static int              MaxOnLine = { 0 };
static bool             Typing = false;
static error_handle     ErrorStatus = { 0 };
static object_loc       DefaultLocation = LOC_DEFAULT;
static COPYPTR          CopySpecs;
static file_parse       Parse1;
static file_parse       Parse2;
static file_parse       Parse3;
static char             Name1[MAX_DRIVE + MAX_PATH + MAX_NAME + MAX_EXT + 2];
static char             Name2[MAX_DRIVE + MAX_PATH + MAX_NAME + MAX_EXT + 2];
static char             Name3[MAX_DRIVE + MAX_PATH + MAX_NAME + MAX_EXT + 2];
static int              FilesCopied;
static int              DirectoriesMade;

static const char * const HelpText[] = {
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

#if 0
static const char           NullStr[] = { NULLCHAR };

static const char           _Literal_No_Mem_4_Path[] = { "no memory for PATH" };

static const char * const   ErrMessages[] = {
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
#endif

static const char * const Day[] = {
    "Sun  ",
    "Mon  ",
    "Tue  ",
    "Wed  ",
    "Thu  ",
    "Fri  ",
    "Sat  "
};

/**************************************************************************/
/* UTILITIES                                                              */
/**************************************************************************/

static void Help( void )
{
    const char  * const *txts;
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

char *StrCopyDst( const char *src, char *dest )
{
    while( (*dest = *src++) != NULLCHAR ) {
        dest++;
    }
    return( dest );
}

static const char *RealRFXName( const char *name, object_loc *loc )
{
    obj_attrs   oattrs;

    oattrs = 0;
    name = RealFName( name, &oattrs );
    if( oattrs & OP_LOCAL ) {
        *loc = LOC_LOCAL;
    } else if( oattrs & OP_REMOTE ) {
        *loc = LOC_REMOTE;
    } else {
        *loc = LOC_DEFAULT;
    }
    return( name );
}

static obj_attrs RFX2Acc( object_loc loc )
{
    if( loc == LOC_DEFAULT )
        loc = DefaultLocation;
    if( loc == LOC_LOCAL )
        return( OP_LOCAL );
    return( OP_REMOTE );
}

static const char *RealName( const char *name, object_loc *loc )
{
    name = RealRFXName( name, loc );
    if( *loc == LOC_DEFAULT ) {
        *loc = DefaultLocation;
    }
    if( *name == NULLCHAR )
        return( "." );
    return( name );
}

void StartupErr( const char *err )
{
    Error( err );
    exit( 2 );
    // never return
}

void RestoreHandlers( void )
{
}

void GrabHandlers( void )
{
}

static void *DbgAlloc( size_t size )
{
    return( malloc( size ) );
}

static void DbgFree( void *chunk )
{
    free( chunk );
}
#if 0
static void FreeRing( void )
{
}
#endif
static error_handle TransSetErr( error_handle errh )
{
    ErrorStatus = errh;
    return( errh );
}

static error_handle SysSetLclErr( sys_error err )
{
    return( TransSetErr( StashErrCode( err, OP_LOCAL ) ) );
}

static char *MyStrDup( const char *str ) {

    char *new;

    new = DbgAlloc( strlen( str ) + 1 );
    strcpy( new, str );
    return( new );
}

static char *Copy( const void *s, void *d, unsigned len ) {

    char        *dst = d;
    const char  *src = s;

    while( len-- > 0 ) {
        *dst++ = *src++;
    }
    return( dst );
}

static char *Fill( void *d, int len, char filler ) {

    char *dst = d;
    while( len-- > 0 ) {
        *dst++ = filler;
    }
    return( dst );
}

static char *CopyStr( const char *src, char *dst )
{
    while( (*dst = *src) != NULLCHAR ) {
        ++src;
        ++dst;
    }
    return( dst );
}

static void DItoD( long s, char *d ) {

    if( s == 0 ) {
        *d = '0';
    } else {
        do {
            *d-- = ( s % 10 ) + '0';
            s /= 10;
        } while( s != 0 );
    }
}

static void ItoD( unsigned int i, char *b ) {

    b[1] = ( i % 10 ) + '0';
    i /= 10;
    b[0] = ( i % 10 ) + '0';
}

/**************************************************************************/
/* ACTUAL OS CALLS                                                        */
/**************************************************************************/

static char GetDrv( object_loc loc )
/**********************************/
{
    if( loc == LOC_REMOTE ) {
        return( RemoteGetDrv() + 'A' );
    } else {
        return( LocalGetDrv() + 'A' );
    }
}

static void SetDrv( char drive, object_loc loc )
/**********************************************/
{
    if( loc == LOC_REMOTE ) {
        RemoteSetDrv( drive - 'A' );
    } else {
        LocalSetDrv( drive - 'A' );
    }
    if( GetDrv( loc ) != drive ) {
        SysSetLclErr( IO_BAD_DRIVE );
    }
}

static error_handle RemoveDir( const char *name, object_loc loc )
/***************************************************************/
{
    if( loc == LOC_REMOTE ) {
        return( RemoteRmDir( name ) );
    } else {
        return( LocalRmDir( name ) );
    }
}

static error_handle SetDir( const char *name, object_loc loc )
/************************************************************/
{
    if( loc == LOC_REMOTE ) {
        return( RemoteSetCWD( name ) );
    } else {
        return( LocalSetCWD( name ) );
    }
}

static error_handle GetDir( int drive, char *name, unsigned max_len, object_loc loc )
/***********************************************************************************/
{
    /* drive=0 means current drive A:=1, B:=2, etc. */
    if( loc == LOC_REMOTE ) {
        return( RemoteGetCwd( drive, name, max_len ) );
    } else {
        return( LocalGetCwd( drive, name, max_len ) );
    }
}

static error_handle Erase( const char *name, object_loc loc )
/***********************************************************/
{
    return( FileRemove( name, RFX2Acc( loc ) ) );
}

static error_handle MakeDir( const char *name, object_loc loc )
/*************************************************************/
{
    if( loc == LOC_REMOTE ) {
        return( RemoteMkDir( name ) );
    } else {
        return( LocalMkDir( name ) );
    }
}

static long GetAttrs( const char *name, object_loc loc )
/*******************************************************/
{
    if( loc == LOC_REMOTE ) {
        return( RemoteGetFileAttr( name ) );
    } else {
        return( LocalGetFileAttr( name ) );
    }
}

static long SetAttrs( const char *name, object_loc loc, long dos_attrib )
/***********************************************************************/
{
    if( loc == LOC_REMOTE ) {
        return( RemoteSetFileAttr( name, dos_attrib ) );
    } else {
        return( LocalSetFileAttr( name, dos_attrib ) );
    }
}

static bool IsDevice( const char *name, object_loc loc )
/******************************************************/
{
    file_handle     fh;

    if( GetAttrs( name, loc ) != RFX_INVALID_FILE_ATTRIBUTES )
        return( false );
    fh = FileOpen( name, OP_READ | RFX2Acc( loc ) );
    if( fh == NIL_HANDLE )
        return( false );
    FileClose( fh );
    return( true );
}


static error_handle _FindFirst( const char *name, object_loc loc, int attr, rfx_find *info, unsigned info_len )
/*************************************************************************************************************/
{
    if( loc == LOC_REMOTE ) {
        return( RemoteFindFirst( name, info, info_len, attr ) );
    } else {
        return( LocalFindFirst( name, info, info_len, attr ) );
    }
}

static int _FindNext( object_loc loc, rfx_find *info, unsigned info_len )
/***********************************************************************/
{
    if( loc == LOC_REMOTE ) {
        return( RemoteFindNext( info, info_len ) );
    } else {
        return( LocalFindNext( info, info_len ) );
    }
}

static error_handle _FindClose( object_loc loc, rfx_find *info, unsigned info_len )
/*********************************************************************************/
{
    if( loc == LOC_REMOTE ) {
        return( RemoteFindClose( info, info_len ) );
    } else {
        return( LocalFindClose( info, info_len ) );
    }
}

static error_handle Rename( const char *f1, const char *f2, object_loc loc )
/**************************************************************************/
{
    if( loc == LOC_REMOTE ) {
        return( RemoteRename( f1, f2 ) );
    } else {
        return( LocalRename( f1, f2 ) );
    }
}

static long FreeSpace( int drive, object_loc loc )
/************************************************/
{
    if( loc == LOC_REMOTE ) {
        return( RemoteGetFreeSpace( drive ) );
    } else {
        return( LocalGetFreeSpace( drive ) );
    }
}

static int  GetFreeSpace( dir_handle *dh, object_loc loc )
{
    int         drive;

    if( dh->path[1] == ':' ) {
        drive = dh->path[0] - 'A' + 1;
    } else {
        drive = 0;
    }
    dh->free = FreeSpace( drive, loc );
    return( 1 );
}

static void SameDate( file_handle fh_src, object_loc src_loc, file_handle fh_dst, object_loc dst_loc )
/****************************************************************************************************/
{
    int     time, date;

    if( src_loc == LOC_REMOTE ) {
        RemoteDateTime( GetSystemHandle( fh_src ), &time, &date, 0 );
    } else {
        LocalDateTime( GetSystemHandle( fh_src ), &time, &date, 0 );
    }
    if( dst_loc == LOC_REMOTE ) {
        RemoteDateTime( GetSystemHandle( fh_dst ), &time, &date, 1 );
    } else {
        LocalDateTime( GetSystemHandle( fh_dst ), &time, &date, 1 );
    }
}

static bool Option( const char *str, char opt )
{
    if( *str == '/' || *str == '-' ) {
        if( tolower( str[1] ) == opt ) {
            return( true );
        }
    }
    return( false );
}

/**************************************************************************/
/* PROCESS COMMANDS                                                       */
/**************************************************************************/

static void CopyCmd( const char *src, char *dst )
{
    char    c;

    while( (c = *src++) != NULLCHAR ) {
        if( c == '/' )
            *dst++ = ' ';
        *dst++ = c;
    }
    *dst = NULLCHAR;
}

/**************************************************************************/
/* FILE NAME PARSING                                                      */
/**************************************************************************/

static char    *CopyMax( const char *src_name, char *buff, unsigned src_len, unsigned buff_len )
{
    while( src_len > 0 && buff_len > 0 ) {
        *buff++ = *src_name++;
        --src_len; --buff_len;
    }
    return( buff );
}

static void    CopyStrMax( const char *src, char *dst, size_t max_len )
{
    size_t      len;

    len = strlen( src );
    if( len > max_len ) {
        Copy( src, dst, max_len );
        dst[max_len] = NULLCHAR;
    } else {
        StrCopyDst( src, dst );
    }
}

static const char    *_FileParse( const char *name, file_parse *file )
{
    const char  *curr;
    const char  *dosname;
    const char  *p;
    char        *p1;
    char        ch;
    int         extlen;

    Fill( file, sizeof( file_parse ), 0 );
    dosname = name;
    if( dosname[1] == ':' ) {
        file->drive[0] = DRIVECHAR( *dosname );
        file->drive[1] = ':';
        file->drive[2] = NULLCHAR;
    }
    curr = dosname + strlen( dosname ) - 1;
    extlen = 0;
    for( p = curr; p >= dosname; --p ) {
        ch = *p;
        if( ch == '/' )
            break;
        if( ch == '\\' )
            break;
        if( ch == ':' )
            break;
        ++extlen;
        if( ch == '.' ) {
            CopyStrMax( p, file->ext, MAX_EXT );
            curr = p - 1;
            break;
        }
    }
    file->slash = 0;
    for( p = curr; p >= dosname; --p ) {
        ch = *p;
        if( ch == '/' || ch == '\\' || ch == ':' ) {
            file->slash = 1;
            break;
        }
        if( ch == ':' )
            break;
        if( ch == '.' ) {
            break;
        }
    }
    *CopyMax( p + 1, file->name, curr - p, MAX_NAME ) = NULLCHAR;
    curr = p;
    for( p = curr; p >= dosname; --p ) {
        ch = *p;
        if( ch == ':' ) {
            break;
        }
    }
    p1 = CopyMax( p + 1, file->path, curr - p, MAX_PATH );
    if( extlen == 1 ) {
        if( file->name[0] == NULLCHAR ) {
            *p1++ = '.';
            file->slash = 0;
            file->ext[0] = NULLCHAR;
        }
    }
    *p1 = NULLCHAR;
//    strupr( file->drive );
//    strupr( file->path );
//    strupr( file->name );
//    strupr( file->ext );
    return( dosname );
}

static void    Replace( const char *from, const char *to, char *into )
{
    while( *to != NULLCHAR ) {
        switch( *to ) {
        case '?':
            if( *from != '?' && *from != '*' && *from != NULLCHAR ) {
                *into++ = *from;
            } else if( *into != NULLCHAR ) {
                into++;
            }
            break;
        case '*':
            while( *into != NULLCHAR ) {
                ++into;
            }
            return;
        default:
            *into = *to;
            into++;
            break;
        }
        ++to;
        if( *from != NULLCHAR ) {
            ++from;
        }
    }
    *into = NULLCHAR;
}

static bool IsDir( const char *src, object_loc src_loc )
{
    long attr;

    attr = GetAttrs( src, src_loc );
    return( attr != RFX_INVALID_FILE_ATTRIBUTES && (attr & IO_SUBDIRECTORY) );
}

static void    FinishName( const char *fn, file_parse *parse, object_loc loc, bool addext )
{
    char        *endptr;

    endptr = parse->path + strlen( parse->path );
    parse->device = 0;
    if( parse->name[0] == NULLCHAR ) {
        if( parse->ext[0] == NULLCHAR ) {
            if( parse->slash || parse->path[0] == NULLCHAR ) {
                CopyStr( "*", parse->name );
                CopyStr( ".*", parse->ext );
            } else {
                *endptr++ = '\\';
                *endptr++ = NULLCHAR;
                CopyStr( "*", parse->name );
                CopyStr( ".*", parse->ext );
            }
        } else {
            parse->name[0] = '*';
            parse->name[1] = NULLCHAR;
        }
    } else if( IsDevice( fn, loc ) ) {
        parse->device = 1;
    } else if( IsDir( fn, loc ) ) {
        endptr = CopyStr( parse->name, endptr );
        endptr = CopyStr( parse->ext, endptr );
        endptr = CopyStr( "\\", endptr );
        CopyStr( "*", parse->name );
        CopyStr( ".*", parse->ext );
    } else if( ( parse->ext[0] == NULLCHAR ) && addext ) {
        CopyStr( ".*", parse->ext );
    }
    if( !parse->device && parse->drive[0] == NULLCHAR ) {
        parse->drive[0] = GetDrv( loc );
        parse->drive[1] = ':';
    }
}

static char    *Squish( file_parse *parse, char *into )
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

/**************************************************************************/
/* RENAME                                                                 */
/**************************************************************************/

static error_handle   Renamef( const char *fn1, object_loc f1loc, const char *fn2, object_loc f2loc )
{
    error_handle    errh;
    char            *endpath;
    rfx_find        info;

    fn1 = _FileParse( fn1, &Parse1 );
    fn2 = _FileParse( fn2, &Parse2 );
    Copy( &Parse1, &Parse3, sizeof( file_parse ) );
    if( Parse1.drive[0] == NULLCHAR ) {
        Parse1.drive[0] = GetDrv( f1loc );
        Parse1.drive[1] = ':';
    }
    if( Parse2.drive[0] == NULLCHAR ) {
        CopyStr( Parse1.drive, Parse2.drive );
    }
    if( Parse2.path[0] == NULLCHAR ) {
        CopyStr( Parse1.path, Parse2.path );
    }
    if( f1loc != f2loc ) {
        SysSetLclErr( IO_DIFF_DEV );
        return( 0 );
    }
    if( Parse1.drive[0] != Parse2.drive[0] ) {
        SysSetLclErr( IO_DIFF_DEV );
        return( 0 );
    }
    if( Parse2.name[0] == NULLCHAR ) {
        SysSetLclErr( IO_DUP_OR_NOT_FOUND );
        return( 0 );
    }
    endpath = Squish( &Parse1, Name1 );
    errh = _FindFirst( Name1, f1loc, IO_NORMAL, &info, sizeof( info ) );
    if( errh != 0 ) {
        SysSetLclErr( IO_FILE_NOT_FOUND );
    } else {
        for( ;; ) {
            if( CtrlCHit() ) {
                errh = SysSetLclErr( IO_INTERRUPT );
                break;
            }
            CopyStr( info.name, endpath );
            _FileParse( Name1, &Parse3 );
            Replace( Parse1.name, Parse2.name, Parse3.name );
            Replace( Parse1.ext, Parse2.ext, Parse3.ext );
            CopyStr( Parse2.path, Parse3.path );
            Squish( &Parse3, Name2 );
            errh = Rename( Name1, Name2, f1loc );
            if( errh != 0 ) {
                if( REAL_CODE( errh ) == IO_FILE_NOT_FOUND ) {
                    SysSetLclErr( IO_DUP_OR_NOT_FOUND );
                } else {
                    TransSetErr( errh );
                }
                break;
            }
            if( _FindNext( f1loc, &info, sizeof( info ) ) ) {
                break;
            }
        }
        _FindClose( f1loc, &info, sizeof( info ) );
    }
    return( errh );
}

static void ProcRename( int argc, char **argv )
{
    object_loc  src_loc, dst_loc;
    const char  *src;
    const char  *dst;
    int         i;

    dst = NULL;
    src = NULL;
    for( i = 0; i < argc; ++i ) {
        if( src == NULL ) {
            src = argv[i];
        } else if( dst == NULL ) {
            dst = argv[i];
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
    if( dst_loc == LOC_DEFAULT )
        dst_loc = src_loc;
    if( *dst == NULLCHAR )
        dst = ".";
    Renamef( src, src_loc, dst, dst_loc );
}

/**************************************************************************/
/* COPY                                                                   */
/**************************************************************************/

static void AddCopySpec( const char *src, const char *dst, object_loc src_loc, object_loc dst_loc )
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

static void FreeCopySpec( COPYPTR junk )
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

static bool HasWildCards( const char *src )
{
    if( strchr( src, '?' ) != NULL )
        return( true );
    if( strchr( src, '*' ) != NULL )
        return( true );
    return( false );
}


static void WrtCopy( const char *src, const char *dst, object_loc src_loc, object_loc dst_loc )
{
    size_t      len;

    memset( Buff, ' ', 80 );
    Buff[0] = '\r';
    len = strlen( src );
    strncpy( Buff + 1, src, len );
    len += 2;
    if( len > MaxOnLine ) {
        MaxOnLine = len;
    }
    Buff[MaxOnLine] = '\r';
    if( src_loc == LOC_REMOTE ) {
        RemoteWriteConsole( Buff, MaxOnLine );
    } else if( !Typing ) {
        WriteStream( STD_ERR, Buff, MaxOnLine );
    }
    memset( Buff, ' ', 80 );
    Buff[0] = '\r';
    len = strlen( dst );
    strncpy( Buff + 1, dst, len );
    len += 2;
    if( len > MaxOnLine ) {
        MaxOnLine = len;
    }
    Buff[MaxOnLine] = '\r';
    if( dst_loc == LOC_REMOTE ) {
        RemoteWriteConsole( Buff, MaxOnLine );
    } else if( !Typing ) {
        WriteStream( STD_ERR, Buff, MaxOnLine );
    }
}

static void FiniCopy( file_handle fh_src, const char *src_name, object_loc src_loc,
               file_handle fh_dst, const char *dst_name, object_loc dst_loc )
{
    SameDate( fh_src, src_loc, fh_dst, dst_loc );
    FileClose( fh_src );
    FileClose( fh_dst );
    SetAttrs( dst_name, dst_loc, GetAttrs( src_name, src_loc ) );
    ++FilesCopied;
}


static error_handle DoCopy( const char *src_name, const char *dst_name, object_loc src_loc, object_loc dst_loc )
{
    file_handle     fh_src, fh_dst;
    size_t          read_len;
    size_t          write_len;
    error_handle    errh;

    WrtCopy( src_name, dst_name, src_loc, dst_loc );
    fh_src = FileOpen( src_name, OP_READ | RFX2Acc( src_loc ) );
    if( fh_src == NIL_HANDLE )
        return( StashErrCode( IO_FILE_NOT_FOUND, OP_LOCAL ) );
    fh_dst = FileOpen( dst_name, OP_WRITE | OP_CREATE | RFX2Acc( dst_loc ) );
    if( fh_dst == NIL_HANDLE ) {
        FileClose( fh_src );
        return( StashErrCode( IO_NO_ACCESS, OP_LOCAL ) );
    }
    for( ;; ) {
        if( CtrlCHit() ) {
            FiniCopy( fh_src, src_name, src_loc, fh_dst, dst_name, dst_loc );
            return( StashErrCode( IO_INTERRUPT, OP_LOCAL ) );
        }
        read_len = ReadStream( fh_src, Buff, BUFF_LEN );
        if( read_len == ERR_READ ) {
            errh = GetLastErr();
            FiniCopy( fh_src, src_name, src_loc, fh_dst, dst_name, dst_loc );
            return( errh );
        }
        if( read_len == 0 )
            break;
        write_len = WriteStream( fh_dst, Buff, read_len );
        if( write_len == ERR_WRITE ) {
            errh = GetLastErr();
            FiniCopy( fh_src, src_name, src_loc, fh_dst, dst_name, dst_loc );
            return( errh );
        }
        if( write_len != read_len ) {
            if( ( write_len == ( read_len - 1 ) ) && ( Buff[write_len] == 0x1A ) )
                break;
            FiniCopy( fh_src, src_name, src_loc, fh_dst, dst_name, dst_loc );
            return( StashErrCode( IO_DISK_FULL, OP_LOCAL ) );
        }
    }
    FiniCopy( fh_src, src_name, src_loc, fh_dst, dst_name, dst_loc );
    return( StashErrCode( IO_OK, OP_LOCAL ) );
}

static void    RRecurse( const char *f1, const char *f2, object_loc f1loc, object_loc f2loc )
{
    error_handle    errh;
    char            *endptr;
    char            *endpath;
    char            ch;
    rfx_find        info;

    f1 = _FileParse( f1, &Parse1 );
    FinishName( f1, &Parse1, f1loc, true );
    Copy( &Parse1, &Parse3, sizeof( file_parse ) );
    CopyStr( "*", Parse3.name );
    CopyStr( ".*", Parse3.ext );
    endpath = Squish( &Parse3, Name1 );
    f2 = _FileParse( f2, &Parse2 );
    FinishName( f2, &Parse2, f2loc, true );
    errh = _FindFirst( Name1, f1loc, IO_SUBDIRECTORY, &info, sizeof( info ) );
    if( errh == 0 ) {
        endpath = Squish( &Parse1, Name1 );
        for(;;) {
            if( info.attr & IO_SUBDIRECTORY ) {
                if( info.name[0] != '.' ) {
                    CopyStr( endpath, Name3 );
                    CopyStr( Name3, CopyStr( "\\", CopyStr( info.name, endpath ) ) );
                    endpath = Squish( &Parse2, Name2 );
                    CopyStr( endpath, Name3 );
                    endptr = CopyStr( info.name, endpath );
                    CopyStr( Name3, CopyStr( "\\", endptr ) );
                    ch = *endptr;
                    *endptr = NULLCHAR;
                    if( !IsDir( Name2, f2loc ) ) {
                        errh = MakeDir( Name2, f2loc );
                        if( errh != 0 ) {
                            Error( "Unable to make directory" );
                            SysSetLclErr( IO_NO_ACCESS );
                            break;
                        }
                        ++DirectoriesMade;
                    }
                    AddCopySpec( Name1, Name2, f1loc, f2loc );
                    endpath = Squish( &Parse1, Name1 );
                }
            }
            if( _FindNext( f1loc, &info, sizeof( info ) ) ) {
                break;
            }
        }
        _FindClose( f1loc, &info, sizeof( info ) );
    }
}

static error_handle   CopyASpec( const char *f1, const char *f2, object_loc f1loc, object_loc f2loc )
{
    error_handle    errh;
    char            *endptr;
    char            *endpath;
    unsigned_32     dst_entryid;
    rfx_find        info;

    f1 = _FileParse( f1, &Parse1 );
    FinishName( f1, &Parse1, f1loc, true );
    f2 = _FileParse( f2, &Parse2 );
    FinishName( f2, &Parse2, f2loc, true );
    Copy( &Parse1, &Parse3, sizeof( file_parse ) );
    if( Parse2.name[0] == NULLCHAR )
        return( StashErrCode( IO_FILE_NOT_FOUND, OP_LOCAL ) );
    dst_entryid = DTARFX_INVALID_ID;
    if( ( f1loc == f2loc ) && ( Parse1.drive[0] == Parse2.drive[0] ) ) {
        Squish( &Parse2, Name2 );
        errh = _FindFirst( Name2, f2loc, IO_SUBDIRECTORY, &info, sizeof( info ) );
        if( errh == 0 ) {
            dst_entryid = DTARFX_ID_OF( &info );
            _FindClose( f2loc, &info, sizeof( info ) );
        }
    }
    endpath = Squish( &Parse1, Name1 );
    Squish( &Parse2, Name2 );
    WrtCopy( Name1, Name2, f1loc, f2loc );
    errh = _FindFirst( Name1, f1loc, IO_NORMAL, &info, sizeof( info ) );
    if( errh == 0 ) {
        for(;;) {
            CopyStr( info.name, endpath );
            if( Parse2.device ) {
                Squish( &Parse2, Name2 );
                errh = DoCopy( Name1, Name2, f1loc, f2loc );
            } else {
                _FileParse( Name1, &Parse3 );
                Replace( Parse1.name, Parse2.name, Parse3.name );
                Replace( Parse1.ext, Parse2.ext, Parse3.ext );
                CopyStr( Parse2.path, Parse3.path );
                CopyStr( Parse2.drive, Parse3.drive );
                endptr = Squish( &Parse3, Name2 );
                if( f1loc == f2loc && DTARFX_ID_OF( &info ) == dst_entryid && strcmp( endptr, endpath ) == 0 ) {
                    errh = StashErrCode( IO_CANT_COPY_TO_SELF, OP_LOCAL );
                } else {
                    errh = DoCopy( Name1, Name2, f1loc, f2loc );
                }
            }
            if( errh != 0 )
                break;
            if( _FindNext( f1loc, &info, sizeof( info ) ) ) {
                break;
            }
        }
        _FindClose( f1loc, &info, sizeof( info ) );
    }
    return( errh );
}


static void WildCopy( bool recursive )
{
    COPYPTR         list;
    bool            first;
    error_handle    errh;
    bool            none_in_root;

    first = true;
    none_in_root = false;
    while( (list = CopySpecs) != NULL ) { /* Careful. List shifts underfoot */
        errh = CopyASpec( list->src, list->dst, list->src_loc, list->dst_loc );
        WriteNL( STD_ERR );
        RemoteWriteConsoleNL();
        if( errh != 0 ) {
            if( REAL_CODE( errh ) == 0x02 ||
                REAL_CODE( errh ) == 0x12 ) { /* File not found */
                if( first ) { /* only crap out if top of tree */
                    none_in_root = true;
                }
            } else {
                TransSetErr( errh );
                return;
            }
        }
        if( recursive ) {
            RRecurse( list->src, list->dst, list->src_loc, list->dst_loc );
            if( ErrorStatus != 0 ) {
                return;
            }
        }
        if( list == NULL && first && none_in_root ) {
            TransSetErr( errh );
            return;
        }
        first = false;
        FreeCopySpec( list );
        MaxOnLine = 0;
    }
}

static void ProcCopy( int argc, char **argv )
{
    int         recursive;
    object_loc  src_loc, dst_loc;
    const char  *src_name, *dst_name;
    int         i;
    char        name[80];
    char        *endp;

    recursive = false;
    dst_name = NULL;
    src_name = NULL;
    FilesCopied = 0;
    DirectoriesMade = 0;
    for( i = 0; i < argc; ++i ) {
        if( Option( argv[i], 's' ) ) {
            recursive = true;
        } else if( src_name == NULL ) {
            src_name = argv[i];
        } else if( dst_name == NULL ) {
            dst_name = argv[i];
        } else {
            WhatDidYouSay();
            return;
        }
    }
    if( src_name == NULL ) {
        WhatDidYouSay();
        return;
    }
    src_name = RealName( src_name, &src_loc );
    if( dst_name == NULL ) {
        dst_name = ".";
        if( src_loc == LOC_LOCAL ) {
            dst_loc = LOC_REMOTE;
        } else if( src_loc == LOC_REMOTE ) {
            dst_loc = LOC_LOCAL;
        } else {
            dst_loc = LOC_DEFAULT;
        }
    } else {
        dst_name = RealName( dst_name, &dst_loc );
    }
    if( src_name != NULL && dst_name != NULL ) {
        strcpy( name, dst_name );
        dst_name = name;
        endp = name + strlen( name );
        if( endp[-1] == ':' || endp[-1] == '\\' ) {
            *endp++ = '.';
            *endp = NULLCHAR;
        }
        if( !HasWildCards( dst_name ) ) {
            i = 0;
            if( HasWildCards( src_name ) || IsDir( src_name, src_loc ) ) {
                if( !IsDir( dst_name, dst_loc ) ) {
                    MakeDir( dst_name, dst_loc );
                    i = 1;
                }
                if( IsDir( dst_name, dst_loc ) ) {
                    DirectoriesMade += i;
                    strcpy( endp, "\\*.*" );
                }
            }
        }
        AddCopySpec( src_name, dst_name, src_loc, dst_loc );
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

static void ProcType( int argc, char **argv )
{
    object_loc  src_loc;
    const char  *src;

    src = NULL;
    if( argc != 1 ) {
        WhatDidYouSay();
        return;
    }
    src = RealName( argv[0], &src_loc );
    AddCopySpec( src, "con", src_loc, LOC_LOCAL );
    Typing = true;
    WildCopy( false );
    Typing = false;
}

/**************************************************************************/
/* DIR                                                                    */
/**************************************************************************/

static  void    DirClosef( dir_handle *dh )
{
    _FindClose( dh->location, &dh->info, sizeof( dh->info ) );
    DbgFree( dh );
}

static dir_handle      *DirOpenf( const char *fspec, object_loc fnloc )
{
    dir_handle      *dh;
    error_handle    errh;
    file_parse      parse;

    dh = (dir_handle *)DbgAlloc( sizeof( dir_handle ) );
    if( dh == NULL ) {
        SysSetLclErr( IO_FIND_ERROR );
    } else {
        dh->status = RFX_OK;
        dh->location = fnloc;
        fspec = _FileParse( fspec, &parse );
        if( parse.name[0] == NULLCHAR ) {
            parse.name[0] = '*';
            parse.name[1] = NULLCHAR;
            if( parse.ext[0] == NULLCHAR ) {
                parse.ext[0] = '.';
                parse.ext[1] = '*';
                parse.ext[2] = NULLCHAR;
                if( !parse.slash && parse.path[0] != NULLCHAR ) {
                    CopyStr( "\\", parse.path + strlen( parse.path ) );
                }
            }
        } else if( IsDir( fspec, fnloc ) ) {
            CopyStr( "\\", CopyStr( parse.ext, CopyStr( parse.name, parse.path + strlen( parse.path ) ) ) );
            parse.name[0] = '*';
            parse.name[1] = NULLCHAR;
            parse.ext[0] = '.';
            parse.ext[1] = '*';
            parse.ext[2] = NULLCHAR;
        } else if( parse.ext[0] == NULLCHAR ) {
            parse.ext[0] = '.';
            parse.ext[1] = '*';
            parse.ext[2] = NULLCHAR;
        }
        Squish( &parse, dh->path );
        if( GetFreeSpace( dh, fnloc ) ) {
            errh = _FindFirst( dh->path, dh->location, IO_SUBDIRECTORY, &dh->info, sizeof( dh->info ) );
            if( errh == 0 ) {
                return( dh );
            }
            SysSetLclErr( IO_FIND_ERROR );
        } else {
            SysSetLclErr( IO_BAD_DRIVE );
        }
        DirClosef( dh );
    }
    return( NULL );
}

static void     FormatDTA( char *buff, const rfx_find *info, bool wide )
{
    char                *d;
    const char          *src;
    unsigned int        date;
    unsigned int        time;
    unsigned int        hour;

    Fill( buff, 39, ' ' );
    buff[39] = NULLCHAR;
    if( info->attr & IO_SUBDIRECTORY ) {
        *CopyStr( info->name, buff ) = ' ';
        if( wide ) {                    /* 11-jun-90 */
            Copy( "     ", buff + 13, 5 );
        } else {
            Copy( "<DIR>", buff + 13, 5 );
        }
    } else {
        d = buff;
        src = info->name;
        while( *src != '.' && *src != NULLCHAR ) {
            *d++ = *src++;
        }
        if( *src == '.' ) {
            src++;
            d = buff + 9;
            while( *src != NULLCHAR ) {
                *d++ = *src++;
            }
        }
        d = buff + 20;
        if( info->size == 0 ) {
            *d = '0';
        } else {
            DItoD( info->size, d );
        }
    }
    date = info->date;
    ItoD( ( date >> 5 ) & 0x000F, buff + 23 ); /* month */
    ItoD( date & 0x0001F, buff + 26 );         /* day */
    ItoD( ( date >> 9 ) + 1980, buff + 29 );
    buff[28] = '-';
    buff[25] = '-';
    time = info->time;
    hour = time >> 11;
    if( hour <= 11 ) {
        buff[38] = 'a';
    } else {
        hour -= 12;
        buff[38] = 'p';
    }
    if( hour == 0 )
        hour = 12;
    ItoD( hour, buff + 33 );
    buff[35] = ':';
    ItoD( ( time >> 5 ) & 0x003F, buff + 36 );
}

static void    DirReadf( dir_handle *dh, char *buff, bool wide )
{
    if( dh->status == RFX_EOF ) {
        *buff = NULLCHAR;
    } else {
        FormatDTA( buff, &dh->info, wide );
        if( _FindNext( dh->location, &dh->info, sizeof( dh->info ) ) ) {
            dh->status = RFX_EOF;
        }
    }
}

static void ProcDir( int argc, char **argv )
{
    int         wide;
    int         pause;
    object_loc  src_loc;
    const char  *src;
    dir_handle  *dh;
    int         count;
    int         i;
    int         line;

    wide = 0;
    pause = 0;
    src = NULL;
    for( i = 0; i < argc; ++i ) {
        if( Option( argv[i], 'w' ) ) {
            wide = 1;
        } else if( Option( argv[i], 'p' ) ) {
            pause = 1;
        } else if( src == NULL ) {
            src = argv[i];
        } else {
            WhatDidYouSay();
            return;
        }
    }
    if( src == NULL ) {
        src = "*.*";
    }
    src = RealName( src, &src_loc );
    dh = DirOpenf( src, src_loc );
    count = 0;
    if( dh != NULL ) {
        i = 0;
        line = 0;
        while( !CtrlCHit() ) {
            DirReadf( dh, Buff, wide );
            if( Buff[0] == NULLCHAR )
                break;
            ++i;
            if( wide ) {
                Buff[14] = NULLCHAR;
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
        if( count != 0 ) {
            WriteNL( STD_OUT );
        }
        #define MSG "        x File(s)         x bytes free"
        strcpy( Buff, MSG );
        DItoD( i, Buff + 8 );
        DItoD( dh->free, Buff + 26 );
        WriteText( STD_OUT, Buff, sizeof( MSG ) - 1 );
        DirClosef( dh );
    } else {
        SysSetLclErr( IO_FILE_NOT_FOUND );
    }
}

/**************************************************************************/
/* CD                                                                     */
/**************************************************************************/

static void ProcCD( int argc, char **argv, int crlf )
{
    object_loc  src_loc;
    const char  *src;

    if( argc == 1 ) {
        src = RealRFXName( argv[0], &src_loc );
        if( src_loc == LOC_DEFAULT ) {
            src_loc = DefaultLocation;
        }
    } else {
        src = "";
        src_loc = DefaultLocation;
        if( argc != 0 ) {
            WhatDidYouSay();
        }
    }
    if( *src == NULLCHAR ) {
        if( src_loc == LOC_REMOTE ) {
            WriteStream( STD_OUT, "@R", 2 );
        } else {
            WriteStream( STD_OUT, "@L", 2 );
        }
        Buff[0] = GetDrv( src_loc );
        Buff[1] = ':';
        Buff[2] = '\\';
        WriteStream( STD_OUT, Buff, 3 );
        GetDir( 0, Buff, sizeof( Buff ), src_loc );
        WriteStream( STD_OUT, Buff, strlen( Buff ) );
        if( crlf ) {
            WriteNL( STD_OUT );
        }
    } else if ( ( src[1] == ':' ) && ( src[2] == NULLCHAR ) ) {
        if( src_loc == LOC_REMOTE ) {
            WriteStream( STD_OUT, "@R", 2 );
        } else {
            WriteStream( STD_OUT, "@L", 2 );
        }
        Buff[0] = DRIVECHAR( *src );
        Buff[1] = ':';
        Buff[2] = '\\';
        WriteStream( STD_OUT, Buff, 3 );
        GetDir( Buff[0] - 'A' + 1, Buff, sizeof( Buff ), src_loc );
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

static void ProcMakeDir( int argc, char **argv )
{
    object_loc  src_loc;
    const char  *src;

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

static error_handle   Scratchf( const char *fn, object_loc fnloc )
{
    error_handle    errh;
    char            *endptr;
    rfx_find        info;

    fn = _FileParse( fn, &Parse1 );
    FinishName( fn, &Parse1, fnloc, false );
    Squish( &Parse1, Name1 );
    errh = _FindFirst( Name1, fnloc, IO_NORMAL, &info, sizeof( info ) );
    if( errh != 0 ) {
        SysSetLclErr( IO_FILE_NOT_FOUND );
    } else {
        endptr = Parse1.drive;
        if( Parse1.drive[0] != NULLCHAR ) {
            endptr += 2;
        }
        endptr = CopyStr( Parse1.path, endptr );
        for(;;) {
            CopyStr( info.name, endptr );
            errh = Erase( Parse1.drive, fnloc );
            if( errh != 0 ) {
                TransSetErr( errh );
                return( errh );
            }
            if( _FindNext( fnloc, &info, sizeof( info ) ) ) {
                break;
            }
        }
        _FindClose( fnloc, &info, sizeof( info ) );
    }
    return( errh );
}

static void BuildDFSList( void )
{
    COPYPTR next_last, last, curr;

    for( next_last = NULL; next_last != CopySpecs; ) {
        last = next_last;
        next_last = CopySpecs;
        for( curr = CopySpecs; curr != last; curr = curr->next ) {
            RRecurse( curr->src, curr->dst, curr->src_loc, curr->dst_loc );
            if( ErrorStatus != 0 ) {
                return;
            }
        }
    }
}

static void ProcErase( int argc, char **argv )
{
    object_loc  src_loc;
    const char  *src;
    bool        recursive;
    int         i;
    bool        erased_one;

    recursive = false;
    src = NULL;
    for( i = 0; i < argc; ++i ) {
        if( Option( argv[i], 's' ) ) {
            recursive = true;
        } else if( src == NULL ) {
            src = argv[i];
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
    if( recursive )
        BuildDFSList();
    if( ErrorStatus != 0 )
        return;
    erased_one = false;
    while( CopySpecs != NULL ) {
        ErrorStatus = 0;
        Scratchf( CopySpecs->src, CopySpecs->src_loc );
        if( ErrorStatus == 0 )
            erased_one = true;
        FreeCopySpec( CopySpecs );
    }
    if( erased_one ) {
        SysSetLclErr( IO_OK );
    } else {
        SysSetLclErr( IO_FILE_NOT_FOUND );
    }
}

/**************************************************************************/
/* RMDIR                                                                  */
/**************************************************************************/

static void ProcDelDir( int argc, char **argv )
{
    object_loc      src_loc;
    const char      *src;
    char            *tmp;
    bool            recursive;
    int             i;
    error_handle    errh;

    recursive = false;
    src = NULL;
    for( i = 0; i < argc; ++i ) {
        if( Option( argv[i], 's' ) ) {
            recursive = true;
        } else if( src == NULL ) {
            src = argv[i];
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
        tmp = strstr( CopySpecs->src, "\\*.*" );
        if( tmp != NULL ) {
            *tmp = NULLCHAR;
            errh = RemoveDir( CopySpecs->src, CopySpecs->src_loc );
            if( errh != 0 ) {
                TransSetErr( errh );
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

static bool ProcDrive( int argc, char **argv )
{
    const char  *src_name;
    object_loc  src_loc;
    size_t      len;

    if( argc != 1 )
        return( false );
    src_name = argv[0];
    src_name = RealName( src_name, &src_loc );
    len = strlen( src_name ) - 1;
    if( src_name[len] != ':' )
        return( false );
    if( len == 0 ) {
        DefaultLocation = src_loc;
        return( true );
    } else if( len == 1 ) {
        SetDrv( *src_name, src_loc );
        DefaultLocation = src_loc;
        return( true );
    }
    return( false );
}

/**************************************************************************/
/* MAIN LINEISH                                                           */
/**************************************************************************/

static void CheckError( void )
{
    char    *p;
    char    buff[256];

    if( ErrorStatus != 0 ) {
        p = SysErrMsg( ErrorStatus, buff );
        WriteText( STD_ERR, buff, p - buff );
    }
    while( CopySpecs != NULL ) {
        FreeCopySpec( CopySpecs );
    }
}

static int ProcessArgv( int argc, char **argv, const char *cmd ) {

    ErrorStatus = 0;
    strlwr( argv[0] );
    if( strcmp( argv[0], "copy" ) == 0 ) {
        ProcCopy( argc - 1, argv + 1 );
    } else if( strcmp( argv[0], "dir" ) == 0 ) {
        ProcDir( argc - 1, argv + 1 );
    } else if( strcmp( argv[0], "md"    ) == 0
            || strcmp( argv[0], "mkdir" ) == 0 ) {
        ProcMakeDir( argc - 1, argv + 1 );
    } else if( strcmp( argv[0], "del"    ) == 0
            || strcmp( argv[0], "erase" ) == 0 ) {
        ProcErase( argc - 1, argv + 1 );
    } else if( strcmp( argv[0], "rd"    ) == 0
            || strcmp( argv[0], "rmdir" ) == 0 ) {
        ProcDelDir( argc - 1, argv + 1 );
    } else if( strcmp( argv[0], "exit" ) == 0 ) {
        return( -1 );
    } else if( strcmp( argv[0], "cd"    ) == 0
            || strcmp( argv[0], "chdir" ) == 0 ) {
        ProcCD( argc - 1, argv + 1, true );
    } else if( strcmp( argv[0], "ren" ) == 0 ) {
        ProcRename( argc - 1, argv + 1 );
    } else if( strcmp( argv[0], "type" ) == 0 ) {
        ProcType( argc - 1, argv + 1 );
    } else if( strcmp( argv[0], "?" ) == 0 ) {
        Help();
    } else if( strcmp( argv[0], "" ) == 0 ) {
    } else if( !ProcDrive( argc, argv ) ) {
        if( cmd != NULL ) {
            system( cmd );
        }
    }
    CheckError();
    return( 0 );
}

static int ProcessCmd( const char *cmd ) {

    int         argc;
    char        *argv[20];
    char        *p;
    char        cmd_tokens[255];

    CopyCmd( cmd, cmd_tokens );
    WriteNL( STD_ERR );
    if( cmd_tokens[0] == NULLCHAR )
        return( 0 );
    argc = 0;
    p = strtok( cmd_tokens, " " );
    while( p != NULL ) {
        argv[argc] = p;
        ++argc;
        p = strtok( NULL, " " );
    }
    if( argc ) {
        return( ProcessArgv( argc, argv, cmd ) );
    } else {
        return( 0 );    /* No useful input. */
    }
}

static void OutName( void )
{
    static const char Name[] = { "[RFX] " };

    WriteStream( STD_ERR, Name, sizeof( Name ) - 1 );
}

static void Prompt( void )
{
    const char  *prompt;
    char        drv;
    int         hour, min, sec, hundredths;
    int         year, month, day, weekday;

    prompt = getenv( "PROMPT" );
    if( prompt == NULL )
        prompt = "$n$g";
    OutName();
    while( *prompt != NULLCHAR ) {
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
                strcpy( Buff, Day[weekday] );
                ItoD( month, Buff+5 );
                Buff[7] = '-';
                ItoD( day, Buff+8 );
                Buff[10] = '-';
                DItoD( year, Buff + 14 );
                WriteStream( STD_ERR, Buff, 15 );
                break;
            case 'p':
                ProcCD( 0, NULL, false );
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

static bool isInteractive( file_handle fh, object_loc loc )
{
    if( loc == LOC_REMOTE ) {
        return( false );
    } else {
        return( LocalInteractive( GetSystemHandle( fh ) ) );
    }
}

static void Interactive( void )
{
    char    *p;
    bool    interactive;

    interactive = isInteractive( STD_IN, LOC_LOCAL );
    if( interactive ) {
        Error( banner1w( "Remote File eXchange program", _RFX_VERSION_ ) );
        Error( banner2 );
        Error( banner2a( 1990 ) );
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
            *p = NULLCHAR;
        }
        CtrlCHit();
        if( ProcessCmd( Buff ) != 0 ) {
            break;
        }
    }
}

int main( int argc, char **argv )
{
    InitDbgSwitches();
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
    DefaultLocation = LOC_LOCAL;
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
