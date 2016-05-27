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
* Description:  Linker binary i/o interface routines (ZDOS flavour).
*
****************************************************************************/

#include <stdio.h>
#include <dos.h>
#include <string.h>
#include <stdlib.h>
#include <process.h>
#include <conio.h>
#include <unistd.h>
#include <zdos.h>
#include "linkstd.h"
#include "msg.h"
#include "alloc.h"
#include "wlnkmsg.h"
#include "wressetr.h"
#include "objio.h"
#include "fileio.h"

typedef enum {
    NOT_HIT,
    BREAK_DETECTED,
    BREAK_HANDLED
} break_status;

static bool         BreakCond;
static break_status CaughtBreak = NOT_HIT;
static int          OpenFiles   = 0;                    // the number of open files
static unsigned     LastResult;
static char         Rc_Buffer[RESOURCE_MAX_SIZE];

#define     TOOMANY             4
#define     ACCESS_DENIED       5

extern void         CheckBreak( void );

void LnkFilesInit( void )
/***********************/
{
}

void PrintIOError( unsigned msg, char *types, char *name )
/********************************************************/
{
    char    rc_buff[RESOURCE_MAX_SIZE];

    Msg_Get( MSG_IOERRLIST_0 + LastResult, rc_buff );
    LnkMsg( msg, types, name, rc_buff );
}

static int DoOpen( char *name, bool create, unsigned mode )
/*********************************************************/
{
    int h;

    CheckBreak();
    for( ;; ) {
        if( OpenFiles >= MAX_OPEN_FILES ) CleanCachedHandles();
        if( create ) {
            h  = DosCreateFile( mode, name );
        } else {
            h = DosOpenFile( mode, name );
        }
        if( h  > 0 ) {
            OpenFiles++;
            break;
        }
        if( h != -TOOMANY )
            break;
        if( !CleanCachedHandles() )
            break;
    }
    return( h );
}


static char *QErrMsg( unsigned status )
/*************************************/
{
    Msg_Get( MSG_IOERRLIST_0 + ( status & 0x7fffffff ), Rc_Buffer );
    return( Rc_Buffer );
}

f_handle QOpenR( char *name )
/***************************/
{
    int h;

    h = DoOpen( name, false, MODE_READ_ONLY );
    if( h > 0 )
        return( h );
    LnkMsg( FTL+MSG_CANT_OPEN, "12", name, QErrMsg( -h ) );
    return( NIL_FHANDLE );
}

f_handle QOpenRW( char *name )
/****************************/
{
    int h;

    h = DoOpen( name, true, _A_NORMAL );
    if( h > 0 )
        return( h );
    LnkMsg( FTL+MSG_CANT_OPEN, "12", name, QErrMsg( -h ) );
    return( NIL_FHANDLE );
}

unsigned QRead( f_handle file, void *buffer, unsigned len, char *name )
/*********************************************************************/
{
/* read into far memory */
    int h;

    CheckBreak();
    h = DosReadFile( len, buffer, file );
    if( h < 0 ) {
        if( name != NULL ) {
            LnkMsg( ERR+MSG_IO_PROBLEM, "12", name, QErrMsg( -h ) );
        } else {
            return( -1 );
        }
    }
    return( h );
}

static unsigned TestWrite( f_handle file, void *buffer, unsigned len, char *name )
/*****************************************************************************/
{
    int     h;
    char    rc_buff[RESOURCE_MAX_SIZE];

    CheckBreak();
    if( len == 0 )
        return( 0 );

    h = DosWriteFile( len, buffer, file );
    if( name != NULL ) {
        if( h < 0 ) {
            LnkMsg( ERR+MSG_IO_PROBLEM, "12", name, QErrMsg( -h ) );
            return( ~0 );
        } else if( h != len ) {
            Msg_Get( MSG_IOERRLIST_7, rc_buff );
            LnkMsg( (FTL+MSG_IO_PROBLEM) & ~OUT_MAP, "12", name, rc_buff );
        }
    }
    return( h );
}

#define QWRITE_BLOCK_SIZE  (16*1024)

unsigned QWrite( f_handle file, void *buffer, unsigned len, char *name )
/**********************************************************************/
{
    for( ; len > QWRITE_BLOCK_SIZE; len -= QWRITE_BLOCK_SIZE ) {
        if( TestWrite( file, buffer, QWRITE_BLOCK_SIZE, name ) != QWRITE_BLOCK_SIZE )
            return 0;
        buffer = (char *)buffer + QWRITE_BLOCK_SIZE;
    }
    return TestWrite( file, buffer, len, name );
}

char    NLSeq[] = { "\r\n" };

void QWriteNL( f_handle file, char *name )
/****************************************/
{
    QWrite( file, NLSeq, sizeof( NLSeq ) - 1, name );
}

