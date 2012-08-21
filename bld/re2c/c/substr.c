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


#include <string.h>
#include "substr.h"
#include "globals.h"

void SubStr_out( const SubStr *s, FILE *o )
{
    uint    i;

    fwrite( s->str, s->len, 1, o );
    for( i = 0; i < s->len; i++ ) {
        if( s->str[i] == '\n' ) {
            oline++;
        }
    }
}

int SubStr_eq( const SubStr *s1, const SubStr *s2 )
{
    return( s1->len == s2->len && memcmp( s1->str, s2->str, s1->len ) == 0 );
}

void SubStr_init( SubStr *r, char *s, uint l )
{
    r->str = s;
    r->len = l;
}

void Str_init( Str *r, const SubStr* s )
{
    SubStr_init( r, malloc( s->len ), s->len );
    memcpy( r->str, s->str, s->len );
}
