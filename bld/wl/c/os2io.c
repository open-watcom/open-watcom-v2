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
  OS2IO -- linker binary i/o interface routines for os/2

*/

#include <stdio.h>
#include <stddef.h>
#include <dos.h>
#define INCL_BASE
#include <os2.h>
#include <string.h>
#include <stdlib.h>
#include <process.h>
#include <conio.h>
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

static bool         DieNow = FALSE;
static bool         BreakCond;
static int          OpenFiles;              // the number of open files
static break_status CaughtBreak = NOT_HIT;
static USHORT       LastResult;             // the error number of the last io op.

#define     TOOMANY             4
#define     ACCESS_DENIED       5
#define     LAST_MESSAGE        6

enum {                  // open modes
    READ_ACCESS = 0,
    WRITE_ACCESS,
    READWRITE_ACCESS,
    DENY_WRITE = 0x0020
};

enum {              // actions to take if the file does/does not exist
    OPEN_THE_FILE = 1,
    TRUNCATE_FILE = 2,
    CREATE_FILE = 0x10
};

extern void LnkFilesInit( void )
/******************************/
// the linker doesn't use stdaux or stdprn, so close these.
{
#if 0
    OpenFiles = 2;      // will be 0 when done closing stdaux & stdprn.
    QClose( STDAUX_HANDLE, "stdaux" );
    QClose( STDPRN_HANDLE, "stdprn" );
#endif
    OpenFiles = 0;
}

extern void PrintIOError( unsigned msg, char *types, char *name )
/***************************************************************/
{
    char        rc_buff[RESOURCE_MAX_SIZE];

    if( LastResult > LAST_MESSAGE ) {
        if( (msg & NUM_MSK) == MSG_CANT_OPEN ) {
            msg = (msg & ~NUM_MSK) + MSG_CANT_OPEN_NO_REASON;
        } else {
            msg = (msg & ~NUM_MSK) + MSG_IO_PROBLEM_NO_REASON;
        }
        LnkMsg( msg, "s", name );
    } else {
        Msg_Get( MSG_IOERRLIST_0 + LastResult, rc_buff );
        LnkMsg( msg, types, name, rc_buff );
    }
}

static HFILE DoOpen( char *name, bool create, USHORT mode, bool suicide )
/***********************************************************************/
{
    HFILE   h;
    USHORT  action;
    USHORT  openflags;
    tiny_ret_t  retval;

    CheckBreak();
    if( LinkState & RUNNING_OS2_FLAG ) {
//        mode |= DENY_WRITE;
        mode |= 0x40;
    }
    if( create ) {
        openflags = TRUNCATE_FILE | CREATE_FILE;
    } else {
        openflags = OPEN_THE_FILE;
    }
    for( ;; ) {
        if( OpenFiles >= MAX_OPEN_FILES ) CleanCachedHandles();
        if( LinkState & RUNNING_OS2_FLAG ) {
            LastResult = DosOpen( name, &h, &action, 0, 0, openflags, mode, 0 );
        } else {
            if( create ) {
                retval = TinyCreate( name, 0 );
            } else {
                retval = TinyOpen( name, mode );
            }
            if( TINY_OK( retval ) ) {
                h = TINY_INFO( retval );
                LastResult = 0;
            } else {
                LastResult = TINY_INFO( retval );
            }
        }
        if( LastResult == 0 ) {
            OpenFiles++;
            break;
        }
        if( LastResult != TOOMANY ) break;
        if( !CleanCachedHandles() ) break;
    }
    if( LastResult != 0 ) {
        if( suicide ) {
            PrintIOError( FTL+MSG_CANT_OPEN, "12", name );
        } else {
            h = NIL_HANDLE;
        }
    }
    return( h );
}

extern f_handle QOpenR( char *name )
/**********************************/
{
    return( DoOpen( name, FALSE, READ_ACCESS, TRUE ) );
}

extern f_handle QOpenRW( char *name )
/***********************************/
{
    return( DoOpen( name, TRUE, READWRITE_ACCESS, TRUE ) );
}

extern f_handle ExeCreate( char *name )
/*************************************/
{
    return( DoOpen( name, TRUE, READWRITE_ACCESS, FALSE ) );
}

extern f_handle ExeOpen( char *name )
/***********************************/
{
    return( DoOpen( name, FALSE, READWRITE_ACCESS, FALSE ) );
}

extern unsigned QRead( f_handle file, void *buffer, unsigned len, char *name )
/****************************************************************************/
{
    USHORT  numread;

    CheckBreak();
    DieNow = TRUE;      // make sure we die for ctrl-c's here
    LastResult = DosRead( file, buffer, len, &numread );
    DieNow = FALSE;
    if( LastResult > 0 ) {
        if( name != NULL ) {
            PrintIOError( ERR+MSG_IO_PROBLEM, "12", name );
        }
        numread = 0;
    }
    return( numread );
}

