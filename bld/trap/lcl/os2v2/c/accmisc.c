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
* Description:  Implementation of remote access routines.
*
****************************************************************************/


#include <stdlib.h>
#include <string.h>

#define INCL_DOSPROCESS
#define INCL_DOSMISC
#define INCL_DOSERRORS
#define INCL_KBD
#include <os2.h>
#include "mon.h"
#include "trpimp.h"
#include "dosdebug.h"
#include "softmode.h"
#include "os2trap.h"
#include "trperr.h"
#include "os2err.h"
#include "doserr.h"
#include "os2extx.h"
#include "os2v2acc.h"

/*
 * globals
 */
PID             Pid = 0;
bool            AtEnd = FALSE;
ULONG           SID;                   /* Session ID */
bool            Remote;
char            UtilBuff[BUFF_SIZE];
HFILE           SaveStdIn;
HFILE           SaveStdOut;
bool            CanExecTask;
HMODULE         *ModHandles = NULL;
int             NumModHandles = 0;
int             CurrModHandle = 0;
ULONG           ExceptNum;
scrtype         Screen;

extern  void    DebugSession( void );
extern  void    AppSession( void );


trap_retval ReqRead_io( void )
{
    return( 0 );
}

trap_retval ReqWrite_io( void )
{
    write_io_ret        *ret;

    ret = GetOutPtr( 0 );
    ret->len = 0;
    return( sizeof( *ret ) );
}

trap_retval ReqFile_get_config( void )
{
    file_get_config_ret *ret;

    ret = GetOutPtr( 0 );

    ret->file.ext_separator = '.';
    ret->file.drv_separator = ':';
    ret->file.path_separator[0] = '\\';
    ret->file.path_separator[1] = '/';
    ret->file.newline[0] = '\r';
    ret->file.newline[1] = '\n';
    return( sizeof( *ret ) );
}

long OpenFile( char *name, USHORT mode, int flags )
{
    HFILE       hdl;
    ULONG       action;
    ULONG       openflags;
    ULONG       openmode;
    APIRET      rc;

    if( flags & OPEN_CREATE ) {
        openflags = 0x12;
        openmode = 0x2042;
    } else {
        openflags = 0x01;
        openmode = mode | 0x2040;
    }
    if( flags & OPEN_PRIVATE ) {
        openmode |= 0x80;
    }
    rc = DosOpen( name,         /* name */
                &hdl,           /* handle to be filled in */
                &action,        /* action taken */
                0,              /* initial allocation */
                0,              /* normal file */
                openflags,      /* open the file */
                openmode,       /* deny-none, inheritance */
                0 );            /* reserved */
    if( rc != 0 )
        return( 0xFFFF0000 | rc );
    return( hdl );
}

#define READONLY    0
#define WRITEONLY   1
#define READWRITE   2

trap_retval ReqFile_open( void )
{
    file_open_req       *acc;
    file_open_ret       *ret;
    unsigned_8          flags;
    long                retval;
    static int MapAcc[] = { READONLY, WRITEONLY, READWRITE };

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    if( acc->mode & TF_CREATE ) {
        flags = OPEN_PRIVATE | OPEN_CREATE;
        acc->mode &= ~TF_CREATE;
    } else {
        flags = OPEN_PRIVATE;
    }
    retval = OpenFile( GetInPtr( sizeof( file_open_req ) ),
                      MapAcc[acc->mode - 1], flags );
    if( retval < 0 ) {
        ret->err = retval;
        ret->handle = 0;
    } else {
        ret->err = 0;
        ret->handle = retval;
    }
    return( sizeof( *ret ) );
}

trap_retval ReqFile_seek( void )
{
    file_seek_req       *acc;
    file_seek_ret       *ret;
    unsigned long       pos;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->err = DosSetFilePtr( acc->handle, acc->pos, acc->mode, &pos );
    ret->pos = pos;
    return( sizeof( *ret ) );
}


