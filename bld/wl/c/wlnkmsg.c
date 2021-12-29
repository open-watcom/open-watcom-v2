/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Linker message output and message resource low-level functions.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#ifdef __WATCOMC__
#include <process.h>
#include "clibint.h"
#endif
#include "wio.h"
#include "linkstd.h"
#include "dbginfo.h"
#include "alloc.h"
#include "mapio.h"
#include "msg.h"
#include "fileio.h"
#include "ideentry.h"
#include "loadfile.h"
#include "wreslang.h"
#include "wressetr.h"
#include "wresset2.h"
#include "rcrtns.h"
#include "wlnkmsg.h"
#include "posixfp.h"

#include "clibext.h"


static  HANDLE_INFO     hInstance = { 0 };
static  unsigned        MsgShift;

bool InitMsg( void )
{
    char        msg_buff[RESOURCE_MAX_SIZE];
#if defined( IDE_PGM ) || !defined( __WATCOMC__ )
    char        imageName[_MAX_PATH];
#else
    char        *imageName;
#endif

#if defined( IDE_PGM )
    _cmdname( imageName );
#elif !defined( __WATCOMC__ )
    get_dllname( imageName, sizeof( imageName ) );
#else
    imageName = _LpDllName;;
#endif
    BannerPrinted = false;
    hInstance.status = 0;
    if( OpenResFile( &hInstance, imageName ) ) {
        MsgShift = _WResLanguage() * MSG_LANG_SPACING;
        if( Msg_Get( MSG_GENERAL_HELP_0, msg_buff ) ) {
            return( true );
        }
    }
    CloseResFile( &hInstance );
    WriteStdOutInfo( NO_RES_MESSAGE "\n", ERR, NULL );
    return( false );
}

bool Msg_Get( int resourceid, char *buffer )
{
    if( hInstance.status == 0 || WResLoadString( &hInstance, resourceid + MsgShift, (lpstr)buffer, RESOURCE_MAX_SIZE ) <= 0 ) {
        buffer[0] = '\0';
        return( false );
    }
    return( true );
}

void Msg_Do_Put_Args( char rc_buff[], MSG_ARG_LIST *arg_info, const char *types, ... )
{
    va_list     args;

    va_start( args, types );
    Msg_Put_Args( rc_buff, arg_info, types, args );
    va_end( args );
}

// Write arguments to put into a message and make it printf-like
void Msg_Put_Args(
    char            message[],      // Contains %s, etc. or %digit specifiers
    MSG_ARG_LIST    *arg_info,      // Arguments found
    const char      *types,         // message conversion specifier types
                                    // NULL or strlen <= 3 (arg_info->arg elements)
    va_list         args )          // Initialized va_list
{
    int         argnum = 0;         // Index of argument found
    int         j;                  // General purpose loop index
    int         order[3];           // Mapping of args to arg_info->arg
    char        *percent;           // Position of '%' in message
    char        types_buff[1 + 3];  // readwrite copy of types
    char        specifier;          // Character following '%'
    MSG_ARG     *arginfo;

    if( types != NULL ) {
        strcpy( types_buff, types );
                                        // conversions set order[]; digits->s
        percent = message - 2;          // So strchr below can work
        while( (percent = strchr( percent + 2, '%' )) != NULL ) {
            specifier = percent[1];
            for( j = 0; types_buff[j] != '\0'; j++ ) {  // Match with types
                if( types_buff[j] == specifier ) {
                    order[j] = argnum;
                    argnum++;
                    if( isdigit( specifier ) )          // Digit becomes s
                        types_buff[j] = percent[1] = 's';
                    break;
                }
            }
        }
        // Re-order sequential arguments
        for( j = 0; j < argnum; j++ ) {
            arginfo = arg_info->arg + order[j];
            switch( types_buff[j] ) {
            case 's':
                arginfo->string = va_arg( args, char * );
                break;
            case 'x':
            case 'd':
                arginfo->int_16 = (signed_16)va_arg( args, unsigned int );
                break;
            case 'l':
                arginfo->int_32 = (signed_32)va_arg( args, unsigned long );
                break;
            case 'A':
            case 'a':
                arginfo->address = va_arg( args, targ_addr * );
                break;
            case 'S':
                arginfo->symb = va_arg( args, symbol * );
                break;
            }
        }
    }
    arg_info->index = 0;
}

