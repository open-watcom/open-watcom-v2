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
//#include "spy.h"

#define msgpick( a, b, c, d, e, f )     { d }

char *MessageArray[] =  {
    #include "spyarray.h"
};

unsigned MessageArraySize = sizeof( MessageArray ) / sizeof( char * );

#define NO_COLS                 3
#define NO_DIALOGS              4
#define MSGS_PER_DIALOG         ( no_rows * NO_COLS )

#define ITEM_HITE               10
#define ITEM_WIDTH              118
#define X_SPACE                 2

#define X_ORIGIN                2
#define Y_ORIGIN                4

#define BUTTON_HITE             36
#define BUTTON_SPACE            10

#define XPOS( x )               ( X_ORIGIN + x * ( ITEM_WIDTH + X_SPACE ) )
#define YPOS( x )               ( Y_ORIGIN + x * ITEM_HITE )

#define DIALOG_WIDTH            ( NO_COLS * ( ITEM_WIDTH + X_SPACE ) + 2 *\
                                  X_ORIGIN )
#define DIALOG_HITE             ( ( no_rows * ITEM_HITE ) + 2 * Y_ORIGIN +\
                                  BUTTON_HITE + BUTTON_SPACE )

void main( int argc, char *argv[] ) {

    FILE        *dlgfile;
    FILE        *hfile;
    unsigned    i, j, k;
    unsigned    index;
    unsigned    last;
    unsigned    button_top;
    unsigned    no_rows;

    if( argc != 3 ) {
        printf( "mkdlg [ .dlg filename] [ .h filename ]\n\n" );
        exit( 0 );
    }
    dlgfile = fopen( argv[1], "wt" );
    if( dlgfile == NULL ) {
        printf( "Unable to open .dlg file\n" );
        exit( 1 );
    }
    hfile = fopen( argv[2], "wt" );
    if( hfile == NULL ) {
        printf( "Unable to open .h file\n" );
        exit( 1 );
    }
    no_rows = MessageArraySize / ( NO_COLS * NO_DIALOGS );
    if( MessageArraySize % ( NO_COLS * NO_DIALOGS ) != 0 ) no_rows++;

    for( i=0; i < NO_DIALOGS; i++ ) {

        /*
         * write the dialog header
         */

        fprintf( dlgfile, "SPYMSGS%u DIALOG 2, -10, %u, %u\n", i + 1,
                    DIALOG_WIDTH, DIALOG_HITE );

        fprintf( dlgfile, "STYLE WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU\n" );
        last = ( i + 1 ) * MSGS_PER_DIALOG - 1;
        if( last >= MessageArraySize ) last = MessageArraySize - 1;
        fprintf( dlgfile, "CAPTION \"\"\n",
                MessageArray[ i * MSGS_PER_DIALOG ],
                MessageArray[ last ] );
//      fprintf( hfile, "#define MSG_RANGE_%u \"Messages %s to %s\"\n",
//              i, MessageArray[ i * MSGS_PER_DIALOG ],
//              MessageArray[ last ] );
        fprintf( hfile, "#define MSG_RANGE_%u_FIRST \"%s\"\n",
                i, MessageArray[ i * MSGS_PER_DIALOG ] );
        fprintf( hfile, "#define MSG_RANGE_%u_LAST  \"%s\"\n",
                i, MessageArray[ last ] );
        fprintf( dlgfile, "FONT 8, \"Helv\"\n" );
        fprintf( dlgfile, "BEGIN\n" );

        /*
         * add the check boxes
         */

        for( j=0; j < NO_COLS; j ++ ) {
            for( k=0; k < no_rows; k++ ) {
                index = i * MSGS_PER_DIALOG + j * no_rows + k;
                if( index >= MessageArraySize ) break;
                fprintf( dlgfile,
                        "\tCHECKBOX       \"%s\", DLGMSG%u+%u, %u, %u, %u, %u\n",
                        MessageArray[ index ], i + 1,
                        index - i * MSGS_PER_DIALOG, XPOS( j ),
                        YPOS( k ), ITEM_WIDTH, ITEM_HITE );
            }
        }

        button_top = YPOS( no_rows ) + BUTTON_SPACE;
        fprintf( dlgfile,
            "\tPUSHBUTTON     \"Clear All\", DLGMSG_CLEARALL, 74, %u, 40, 14\n",
            button_top );
        fprintf( dlgfile, "\tPUSHBUTTON     \"OK\", IDOK, 192, %u, 40, 14\n",
                button_top );
        fprintf( dlgfile,
            "\tPUSHBUTTON     \"Cancel\", IDCANCEL, 236, %u, 40, 14\n",
            button_top );
        fprintf( dlgfile,
            "\tPUSHBUTTON     \"Set All\", DLGMSG_SETALL, 118, %u, 40, 14\n",
            button_top );
        fprintf( dlgfile,
            "\tPUSHBUTTON     \"Prev ...\", DLGMSG_PREV, 124, %u, 46, 16\n",
            button_top + 20 );
        fprintf( dlgfile,
            "\tPUSHBUTTON     \"Next ...\", DLGMSG_NEXT, 182, %u, 46, 16\n",
            button_top + 20 );
        fprintf( dlgfile, "END\n\n" );
    }

    /*
     * create the .h file
     */
    fprintf( hfile, "\n" );
    fprintf( hfile, "#define DLGMSG1             5000\n" );
    fprintf( hfile, "#define DLGMSG2             DLGMSG1 + %u\n",
            NO_COLS * no_rows );
    fprintf( hfile, "#define DLGMSG3             DLGMSG2 + %u\n",
            NO_COLS * no_rows );
    fprintf( hfile, "#define DLGMSG4             DLGMSG3 + %u\n",
            NO_COLS * no_rows );
    fprintf( hfile, "#define DLGMSG4END          DLGMSG4 + %u\n\n",
            MessageArraySize - NO_COLS * no_rows * 3 );
    fprintf( hfile, "#define DLGMSG_CLEARALL     4800\n" );
    fprintf( hfile, "#define DLGMSG_SETALL       4801\n" );
    fprintf( hfile, "#define DLGMSG_NEXT         4082\n" );
    fprintf( hfile, "#define DLGMSG_PREV         4803\n" );
    exit( 0 );
}