trap_retval ReqFile_read( void )
{
    ULONG               read_len;
    file_read_req       *acc;
    file_read_ret       *ret;
    char                *buff;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    buff = GetOutPtr( sizeof( *ret ) );
    ret->err = DosRead( acc->handle, buff, acc->len, &read_len );
    return( sizeof( *ret ) + read_len );
}

trap_retval ReqFile_write( void )
{
    ULONG               len;
    ULONG               written_len;
    char                *ptr;
    file_write_req      *acc;
    file_write_ret      *ret;

    acc = GetInPtr( 0 );
    ptr = GetInPtr( sizeof( *acc ) );
    len = GetTotalSize() - sizeof( *acc );
    ret = GetOutPtr( 0 );
    ret->err = DosWrite( acc->handle, ptr, len, &written_len );
    ret->len = written_len;
    return( sizeof( *ret ) );
}

trap_retval ReqFile_close( void )
{
    file_close_req      *acc;
    file_close_ret      *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->err = DosClose( acc->handle );
    return( sizeof( *ret ) );
}

trap_retval ReqFile_erase( void )
{
    file_erase_ret      *ret;

    ret = GetOutPtr( 0 );
    ret->err = DosDelete( (char *)GetInPtr( sizeof( file_erase_req ) ) );
    return( sizeof( *ret ) );
}

trap_retval ReqThread_get_extra( void )
{
    char *ch;

    ch = GetOutPtr( 0 );
    *ch = '\0';
    return( 1 );
}

trap_retval ReqSet_user_screen( void )
{
    AppSession();
    Screen = USER_SCREEN;
    return( 0 );
}

trap_retval ReqSet_debug_screen( void )
{
    DebugSession();
    Screen = DEBUG_SCREEN;
    return( 0 );
}

void RestoreScreen( void )
{
    if( Screen == USER_SCREEN ) {
        ReqSet_user_screen();
    } else {
        ReqSet_debug_screen();
    }
}

trap_retval ReqRead_user_keyboard( void )
{
    HMONITOR    mon;
    struct {
        USHORT  length;
        char    resv[18];
        char    buff[64];
    }           ibuff, obuff;
    USHORT      keysize;
    struct  {
        USHORT      mflag;
        KBDKEYINFO  info;
        USHORT      flag;
    }           key;
    ULONG       delay;
    ULONG       starttime;
    ULONG       msecs;
    USHORT      rc;

    read_user_keyboard_req      *acc;
    read_user_keyboard_ret      *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    delay = acc->wait * 1000UL;
    ret->key = 0;
    if( DosMonOpen( "KBD$", &mon ) != 0 ) {
        if( delay == 0 )
            delay = 5000;
        DosSleep( delay );
        return( sizeof( *ret ) );
    }
    ibuff.length = sizeof( ibuff );
    obuff.length = sizeof( obuff );
    /* Register monitor for debuggee's session */
    if( DosMonReg( mon, (void *)&ibuff, (void *)&obuff, 1, SID) != 0 ) {
        DosMonClose(mon);
        if( delay == 0 )
            delay = 5000;
        DosSleep( delay );
        return( sizeof( *ret ) );
    }
    DosQuerySysInfo( QSV_MS_COUNT, QSV_MS_COUNT, &starttime, sizeof( starttime ) );
    for( ;; ) {
        keysize = sizeof( key );
        rc = DosMonRead( (void *)&ibuff, DCWW_NOWAIT, (void *)&key, &keysize );
        switch( rc ) {
        case 0:
            break;
        default:
            if( delay == 0 )
                delay = 5000;
            /* fall through */
        case ERROR_MON_BUFFER_EMPTY:
            key.flag = 0;
            break;
        }
        DosQuerySysInfo( QSV_MS_COUNT, QSV_MS_COUNT, &msecs, sizeof( msecs ) );
        if( delay != 0 && (msecs - starttime) > delay ) {
            DosMonClose( mon );
            return( sizeof( *ret ) );
        }
        if( key.flag & 0x40 )
            break; /* a key break code */
    }
    DosMonClose( mon );
    ret->key = key.info.chChar + ((unsigned)key.info.chScan << 8);
    return( sizeof( *ret ) );
}

