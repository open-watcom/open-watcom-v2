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
* Description:  WHEN YOU FIGURE OUT WHAT THIS MODULE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "stdio.h"

#define ArraySize( x ) ( sizeof( x ) / sizeof( *x ) )
#define DLG_ROW( n ) (n)
#define DLG_COL( n ) (n)



#define BUTTON_GAP( c, o, w, i ) ( ( (c)-(o)*(w) - 2*(i) ) / ( (o)-1 ) )
#define BUTTON_POS( num, of, cols, bwidth ) \
    ( of == 1 ?  \
        (cols - bwidth)/2 : \
        ( 3 + (num-1) * ( bwidth + BUTTON_GAP( cols, of, bwidth, 3 ) ) ) )

enum {
    GUI_GROUPBOX,
    GUI_STATIC,
    GUI_PUSH_BUTTON,
    GUI_DEFPUSH_BUTTON,
    GUI_CHECK_BOX,
    GUI_RADIO_BUTTON,
    GUI_EDIT,
    GUI_LISTBOX,
    GUI_COMBOBOX,
};

typedef struct gui_control_info
{
    int type;
    char *text;
    int x, y, width, height;
    int id;
    char *symbol;
} gui_control_info;

#define DLG_RECT( x1, y1, x2, y2 ) \
      DLG_COL( x1 ), DLG_ROW( y1 ), \
        DLG_COL( (x2)-(x1)+1 ), DLG_ROW( (y2)-(y1)+1 )

#define DLG_BOX( s, x1, y1, x2, y2 ) \
    { GUI_GROUPBOX, s, \
      DLG_RECT( x1, y1, x2, y2 ), \
      -1, "" }

