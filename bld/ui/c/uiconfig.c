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


#ifdef UNIX
    #include "clibext.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "uidef.h"
#include "uiattrs.h"

bool uiconfig( char *fn, char **envvars )
/***************************************/
{
    char        buffer[ _MAX_PATH + ATTR_LAST * 4 ];
    void        *config;
    int         i;
    char        *colour;
    int         slen;
    int         blen;
    char        *s;
    ATTR        attr;

    _unused( envvars );

    UIData->no_snow = TRUE;
    uiattrs();
    if( UIData->colour == M_MONO ) {
        colour = "ATTR_MONO";
        slen = 9;
    } else if( UIData->colour == M_BW ) {
        colour = "ATTR_BW";
        slen = 7;
    } else {
        colour = "ATTR_COL";
        slen = 8;
    }
    #if !defined( NLM )
        for( ; envvars != NULL  &&  *envvars != NULL; ++envvars ) {
            _searchenv( fn, *envvars, buffer );
            if( buffer[0] != '\0' ) {
                break;
            }
        }
        fn = buffer;
    #endif
    if( fn != NULL && fn[0] != '\0' ) {
        config = fopen( fn, "r" );
        if( config != NULL ) {
            while( fgets( buffer, _MAX_PATH+ATTR_LAST*4, config ) != NULL ) {
                blen = strlen( buffer );
                if( blen > slen && memicmp( colour, buffer, slen ) == 0 ) {
                    s = &buffer[ slen ];
                    for( i = 0 ; i < ATTR_LAST && *s == ' '; ++i ) {
                        attr = 0;
                        while( *s == ' ' ) ++s;
                        while( *s != '\0' && *s != '\n' && *s != ' ' ) {
                            attr = 10*attr + (*s-'0');
                            ++s;
                        }
                        if( attr != 0 ){
                            UIData->attrs[i] = attr;
                        }
                    }
                } else if( blen > 9 && memicmp( "SNOWCHECK", buffer, 9 ) == 0 ) {
                    UIData->no_snow = ( buffer[ 10 ] == '0' );
                } else if( blen > 11 && memicmp( "MOUSE_SPEED", buffer, 11 ) == 0 ) {
                    uimousespeed( strtol( &buffer[ 12 ], NULL, 10 ) );
                } else if( blen > 8 && memicmp( "GRAPHICS", buffer, 8 ) == 0 ) {
                    UIData->no_graphics = ( buffer[ 9 ] == '0' );
                }
            }
            fclose( config );
            return( TRUE );
        }
    }
    return( FALSE );
}
