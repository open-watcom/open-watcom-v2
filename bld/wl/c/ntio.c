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
  NTIO -- POSIX conforming versions of the linker i/o functions

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <process.h>
#include <conio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "linkstd.h"
#include "msg.h"
#include "alloc.h"
#include "wlnkmsg.h"
#include "objio.h"
#include "fileio.h"

#ifdef __OSI__
extern  char    *_BreakFlagPtr;
#endif

static int      OpenFiles;      // the number of open files
static unsigned LastResult;
static bool     CaughtBreak;    // set to TRUE if break hit.

#define TOOMANY EMFILE

extern void LnkFilesInit( void )
/******************************/
// the linker doesn't use stdaux or stdprn, so close these.
{
    OpenFiles = 0;
    CaughtBreak = FALSE;
#if _LINKER != _DLLHOST
    setmode( STDIN_HANDLE, O_BINARY );
    setmode( STDOUT_HANDLE, O_BINARY );
#endif
#if _OS == _OS2V2
    QWrite( STDOUT_HANDLE, " \b",  2, NULL );   // to avoid op quiet problem
#endif
}

extern void PrintIOError( unsigned msg, char *types, char *name )
/***************************************************************/
{
    LnkMsg( msg, types, name, strerror( errno ) );
}

static int DoOpen( char *name, unsigned mode, bool isexe )
/********************************************************/
{
    int     h;

    isexe = isexe;
    CheckBreak();
    mode |= O_BINARY;
    for( ;; ) {
        if( OpenFiles >= MAX_OPEN_FILES ) CleanCachedHandles();
        h = open( name, mode, S_IRUSR | S_IWUSR );
        if( h != -1 ) {
            OpenFiles++;
            break;
        }
        if( errno != TOOMANY ) break;
        if( !CleanCachedHandles() ) break;
    }
    return( h );
}

extern f_handle QOpenR( char *name )
/**********************************/
{
    int     h;

    h = DoOpen( name, O_RDONLY, FALSE );
    if( h != -1 ) return( h );
    LnkMsg( FTL+MSG_CANT_OPEN, "12", name, strerror( errno )  );
    return( NIL_HANDLE );
}

extern f_handle QOpenRW( char *name )
/***********************************/
{
    int     h;

    h = DoOpen( name, O_RDWR | O_CREAT | O_TRUNC, FALSE );
    if( h != -1 ) return( h );
    LnkMsg( FTL+MSG_CANT_OPEN, "12", name, strerror( errno ) );
    return( NIL_HANDLE );
}

extern int ResOpen( const char *path, int access, ... )
/*****************************************************/
/* a simple open cover routine for wres stuff */
{
    int     perm;

    perm = 0666;
    return( open( path, access, perm ) );
}

extern f_handle ExeCreate( char *name )
/**************************************/
{
    int     h;

    h = DoOpen( name, O_RDWR | O_CREAT | O_TRUNC, TRUE );
    if( h != -1 ) return( h );
    LnkMsg( FTL+MSG_CANT_OPEN, "12", name, strerror( errno ) );
    return( NIL_HANDLE );
}

extern f_handle ExeOpen( char *name )
/***********************************/
{
    int     h;

    h = DoOpen( name, O_RDWR, TRUE );
    if( h != -1 ) return( h );
    LnkMsg( FTL+MSG_CANT_OPEN, "12", name, strerror( errno ) );
    return( NIL_HANDLE );
}

    #define doread( f, b, l )  read( f, b, l )
    #define dowrite( f, b, l ) write( f, b, l )

extern unsigned QRead( f_handle file, void *buffer, unsigned len, char *name )
/****************************************************************************/
/* read into far memory */
{
    int     h;

    CheckBreak();
    h = doread( file, buffer, len );
    if( h == -1 ) {
        LnkMsg( ERR+MSG_IO_PROBLEM, "12", name, strerror( errno ) );
    }
    return( h );
}

extern unsigned QWrite( f_handle file, void *buffer, unsigned len, char *name )
/*****************************************************************************/
/* write from far memory */
{
    int     h;
    char    rc_buff[RESOURCE_MAX_SIZE];

    #ifdef _INT_DEBUG
    {
        unsigned long pos = QPos(file);
        if( pos <= SpyWrite && SpyWrite <= pos+len
            && file == Root->outfile->handle) {
            DEBUG((DBG_ALWAYS, "About to write to %s (handle %d) %d bytes at position %d:",
                name, file, len, pos));
            PrintMemDump(buffer, len, DUMP_BYTE);
        }
    }
    #endif

    if( len == 0 ) return( 0 );
    CheckBreak();
    h = dowrite( file, buffer, len );
    if( name != NULL ) {
        if( h == -1 ) {
            LnkMsg( ERR+MSG_IO_PROBLEM, "12", name, strerror( errno ) );
        } else if( h != len ) {
            Msg_Get( MSG_IOERRLIST_7, rc_buff );
            LnkMsg( (FTL+MSG_IO_PROBLEM) & ~OUT_MAP, "12", name, rc_buff );
        }
    }
    return( h );
}