#define DLG_DYNSTRING( s, i, x1, y1, x2 ) \
    { GUI_STATIC, s, \
      DLG_RECT( x1, y1, x2, y1 ), \
      i, #i }

#define DLG_STRING( s, x1, y1, x2 ) \
    DLG_DYNSTRING( s, -1, x1, y1, x2 )

#define DLG_BUTTON( s, i, x1, y1, x2 ) \
    { GUI_PUSH_BUTTON, s, \
      DLG_RECT( x1, y1, x2, y1 ), \
      i, #i }

#define DLG_DEFBUTTON( s, i, x1, y1, x2 ) \
    { GUI_DEFPUSH_BUTTON, s, \
      DLG_RECT( x1, y1, x2, y1 ), \
      i, #i }

#define DLG_CHECK( s, i, x1, y1, x2 ) \
  { GUI_CHECK_BOX, s, \
    DLG_RECT( x1, y1, x2, y1 ), \
    i, #i }

#define DLG_DORADIO( g, s, i, x1, y1, x2 ) \
  { GUI_RADIO_BUTTON, s, \
    DLG_RECT( x1, y1, x2, y1 ), \
    i, #i }

#define DLG_RADIO_START( s, i, x1, y1, x2 ) \
    DLG_DORADIO( GUI_GROUP, s, i, x1, y1, x2 )

#define DLG_RADIO( s, i, x1, y1, x2 ) \
    DLG_DORADIO( GUI_NONE, s, i, x1, y1, x2 )

#define DLG_RADIO_END( s, i, x1, y1, x2 ) \
    DLG_DORADIO( GUI_GROUP, s, i, x1, y1, x2 )

#define DLG_DOEDIT( s, i, x1, y1, x2, v ) \
  { GUI_EDIT, s, \
    DLG_RECT( x1, y1, x2, y1 ), \
    i, #i }

#define DLG_EDIT( s, i, x1, y1, x2 ) \
    DLG_DOEDIT( s, i, x1, y1, x2, 0 )

#define DLG_INVISIBLE_EDIT( s, i, x1, y1, x2 ) \
    DLG_DOEDIT( s, i, x1, y1, x2, GUI_EDIT_INVISIBLE )

#define DLG_LIST_BOX( s, i, x1, y1, x2, y2 ) \
    { GUI_LISTBOX, s, \
      DLG_RECT( x1, y1, x2, y2 ), \
      i, #i }

#define DLG_COMBO_BOX( s, i, x1, y1, x2, y2 ) \
    { GUI_COMBOBOX, s, \
      DLG_RECT( x1, y1, x2, y2 ), \
      i, #i }

#define DlgOpen( sym, id, name, rows, cols, ctl, size, event, extra ) \
    PrintDlg( sym, id, name, ctl, size, rows, cols )

#include "toconv.h"

void PrintDlg(char *sym, int id, char*name,gui_control_info*ctl,int num_controls,int rows,int cols)
{
    int i;
    FILE *hfile, *dlgfile;

    #define ROW_SCALE 14
    #define COL_SCALE 4

    hfile = fopen( "test.h", "w" );
    dlgfile = fopen( "test.dlg", "w" );
    fprintf( hfile, "#define %s %d\n\n", sym, id );
    for( i = 0; i < num_controls; ++i ) {
        if( ctl[i].id != -1 && ctl[i].symbol && ctl[i].symbol[0] ) {
            fprintf( hfile, "#define %s %d\n", ctl[i].symbol, ctl[i].id );
        }
        if( !ctl[i].symbol || !ctl[i].symbol[0] ) {
            ctl[i].symbol = "0";
        }
    }
    fprintf( dlgfile, "\n%s DIALOG %d, %d, %d, %d\n", sym, COL_SCALE, ROW_SCALE, COL_SCALE*(cols+2), ROW_SCALE*(rows+1) );
    fprintf( dlgfile, "STYLE DS_MODALFRAME | WS_CAPTION | WS_POPUP | WS_VISIBLE | WS_SYSMENU\n" );
    fprintf( dlgfile, "CAPTION \"%s\"\n", name );
//    fprintf( dlgfile, "FONT 8, \"Helv\"\n" );
    fprintf( dlgfile, "BEGIN\n" );
    for( i = 0; i < num_controls; ++i, ++ctl ) {
        int x = ( ctl->x + 1 ) * COL_SCALE;
        int y = ( ctl->y + 1 ) * ROW_SCALE;
        int width = ( ctl->width ) * COL_SCALE;
        int height = ( ctl->height ) * ROW_SCALE;
        fprintf( dlgfile, "    CONTROL " );
        switch( ctl->type ) {
        case GUI_GROUPBOX:
            fprintf( dlgfile, "\"%s\", 0, \"BUTTON\", BS_GROUPBOX | WS_CHILD | WS_VISIBLE ", ctl->text );
            height -= ROW_SCALE/2;
            break;
        case GUI_STATIC:
            fprintf( dlgfile, "\"%s\", 0, \"STATIC\", SS_LEFT | WS_CHILD | WS_VISIBLE ", ctl->text );
            break;
        case GUI_PUSH_BUTTON:
            fprintf( dlgfile, "\"%s\", %s, \"BUTTON\", BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE | WS_TABSTOP ", ctl->text, ctl->symbol );
            height = 14;
            break;
        case GUI_DEFPUSH_BUTTON:
            fprintf( dlgfile, "\"%s\", %s, \"BUTTON\", BS_DEFPUSHBUTTON | WS_CHILD | WS_VISIBLE | WS_TABSTOP ", ctl->text, ctl->symbol );
            height = 14;
            break;
        case GUI_CHECK_BOX:
            fprintf( dlgfile, "\"%s\", %s, \"BUTTON\", BS_AUTOCHECKBOX | WS_CHILD | WS_VISIBLE | WS_TABSTOP ", ctl->text, ctl->symbol );
            break;
        case GUI_RADIO_BUTTON:
            fprintf( dlgfile, "\"%s\", %s, \"BUTTON\", BS_AUTORADIOBUTTON | WS_CHILD | WS_VISIBLE ", ctl->text, ctl->symbol );
            break;
        case GUI_EDIT:
            fprintf( dlgfile, "\"\", %s, \"EDIT\", ES_LEFT | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP ", ctl->symbol );
            height = 12;
            break;
        case GUI_LISTBOX:
            fprintf( dlgfile, "\"\", %s, \"LISTBOX\", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | WS_TABSTOP ", ctl->symbol );
            break;
        case GUI_COMBOBOX:
            fprintf( dlgfile, "\"\", %s, \"COMBOBOX\", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | WS_TABSTOP ", ctl->symbol );
            break;
        }
        fprintf( dlgfile, ", %d, %d, %d, %d\n", x, y, width, height );
    }
    fprintf( dlgfile, "END\n" );
    fclose( hfile );
    fclose( dlgfile );
}

main()
{
    Doit
}
