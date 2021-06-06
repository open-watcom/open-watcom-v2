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
* Description:  POSIX df utility
*               Displays disk free space
*
****************************************************************************/


#if defined( __OS2__ )
#include <os2.h>
#elif defined( __NT__ )
#include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <direct.h>
#include "misc.h"
#include "getopt.h"
#include "argvenv.h"

#include "clibext.h"


#define _osmode_REALMODE()  (_osmode == DOS_MODE)

typedef enum {
    DRIVE_NONE,
    DRIVE_IS_REMOVABLE,
    DRIVE_IS_FIXED,
    DRIVE_IS_REMOTE
} drive_type;

static char     includeNetwork;

char            *OptEnvVar = "df";

static const char *usageMsg[] = {
    "Usage: df [-?n] [@env] [drive list]",
    "\tenv            : environment variable to expand",
    "\tdrive list     : specify which drives do show",
    "\tOptions: -?    : print this list",
    "\t\t -n    : include network drives",
    NULL
};

#if defined( __DOS__ ) || defined( __OS2__ ) && defined( _M_I86 )
extern short CheckRemovable( char );
#pragma aux CheckRemovable = \
        "mov  ax,4408h"     \
        "int 21h"           \
        "cmp  ax,0fh"       \
        "jne short ok"      \
        "xor  ax,ax"        \
        "jmp short done"    \
    "ok: inc  ax"           \
    "done:"                 \
    __parm      [__bl] \
    __value     [__ax] \
    __modify    []

extern short CheckRemote( char );
#pragma aux CheckRemote = \
        "mov  ax,4409h"     \
        "int 21h"           \
        "mov  ax,-1"        \
        "jc short ibrx"     \
        "inc  ax"           \
        "test dh,10h"       \
        "jz short ibrx"     \
        "inc  ax"           \
    "ibrx:"                 \
    __parm      [__bl] \
    __value     [__ax] \
    __modify    [__dx]

/*
 * doGetDriveType - get the type of drive A-Z
 */
#ifdef __DOS__
static drive_type doGetDriveType( int drv )
#else
static drive_type dosDoGetDriveType( int drv )
#endif
{
    int         dl;
    drive_type  rc;

    dl = toupper( drv ) - 'A' + 1;
    rc = CheckRemovable( dl );
    if( rc == DRIVE_IS_FIXED ) {
        if( CheckRemote( dl ) ) {
            rc = DRIVE_IS_REMOTE;
        }
    }
    return( rc );

} /* doGetDriveType */
#endif

#if defined( __OS2__ )

#ifdef _M_I86
#define STUPID_UINT     unsigned short
#else
#define STUPID_UINT     unsigned long
#endif
/*
 * doGetDriveType - get the type of drive A-Z
 */
static drive_type doGetDriveType( int drv )
{
    STUPID_UINT disk;
    ULONG       map;
    int         i;
    char        fname[3];
    HFILE       hf;
    STUPID_UINT act;
    STUPID_UINT type;
    int         rc;

#ifdef _M_I86
    if( _osmode_REALMODE() ) {
        return( dosDoGetDriveType( drv ) );
    } else {
#endif
        drv = toupper( drv );
        DosQCurDisk( &disk, &map );
        for( i = 'A'; i <= 'Z'; i++ ) {
            if( drv == i ) {
                if( map & 1 ) {
                    fname[0] = tolower( i );
                    fname[1] = ':';
                    fname[2] = 0;
                    rc = DosOpen( fname, &hf, &act, 0,
                                FILE_NORMAL,
                                OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,
                                OPEN_FLAGS_DASD | OPEN_SHARE_DENYNONE | OPEN_ACCESS_READONLY,
                                0 );
                    if( rc == 0 ) {
                        DosQHandType( hf, &type, &act );
                        DosClose( hf );
                        if( type & 0x8000 ) {
                            return( DRIVE_IS_REMOTE );
                        }
                    } else {
                        // KLUDGE - this only seems to fail for network drives?!
                        return( DRIVE_IS_REMOTE );
                    }
                    return( DRIVE_IS_FIXED );
                } else {
                    return( DRIVE_NONE );
                }
            }
            map >>= 1;
        }
        return( DRIVE_NONE );
#ifdef _M_I86
    }
#endif

} /* doGetDriveType */

#elif defined( __NT__ )

/*
 * doGetDriveType - get the type of drive A-Z
 */
static drive_type doGetDriveType( int drv )
{
    char        path[4];
    DWORD       type;

    path[0] = (char)drv;
    path[1] = ':';
    path[2] = '\\';
    path[3] = 0;
    type = GetDriveType( path );
    if( type == 1 ) {
        return( DRIVE_NONE );
    }
    if( type == DRIVE_REMOVABLE ) {
        return( DRIVE_IS_REMOVABLE );
    }
    if( type == DRIVE_REMOTE ) {
        return( DRIVE_IS_REMOTE );
    }
    return( DRIVE_IS_FIXED );

} /* doGetDriveType */
#endif

/*
 * doDF - do disk info for a specific drive
 */
static void doDF( int drive )
{
    struct diskfree_t   df;
    unsigned long long  cl_bytes;
    unsigned long long  total;
    unsigned long long  avail;
    unsigned long long  used;
    drive_type          dt;

    dt = doGetDriveType( drive );
    if( dt == DRIVE_NONE ) {
        return;
    }
    if( dt == DRIVE_IS_REMOTE && !includeNetwork ) {
        return;
    }
    if( _getdiskfree( toupper( drive ) - 'A' + 1, &df ) == 0 ) {
        cl_bytes = (unsigned long long)df.sectors_per_cluster * (unsigned long long)df.bytes_per_sector;
        total = ( df.total_clusters * cl_bytes ) / 1024;
        avail = ( df.avail_clusters * cl_bytes ) / 1024;
        used = total - avail;
        printf( " %c   %10lld  %10lld  %10lld    %2d%%\n",
            toupper( drive ), total, used, avail, (int)( ( 100 * used ) / total ) );
    }

} /* doDF */


int main( int argc, char *argv[] )
{
    int         ch;
    int         i;

    argv = ExpandEnv( &argc, argv );

    for( ;; ) {
        ch = GetOpt( &argc, argv, "n", usageMsg );
        if( ch == -1 ) {
            break;
        }
        switch( ch ) {
        case 'n':
            includeNetwork = 1;
            break;
        }
    }

    printf( "drive  KBytes        used       avail   capacity\n" );
    if( argc > 1 ) {
        includeNetwork = 1;
        for( i = 1; i < argc; i++ ) {
            doDF( argv[i][0] );
        }
    } else {
        for( i = 'C'; i <= 'Z'; i++ ) {
            doDF( i );
        }
    }

    return( 0 );

} /* main */
