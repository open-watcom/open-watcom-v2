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


#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "ctags.h"


/*
 * ScanFortran - scan a fortran file
 */
void ScanFortran( void )
{
    char        buff[MAX_STR];
    char        token[MAX_STR];
    char        *buffptr;
    int         i;

    while( GetString( buff, sizeof( buff ) ) ) {

        buffptr = buff;
        while( isspace( *buffptr ) ) {
            buffptr++;
        }
        if( *buffptr == 0 ) {
            continue;
        }
        if( !MyStricmp( &buffptr, "character" ) ||
            !MyStricmp( &buffptr, "complex" ) ||
            !MyStricmp( &buffptr, "integer" ) ||
            !MyStricmp( &buffptr, "logical" ) ||
            !MyStricmp( &buffptr, "real" ) ) {
            while( isspace( *buffptr ) ) {
                buffptr++;
            }
            if( *buffptr == '*' ) {
                buffptr++;
                while( isspace( *buffptr ) ) {
                    buffptr++;
                }
                if( !isdigit( *buffptr ) ) {
                    buffptr--;
                } else {
                    while( isdigit( *buffptr ) ) {
                        buffptr++;
                    }
                }
            }
        } else if( !MyStricmp( &buffptr, "double" ) ) {
            while( isspace( *buffptr ) ) {
                buffptr++;
            }
            if( MyStricmp( &buffptr, "precision" ) ) {
                continue;
            }
        }

        while( isspace( *buffptr ) ) {
            buffptr++;
        }
        if( MyStricmp( &buffptr, "function" ) &&
            MyStricmp( &buffptr, "program" ) &&
            MyStricmp( &buffptr, "procedure" ) &&
            MyStricmp( &buffptr, "subroutine" ) ) {
            continue;
        }
        while( isspace( *buffptr ) ) {
            buffptr++;
        }
        i = 0;
        while( IsTokenChar( *buffptr ) ) {
            token[i++] = *buffptr++;
        }
        if( i == 0 ) {
            continue;
        }
        token[i] = 0;
        RecordCurrentLineData();
        AddTag( token );
    }

} /* ScanFortran */
