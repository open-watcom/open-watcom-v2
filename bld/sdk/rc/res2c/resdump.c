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
#include <stdlib.h>
#include <string.h>
#define INCL_WIN
#include <os2.h>
#include <io.h>
#include <fcntl.h>
#include "loadchar.h"
#include "loadtype.h"

static char *type_table[] = {
    "un_Known",
    "rt_Pointer",
    "rt_Bitmap",
    "rt_Menu",
    "rt_Dialog",
    "rt_String",
    "rt_Fontdir",
    "rt_Font",
    "rt_Acceltable",
    "rt_Rcdata"
    "rt_Message",
    "rt_Dlginclude",
    "rt_Vkeytbl",
    "rt_Keytbl",
    "rt_Chartbl",
    "rt_Displayinfo",
    "rt_Fkashort",
    "rt_Fkallong"
    "rt_Helptable",
    "rt_Helpsubtable",
    "rt_Fddir",
    "rt_Fd"
};

static void startComment( void ) {
    putchar('/');
    puts("*");
}

static void endComment( void ) {
    putchar('*');
    puts("/");
}

int main( int argc, char **argv ) {

    int         fh;
    int         ch;
    read_file   c_info;
    char        *trash;
    char        *s_name;
    char        *s_type;
    long        size;
    int         type;
    int         name;
    char        junk[15];
    ULONG       x;


    if ( argc < 2 ) {
        puts("Filename not specified");
        exit( 1 );
    }

    fh = open( argv[1], O_RDONLY | O_BINARY );
    if ( fh == -1 ) {
        puts("Error opening file");
        exit( 1 );
    }

    trash = malloc( 10240 );
    if ( !trash ) {
        puts("Out of memory ");
        close( fh );
        exit( 1 );
    }


    ch = PeekFirstChar( trash, 10240, fh, &c_info );
    if ( ch == -1 ) {
        puts("Error reading file");
        close( fh );
        exit( 1 );
    }

    while ( ch != -1 ) {

        startComment();
        printf(" * The Resource is of type ");
        if ( ch == 0xff ) {
            GetNextChar( &c_info );
            type = GetNextShort( &c_info );
            if ( ( type < 0 ) || ( type >= RT_MAX ) ) {
                type = 0;
            }
            puts( type_table[ type ] );
            s_type = NULL;
        } else {
            s_type = GetNextString( &c_info );
            puts( s_type );
        }

        ch = PeekNextChar( &c_info );
        if ( ch == -1 ) {
            puts("File read error occured");
            close( fh );
            exit( 1 );
        }

        printf(" * The name is " );
        if ( ch == 0xff ) {
            GetNextChar( &c_info );
            name = GetNextShort( &c_info );
            s_name = itoa( name, junk, 10 );
            puts( s_name );
        } else {
            s_name = GetNextString( &c_info );
            puts( s_name );
        }

        printf(" * The flags are %X\n", GetNextShort( &c_info ) );
        size = GetNextLong( &c_info );
        printf(" * Total size of the resource is %lu bytes\n ", size );
        endComment();
        puts("");
        printf("static char %s_%s[] = {\n",
            ( s_type ? s_type : type_table[type] ), s_name );
        for( x = 0; x < size; x++ ) {
            ch = GetNextChar( &c_info );
            if ( ch == -1 ) {
                puts("File read error occured");
                close( fh );
                exit( 1 );
            }
            if ( !( ( x + 1 ) % 13 ) ) {
                puts(" ");
            }
            printf("0x%2.2X, ", ch );
        }
        puts(" }; \n\n");
        ch = PeekNextChar( &c_info );
    }
    close( fh );
    return(0);
};