trap_retval ReqGet_err_text( void )
{
    static char *DosErrMsgs[] = {
        #define pick(a,b)   b,
        #include "dosmsgs.h"
        #undef pick
    };
    get_err_text_req    *acc;
    char                *err_txt;
    char                *s;
    char                *d;
    ULONG               msg_len;
    char                ch;
    unsigned            err;
    static char *OS2ErrMsgs[] = {
        #define pick(a,b)   b,
        #include "os2msgs.h"
        #undef pick
    };

    err_txt = GetOutPtr( 0 );
    acc = GetInPtr( 0 );
    err = acc->err & 0xffff;
    if( acc->err & ERROR_OS2_TRAP_FILE_OWN ) {
        strcpy( err_txt, OS2ErrMsgs[err] );
    } else if( DosGetMessage( NULL, 0, err_txt, 80, err, "OSO001.MSG", &msg_len ) == 0 ) {
        err_txt[msg_len] = '\0';
        s = d = err_txt;
        if( s[0] == 'S' && s[1] == 'Y' && s[2] == 'S' ) {
            /* Got the SYSxxxx: at the front. Take it off. */
            s += 3;
            for( ;; ) {
                ch = *s++;
                if( ch == ':' )
                    break;
                if( ch < '0' || ch > '9' ) {
                    s = err_txt;
                    break;
                }
            }
        }
        while( *s == ' ' )
            ++s;
        for( ;; ) {
            ch = *s++;
            if( ch == '\0' )
                break;
            if( ch == '\n' )
                ch = ' ';
            if( ch != '\r' )
                *d++ = ch;
        }
        while( d > err_txt && d[-1] == ' ' )
            --d;
        *d = '\0';
    } else if( err < ERR_LAST ) {
        strcpy( err_txt, DosErrMsgs[err] );
    } else {
        strcpy( err_txt, TRP_ERR_unknown_system_error );
        ultoa( err, err_txt + strlen( err_txt ), 16 );
    }
    return( strlen( err_txt ) + 1 );
}

static trap_elen Redirect( bool input )
{
    HFILE                   std_hndl;
    HFILE                   *var;
    long                    bigint;
    redirect_stdout_ret     *ret;
    char                    *file_name;

    ret = GetOutPtr( 0 );
    file_name = GetInPtr( sizeof( redirect_stdout_req ) );
    ret->err = 0;
    if( input ) {
        std_hndl = 0;
        var = &SaveStdIn;
    } else {
        std_hndl = 1;
        var = &SaveStdOut;
    }
    if( *file_name == '\0' ) {
        if( *var != NIL_DOS_HANDLE ) {
            bigint = TaskDupFile( *var, std_hndl );
            if( bigint < 0 ) {
                ret->err = 1;   // error
            } else {
                TaskCloseFile( *var );
                *var = NIL_DOS_HANDLE;
            }
        }
    } else {
        if( *var == NIL_DOS_HANDLE ) {
            *var = TaskDupFile( std_hndl, NIL_DOS_HANDLE );
        }
        if( input ) {
            bigint = TaskOpenFile( file_name, 0, 0 );
        } else {
            bigint = TaskOpenFile( file_name, 0, OPEN_CREATE );
        }
        if( bigint < 0 ) {
            ret->err = 1;   // error
        } else {
            TaskDupFile( (HFILE)bigint, std_hndl );
            TaskCloseFile( (HFILE)bigint );
        }
    }
    return( sizeof( *ret ) );
}

trap_retval ReqRedirect_stdin( void )
{
    return( Redirect( TRUE ) );
}

trap_retval ReqRedirect_stdout( void )
{
    return( Redirect( FALSE ) );
}

