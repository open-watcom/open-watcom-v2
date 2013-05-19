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
* Description:  POSIX conforming versions of the linker I/O functions.
*
****************************************************************************/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "wio.h"
#include <termios.h>
#ifdef __WATCOMC__
#include <process.h>
#endif
#include "linkstd.h"
#include "msg.h"
#include "alloc.h"
#if defined( __QNX__ )
#include <sys/proc_msg.h>
#endif
#include "wlnkmsg.h"
#include "wressetr.h"   // from wres project
#include "objio.h"
#include "alloc.h"
#include "fileio.h"

static int      OpenFiles;          // the number of open files
static unsigned LastResult;
static bool     CaughtBreak = FALSE;    // set to TRUE if break hit.

#define TOOMANY EMFILE

void LnkFilesInit( void )
/******************************/
{
    OpenFiles = 0;
}

void PrintIOError( unsigned msg, char *types, char *name )
/***************************************************************/
{
    LnkMsg( msg, types, name, strerror( errno ) );
}

static int DoOpen( char *name, unsigned mode, bool isexe )
/********************************************************/
{
    int         h;
    int         pmode;
    struct stat st;

    CheckBreak();
    pmode = PMODE_RW;
    if( isexe )
        pmode = PMODE_RWX;
    mode |= O_BINARY;
    for( ;; ) {
        if( OpenFiles >= MAX_OPEN_FILES )
            CleanCachedHandles();
        if ( ( mode & O_CREAT ) && !stat( name, &st) )
            unlink( name );
        h = open( name, mode, pmode );
        if( h != -1 ) {
            OpenFiles++;
            break;
        }
        if( errno == ENOMEM ) {
            if( !FreeUpMemory() ) {
                break;
            }
        } else {
            if( errno != TOOMANY )
                break;
            if( !CleanCachedHandles() ) {
                break;
            }
        }
    }
    return( h );
}

f_handle QOpenR( char *name )
/**********************************/
{
    int     h;

    h = DoOpen( name, O_RDONLY, FALSE );
    if( h != -1 ) return( h );
    LnkMsg( FTL+MSG_CANT_OPEN, "12", name, strerror( errno )  );
    return( NIL_FHANDLE );
}

f_handle QOpenRW( char *name )
/***********************************/
{
    int     h;

    h = DoOpen( name, O_RDWR | O_CREAT | O_TRUNC, FALSE );
    if( h != -1 ) return( h );
    LnkMsg( FTL+MSG_CANT_OPEN, "12", name, strerror( errno ) );
    return( NIL_FHANDLE );
}

f_handle ExeCreate( char *name )
/**************************************/
{
    int     h;

    h = DoOpen( name, O_RDWR | O_CREAT | O_TRUNC, TRUE );
    if( h != -1 ) return( h );
    LnkMsg( FTL+MSG_CANT_OPEN, "12", name, strerror( errno ) );
    return( NIL_FHANDLE );
}

f_handle ExeOpen( char *name )
/***********************************/
{
    int     h;

    h = DoOpen( name, O_RDWR, TRUE );
    if( h != -1 ) return( h );
    LnkMsg( FTL+MSG_CANT_OPEN, "12", name, strerror( errno ) );
    return( NIL_FHANDLE );
}

#if defined( __QNX__ )
/*
    QNX only allows 32K-1 bytes to be read/written at any one time, so bust
    up any I/O larger than that.
*/
#define MAX_OS_TRANSFER (32U*1024 - 512)

static unsigned doread( int file, void *buffer, unsigned len )
{
    unsigned    total;
    int         h;
    int         amount;

    total = 0;
    for( ;; ) {
        if( len == 0 ) return( total );
        amount = (len > MAX_OS_TRANSFER) ? MAX_OS_TRANSFER : len;
        h = read( file, buffer, amount );
        if( h < 0 ) return( h );
        total += h;
        if( h != amount ) return( total );
        buffer = (char *)buffer + amount;
        len -= amount;
    }
}