void QClose( f_handle file, char *name )
/**************************************/
{
/* file close */
    int h;

    CheckBreak();
    h = DosCloseFile( file );
    OpenFiles--;
    if( h >= 0  || name == NULL )
        return;
    LnkMsg( ERR+MSG_IO_PROBLEM, "12", name, QErrMsg( -h ) );
}

long QLSeek( f_handle file, long position, int start, char *name )
/****************************************************************/
{
    int         h;
    unsigned    pos;

    CheckBreak();

    h = DosSetFilePosition ( start, position, file, &pos );
    if( h < 0 && name != NULL ) {
        LnkMsg( ERR+MSG_IO_PROBLEM, "12", name, QErrMsg( -h ) );
        return( -1 );   /* for xmem checking */
    }
    return( pos );
}

void QSeek( f_handle file, long position, char *name )
/****************************************************/
{
    QLSeek( file, position, SEEK_FROM_START, name );
}

unsigned long QPos( f_handle file )
/*********************************/
{
    unsigned    pos;

    DosSetFilePosition ( SEEK_FROM_CURRENT, 0, file, &pos );
    return( pos );
}

unsigned long QFileSize( f_handle file )
/**************************************/
{
    unsigned    curpos;
    unsigned    size;

    CheckBreak();
    DosSetFilePosition ( SEEK_FROM_CURRENT, 0, file, &curpos );
    DosSetFilePosition ( SEEK_FROM_END, 0, file, &size );
    DosSetFilePosition ( SEEK_FROM_START, curpos, file, &curpos );
    return( size );
}

void QDelete( char *name )
/************************/
{
    int h;

    if( name == NULL )
        return;
    h = DosDeleteFile( name );
    if( h < 0 ) {
        if( h != -2 ) {  /* file not found is OK */
            LnkMsg( ERR+MSG_IO_PROBLEM, "12", name, QErrMsg( -h ) );
        }
    }
}

bool QReadStr( f_handle file, char *dest, unsigned size, char *name )
/*******************************************************************/
{
/* quick read string (for reading directive file) */
    bool    eof;
    char    ch;

    eof = false;
    while( --size > 0 ) {
        if( QRead( file, &ch, 1, name ) == 0 ) {
            eof = true;
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
/*****************************/
{
    if ( DosIoctlGetDeviceInfo ( file ) & 0x80 )
        return( true );
    else
        return( false );  // don't write the prompt if input not from stdin
}

f_handle ExeCreate( char *name )
/******************************/
{
    int h;

    h = DoOpen( name, true, _A_NORMAL );
    LastResult = h & 0x7fffffff;
    if( h > 0 )
        return( h );
    return( NIL_FHANDLE );
}

static f_handle NSOpen( char *name, unsigned mode )
/*************************************************/
{
    int h;

    h = DoOpen( name, false, mode );
    LastResult = h & 0x7fffffff;
    if( h > 0 )
        return( h );
    return( NIL_FHANDLE );
}

f_handle ExeOpen( char *name )
/****************************/
{
    return( NSOpen( name, MODE_READ_AND_WRITE ) );
}

f_handle QObjOpen( char *name )
/*****************************/
{
    return( NSOpen( name, MODE_READ_ONLY ) );
}

f_handle TempFileOpen( char *name )
/*********************************/
{
// open without suiciding. Don't create the file
    return( NSOpen( name, MODE_READ_ONLY ) );
}

bool QSysHelp( char **cmd_ptr )
{
    cmd_ptr = cmd_ptr;
    return( false );
}

bool QModTime( char *name, time_t *time )
/***************************************/
{
    int         result;
    struct stat buf;

    result = stat( name, &buf );
    *time = buf.st_mtime;
    return result != 0;
}

time_t QFModTime( int handle )
/****************************/
{
    struct stat buf;

    fstat( handle, &buf );
    return buf.st_mtime;
}

int WaitForKey( void )
/********************/
{
    return( getch() );
}

void GetCmdLine( char *buff )
/***************************/
{
    getcmd( buff );
}

void TrapBreak( int sig_num )
/***************************/
{
    sig_num = sig_num;          // to avoid a warning, will be optimized out.
    if( CaughtBreak != BREAK_HANDLED ) {
        CaughtBreak = BREAK_DETECTED;
    }
}

void CheckBreak( void )
/*********************/
{
    if( CaughtBreak == BREAK_DETECTED ) {
        CaughtBreak = BREAK_HANDLED;
        LnkMsg( FTL+MSG_BREAK_HIT, NULL );    /* suicides */
    }
}

void SetBreak( void )
/*******************/
{
    BreakCond = DosGetBreakFlag();
    DosSetBreakFlag( 1 );
}

void RestoreBreak( void )
/***********************/
{
    DosSetBreakFlag( BreakCond );
}
