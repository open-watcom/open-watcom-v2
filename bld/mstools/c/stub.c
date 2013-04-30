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


#if defined( __WATCOMC__ ) || !defined( __UNIX__ )
#include <process.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "watcom.h"
#include "memory.h"
#include "stub.h"

#ifndef __NT__
    #error stub.c is for NT only
#endif


/*
 * Detect which type of CPU is installed.
 */
int GetCpuTypeStub( void )
/************************/
{
    SYSTEM_INFO         sysinfo;
    char *              env;

    /*** Try using GetSystemInfo's data ***/
    GetSystemInfo( &sysinfo );
    switch( sysinfo.dwProcessorType ) {
      case PROCESSOR_INTEL_386:
      case PROCESSOR_INTEL_486:
      case PROCESSOR_INTEL_PENTIUM:
        return( STUB_FOUND_386 );
        break;
      case PROCESSOR_AMD_X8664:
        return( STUB_FOUND_X64 );
        break;
      case PROCESSOR_ALPHA_21064:
        return( STUB_FOUND_AXP );
        break;
#ifdef __PPC__
      case PROCESSOR_PPC_601:
      case PROCESSOR_PPC_603:
      case PROCESSOR_PPC_604:
      case PROCESSOR_PPC_620:
        return( STUB_FOUND_PPC );
        break;
#endif
      default:
        break;
    }

    /*** It's a new CPU; check PROCESSOR_ARCHITECTURE ***/
    env = getenv( "PROCESSOR_ARCHITECTURE" );
    if( env != NULL ) {
        if( !stricmp( env, "x86" ) ) {
            return( STUB_FOUND_386 );
        } else if( !stricmp( env, "AMD64" ) ) {
            return( STUB_FOUND_X64 );
        } else if( !stricmp( env, "alpha" ) ) {
            return( STUB_FOUND_AXP );
        } else if( !stricmp( env, "ppc" ) ) {
            return( STUB_FOUND_PPC );
        }
    }

    return( STUB_FOUND_UNKNOWN );
}


/*
 * Execute progname.  If successful, this function does not return.
 */
void SpawnProgStub( const char *progname )
/****************************************/
{
    char                drive[_MAX_DRIVE];
    char                dir[_MAX_DIR];
    char                fname[_MAX_FNAME];
    char                ext[_MAX_EXT];
    char                fullPath[_MAX_PATH];
    size_t              len;
    char *              argv[3];
    int                 rc;

    /*** Make a copy of the command line ***/
    argv[0] = (char*)progname;
    len = _bgetcmd( NULL, 0 ) + 1;
    argv[1] = AllocMem( len );
    getcmd( argv[1] );
    argv[2] = NULL;

    /*** Try to execute the program ***/
    rc = spawnvp( P_WAIT, progname, (const char **)argv );
    if( rc != -1 ) {
        exit( rc );
    }

    /*** Didn't work; try looking in the same directory as this program ***/
    _fullpath( fullPath, progname, _MAX_PATH );
    _splitpath( fullPath, drive, dir, NULL, NULL );
    _splitpath( progname, NULL, NULL, fname, ext );
    _makepath( fullPath, drive, dir, fname, ext );
    rc = spawnvp( P_WAIT, fullPath, (const char **)argv );
    if( rc != -1 ) {
        exit( rc );
    }
}
