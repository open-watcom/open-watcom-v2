/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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


#include <string.h>
#if defined( __NT__ )
#include <windows.h>
#endif
#include "vdm.h"
#include "bool.h"
#include "trptypes.h"


bool ValidName( const char *name )
{
    char *end;
    char *dot;

    end = memchr( name, 0, MAX_NAME );
    if( end == NULL )
        return( false );
    if( end == name )
        return( false );
    dot = memchr( name, '.', end - name );
    if( dot != NULL ) {
        if( dot == name )
            return( false );
        if( dot - name > 8 )
            return( false );
        if( end - dot > 4 )
            return( false );
        if( dot[1] == '.' || dot[2] == '.' || dot[3] == '.' ) {
            return( false );
        }
    } else {
        if( end - name > 8 ) {
            return( false );
        }
    }
    while( name < end ) {
        if( *name <= 0x1f )
            return( false );
        switch( *name ) {
        case '"':
        case '/':
        case '\\':
        case ':':
        case '*':
        case '?':
        case '-':
        case '<':
        case '>':
        case '&':
        case '|':
            return( false );
        }
        ++name;
    }
    return( true );
}
