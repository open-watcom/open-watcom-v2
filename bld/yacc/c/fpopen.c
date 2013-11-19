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
* Description:  Path-searching file open routine.
*
****************************************************************************/


#include <string.h>
#include <stdio.h>
#include "walloca.h"


FILE *fpopen( char *path, char *name )
/*
 *  Attempt to open 'name', first in the current directory, then along
 *  'path' (a list of ';' separated directory names.)
 */
{
    FILE        *file;
    char        *qualname, *p;
    int         dirlen, namelen;

    if( !(file = fopen( name, "r" )) && path ) {
        namelen = strlen( name );
        qualname = alloca( strlen( path ) + namelen + 1 );
        for( ;; ) {
            for( p = path; *p != '\0' && *p != ';'; ++p );
            memcpy( qualname, path, dirlen = p - path );
            if( p > path && p[-1] != '\\' && p[-1] != '/' )
                qualname[dirlen++] = '/';
            memcpy( &qualname[dirlen], name, namelen );
            qualname[dirlen + namelen] = '\0';
            if( (file = fopen( qualname, "r" )) || !*p )
                break;
            path = &p[1];
        }
    }
    return( file );
}
