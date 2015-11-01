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
* Description:  Trap file access wrapper routine.
*
****************************************************************************/


#include "madregs.h"
#include "trptypes.h"
#include "trpld.h"
#if defined( __WINDOWS__ ) && !defined(SERVER)
#include <windows.h>
#include "trpsys.h"
#elif defined( ENABLE_TRAP_LOGGING )
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winbase.h>    /* For GetSystemTime */
#endif


trap_version    TrapVer;
trap_req_func   *ReqFunc;
#ifdef ENABLE_TRAP_LOGGING
char            *TrapTraceFileName = NULL;
bool            TrapTraceFileFlush = FALSE;
#endif

static void     (*pFailure)(void) = NULL;
static void     (*pAccess)(void) = NULL;
#ifdef ENABLE_TRAP_LOGGING
static FILE     *TrapTraceFileHandle = NULL;
#endif

#ifdef ENABLE_TRAP_LOGGING
int OpenTrapTraceFile( void )
{
    if( TrapTraceFileName != NULL ) {
        if( TrapTraceFileHandle != NULL )
            return( -1 );
        TrapTraceFileHandle = fopen( TrapTraceFileName, "wb" );
        if( TrapTraceFileHandle == NULL ) {
            return( -1 );
        }
    }
    return( 0 );
}

int CloseTrapTraceFile( void )
{
    if( TrapTraceFileHandle != NULL ){
        fclose( TrapTraceFileHandle );
        TrapTraceFileHandle = NULL;
    }
    return( 0 );
}
#endif

static void Failure( void )
{
    if( pFailure ) {
        pFailure();
    }
}

static void Access( void )
{
    if( pAccess ) {
        pAccess();
    }
}

void TrapSetFailCallBack( void (*func)(void) )
{
    pFailure = func;
}

void TrapSetAccessCallBack( void (*func)(void) )
{
    pAccess = func;
}

void TrapFailAllRequests()
{
    ReqFunc = NULL;
}

static trap_retval ReqFuncProxy( trap_elen num_in_mx, in_mx_entry_p mx_in, trap_elen num_out_mx, mx_entry_p mx_out )
{
    trap_retval     result;

#ifdef ENABLE_TRAP_LOGGING
    if( TrapTraceFileHandle != NULL ) {
        unsigned        ix;
        unsigned short  rectype = 4;   /* Request */
        unsigned short  length = 0;
        SYSTEMTIME      st;

        GetSystemTime( &st );

        for( ix = 0 ; ix < num_in_mx ; ix++ ) {
            length += mx_in[ix].len;
        }
        fwrite( &rectype, sizeof( rectype ), 1, TrapTraceFileHandle );
        fwrite( &st.wHour, sizeof( WORD ), 1, TrapTraceFileHandle );
        fwrite( &st.wMinute, sizeof( WORD ), 1, TrapTraceFileHandle );
        fwrite( &st.wSecond, sizeof( WORD ), 1, TrapTraceFileHandle );
        fwrite( &st.wMilliseconds, sizeof( WORD ), 1, TrapTraceFileHandle );

        fwrite( &length, sizeof( length ), 1, TrapTraceFileHandle );
        for( ix = 0 ; ix < num_in_mx ; ix++ ) {
            fwrite( mx_in[ix].ptr, mx_in[ix].len, 1, TrapTraceFileHandle );
        }
        if( TrapTraceFileFlush ) {
            fflush( TrapTraceFileHandle );
        }
    }
#endif

    result = ReqFunc( num_in_mx, mx_in, num_out_mx, mx_out );

#ifdef ENABLE_TRAP_LOGGING
    if( TrapTraceFileHandle != NULL ) {
        /* result is the length of data returned or REQUEST_FAILED */
        /* Only worth tracing if there is data though */
        if( result > 0 ) {
            unsigned        ix;
            unsigned short  rectype = 5;   /* reply*/
            unsigned short  length = result;
            SYSTEMTIME      st;

            GetSystemTime(&st);

            fwrite( &rectype, sizeof( rectype ), 1, TrapTraceFileHandle );
            fwrite( &st.wHour, sizeof(WORD), 1, TrapTraceFileHandle );
            fwrite( &st.wMinute, sizeof(WORD), 1, TrapTraceFileHandle );
            fwrite( &st.wSecond, sizeof(WORD), 1, TrapTraceFileHandle );
            fwrite( &st.wMilliseconds, sizeof(WORD), 1, TrapTraceFileHandle );
            fwrite( &length, sizeof( length ), 1, TrapTraceFileHandle );

            for( ix = 0 ; ix < num_out_mx ; ix++ ) {
                unsigned to_write = mx_out[ix].len;
                if( to_write > length )
                    to_write = length;
                fwrite( mx_out[ix].ptr, to_write, 1, TrapTraceFileHandle );
                length -= to_write;
                if( 0 == length ) {
                    break;
                }
            }
            if( TrapTraceFileFlush ) {
                fflush( TrapTraceFileHandle );
            }
        }
    }
#endif

    return( result );
}


unsigned TrapAccess( unsigned num_in_mx, in_mx_entry_p mx_in, unsigned num_out_mx, mx_entry_p mx_out  )
{
    trap_retval     result;

    if( ReqFunc == NULL )
        return( (unsigned)-1 );

    result = ReqFuncProxy( num_in_mx, mx_in, num_out_mx, mx_out );
    if( result == REQUEST_FAILED ) {
        Failure();
    }
    Access();
#if defined(__WINDOWS__) && !defined(SERVER)
    TrapHardModeCheck();
#endif
    if( result == REQUEST_FAILED )
        return( (unsigned)-1 );
    return( result );
}

unsigned TrapSimpAccess( unsigned in_len, in_data_p in_data, unsigned out_len, out_data_p out_data )
{
    in_mx_entry     in[1];
    mx_entry        out[1];
    unsigned        result;

    in[0].ptr = in_data;
    in[0].len = in_len;
    if( out_len != 0 ) {
        out[0].ptr = out_data;
        out[0].len = out_len;
        result = TrapAccess( 1, in, 1, out );
    } else {
        result = TrapAccess( 1, in, 0, NULL );
    }
    return( result );
}