static char *DOSEnvFind( char *src )
{
    char        *env;

    if( DosScanEnv( src, &env ) == 0 )
        return( env );

    return( NULL );
}

static void DOSEnvLkup( char *src, char *dst )
{
    char       *env;

    env = DOSEnvFind( src );
    if( env == NULL ) {
        *dst = '\0';
    } else {
        strcpy( dst, env );
    }
}

char *StrCopy( const char *src, char *dst )
{
    strcpy( dst, src );
    return( strlen( dst ) + dst );
}

trap_retval ReqFile_run_cmd( void )
{
    char                *dst;
    char                *src;
    char                *args;
    int                 length;
    HFILE               savestdin;
    HFILE               savestdout;
    HFILE               console;
    HFILE               new;
    ULONG               act;
    RESULTCODES         res;
    file_run_cmd_ret    *ret;


    DOSEnvLkup( "COMSPEC", UtilBuff );

    src = GetInPtr( sizeof( file_run_cmd_req ) );
    ret = GetOutPtr( 0 );
    length = GetTotalSize() - sizeof( file_run_cmd_req );
    while( length != 0 && *src == ' ' ) {
        ++src;
        --length;
    }
    if( length == 0 ) {
        args = NULL;
    } else {
        args = UtilBuff + strlen( UtilBuff ) + 1;
        // StrCopy return a ptr pointing to the end of string at dst
        dst = StrCopy( UtilBuff, args ) + 1;
        dst = StrCopy( "/C ", dst );
        while( --length >= 0 ) {
            *dst = *src;
            ++dst;
            ++src;
        }
        *dst++ = '\0';
        *dst   = '\0';
    }
    DebugSession();

    savestdin  = 0xffff;
    savestdout = 0xffff;
    DosDupHandle( 0, &savestdin );
    DosDupHandle( 1, &savestdout );
    if( DosOpen( "CON", &console, &act, 0, 0, 0x11, 0x42, 0 ) == 0 ) {
        new = 0;
        DosDupHandle( console, &new );
        new = 1;
        DosDupHandle( console, &new );
        DosClose( console );
    }

    ret->err = DosExecPgm( NULL, 0,     /* don't care about fail name */
                EXEC_SYNC,              /* execflags */
                args,                   /* args */
                NULL,                   /* inherit environment */
                &res,                   /* result codes */
                UtilBuff );             /* pgmname */

    new = 0;
    DosDupHandle( savestdin, &new );
    DosClose( savestdin );
    new = 1;
    DosDupHandle( savestdout, &new );
    DosClose( savestdout );
    RestoreScreen();
    return( sizeof( *ret ) );
}


long TryPath( const char *name, char *end, const char *ext_list )
{
    long         rc;
    char         *p;
    int          done;
    FILEFINDBUF3 info;
    HDIR         hdl = HDIR_SYSTEM;
    ULONG        count = 1;

    done = 0;
    do {
        if( *ext_list == '\0' )
            done = 1;
        for( p = end; *p = *ext_list; ++p, ++ext_list )
            ;
        count = 1;
        rc = DosFindFirst( name, &hdl, FILE_NORMAL, &info, sizeof( info ), &count, FIL_STANDARD );
        if( rc == 0 ) {
            return( 0 );
        }
    } while( !done );
    return( 0xffff0000 | rc );
}

long FindFilePath( const char *pgm, char *buffer, const char *ext_list )
{
    const char  *p;
    char        *p2;
    const char  *p3;
    APIRET      rc;
    int         have_ext;
    int         have_path;

    have_ext = 0;
    have_path = 0;
    for( p = pgm, p2 = buffer; *p2 = *p; ++p, ++p2 ) {
        switch( *p ) {
        case '\\':
        case '/':
        case ':':
            have_path = 1;
            have_ext = 0;
            break;
        case '.':
            have_ext = 1;
            break;
        }
    }
    if( have_ext )
        ext_list = "";
    rc = TryPath( buffer, p2, ext_list );
    if( rc == 0 || have_path )
        return( rc );
    if( DosScanEnv( "PATH", &p2 ) != 0 )
        return( rc );
    p = p2;
    for( ;; ) {
        if( *p == '\0' )
            break;
        p2 = buffer;
        while( *p ) {
            if( *p == ';' )
                break;
            *p2++ = *p++;
        }
        if( (p2[-1] != '\\') && (p2[-1] != '/') ) {
            *p2++ = '\\';
        }
        for( p3 = pgm; *p2 = *p3; ++p2, ++p3 )
            ;
        rc = TryPath( buffer, p2, ext_list );
        if( rc == 0 )
            break;
        if( *p == '\0' )
            break;
        ++p;
    }
    return( rc );
}