static unsigned dowrite( int file, void *buffer, unsigned len )
{
    unsigned    total;
    int         h;
    int         amount;

    total = 0;
    for( ;; ) {
        if( len == 0 ) return( total );
        amount = (len > MAX_OS_TRANSFER) ? MAX_OS_TRANSFER : len;
        h = write( file, buffer, amount );
        if( h < 0 ) return( h );
        total += h;
        if( h != amount ) return( total );
        buffer = (char *)buffer + amount;
        len -= amount;
    }
}
#else
    #define doread( f, b, l )  read( f, b, l )
    #define dowrite( f, b, l ) write( f, b, l )
#endif


unsigned QRead( f_handle file, void *buffer, unsigned len, char *name )
/****************************************************************************/
{
    int     h;

    CheckBreak();
    h = doread( file, buffer, len );
    if( h == -1 ) {
        LnkMsg( ERR+MSG_IO_PROBLEM, "12", name, strerror( errno ) );
    }
    return( h );
}

unsigned QWrite( f_handle file, void *buffer, unsigned len, char *name )
/*****************************************************************************/
{
    int     h;
    char    rc_buff[RESOURCE_MAX_SIZE];

    if( len == 0 )
        return( 0 );
    CheckBreak();
    h = dowrite( file, buffer, len );
    if( h < 0 ) {
        LnkMsg( ERR+MSG_IO_PROBLEM, "12", name, strerror( errno ) );
    } else if( h != len ) {
        if( name != NULL ) {
            Msg_Get( MSG_IOERRLIST_7, rc_buff );
            LnkMsg( (FTL+MSG_IO_PROBLEM) & ~OUT_MAP, "12", name, rc_buff );
        }
    }
    return( h );
}

char NLSeq[] = { "\n" };

void QWriteNL( f_handle file, char *name )
/***********************************************/
{
    QWrite( file, NLSeq, sizeof( NLSeq ) - 1, name );
}

void QClose( f_handle file, char *name )
/*********************************************/
/* file close */
{
    int         h;

    CheckBreak();
    h = close( file );
    OpenFiles--;
    if( h != -1 ) return;
    LnkMsg( ERR+MSG_IO_PROBLEM, "12", name, strerror( errno ) );
}

long QLSeek( f_handle file, long position, int start, char *name )
/***********************************************************************/
/* do a seek from a particular point */
{
    long int    h;

    CheckBreak();
    h = lseek( file, position, start );
    if( h == -1 && name != NULL ) {
        LnkMsg( ERR+MSG_IO_PROBLEM, "12", name, strerror( errno ) );
    }
    return( h );
}

void QSeek( f_handle file, unsigned long position, char *name )
/*************************************************************/
{
    QLSeek( file, position, SEEK_SET, name );
}

unsigned long QPos( f_handle file )
/*********************************/
{
    CheckBreak();
    return( lseek( file, 0L, SEEK_CUR ) );
}

unsigned long QFileSize( f_handle file )
/**************************************/
{
    long    curpos;
    long    size;

    CheckBreak();
    size = 0;
    curpos = lseek( file, 0L, SEEK_CUR  );
    if( curpos != -1L ) {
        size = lseek( file, 0L, SEEK_END  );
        if( size == -1L ) {
            size = 0;
        }
        lseek( file, curpos, SEEK_SET );
    }
    return( size );
}

void QDelete( char *name )
/*******************************/
{
    int   h;

    if( name == NULL ) return;
    h = remove( name );
    if( h == -1 && errno != ENOENT ) { /* file not found is OK */
        LnkMsg( ERR+MSG_IO_PROBLEM, "12", name, strerror( errno ) );
    }
}

bool QReadStr( f_handle file, char *dest, unsigned size, char *name )
/**************************************************************************/
/* quick read string (for reading directive file) */
{
    bool            eof;
    char            ch;

    eof = FALSE;
    while( --size > 0 ) {
        if( QRead( file, &ch, 1, name ) == 0 ) {
            eof = TRUE;
            break;
        } else if( ch != '\r' ) {
            *dest++ = ch;
        }
        if( ch == '\n' ) break;
    }
    *dest = '\0';
    return( eof );
}

