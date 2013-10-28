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
* Description:  Platform dependent internal helper functions for wmake.
*
****************************************************************************/


#include <stdio.h>
#include "make.h"
#include "mcache.h"
#include "mmemory.h"
#include "mrcmsg.h"
#include "msg.h"
#include "mupdate.h"
#if defined( __DOS__ )
    #include <dos.h>
    #include "tinyio.h"
#endif
#include "pcobj.h"
#include "msysdep.h"

#if defined( __DOS__ )

int __far critical_error_handler( unsigned deverr, unsigned errcode, unsigned __far *devhdr )
{
    deverr = deverr; errcode = errcode; devhdr = devhdr;
    return( _HARDERR_FAIL );
}

void InitHardErr( void )
{
    _harderr( critical_error_handler );
}

#else

void InitHardErr( void )
{
}

#endif

#if defined( __DOS__ )
//extern char             DOSSwitchChar(void);
char             DOSSwitchChar(void);
#pragma aux             DOSSwitchChar = \
        "mov ax,3700h"  \
        "int 21h"       \
        parm caller     [] \
        value           [dl] \
        modify          [ax dx];
#endif

int SwitchChar( void )
/***************************/
{
#if defined( __DOS__ )
    return( DOSSwitchChar() );
#elif   defined( __OS2__ ) || defined( __NT__ )
    return( '/' );
#elif   defined( __UNIX__ )
    return( '-' );

#endif
}

#if defined( __DOS__ ) && defined ( _M_I86 )
/* see page 90-91 of "Undocumented DOS" */

//extern void __far *       _DOS_list_of_lists( void );
void __far *       _DOS_list_of_lists( void );
#pragma aux             _DOS_list_of_lists = \
        "mov ax,5200h"  \
        "int 21h"       \
        parm caller     [] \
        value           [es bx] \
        modify          [ax es bx];

int OSCorrupted( void )
/****************************/
{
    _Packed struct mcb {
        UINT8   id;
        UINT16  owner;
        UINT16  len;
    } __far *chain;
    UINT16 __far  *first_MCB;
    UINT16      chain_seg;
    UINT16      new_chain_seg;

    first_MCB = _DOS_list_of_lists();
    if( FP_OFF( first_MCB ) == 1 ) {    /* next instr will hang! */
        /* list of lists DOS call may have been interrupted */
        return( 1 );
    }
    chain_seg = first_MCB[-1];
    for( ;; ) {
        chain = MK_FP( chain_seg, 0 );
        if( chain->id == 'Z' ) {
            break;
        }
        if( chain->id != 'M' ) {
            return( 1 );
        }
        new_chain_seg = chain_seg + ( chain->len + 1 );
        if( new_chain_seg <= chain_seg ) {
            return( 1 );
        }
        chain_seg = new_chain_seg;
    }
    return( 0 );
}
#else
int OSCorrupted( void )
/****************************/
{
    return( 0 );
}
#endif

#if defined( __DOS__ )
RET_T TouchFile( const char *name )
/****************************************/
{
    tiny_date_t     dt;
    tiny_time_t     tm;
    tiny_ftime_t    p_hms;
    tiny_fdate_t    p_ymd;
    tiny_ret_t      ret;

    ret = TinyOpen( name, TIO_WRITE );
    if( TINY_OK( ret ) ) {
        dt = TinyGetDate();
        p_ymd.year  = dt.year + (1900 - 1980);
        p_ymd.month = dt.month;
        p_ymd.day   = dt.day_of_month;

        tm = TinyGetTime();
        p_hms.hours   = tm.hour;
        p_hms.minutes = tm.minutes;
        p_hms.twosecs = tm.seconds / 2;

        TinySetFileStamp( TINY_INFO( ret ), p_hms, p_ymd );
        TinyClose( TINY_INFO( ret ) );
    } else {
        ret = TinyCreate( name, TIO_NORMAL );
        if( TINY_OK( ret ) ) {
            TinyClose( TINY_INFO( ret ) );
        } else {
            return( RET_ERROR );
        }
    }
    return( RET_SUCCESS );
}
#else

#include <sys/stat.h>
#include <sys/types.h>
#if defined(__UNIX__)
    #include <utime.h>
#else
    #include <sys/utime.h>
#endif

RET_T TouchFile( const char *name )
/****************************************/
{
    int     fh;

    if( utime( name, 0 ) < 0 ) {
        fh = creat( name, PMODE_RW );
        if( fh < 0 ) {
            return( RET_ERROR );
        }
        close( fh );
    }
    return( RET_SUCCESS );
}
#endif

