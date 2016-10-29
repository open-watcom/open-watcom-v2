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
* Description:  Linker binary i/o interface routines (DOS flavour).
*
****************************************************************************/


#include <stdio.h>
#include <dos.h>
#include <string.h>
#include <stdlib.h>
#include <process.h>
#include <conio.h>
#include <sys/stat.h>
#include <unistd.h>
#include "linkstd.h"
#include "msg.h"
#include "alloc.h"
#include "wlnkmsg.h"
#include "wressetr.h"
#include "tinyio.h"
#include "objio.h"
#include "fileio.h"

typedef enum {
    NOT_HIT,
    BREAK_DETECTED,
    BREAK_HANDLED
} break_status;

static bool         BreakCond;
static break_status CaughtBreak = NOT_HIT;
static int          OpenFiles;          // the number of open files
static unsigned     LastResult;
static int          AuxFilesClosed = false;
static char         Rc_Buffer[RESOURCE_MAX_SIZE];

#define     TOOMANY             4
#define     ACCESS_DENIED       5

void LnkFilesInit( void )
/******************************/
// the linker doesn't use stdaux or stdprn, so close these.
{
    CaughtBreak = NOT_HIT;
    if( !AuxFilesClosed ) {
        OpenFiles = 2;      // will be 0 when done closing stdaux & stdprn.
        QClose( STDAUX_HANDLE, "stdaux" );
        QClose( STDPRN_HANDLE, "stdprn" );
        AuxFilesClosed = true;
        OpenFiles = 0;
    }
}

void PrintIOError( unsigned msg, const char *types, const char *name )
/***************************************************************/
{
    char        rc_buff[RESOURCE_MAX_SIZE];

    Msg_Get( MSG_IOERRLIST_0 + LastResult, rc_buff );
    LnkMsg( msg, types, name, rc_buff );
}

void CheckBreak( void )
/****************************/
{
    if( CaughtBreak == BREAK_DETECTED ) {
        CaughtBreak = BREAK_HANDLED;
        LnkMsg( FTL+MSG_BREAK_HIT, NULL );    /* suicides */
    }
}

static tiny_ret_t DoOpen( const char *name, bool create, unsigned mode )
/**********************************************************************/
{
    tiny_ret_t       h;

    CheckBreak();
    for( ;; ) {
        if( OpenFiles >= MAX_OPEN_FILES )
            CleanCachedHandles();
        if( create ) {
            h  = TinyCreate( name, mode );
        } else {
            h = TinyOpen( name, mode );
        }
        if( TINY_OK( h ) ) {
            OpenFiles++;
            break;
        }
        if( TINY_INFO( h ) != TOOMANY )
            break;
        if( !CleanCachedHandles() ) {
            break;
        }
    }
    return( h );
}


static char *QErrMsg( unsigned status )
/*************************************/
{
    Msg_Get( MSG_IOERRLIST_0 + TINY_INFO( status ), Rc_Buffer );
    return( Rc_Buffer );
}


f_handle QOpenR( const char *name )
/*********************************/
{
    tiny_ret_t h;

    h = DoOpen( name, false, TIO_READ );
    if( TINY_OK( h ) )
        return( (f_handle)TINY_INFO( h ) );
    LnkMsg( FTL+MSG_CANT_OPEN, "12", name, QErrMsg( TINY_INFO( h ) ) );
    return( NIL_FHANDLE );
}


f_handle QOpenRW( const char *name )
/**********************************/
{
    tiny_ret_t h;

    h = DoOpen( name, true, TIO_NORMAL );
    if( TINY_OK( h ) )
        return( (f_handle)TINY_INFO( h ) );
    LnkMsg( FTL+MSG_CANT_OPEN, "12", name, QErrMsg( TINY_INFO( h ) ) );
    return( NIL_FHANDLE );
}

size_t QRead( f_handle file, void *buffer, size_t len, const char *name )
/***********************************************************************/
/* read into far memory */
{
    tiny_ret_t   h;

    CheckBreak();
    h = TinyRead( file, buffer, len );
    if( TINY_ERROR( h ) ) {
        if( name != NULL ) {
            LnkMsg( ERR+MSG_IO_PROBLEM, "12", name, QErrMsg( TINY_INFO( h ) ) );
        } else {
            return( -1 );
        }
    }
    return( TINY_INFO(h) );

}

static size_t TestWrite( f_handle file, const void *buffer, size_t len, const char *name )
/****************************************************************************************/
{
    tiny_ret_t  h;
    char        rc_buff[RESOURCE_MAX_SIZE];

    CheckBreak();
    if( len == 0 )
        return( 0 );

    h = TinyWrite( file, buffer, len );
    if( name != NULL ) {
        if( TINY_ERROR( h ) ) {
            LnkMsg( ERR+MSG_IO_PROBLEM, "12", name, QErrMsg( TINY_INFO( h ) ) );
            return( -1 );
        } else if( TINY_INFO( h ) != len ) {
            if( name != NULL ) {
                Msg_Get( MSG_IOERRLIST_7, rc_buff );
                LnkMsg( (FTL+MSG_IO_PROBLEM) & ~OUT_MAP, "12", name, rc_buff );
            }
        }
    }
    return( TINY_INFO(h) );
}

#define QWRITE_BLOCK_SIZE   (16*1024)

