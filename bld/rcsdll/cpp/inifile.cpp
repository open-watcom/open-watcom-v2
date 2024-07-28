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
* Description:  Configuration profile routines for multiple OSes.
*
****************************************************************************/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#if defined( __OS2__ )
    #define INCL_WINSHELLDATA
    #include <os2.h>
#elif defined( __WINDOWS__ ) || defined( __NT__ )
    #include <windows.h>
#elif defined( __UNIX__ )
    #include "watcom.h"
#elif defined( __DOS__ )
#else
    #error UNSUPPORTED OS
#endif

#include "clibext.h"


#define RCS_CFG     "rcs.cfg"
#define RCS_SECTION "rcs settings"
#define RCS_KEY     "rcs settings"
#define RCS_DEFAULT "generic"

#if defined( __UNIX__ )
    #define REL_RCS_CFG     "/.wvi/" RCS_CFG
    #define ABS_RCS_CFG     "/usr/watcom/wvi/" RCS_CFG
#elif defined( __DOS__ )
    #define REL_RCS_CFG     "\\binw\\" RCS_CFG
    #define ABS_RCS_CFG     "c:\\" RCS_CFG
#endif

#if defined( __OS2__ )

    // just use the os/2 user .ini file
int MyGetProfileString( const char *dir, char *buffer, int len )
{
    (void)dir;
  #ifdef _M_I86
    return( (unsigned)PrfQueryProfileString( HINI_USERPROFILE, RCS_SECTION, RCS_KEY, RCS_DEFAULT, (void *)buffer, (unsigned)len ) );
  #else
    return( PrfQueryProfileString( HINI_USERPROFILE, RCS_SECTION, RCS_KEY, RCS_DEFAULT, (void *)buffer, len ) );
  #endif
}

int MyWriteProfileString( const char *dir, const char *string )
{
    (void)dir;
  #ifdef _M_I86
    return( PrfWriteProfileString( HINI_USERPROFILE, RCS_SECTION, RCS_KEY, (char *)string ) );
  #else
    return( PrfWriteProfileString( HINI_USERPROFILE, RCS_SECTION, RCS_KEY, string ) );
  #endif
}

#elif defined( __WINDOWS__ ) || defined( __NT__ )

int MyGetProfileString( const char *dir, char *buffer, int len )
{
    (void)dir;
    return( GetPrivateProfileString( RCS_SECTION, RCS_KEY, RCS_DEFAULT, buffer, len, RCS_CFG ) );
}

int MyWriteProfileString( const char *dir, const char *string )
{
    (void)dir;
    return( WritePrivateProfileString( RCS_SECTION, RCS_KEY, string, RCS_CFG ) );
}

#else

int MyGetProfileString( const char *dir, char *buffer, int len )
{
    char path[_MAX_PATH];
    FILE *fp;

  #if defined( __UNIX__ )
    if( dir == NULL ) {
        dir = getenv( "HOME" );
        if( dir == NULL ) {
            dir = ".";
        }
    }
  #endif
    strncpy( path, dir, _MAX_PATH - sizeof( REL_RCS_CFG ) );
    path[_MAX_PATH - sizeof( REL_RCS_CFG )] = '\0';
    strcat( path, REL_RCS_CFG );

    fp = fopen( path, "r" );
    if( fp == NULL ) {
        fp = fopen( ABS_RCS_CFG, "r" );
        if( fp == NULL ) {
            strncpy( buffer, RCS_DEFAULT, len );
            return( 0 );
        }
    }
    fgets( buffer, len, fp );
    fclose( fp );
    return( 1 );
}

int MyWriteProfileString( const char *dir, const char *string )
{
    char path[_MAX_PATH];
    FILE *fp;

  #if defined( __UNIX__ )
    if( dir == NULL ) {
        dir = getenv( "HOME" );
        if( dir == NULL ) {
            dir = ".";
        }
    }
  #endif
    strncpy( path, dir, _MAX_PATH - sizeof( REL_RCS_CFG ) );
    path[_MAX_PATH - sizeof( REL_RCS_CFG )] = '\0';
    strcat( path, REL_RCS_CFG );

    fp = fopen( path, "w" );
    if( fp == NULL ) {
        fp = fopen( ABS_RCS_CFG, "w" );
        if( fp == NULL ) {
            return( 0 );
        }
    }
    fputs( string, fp );
    fclose( fp );
    return( 1 );
}

#endif
