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
* Description:  Shared procedures and data for encoding WASM keywords
*
****************************************************************************/


#define KEY_MAX_LEN 80

typedef struct sword {
    char        *word;
    unsigned    index;
} sword;
   
static char enum_key[ KEY_MAX_LEN + 10 ];

static int str_compare( const void *p1, const void *p2 )
/******************************************************/
{
    return( strcmp( ((const sword *)p1)->word,
                    ((const sword *)p2)->word ) );
}

static char *get_enum_key( const char *src )
/******************************************/
{
    int     add_underscore;
    char    *dst;

    add_underscore = 1;
    dst = enum_key;
    *dst++ = 'T';
    while( *src ) {
        if( *src == '.' ) {
            *dst++ = '_';
            *dst++ = 'D';
            *dst++ = 'O';
            *dst++ = 'T';
            add_underscore = 1;
        } else if( *src == '?' ) {
            *dst++ = '_';
            *dst++ = 'U';
            *dst++ = 'N';
            add_underscore = 1;
        } else {
            if( add_underscore ) {
                *dst++ = '_';
                add_underscore = 0;
            }
            *dst++ = (char)toupper( *src );
        }
        ++src;
    }
    *dst = '\0';
    return( enum_key );
}
