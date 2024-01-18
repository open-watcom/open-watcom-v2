/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2024 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Librarian command line parsing (AR mode).
*
****************************************************************************/


#include "wlib.h"
#include "cmdlinea.h"


#define MAX_TOKEN_LEN   260

#define eatwhite( c ) while( *(c) != '\0' && isspace( *(unsigned char *)(c) ) ) ++(c);
#define notwhite( c ) ( (c) != '\0' && !isspace( (unsigned char)(c) ) )
#define my_tolower( c ) tolower( (unsigned char)(c) )

static const char *ParseArOption( const char *c, operation *ar_mode )
{
    const char  *start = c;

    while( notwhite( *c ) ) {
        switch( my_tolower( *c ) ) {
        case '?':
            Usage();
            break;
        case 'c':
            if( Options.no_c_warn ) {
                FatalError( ERR_DUPLICATE_OPTION, start );
            }
            Options.no_c_warn = true;
            break;
        case 'd':
            if( *ar_mode != OP_NONE ) {
                FatalError( ERR_BAD_OPTION, c[0] );
            }
            *ar_mode = OP_DELETE;
            break;
        case 'p':
            //ignore not implemented
            break;
        case 'r':
            if( *ar_mode != OP_NONE ) {
                FatalError( ERR_BAD_OPTION, c[0] );
            }
            *ar_mode = OP_ADD | OP_DELETE;
            break;
        case 't':
            if( *ar_mode != OP_NONE ) {
                FatalError( ERR_BAD_OPTION, c[0] );
            }
            *ar_mode = OP_TABLE;
            Options.list_contents = true;
            break;
        case 'u':
            if( Options.update ) {
                FatalError( ERR_DUPLICATE_OPTION, start );
            }
            Options.update = true;
            break;
        case 'v':
            if( Options.verbose ) {
                FatalError( ERR_DUPLICATE_OPTION, start );
            }
            Options.verbose = true;
            break;
        case 'x':
            if( *ar_mode != OP_NONE ) {
                FatalError( ERR_BAD_OPTION, c[0] );
            }
            *ar_mode = OP_EXTRACT;
            break;
        case '-':
            break;
        default:
            FatalError( ERR_BAD_OPTION, c[0] );
        }
        c++;
    }
    return( c );
}

void ParseOneLineAr( const char *c, operation *ar_mode )
{
    char        token_buff[MAX_TOKEN_LEN];
    bool        done_options;

    done_options = false;
    for( ;; ) {
        eatwhite( c );
        switch( *c ) {
        case '\0':
            if( *ar_mode == OP_EXTRACT ) {
                Options.explode = true;
            }
            return;
        case '-':
            if( !done_options ) {
                if( *(c + 1) == '-' ) {
                    c += 2;
                    done_options = true;
                } else {
                    c = ParseArOption( c, ar_mode );
                }
                break;
            }
            //fall to default
        default:
            if( *ar_mode == OP_NONE ) {
                c = ParseArOption( c, ar_mode );
                break;
            }
            if( Options.input_name == NULL ) {
                Options.input_name = GetFilenameExt( &c, false, token_buff, EXT_LIB );
            } else {
                char    *p;

                p = GetString( &c, token_buff, true );
                AddCommand( *ar_mode, p );
            }
            break;
        }
    }
}

