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

enum {
    DBG_LITERAL_FIRST = 30000,
#undef LITSTR
#define LITSTR( x, y ) DBG_LITERAL_##x,
    #include "literals.h"
    DBG_LITERAL_LAST,
    MENU_FIRST = 31000,
    #include "menustr.h"
    MENU_LAST
};

typedef struct {
    char *name;
    int value;
} literal_define;

literal_define ToPrint[] = {
    #undef LITSTR
    #define LITSTR( a, b ) "DBG_LITERAL_" #a, DBG_LITERAL_##a,
    #include "literals.h"
    #include "menustr.h"
};

int main( int argc, char *argv[] )
{
    int         i;

    if( argc <= 1 || freopen( argv[1], "w", stdout ) != stdout ) {
        fprintf( stderr, "Can't open output file\n" );
    }
    for( i = 0; i < sizeof( ToPrint ) / sizeof( ToPrint[0] ); ++i ) {
        printf( "#define %s %d\n", ToPrint[i].name, ToPrint[i].value );
    }
    return( 0 );
}
