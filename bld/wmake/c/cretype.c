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


/*
 *  This program can be used to rebuild the IsArray table in mglobs.c.
 *
 */

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
    if( noneyet )   printf( #stuff );       \
    else            printf( " | " #stuff ); \
    noneyet = 0;                            \
}


void main( void )
/***************/
{
    int     i;
    int     noneyet;

    /*printf( "extern UINT8 IsArray[] = {\n" );*/
    printf( "/*STRM_MAGIC*/  0,\n" );
    printf( "/*STRM_END  */  0" );      /* note: no ",\n" !! */

    for( i = 0; i <= 255; i++ ) {
        noneyet = 1;

        if( isprint( i ) ) {
            printf( ",\n/*   '%c'    */  ", i );
        } else {
            printf( ",\n/*   0x%02x   */  ", i );
        }

        if( isws( i ) )     BAR( IS_WS    );
        if( isprt( i ) )    BAR( IS_PRINT );
        if( isalpha( i ) )  BAR( IS_ALPHA );
        if( isextc( i ) )   BAR( IS_EXTC  );
        if( isdirc( i ) )   BAR( IS_DIRC  );
        if( isfilec( i ) )  BAR( IS_FILEC );
        if( ismacc( i ) )   BAR( IS_MACC );
        if( isbarf( i ) )   BAR( IS_BARF );

        if( noneyet )   printf( "0" );
    }

    /*printf("\n};\n");*/
    printf( "\n" );
    exit( 0 );
}