void Msg_Write_Map( int resourceid, ... )
{
    char        msg_buff[RESOURCE_MAX_SIZE];
    va_list     args;

    Msg_Get( resourceid, msg_buff );
    va_start( args, resourceid );
    DoWriteMap( msg_buff, args );
    va_end( args );
}

bool FiniMsg( void )
{
    return( CloseResFile( &hInstance ) );
}

FILE *res_open( const char *name, wres_open_mode omode )
{
    switch( omode ) {
    default:
    case WRES_OPEN_RO:
        return( POSIX2FP( open( name, O_BINARY | O_RDONLY ) ) );
    case WRES_OPEN_RW:
        return( POSIX2FP( open( name, O_BINARY | O_RDWR | O_CREAT, PMODE_RW ) ) );
    case WRES_OPEN_NEW:
        return( POSIX2FP( open( name, O_BINARY | O_WRONLY | O_CREAT | O_TRUNC, PMODE_RW ) ) );
    case WRES_OPEN_TMP:
        return( NULL );
    }
}

bool res_close( FILE *fp )
{
    return( close( FP2POSIX( fp ) ) != 0 );
}

#if defined( _MSC_VER ) && defined( _WIN64 )
static ssize_t  posix_read( int fildes, void *buffer, size_t nbyte )
{
    unsigned    read_len;
    unsigned    amount;
    size_t      size;

    amount = INT_MAX;
    size = 0;
    while( nbyte > 0 ) {
        if( amount > nbyte )
            amount = (unsigned)nbyte;
        read_len = _read( fildes, buffer, amount );
        if( read_len == (unsigned)-1 ) {
            return( (ssize_t)-1 );
        }
        size += read_len;
        if( read_len != amount ) {
            break;
        }
        buffer = (char *)buffer + amount;
        nbyte -= amount;
    }
    return( size );
}
#else
#define posix_read      read
#endif

size_t res_read( FILE *fp, void *buf, size_t len )
/************************************************/
{
    return( posix_read( FP2POSIX( fp ), buf, len ) );
}

size_t res_write( FILE *fp, const void *buf, size_t len )
/*******************************************************/
{
    /* unused parameters */ (void)fp;

    WriteLoad( buf, len );
    return( len );
}

bool res_seek( FILE *fp, long amount, int where )
/***********************************************/
{
    if( fp == hInstance.fp ) {
        if( where == SEEK_SET ) {
            return( lseek( FP2POSIX( fp ), amount + WResFileShift, where ) == -1L );
        } else {
            return( lseek( FP2POSIX( fp ), amount, where ) == -1L );
        }
    }

    DbgAssert( where != SEEK_END );
    DbgAssert( !( where == SEEK_CUR && amount < 0 ) );

    if( FP2POSIX( fp ) == Root->outfile->handle ) {
        if( where == SEEK_CUR ) {
            unsigned long   old_pos;
            unsigned long   new_pos;

            old_pos = PosLoad();
            new_pos = old_pos + amount;
            if( new_pos > old_pos ) {
                PadLoad( (size_t)amount );
            } else {
                SeekLoad( new_pos );
            }
        } else {
            SeekLoad( amount );
        }
        return( false );
    } else {
        return( QLSeek( FP2POSIX( fp ), amount, where, "resource file" ) == -1L );
    }
}

long res_tell( FILE *fp )
/***********************/
{
    if( fp == hInstance.fp ) {
        return( lseek( FP2POSIX( fp ), 0, SEEK_CUR ) );
    } else if( FP2POSIX( fp ) == Root->outfile->handle ) {
        return( PosLoad() );
    } else {
        return( QPos( FP2POSIX( fp ) ) );
    }
}

bool res_ioerr( FILE *fp, size_t rc )
/***********************************/
{
    /* unused parameters */ (void)fp;

    return( rc == -1 );
}

WResSetRtns(res_open,res_close,res_read,res_write,res_seek,res_tell,res_ioerr,ChkLAlloc,LFree);
