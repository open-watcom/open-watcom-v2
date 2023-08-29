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
* Description:  Implementation of remote access routines.
*
****************************************************************************/


#include <stdlib.h>
#include <string.h>
#include <errno.h>
#define INCL_DOSPROCESS
#define INCL_DOSMISC
#define INCL_DOSERRORS
#define INCL_KBD
#include <os2.h>
#include "mon.h"
#include "os2v2acc.h"
#include "trpcomm.h"
#include "softmode.h"
#include "trperr.h"
#include "os2err.h"
#include "doserr.h"
#include "os22path.h"
#include "accmisc.h"


#define TRPH2LH(th)     (HFILE)((th)->handle.u._32[0])
#define LH2TRPH(th,lh)  (th)->handle.u._32[0]=(unsigned_32)lh;(th)->handle.u._32[1]=0

/*
 * globals
 */
PID             Pid = 0;
bool            AtEnd = false;
ULONG           SID;                   /* Session ID */
bool            Remote;
char            UtilBuff[BUFF_SIZE];
HFILE           SaveStdIn;
HFILE           SaveStdOut;
bool            CanExecTask;
scrtype         Screen;

static const ULONG      local_seek_method[] = { FILE_BEGIN, FILE_CURRENT, FILE_END };

trap_retval TRAP_CORE( Read_io )( void )
{
    return( 0 );
}

trap_retval TRAP_CORE( Write_io )( void )
{
    write_io_ret        *ret;

    ret = GetOutPtr( 0 );
    ret->len = 0;
    return( sizeof( *ret ) );
}

trap_retval TRAP_FILE( get_config )( void )
{
    file_get_config_ret *ret;

    ret = GetOutPtr( 0 );

    ret->file.ext_separator = '.';
    ret->file.drv_separator = ':';
    ret->file.path_separator[0] = '\\';
    ret->file.path_separator[1] = '/';
    ret->file.line_eol[0] = '\r';
    ret->file.line_eol[1] = '\n';
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
        openflags = OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS;
        openmode = OPEN_FLAGS_FAIL_ON_ERROR | OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE;
    } else {
        openflags = OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS;
        openmode = OPEN_FLAGS_FAIL_ON_ERROR | OPEN_SHARE_DENYNONE | mode;
    }
    if( flags & OPEN_PRIVATE ) {
        openmode |= OPEN_FLAGS_NOINHERIT;
    }
    rc = DosOpen( name,         /* name */
                &hdl,           /* handle to be filled in */
                &action,        /* action taken */
                0,              /* initial allocation */
                FILE_NORMAL,    /* normal file */
                openflags,      /* open the file */
                openmode,       /* deny-none, inheritance */
                0 );            /* reserved */
    if( rc != 0 )
        return( (-1L << 16) | rc );
    return( hdl );
}

#define READONLY    0
#define WRITEONLY   1
#define READWRITE   2

trap_retval TRAP_FILE( open )( void )
{
    file_open_req       *acc;
    file_open_ret       *ret;
    unsigned_8          flags;
    long                retval;
    int                 mode;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->err = 0;
    mode = READONLY;
    if( acc->mode & DIG_OPEN_WRITE ) {
        mode = WRITEONLY;
        if( acc->mode & DIG_OPEN_READ ) {
            mode = READWRITE;
        }
    }
    flags = OPEN_PRIVATE;
    if( acc->mode & DIG_OPEN_CREATE ) {
        flags |= OPEN_CREATE;
    }
    retval = OpenFile( GetInPtr( sizeof( file_open_req ) ), mode, flags );
    if( retval < 0 ) {
        ret->err = retval;
        retval = 0;
    }
    LH2TRPH( ret, retval );
    return( sizeof( *ret ) );
}

trap_retval TRAP_FILE( seek )( void )
{
    file_seek_req       *acc;
    file_seek_ret       *ret;
    unsigned long       pos;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->err = DosSetFilePtr( TRPH2LH( acc ), acc->pos, local_seek_method[acc->mode], &pos );
    ret->pos = pos;
    return( sizeof( *ret ) );
}


trap_retval TRAP_FILE( read )( void )
{
    ULONG               read_len;
    file_read_req       *acc;
    file_read_ret       *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->err = DosRead( TRPH2LH( acc ), GetOutPtr( sizeof( *ret ) ), acc->len, &read_len );
    return( sizeof( *ret ) + read_len );
}

trap_retval TRAP_FILE( write )( void )
{
    ULONG               written_len;
    file_write_req      *acc;
    file_write_ret      *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->err = DosWrite( TRPH2LH( acc ), GetInPtr( sizeof( *acc ) ), GetTotalSizeIn() - sizeof( *acc ), &written_len );
    ret->len = written_len;
    return( sizeof( *ret ) );
}

trap_retval TRAP_FILE( close )( void )
{
    file_close_req      *acc;
    file_close_ret      *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->err = DosClose( TRPH2LH( acc ) );
    return( sizeof( *ret ) );
}