size_t QWrite( f_handle file, const void *buffer, size_t len, const char *name )
/******************************************************************************/
{
    for( ; len > QWRITE_BLOCK_SIZE; len -= QWRITE_BLOCK_SIZE ) {
        if( TestWrite( file, buffer, QWRITE_BLOCK_SIZE, name ) != QWRITE_BLOCK_SIZE )
            return( 0 );
        buffer = (const char *)buffer + QWRITE_BLOCK_SIZE;
    }
    return( TestWrite( file, buffer, len, name ) );
}

char    NLSeq[] = { "\r\n" };

void QWriteNL( f_handle file, const char *name )
/**********************************************/
{
    QWrite( file, NLSeq, sizeof( NLSeq ) - 1, name );
}

void QClose( f_handle file, const char *name )
/********************************************/
/* file close */
{
    tiny_ret_t h;

    CheckBreak();
    h = TinyClose( file );
    OpenFiles--;
    if( TINY_OK( h ) || name == NULL )
        return;
    LnkMsg( ERR+MSG_IO_PROBLEM, "12", name, QErrMsg( TINY_INFO( h ) ) );
}

long QLSeek( f_handle file, long position, int start, const char *name )
/**********************************************************************/
{
    tiny_ret_t  rc;
    long        pos;

    CheckBreak();
    rc = TinyLSeek( file, position, start, (u32_stk_ptr)&pos );
    if( TINY_ERROR( rc ) ) {
        if( name != NULL )
            LnkMsg( ERR+MSG_IO_PROBLEM, "12", name, QErrMsg( TINY_INFO( rc ) ) );
        return( -1L );   /* for xmem checking */
    }
    return( pos );
}

void QSeek( f_handle file, unsigned long position, const char *name )
/*******************************************************************/
{
    QLSeek( file, position, TIO_SEEK_START, name );
}

unsigned long QPos( f_handle file )
/*********************************/
{
    unsigned long pos;

    CheckBreak();
    if( TINY_ERROR( TinyLSeek( file, 0L, TIO_SEEK_CURR, (u32_stk_ptr)&pos ) ) ) {
        return( -1UL );
    }
    return( pos );
}

unsigned long QFileSize( f_handle file )
/**************************************/
{
    unsigned long   curpos;
    unsigned long   size;

    CheckBreak();
    size = 0;
    if( TINY_OK( TinyLSeek( file, 0L, TIO_SEEK_CUR, (u32_stk_ptr)&curpos ) ) ) {
        TinyLSeek( file, 0UL, TIO_SEEK_END, (u32_stk_ptr)&size );
        TinySeek( file, curpos, TIO_SEEK_START );
    }
    return( size );
}

void QDelete( const char *name )
/******************************/
{
    tiny_ret_t   h;

    if( name == NULL )
        return;
    h = TinyDelete( name );
    if( TINY_ERROR( h ) ) {
        if( TINY_INFO( h ) != 2 ) {  /* file not found is OK */
            LnkMsg( ERR+MSG_IO_PROBLEM, "12", name, QErrMsg( TINY_INFO( h ) ) );
        }
    }
}


bool QReadStr( f_handle file, char *dest, size_t size, const char *name )
/***********************************************************************/
/* quick read string (for reading directive file) */
{
    bool            eof;
    char            ch;

    eof = false;
    while( --size > 0 ) {
        if( QRead( file, &ch, 1, name ) == 0 ) {
            eof = true;
            break;
        } else if( ch != '\r' ) {
            *dest++ = ch;
        }
        if( ch == '\n' ) {
            break;
        }
    }
    *dest = '\0';
    return( eof );
}

bool QIsDevice( f_handle file )
/*****************************/
{
    if( TinyGetDeviceInfo( file ) & TIO_CTL_DEVICE ) {
        return( true );
    } else {
        return( false );  // don't write the prompt if input not from stdin
    }
}

f_handle ExeCreate( const char *name )
/************************************/
{
    tiny_ret_t      h;

    h = DoOpen( name, true, TIO_NORMAL );
    LastResult = TINY_INFO( h );
    if( TINY_OK( h ) )
        return( TINY_INFO( h ) );
    return( NIL_FHANDLE );
}

static f_handle NSOpen( const char *name, unsigned mode )
/*******************************************************/
{
    tiny_ret_t       h;

    h = DoOpen( name, false, mode );
    LastResult = TINY_INFO( h );
    if( TINY_OK( h ) )
        return( TINY_INFO( h ) );
    return( NIL_FHANDLE );
}

f_handle ExeOpen( const char *name )
/**********************************/
{
    return( NSOpen( name, TIO_READ_WRITE ) );
}

f_handle QObjOpen( const char *name )
/***********************************/
{
    return( NSOpen( name, TIO_READ ) );
}

f_handle TempFileOpen( const char *name )
/***************************************/
// open without suiciding. Don't create the file
{
    return( NSOpen( name, TIO_READ ) );
}

bool QSysHelp( char **cmd_ptr )
{
    cmd_ptr = cmd_ptr;
    return( false );
}

bool QModTime( const char *name, time_t *time )
/*********************************************/
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
    return( getch() );
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
    if( CaughtBreak != BREAK_HANDLED ) {
        CaughtBreak = BREAK_DETECTED;
    }
}

void SetBreak( void )
/**************************/
{
    BreakCond = TinyGetCtrlBreak();
    TinySetCtrlBreak( 1 );
}

void RestoreBreak( void )
/******************************/
{
    TinySetCtrlBreak( BreakCond );
}