extern unsigned QWrite( f_handle file, void *buffer, unsigned len, char *name )
/*****************************************************************************/
{
    USHORT  numwritten;
    char    rc_buff[RESOURCE_MAX_SIZE];

    CheckBreak();
    if( len == 0) return( 0 );
    LastResult = DosWrite( file, buffer, len, &numwritten );
    if( LastResult > 0 ) {
        if( name != NULL ) {
            PrintIOError( ERR+MSG_IO_PROBLEM, "12", name );
        }
        numwritten = 0xFFFF;
    } else if( numwritten != len ) {
        if( name != NULL ) {
            Msg_Get( MSG_IOERRLIST_7, rc_buff );
            LnkMsg( (FTL+MSG_IO_PROBLEM) & ~OUT_MAP, "12", name, rc_buff );
        }
    }
    return( numwritten );
}

char    NLSeq[] = { "\r\n" };

extern void QWriteNL( f_handle file, char *name )
/***********************************************/
{
    QWrite( file, NLSeq, sizeof( NLSeq ) - 1, name );
}

extern void QClose( f_handle file, char *name )
/*********************************************/
/* file close */
{
    CheckBreak();
    LastResult = DosClose( file );
    OpenFiles--;
    if( LastResult > 0 && name != NULL ) {
        PrintIOError( ERR+MSG_IO_PROBLEM, "12", name );
    }
}

/* note: code relies on these values being the same as the lseek values */

#define SEEK_FROM_START 0
#define SEEK_FROM_CURR 1
#define SEEK_FROM_END 2

extern long QLSeek( f_handle file, long position, int start, char *name )
/***********************************************************************/
{
    ULONG   newpos;

    CheckBreak();
    LastResult = DosChgFilePtr( file, position, start, &newpos );
    if( LastResult > 0 && name != NULL ) {
        PrintIOError( ERR+MSG_IO_PROBLEM, "12", name );
        return( -1 );   /* for xmem checking */
    }
    return( newpos );
}

extern void QSeek( f_handle file, long position, char *name )
/***********************************************************/
{
    QLSeek( file, position, SEEK_FROM_START, name );
}

extern unsigned long QPos( f_handle file )
/****************************************/
{
    ULONG   newpos;

    CheckBreak();
    LastResult = DosChgFilePtr( file, 0L, SEEK_FROM_CURR, &newpos );
    return( newpos );
}

extern unsigned long QFileSize( f_handle file )
/*********************************************/
{
    unsigned long   curpos;
    unsigned long   dummy;
    unsigned long   size;

    curpos = QPos( file );
    DosChgFilePtr( file, 0, SEEK_FROM_END, &size );
    DosChgFilePtr( file, curpos, SEEK_FROM_START, &dummy );
    return( size );
}

extern void QDelete( char *name )
/*******************************/
{
    LastResult = DosDelete( name, 0 );
    if( LastResult == 2 ) LastResult = 0;   /* file not found is OK */
    if( LastResult > 0 ) {
        PrintIOError( ERR+MSG_IO_PROBLEM, "12", name );
    }
}

extern bool QReadStr( f_handle file, char *dest, unsigned size, char *name )
/**************************************************************************/
/* quick read string (used for reading directive file) */
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
// returns FALSE if not a device, e.g. not stdin/stdout.
{
    USHORT  type;
    USHORT  attr;

    LastResult = DosQHandType( file, &type, &attr );
    return( type == 1 );  // not from stdin/stdout
}

extern f_handle QObjOpen( char *name )
/************************************/
{
    return( DoOpen( name, FALSE, READ_ACCESS, FALSE ) );
}

extern f_handle TempFileOpen( char *name )
/****************************************/
{
    return( DoOpen( name, FALSE, READWRITE_ACCESS, FALSE ) );
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

extern int ResOpen( char *name, int access, ... )
/***********************************************/
/* this just ignores the access parameter, and assumes it knows what the
 * resource stuff really wants to do */
{
    access = access;  /* to avoid a warning */
    return( DoOpen( name, FALSE, READ_ACCESS, FALSE ) );
}

extern char WaitForKey( void )
/****************************/
{
    char result;

    DieNow = TRUE;          // needed since getch doesn't return when ctrl-c
    result = getch();       // is pressed running under DOS
    DieNow = FALSE;
    return result;
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
    if( CaughtBreak != BREAK_HANDLED ) {
        CaughtBreak = BREAK_DETECTED;
    }
    if( DieNow ) {
        CheckBreak();
    }
}

extern void CheckBreak( void )
/****************************/
{
    if( CaughtBreak == BREAK_DETECTED ) {
        CaughtBreak = BREAK_HANDLED;
        LnkMsg( FTL+MSG_BREAK_HIT, NULL );    /* suicides */
    }
}

extern void SetBreak( void )
/**************************/
{
    BreakCond = TinyGetCtrlBreak();
    TinySetCtrlBreak( 1 );
}

extern void RestoreBreak( void )
/******************************/
{
    if( !(LinkState & RUNNING_OS2_FLAG) ) {
        TinySetCtrlBreak( BreakCond );
    }
}