trap_retval TRAP_FILE( erase )( void )
{
    file_erase_ret      *ret;

    ret = GetOutPtr( 0 );
    ret->err = DosDelete( GetInPtr( sizeof( file_erase_req ) ) );
    return( sizeof( *ret ) );
}

trap_retval TRAP_THREAD( get_extra )( void )
{
    char *ch;

    ch = GetOutPtr( 0 );
    *ch = '\0';
    return( 1 );
}

trap_retval TRAP_CORE( Set_user_screen )( void )
{
    AppSession();
    Screen = USER_SCREEN;
    return( 0 );
}

trap_retval TRAP_CORE( Set_debug_screen )( void )
{
    DebugSession();
    Screen = DEBUG_SCREEN;
    return( 0 );
}

void RestoreScreen( void )
{
    if( Screen == USER_SCREEN ) {
        TRAP_CORE( Set_user_screen )();
    } else {
        TRAP_CORE( Set_debug_screen )();
    }
}

trap_retval TRAP_CORE( Read_user_keyboard )( void )
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
    if( DosMonReg( mon, (void *)&ibuff, (void *)&obuff, 1, SID ) != 0 ) {
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
        if( delay != 0 && ( msecs - starttime ) > delay ) {
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

trap_retval TRAP_CORE( Get_err_text )( void )
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
        while( (ch = *s++) != '\0' ) {
            if( ch == '\n' )
                ch = ' ';
            if( ch != '\r' ) {
                *d++ = ch;
            }
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
    ret->err = 0;
    file_name = GetInPtr( sizeof( redirect_stdout_req ) );
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

trap_retval TRAP_CORE( Redirect_stdin )( void )
{
    return( Redirect( true ) );
}

trap_retval TRAP_CORE( Redirect_stdout )( void )
{
    return( Redirect( false ) );
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

trap_retval TRAP_FILE( run_cmd )( void )
{
    char                *dst;
    char                *src;
    char                *args;
    size_t              len;
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
    len = GetTotalSizeIn() - sizeof( file_run_cmd_req );
    while( len > 0 && *src == ' ' ) {
        ++src;
        --len;
    }
    args = NULL;
    if( len > 0 ) {
        args = UtilBuff + strlen( UtilBuff ) + 1;
        // StrCopyDst return a ptr pointing to the end of string at dst
        dst = StrCopyDst( UtilBuff, args ) + 1;
        dst = StrCopyDst( "/C ", dst );
        while( len-- > 0 ) {
            *dst++ = *src++;
        }
        *dst++ = '\0';
        *dst   = '\0';
    }
    DebugSession();

    savestdin  = 0xffff;
    savestdout = 0xffff;
    DosDupHandle( 0, &savestdin );
    DosDupHandle( 1, &savestdout );
    if( DosOpen( "CON", &console, &act, 0,
            FILE_NORMAL,
            OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,
            OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,
            0 ) == 0 ) {
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


trap_retval TRAP_FILE( file_to_fullpath )( void )
{
    char                        *name;
    char                        *fullname;
    file_string_to_fullpath_req *acc;
    file_string_to_fullpath_ret *ret;

    acc  = GetInPtr( 0 );
    name = GetInPtr( sizeof( *acc ) );
    ret  = GetOutPtr( 0 );
    fullname = GetOutPtr( sizeof( *ret ) );
    ret->err = 0;
    if( FindFilePath( acc->file_type, name, fullname ) == 0 ) {
        ret->err = ENOENT;
    }
    return( sizeof( *ret ) + strlen( fullname ) + 1 );
}

trap_retval TRAP_CORE( Split_cmd )( void )
{
    const char      *cmd;
    const char      *start;
    split_cmd_ret   *ret;
    size_t          len;

    cmd = GetInPtr( sizeof( split_cmd_req ) );
    len = GetTotalSizeIn() - sizeof( split_cmd_req );
    start = cmd;
    ret = GetOutPtr( 0 );
    ret->parm_start = 0;
    while( len > 0 ) {
        switch( *cmd ) {
        case '\"':
            cmd++;
            while( --len > 0 && ( *cmd++ != '\"' ) )
                {}
            if( len == 0 )
                continue;
            switch( *cmd ) {
            CASE_SEPS
                ret->parm_start = 1;
                break;
            }
            len = 0;
            continue;
        CASE_SEPS
            ret->parm_start = 1;
            /* fall through */
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

char *AddDriveAndPath( const char *src, char *buff )
{
    ULONG       drive;
    ULONG       map;
    char        *dst;
    ULONG       len;

    dst = buff;
    DosQueryCurrentDisk( &drive, &map );
    if( src[0] == '\0' || src[1] != ':' ) {
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
    return( StrCopyDst( src, dst ) );
}

void MergeArgvArray( const char *src, char *dst, size_t len )
{
    char    ch;

    while( len-- > 0 ) {
        ch = *src++;
        if( ch == '\0' ) {
            if( len == 0 )
                break;
            ch = ' ';
        }
        *dst++ = ch;
    }
    *dst = '\0';
}