trap_retval ReqFile_string_to_fullpath( void )
{
    const char                  *ext_list;
    char                        *name;
    char                        *fullname;
    file_string_to_fullpath_req *acc;
    file_string_to_fullpath_ret *ret;

    acc  = GetInPtr( 0 );
    name = GetInPtr( sizeof( *acc ) );
    ret  = GetOutPtr( 0 );
    fullname = GetOutPtr( sizeof( *ret ) );
    if( acc->file_type != TF_TYPE_EXE ) {
        ext_list = "";
    } else {
        ext_list = OS2ExtList;
    }
    ret->err = FindFilePath( name, fullname, ext_list );
    if( ret->err != 0 ) {
        *fullname = '\0';
    }
    return( sizeof( *ret ) + strlen( fullname ) + 1 );
}

trap_retval ReqSplit_cmd( void )
{
    char            *cmd;
    char            *start;
    split_cmd_ret   *ret;
    trap_elen       len;

    cmd = GetInPtr( sizeof( split_cmd_req ) );
    len = GetTotalSize() - sizeof( split_cmd_req );
    start = cmd;
    ret = GetOutPtr( 0 );
    ret->parm_start = 0;
    while( len != 0 ) {
        switch( *cmd ) {
        case '\"':
            while( --len && (*++cmd != '\"') )
                ;
            if( len != 0 )
                break;
            /* fall down */
        case '\0':
        case ' ':
        case '\t':
            ret->parm_start = 1;
            /* fall down */
        case '/':
        case '=':
        case '(':
        case ';':
        case ',':
            len = 0;
            continue;
        }
        ++cmd;
        --len;
    }
    ret->parm_start += cmd - start;
    ret->cmd_end = cmd - start;
    return( sizeof( *ret ) );
}

char *AddDriveAndPath( char *exe_name, char *buff )
{
    ULONG       drive;
    ULONG       map;
    char        *src;
    char        *dst;
    ULONG       len;

    src = exe_name;
    dst = buff;
    DosQueryCurrentDisk( &drive, &map );
    if( src[0] == '\0' || src[1] == '\0' || src[1] != ':' ) {
        *dst++ = drive - 1 + 'A';
        *dst++ = ':';
    } else {
        *dst++ = *src++;
        *dst++ = *src++;
    }
    if( src[0] != '\\' && src[0] != '/' ) {
        ++dst;
        len = BUFF_SIZE - (dst - buff);
        DosQueryCurrentDir( drive, (PBYTE)dst, &len );
        dst[-1] = '\\';
        if( (*dst == '\\') || (*dst == '\0') ) {
            *dst = '\0';
        } else {
            while( *dst != '\0' ) {
                ++dst;
            }
            *dst++ = '\\';
        }
    }
    return( StrCopy( src, dst ) + 1 );
}

void MergeArgvArray( char *argv, char *dst, unsigned len )
{
    char    ch;
    bool    have_extra;

    have_extra = FALSE;
    for( ;; ) {
        if( len == 0 )
            break;
        ch = *argv;
        if( ch == '\0' )
            ch = ' ';
        *dst = *argv;
        ++dst;
        ++argv;
        --len;
        have_extra = TRUE;
    }
    if( have_extra )
        --dst;
    *dst = '\0';
}