char NLSeq[] = { "\r\n" };

extern void QWriteNL( f_handle file, char *name )
/***********************************************/
{
    QWrite( file, NLSeq, sizeof( NLSeq ) - 1, name );
}

extern void QClose( f_handle file, char *name )
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

extern long QLSeek( f_handle file, long position, int start, char *name )
/***********************************************************************/
{
    long int    h;

    CheckBreak();
    h = lseek( file, position, start );
    if( h == -1 && name != NULL ) {
        LnkMsg( ERR+MSG_IO_PROBLEM, "12", name, strerror( errno ) );
    }
    return( h );
}

extern void QSeek( f_handle file, long position, char *name )
/***********************************************************/
{
    QLSeek( file, position, SEEK_SET, name );
}

extern unsigned long QPos( f_handle file )
/****************************************/
{
    CheckBreak();
    return( lseek( file, 0L, SEEK_CUR ) );
}

extern unsigned long QFileSize( f_handle file )
/*********************************************/
{
    long        result;

    result = filelength( file );
    if( result == -1 ) {
        result = 0;
    }
    return result;
}

extern void QDelete( char *name )
/*******************************/
{
    int   h;

    if( name == NULL ) return;
    h = remove( name );
    if( h == -1 && errno != ENOENT ) { /* file not found is OK */
        LnkMsg( ERR+MSG_IO_PROBLEM, "12", name, strerror( errno ) );
    }
}

extern bool QReadStr( f_handle file, char *dest, unsigned size, char *name )
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

extern bool QIsDevice( f_handle file )
/************************************/
{
    return( isatty( file ) );
}

static f_handle NSOpen( char *name, unsigned mode )
/*************************************************/
{
    int         h;

    h = DoOpen( name, mode, FALSE );
    LastResult = h;
    if( h != -1 ) return( h );
    return( NIL_HANDLE );
}

extern f_handle QObjOpen( char *name )
/************************************/
{
    return( NSOpen( name, O_RDONLY ) );
}

extern f_handle TempFileOpen( char *name )
/****************************************/
{
    return( NSOpen( name, O_RDWR ) );
}

extern int QMakeFileName( char **pos, char *name, char *fname )
/*************************************************************/
{
    char                *pathptr;
    unsigned int        path_len;
    char                *file_ptr;

    pathptr = *pos;
    if( pathptr == NULL ) return( 0 );
    while( *pathptr != '\0' ) {
        if( *pathptr == PATH_LIST_SEP ) *pos = ++pathptr;
        for(;;) {
            if( *pathptr == '\0' ) break;
            if( *pathptr == PATH_LIST_SEP ) break;
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

extern bool QHavePath( char *name )
/*********************************/
{
    return( *name == '\\' || *name == '/' || *(name + 1) == ':' );
}

extern bool QSysHelp( char **cmd_ptr )
{
    cmd_ptr = cmd_ptr;
    return( FALSE );
}

extern bool QModTime( char *name, time_t *time )
/**********************************************/
{
    int         result;
    struct stat buf;

    result = stat( name, &buf );
    *time = buf.st_mtime;
    return result != 0;
}

extern time_t QFModTime( int handle )
/***********************************/
{
    struct stat buf;

    fstat( handle, &buf );
    return buf.st_mtime;
}

extern char WaitForKey( void )
/****************************/
{
    return getch();
}

extern void GetCmdLine( char *buff )
/**********************************/
{
    getcmd( buff );
}

extern void TrapBreak( int sig_num )
/**********************************/
{
    sig_num = sig_num;          // to avoid a warning, will be optimized out.
    CaughtBreak = TRUE;
}

extern void CheckBreak( void )
/****************************/
{
#ifdef __OSI__
    if( *_BreakFlagPtr ) {
        *_BreakFlagPtr = 0;
        LnkMsg( FTL+MSG_BREAK_HIT, NULL );    /* suicides */
    }
#else
    if( CaughtBreak ) {
        CaughtBreak = FALSE;        /* prevent recursion */
        LnkMsg( FTL+MSG_BREAK_HIT, NULL );    /* suicides */
    }
#endif
}

extern void SetBreak( void )
/**************************/
{
}

extern void RestoreBreak( void )
/******************************/
{
}
