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
* Description:  WPI OS/2 INI file routines.
*
****************************************************************************/


#define INCL_PM
#define INCL_DOSNLS
#include <os2.h>
#include "wpi.h"


HINI _wpi_openinifile( WPI_INST inst, char *name )
/************************************************/
{
    char                *ptr;
    char                *end;
    PRFPROFILE          prof;
    HINI                ret;

    ptr = name;
    if( strchr( name, '\\' ) == NULL && strchr( name, ':' ) == NULL ) {
        /* no path; use the system INI directory to prevent OS/2
           from creating multiple INI files for each program */

        prof.cchUserName = 0;
        prof.cchSysName = 0;
        prof.pszSysName = NULL;
        prof.pszUserName = NULL;
        if( PrfQueryProfile( inst.hab, &prof ) ) {
            ptr = _wpi_malloc( prof.cchSysName + strlen( name ) + 1 );
            prof.pszSysName = ptr;
            prof.pszUserName = _wpi_malloc( prof.cchUserName );
            PrfQueryProfile( inst.hab, &prof );
            end = strrchr( ptr, '\\' );
            if( end != NULL ) {
                strcpy( end + 1, name );
            } else {
                strcpy( ptr, name );
            }
        }
    }

    ret = PrfOpenProfile( inst.hab, ptr );

    if( prof.pszSysName != NULL ) {
        _wpi_free( prof.pszSysName );
        _wpi_free( prof.pszUserName );
    }

    return( ret );
}

void _wpi_getinidirectory( WPI_INST inst, LPSTR dir_info, int size )
/******************************************************************/
{
    PRFPROFILE                  prof;
    int                         i;
    char                        c;
    int                         len;

    prof.cchUserName = 0L;
    prof.cchSysName = 0L;

    if( PrfQueryProfile( inst.hab, &prof ) ) {
        if( prof.cchSysName > 0 ) {
            _wpi_malloc2( prof.pszSysName, prof.cchSysName );
            _wpi_malloc2( prof.pszUserName, prof.cchUserName );
            PrfQueryProfile( inst.hab, &prof );
        }

        strcpy( dir_info, prof.pszSysName );
        if( prof.cchSysName <= size ) {
            len = prof.cchSysName;
        } else {
            len = size;
        }

        for( i = len - 1; i >= 0; i-- ) {
            c = dir_info[i];
            dir_info[i] = '\0';
            if( c == '\\' ) {
                break;
            }
        }
    } else {
        dir_info[0] = '\0';
    }
    _wpi_free( prof.pszUserName );
    _wpi_free( prof.pszSysName );
} /* _wpi_getinidirectory */

int _wpi_getprivateprofilestring( HINI hini, LPSTR app,
                LPSTR key, LPSTR def, LPSTR buf, int size, LPSTR dummy )
/**********************************************************************/
{
    ULONG                       len;

    dummy = dummy;

    len = PrfQueryProfileString( hini, app, key, def, buf, size );
    if( len != 0 && len < size && buf[len-1] != '\0' ) {
        buf[len] = '\0';
    }

    return( (int)len );
}
