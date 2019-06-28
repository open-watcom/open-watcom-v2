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
* Description:  This program builds the IsArray table for mglobs.c.
*
****************************************************************************/


#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "bool.h"


    /* foreign language support */
#define islang(__c)     ( ( (__c) >= 0x80 && (__c) <= 0xa7 ) || \
                        ( (__c) >= 0xe0 && (__c) <= 0xee ) )

    /* macro identifiers */
#define ismacc(__c)     ( isalnum(__c) || (__c) == '_' || \
                        islang(__c) || (__c) == '%' )

    /* extension characters */
#define isextc(__c)     ( isalnum(__c) || (__c) == '_' || islang(__c) || \
                        (__c) == '-' || (__c) == '*' || (__c) == '?' || \
                        (__c) == '&' || (__c) == '$' || (__c) == '!' || \
                        (__c) == '#' || (__c) == '%' || (__c) == '\'' || \
                        (__c) == '(' || (__c) == ')' || (__c) == '^' || \
                        (__c) == '`' || (__c) == '{' || (__c) == '}' || \
                        (__c) == '~' || (__c) == '@' \
                        )

    /* directory separator */
#define isdirc(__c2)    ( (__c2) == '/' || (__c2) == '\\' || (__c2) == ':' )

    /* filename character */
#define isfilec(__c3)   ( isextc(__c3) || isdirc(__c3) || (__c3) == '.' )

    /* not quite isspace - renamed to make sure you realize difference */
#define isws(__c)       ( (__c) == ' ' || (__c) == '\t' )


#define isprt(__c)      ( (__c) >= 32 && (__c) <= 255 )

    /* is an illegal character in a file */
#define isbarf(__c)     ( !isprt(__c) && (__c) != '\t' && (__c) != '\a' && \
                            (__c) != '\f' && (__c) != '\n' )


    /* this macro is used to make coding easier below */
#define BAR( stuff )    {                   \
    if( noneyet )   fprintf( fo, #stuff );       \
    else            fprintf( fo, " | " #stuff ); \
    noneyet = false;                        \
}


int main( int argc, char *argv[] )
/********************************/
{
    int     i;
    bool    noneyet;
    FILE    *fo;

    fo = NULL;
    if( argc > 0 ) {
        fo = fopen( argv[1], "w" );
    }
    if( fo == NULL ) {
        fo = stdout;
    }
    /*fprintf( fo, "extern UINT8 IsArray[] = {\n" );*/
    fprintf( fo, "/*STRM_TMP_LEX_START*/  0,\n" );
    fprintf( fo, "/*   STRM_TMP_EOL   */  0,\n" );
    fprintf( fo, "/*    STRM_MAGIC    */  0,\n" );
    fprintf( fo, "/*     STRM_END     */  0,\n" );

    for( i = 0; i <= 255; i++ ) {
        noneyet = true;

        if( i > 0 )
            fprintf( fo, ",\n" );
        if( isprint( i ) ) {
            fprintf( fo, "/*       '%c'        */  ", i );
        } else {
            fprintf( fo, "/*       0x%02x       */  ", i );
        }

        if( isws( i ) ) {
            BAR( IS_WS    );
        }
        if( isprt( i ) ) {
            BAR( IS_PRINT );
        }
        if( isalpha( i ) ) {
            BAR( IS_ALPHA );
        }
        if( isextc( i ) ) {
            BAR( IS_EXTC  );
        }
        if( isdirc( i ) ) {
            BAR( IS_DIRC  );
        }
        if( isfilec( i ) ) {
            BAR( IS_FILEC );
        }
        if( ismacc( i ) ) {
            BAR( IS_MACC );
        }
        if( isbarf( i ) ) {
            BAR( IS_BARF );
        }

        if( noneyet ) {
            fprintf( fo, "0" );
        }
    }
    /*fprintf( fo, "};\n" );*/
    fprintf( fo, "\n" );
    fclose( fo );
    return( 0 );
}
