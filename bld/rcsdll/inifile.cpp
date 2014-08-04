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


#if defined( __OS2__ )
    #include <stdlib.h>
    #include <string.h>
    #include <malloc.h>
    #define INCL_WINSHELLDATA
    extern "C" {
    #include <os2.h>
    }

    // just use the os/2 user .ini file
    int MyGetProfileString( const char *dir, const char *filename, const char *section,
                            const char *key, const char *def, char *buffer, int len ) {
        dir = dir; filename = filename;
        return( PrfQueryProfileString( HINI_USERPROFILE, section, key, def, (void *)buffer, len ) );
    }
    int MyWriteProfileString( const char *dir, const char *filename, const char *section,
                              const char *key, const char *string ) {
        dir = dir; filename = filename;
        return( PrfWriteProfileString( HINI_USERPROFILE, section, key, string ) );
    }
#elif defined( __WINDOWS__ ) || defined( __NT__ )
    #include <windows.h>
    int MyGetProfileString( const char *dir, const char *filename, const char *section,
                            const char *key, const char *def, char *buffer, int len )
    {
        dir =dir; // ignored in this model
        return(GetPrivateProfileString(section,key,def,buffer,len,filename));
    }
    int MyWriteProfileString( const char *dir, const char *filename, const char *section,
                              const char *key, const char *string )
    {
        dir =dir; // ignored in this model
        return(WritePrivateProfileString(section,key,string,filename));
    }
#elif defined( __UNIX__ )
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "watcom.h"
#include "clibext.h"

    int MyGetProfileString( const char *dir, const char *filename, const char *section,
                            const char *key, const char *def, char *buffer, int len )
    {
        char path[_MAX_PATH];
        FILE *fp;

        section=section;
        key=key;

        if( dir == NULL ) {
            dir = getenv( "HOME" );
            if( dir == NULL ) dir = ".";
        }

        strcpy( path, dir );
        strcat( path, "/.wvi/" );
        strcat( path, filename );

        fp = fopen( path, "r" );
        if( !fp ) {
            strcpy( path, "/usr/watcom/wvi/" );
            strcat( path, filename );
            fp = fopen( path, "r" );
            if( !fp ) {
                strncpy( buffer, def, len );
                return( 0 );
            }
        }
        fgets( buffer, len, fp );
        fclose( fp );
        return( 1 );
    }

    int MyWriteProfileString( const char *dir, const char *filename, const char *section,
                              const char *key, const char *string )
    {
        char path[_MAX_PATH];
        FILE *fp;
        section=section;
        key=key;

        if( dir == NULL ) {
            dir = getenv( "HOME" );
            if( dir == NULL ) dir = ".";
        }

        strcpy( path, dir );
        strcat( path, "/.wvi/" );
        strcat( path, filename );

        fp = fopen( path, "w" );
        if( !fp ) {
            strcpy( path, "/usr/watcom/wvi/" );
            strcat( path, filename );
            fp = fopen( path, "w" );
            if( !fp ) return( 0 );
        }
        fputs( string, fp );
        fclose( fp );
        return( 1 );
    }
#elif defined( __DOS__ )
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "clibext.h"

    int MyGetProfileString( const char *dir, const char *filename, const char *section,
                            const char *key, const char *def, char *buffer, int len )
    {
        char path[_MAX_PATH];
        FILE *fp;

        section=section;
        key=key;

        sprintf( path, dir );
        strcat( path, "\\binw\\" );
        strcat( path, filename );

        fp = fopen( path, "r" );
        if( !fp ) {
            sprintf( path, "c:\\" );
            strcat( path, filename );
            fp = fopen( path, "r" );
            if( !fp ) {
                strncpy( buffer, def, len );
                return( 0 );
            }
        }
        fgets( buffer, len, fp );
        fclose( fp );
        return( 1 );
    }

    int MyWriteProfileString( const char *dir, const char *filename, const char *section,
                              const char *key, const char *string )
    {
        char path[_MAX_PATH];
        FILE *fp;
        section=section;
        key=key;

        sprintf( path, dir );
        strcat( path, "\\binw\\" );
        strcat( path, filename );

        fp = fopen( path, "w" );
        if( !fp ) {
            sprintf( path, "c:\\" );
            strcat( path, filename );
            fp = fopen( path, "w" );
            if( !fp ) return( 0 );
        }
        fputs( string, fp );
        fclose( fp );
        return( 1 );
    }
#else
    #error UNSUPPORTED OS
#endif
