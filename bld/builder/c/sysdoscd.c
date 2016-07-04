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
* Description:  chdir with drive letters and remove trailing slash
*
****************************************************************************/

#include <dos.h>
#include <string.h>
#include <direct.h>
#include <ctype.h>
#include "builder.h"

#include "clibext.h"


int SysDosChdir( char *dir )
{
    char        *end;
    size_t      len;
    unsigned    total;

    if( dir[0] == '\0' )
        return( 0 );
    len = strlen( dir );
    if( len > 1 ) {
        end = dir + len - 1;
        switch( *end ) {
        case '\\':
        case '/':
            if( len != 3 || dir[1] != ':' ) {
                *end = '\0';
                --len;
            }
            break;
        }
    }
    if( len > 1 && dir[1] == ':' ) {
        _dos_setdrive( toupper( dir[0] ) - 'A' + 1, &total );
    }
    return( chdir( dir ) );
}