bool QIsDevice( f_handle file )
/************************************/
{
    struct stat     st;
    if( fstat( file, &st ) != 0 ) return( FALSE );
    return( S_ISCHR( st.st_mode ) != 0 );
}

static f_handle NSOpen( char *name, unsigned mode )
/*************************************************/
{
    int         h;

    h = DoOpen( name, mode, FALSE );
    LastResult = h;
    if( h != -1 ) return( h );
    return( NIL_FHANDLE );
}

f_handle QObjOpen( char *name )
/************************************/
{
    return( NSOpen( name, O_RDONLY ) );
}

f_handle TempFileOpen( char *name )
/****************************************/
{
    return( NSOpen( name, O_RDWR ) );
}

int QMakeFileName( char **pos, char *name, char *fname )
/*************************************************************/
{
    char                *pathptr;
    unsigned int        path_len;
    char                *file_ptr;

    pathptr = *pos;
    if( pathptr == NULL )
        return( 0 );
    while( *pathptr != '\0' ) {
        if( IS_PATH_LIST_SEP( *pathptr ) )
            *pos = ++pathptr;
        for(;;) {
            if( *pathptr == '\0' )
                break;
            if( IS_PATH_LIST_SEP( *pathptr ) )
                break;
            pathptr++;
        }
        path_len = pathptr - *pos;
        if( path_len != 0 ) {
            memcpy( fname, *pos, path_len );
            file_ptr = fname + path_len;
            switch( file_ptr[ -1 ] ) {
            CASE_PATH_SEP:
                break;
            default:
                *file_ptr++ = PATH_SEP;
                break;
            }
            strcpy( file_ptr, name );
            *pos = pathptr;
            return( 1 );
        }
        *pos = pathptr;
    }
    return( 0 );
}

bool QHavePath( char *name )
/*********************************/
{
    return( IS_PATH_SEP( *name ) );
}

bool QSysHelp( char **cmd_ptr )
/************************************/
{
#if defined( __I86__ ) && defined( __QNX__ )
//    extern  struct _proc_spawn *__cmd;
    char    *p;

    cmd_ptr = cmd_ptr;
    p = __cmd->data;
    while( *p ) ++p; /* skip over executable name */
    return( p[1] == '?' );
#else
    cmd_ptr = cmd_ptr;
    return FALSE;
#endif
}

bool QModTime( char *name, time_t *time )
/**********************************************/
{
    int         result;
    struct stat buf;

    result = stat( name, &buf );
    *time = buf.st_mtime;
    return result != 0;
}

time_t QFModTime( int handle )
/***********************************/
{
    struct stat buf;

    fstat( handle, &buf );
    return buf.st_mtime;
}

int WaitForKey( void )
/****************************/
{
    struct termios  old;
    struct termios  new;
    unsigned char   result;

    tcgetattr( 0, &old );
    new = old;
    new.c_lflag &= ~(ICANON | ECHO);
    new.c_cc[VMIN] = 1;
    new.c_cc[VTIME] = 0;
    tcsetattr( 0, TCSANOW, &new );
    read( 0, &result, 1 );
    tcsetattr( 0, TCSANOW, &old );
    return( result );
}

void GetCmdLine( char *buff )
/**********************************/
{
    getcmd( buff );
}

void TrapBreak( int sig_num )
/**********************************/
{
    sig_num = sig_num;          // to avoid a warning, will be optimized out.
    CaughtBreak = TRUE;
}

void CheckBreak( void )
/****************************/
{
    if( CaughtBreak ) {
        CaughtBreak = FALSE;        /* prevent recursion */
        LnkMsg( FTL+MSG_BREAK_HIT, NULL );    /* suicides */
    }
}

void SetBreak( void )
/**************************/
{
}

void RestoreBreak( void )
/******************************/
{
}
