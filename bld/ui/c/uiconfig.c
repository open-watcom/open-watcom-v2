/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
#include "uidef.h"
#include "uiattrs.h"

#include "clibext.h"


#define LIT_CONFIG_ATTR_MONO    "ATTR_MONO"
#define LIT_CONFIG_ATTR_BW      "ATTR_BW"
#define LIT_CONFIG_ATTR_COL     "ATTR_COL"

#define LIT_CONFIG_SNOWCHECK    "SNOWCHECK"
#define LIT_CONFIG_MOUSE_SPEED  "MOUSE_SPEED"
#define LIT_CONFIG_GRAPHICS     "GRAPHICS"

#define STRIEQLIT(l,s,c)  (l > sizeof( c ) - 1 && strnicmp(s, c, sizeof( c ) - 1) == 0)

bool uiconfig( const char *fn, char **envvars )
/*********************************************/
{
    char        buffer[_MAX_PATH + ATTR_LAST * 4];
    void        *config;
    int         i;
    char        *colour;
    unsigned    slen;
    unsigned    blen;
    char        *s;
    ATTR        attr;

#if defined( __NETWARE__ )
    /* unused parameters */ (void)envvars;
#endif

    UIData->no_snow = true;
    uiattrs();
#if !defined( __NETWARE__ )
    for( ; envvars != NULL && *envvars != NULL; ++envvars ) {
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
            if( UIData->colour == M_MONO ) {
                colour = LIT_CONFIG_ATTR_MONO;
                slen = sizeof( LIT_CONFIG_ATTR_MONO ) - 1;
            } else if( UIData->colour == M_BW ) {
                colour = LIT_CONFIG_ATTR_BW;
                slen = sizeof( LIT_CONFIG_ATTR_BW ) - 1;
            } else {
                colour = LIT_CONFIG_ATTR_COL;
                slen = sizeof( LIT_CONFIG_ATTR_COL ) - 1;
            }
            while( fgets( buffer, sizeof( buffer ), config ) != NULL ) {
                blen = strlen( buffer );
                if( blen > slen && strnicmp( colour, buffer, slen ) == 0 ) {
                    s = &buffer[slen];
                    for( i = 0; i < ATTR_LAST && *s == ' '; ++i ) {
                        while( *s == ' ' )
                            ++s;
                        attr = 0;
                        while( *s != '\0' && *s != '\n' && *s != ' ' ) {
                            attr = 10 * attr + (*s - '0');
                            ++s;
                        }
                        if( attr != 0 ) {
                            UIData->attrs[i] = attr;
                        }
                    }
                } else if( STRIEQLIT( blen, buffer, LIT_CONFIG_SNOWCHECK ) ) {
                    UIData->no_snow = ( buffer[sizeof( LIT_CONFIG_SNOWCHECK )] == '0' );
                } else if( STRIEQLIT( blen, buffer, LIT_CONFIG_MOUSE_SPEED ) ) {
                    long speed = strtol( &buffer[sizeof( LIT_CONFIG_MOUSE_SPEED )], NULL, 10 );
                    uimousespeed( ( speed < 0 ) ? 0 : (unsigned)speed );
                } else if( STRIEQLIT( blen, buffer, LIT_CONFIG_GRAPHICS ) ) {
                    UIData->no_graphics = ( buffer[sizeof( LIT_CONFIG_GRAPHICS )] == '0' );
                }
            }
            fclose( config );
            return( true );
        }
    }
    return( false );
}
