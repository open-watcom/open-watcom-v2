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


// WMAKEPT -- pass-though to WMAKE
//
// (1) DLL's contained in environment variable WMAKE_DLL are loaded
//
// (2) Command line is passed thru to wmake.
//
// 95/01/26 -- J.W.Welch        -- defined

extern "C" {
#define INCL_DOSMODULEMGR
#include "os2.h"
};

#define TRUE  1
#define FALSE 0

#include <malloc.h>
#include <process.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


static int load_dll             // LOAD A DLL (SYSTEM-DEPENDENT)
    ( unsigned char *dll_name ) // - name of DLL
{
    unsigned char err_bf[100];  // - error buffer
    HMODULE handle;             // - handle
    APIRET rc;                  // - return code
    int ret;                    // - return: TRUE ==> worked

    rc = DosLoadModule( err_bf, sizeof( err_bf ), dll_name, &handle );
    if( 0 == rc ) {
        ret = TRUE;
    } else {
        printf( "WMAKEPT error loading DLL: rc=%d object=%s\n"
              , rc
              , err_bf );
        ret = FALSE;
    }
    return ret;
}


static int load_dlls            // LOAD DLL'S
    ( void )
{
    char const * env;           // - position in WMAKE_DLL pointer
    unsigned char dll_name[ _MAX_PATH ]; // - DLL name
    char const * dp;            // - points at dll_name
    size_t size;                // - size of dll name
    int retn;                   // - return: TRUE ==> ok

    env = getenv( "WMAKE_DLL" );
    retn = TRUE;
    if( 0 != env ) {
        for( ; ; ) {
            dp = env;
            for( ; ; ++env ) {
                if( *env == ';' ) break;
                if( *env == '\0' ) break;
            }
            size = env - dp;
            if( size > 0 ) {
                dll_name[ size ] = '\0';
                memcpy( dll_name, dp, size );
                retn &= load_dll( dll_name );
            }
            if( *env == '\0' ) break;
            ++ env;
        }
    }
    return retn;
}


int main                        // MAIN-LINE
    ( int arg_count             // - # arguments
    , char const * args[] )     // - arguments for make
{
    char const * * pgm_args;    // - arguments passed to make
    unsigned index;             // - index of argument
    int retn;                   // - return

    if( load_dlls() ) {
        pgm_args = (char const **)alloca( ( arg_count + 1 ) * sizeof( char* ) );
        for( index = 1; index < arg_count; ++ index ) {
            pgm_args[ index ] = args[ index ];
        }
        pgm_args[ index ] = 0;
        pgm_args[0] = "WMAKE";
        retn = spawnv( P_WAIT, "wmake", pgm_args );
    } else {
        printf( "WMAKEPT terminated without running WMAKE\n" );
        retn = -1;
    }
    return retn;
}