#define FUZZY_DELTA     60      /* max allowed variance from stored time-stamp */

BOOLEAN IdenticalAutoDepTimes( time_t in_obj, time_t stamp )
/**********************************************************/
{
    time_t  diff_time;

    /* in_obj can be a DOS time so we need to round to the nearest two-second */
    if( in_obj == stamp || in_obj == (stamp & ~1) ) {
        return( TRUE );
    }
    if( in_obj < stamp ) {
        /* stamp is newer than time in .OBJ file */
        if( Glob.fuzzy ) {
            /* check for a "tiny" difference in times (almost identical) */
            diff_time = stamp - in_obj;
            if( diff_time <= FUZZY_DELTA ) {
                return( TRUE );
            }
        }
    }
    return( FALSE );
}

#ifdef DLLS_IMPLEMENTED

#include "idedrv.h"

static DLL_CMD  *dllCommandList;

void OSLoadDLL( char *cmd_name, char *dll_name, char *ent_name )
/**************************************************************/
{
    DLL_CMD     *n;

    // we want newer !loaddlls to take precedence
    n = MallocSafe( sizeof( *n ) );
    n->cmd_name = StrDupSafe( cmd_name );
    n->next = dllCommandList;
    dllCommandList = n;
    IdeDrvInit( &n->inf, StrDupSafe( dll_name ),
                (ent_name == NULL) ? NULL : StrDupSafe( ent_name ) );
}

DLL_CMD *OSFindDLL( char const *cmd_name )
/****************************************/
{
    DLL_CMD     *n;

    for( n = dllCommandList; n != NULL; n = n->next ) {
        if( 0 == stricmp( cmd_name, n->cmd_name ) ) {
            break;
        }
    }
    return( n );
}

#define DLL_PREFIX  "DLL:"
#define DLL_PSIZE   sizeof( DLL_PREFIX ) - 1

int OSExecDLL( DLL_CMD* dll, char const* cmd_args )
/*************************************************
 * Returns the error code returned by IdeDrvDLL
 */
{
    int     retcode = IdeDrvExecDLL( &dll->inf, cmd_args );

    setmode( STDOUT_FILENO, O_TEXT );
    return( retcode );
}
#else

DLL_CMD *OSFindDLL( char const *cmd_name )
/****************************************/
{
    cmd_name = cmd_name;
    return( NULL );
}

void OSLoadDLL( char *cmd_name, char *dll_name, char *ent_name )
/**************************************************************/
{
    cmd_name = cmd_name;
    dll_name = dll_name;
    ent_name = ent_name;
}

int OSExecDLL( DLL_CMD* dll, char const* cmd_args )
/*************************************************/
{
    dll = dll;
    cmd_args = cmd_args;
    return( -1 );
}

#endif

#ifndef NDEBUG
STATIC void cleanDLLCmd( void )
{
#ifdef DLLS_IMPLEMENTED
    DLL_CMD     *n;
    DLL_CMD     *temp;

    n  = dllCommandList;
    while( n != NULL ) {
        FreeSafe( (char *)n->cmd_name );
        if( n->inf.dll_name != NULL ) {
            FreeSafe( (char*) n->inf.dll_name );
        }
        if( n->inf.ent_name != NULL ) {
            FreeSafe( (char *)n->inf.ent_name );
        }
        temp = n;
        n = n->next;
        FreeSafe( temp );
    }
#endif
}
#endif


#ifndef NDEBUG
void DLLFini( void )
{
    cleanDLLCmd();
}
#endif

static sig_atomic_t     sig_count;

void CheckForBreak( void )
{
    if( sig_count > 0 ) {
        sig_count = 0;
        PrtMsg( ERR | USER_BREAK_ENCOUNTERED );
        exit( ExitSafe( EXIT_ERROR ) );
    }
}


static void passOnBreak( void )
{
#ifdef DLLS_IMPLEMENTED
    DLL_CMD     *n;

    for( n = dllCommandList; n != NULL; n = n->next ) {
        IdeDrvStopRunning( &n->inf );
    }
#endif
}


static void breakHandler( int sig_number )
{
    sig_number = sig_number;
    sig_count = 1;
    passOnBreak();
}

void InitSignals( void ) {
    sig_count = 0;
    DoingUpdate = FALSE;
#ifndef __UNIX__
    signal( SIGBREAK, breakHandler );
#endif
    signal( SIGINT, breakHandler );
}
