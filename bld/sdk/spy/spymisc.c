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
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include "spy.h"
#include <commdlg.h>

/*
 * GetHexStr - convert a number to a hex string, padded out with 0's
 */
void GetHexStr( LPSTR res, DWORD num, int padlen )
{
    char        tmp[10];
    int         i;
    int         j,k;

    ultoa( num, tmp, 16 );
    i = strlen( tmp );
    k = 0;
    for( j=i;j<padlen;j++ ) {
        res[k++] = '0';
    }
    for( j=0;j<i;j++ ) {
        res[k++] = tmp[j];
    }

} /* GetHexStr */

/*
 * IsMyWindow - check if a specific window belongs to our task
 */
BOOL IsMyWindow( HWND hwnd )
{

    if( hwnd == NULL ) {
        return( FALSE );
    }
    if( GetWindowTask( hwnd ) == MyTask ) {
        if( hwnd == GetDesktopWindow() ) {
            return( FALSE );
        }
        return( TRUE );
    }
    return( FALSE );

} /* IsMyWindow */

/*
 * GetWindowName - set up a window name string
 */
void GetWindowName( HWND hwnd, char *str )
{
    int         len;
    char        name[64];

    if( hwnd == NULL ) {
        strcpy( str, GetRCString( STR_NONE ) );
        return;
    }
    len = GetWindowText( hwnd, name, sizeof( name ) );
    name[ len ] = 0;
    if( len == 0 ) {
        GetHexStr( str, (UINT) hwnd, 4 );
        str[4] = 0;
    } else {
        sprintf( str,"%0*x: %s", UINT_STR_LEN, (UINT) hwnd, name );
    }

} /* GetWindowName */

/*
 * GetWindowStyleString - get string corrosponding to window style bits
 */
void GetWindowStyleString( HWND hwnd, char *str, char *sstr )
{
    UINT        id;
    DWORD       style;
    WORD        wstyle;
    char        tmp[40];
    int         len;
    char        *rcstr;

    style = GetWindowLong( hwnd, GWL_STYLE );
    wstyle = (WORD) style;

    GetHexStr( str, style, 8 );
    str[8] = 0;
    sstr[0] = 0;

    if( style & WS_POPUP ) {
        strcat( sstr, "WS_POPUP " );
    }
    if( style & WS_CHILD ) {
        strcat( sstr, "WS_CHILD " );
        id = GET_ID( hwnd );
        rcstr = GetRCString( STR_CHILD_ID );
        sprintf( tmp, rcstr, id, UINT_STR_LEN, id );
        strcat( str, tmp );
    }
    if( !(style & (WS_POPUP|WS_CHILD) ) ) {
        strcat( sstr, "WS_OVERLAPPED " );
    }

    if( style & WS_BORDER ) {
        strcat( sstr, "WS_BORDER " );
    }
    if( style & WS_CAPTION ) {
        strcat( sstr, "WS_CAPTION " );
    }
    if( style & WS_SYSMENU ) {
        strcat( sstr, "WS_SYSMENU " );
    }
    if( style & WS_THICKFRAME ) {
        strcat( sstr, "WS_THICKFRAME " );
    }
    if( style & WS_MINIMIZEBOX ) {
        strcat( sstr, "WS_MINIMIZEBOX " );
    }
    if( style & WS_MAXIMIZEBOX ) {
        strcat( sstr, "WS_MAXIMIZEBOX " );
    }
    if( style & WS_MINIMIZE ) {
        strcat( sstr, "WS_MINIMIZE " );
    }
    if( style & WS_VISIBLE ) {
        strcat( sstr, "WS_VISIBLE " );
    }
    if( style & WS_DISABLED ) {
        strcat( sstr, "WS_DISABLED " );
    }
    if( style & WS_CLIPSIBLINGS ) {
        strcat( sstr, "WS_CLIPSIBLINGS " );
    }
    if( style & WS_CLIPCHILDREN ) {
        strcat( sstr, "WS_CLIPCHILDREN " );
    }
    if( style & WS_MAXIMIZE ) {
        strcat( sstr, "WS_MAXIMIZE " );
    }
    if( style & WS_DLGFRAME ) {
        strcat( sstr, "WS_DLGFRAME " );
    }
    if( style & WS_VSCROLL ) {
        strcat( sstr, "WS_VSCROLL " );
    }
    if( style & WS_HSCROLL ) {
        strcat( sstr, "WS_HSCROLL " );
    }
    if( style & WS_GROUP ) {
        strcat( sstr, "WS_GROUP " );
    }
    if( style & WS_TABSTOP ) {
        strcat( sstr, "WS_TABSTOP " );
    }

    len = GetClassName( hwnd, tmp, sizeof( tmp ) );
    tmp[ len ] = 0;
    if( !stricmp( tmp, "button" ) ) {
        if( style & BS_LEFTTEXT ) {
            strcat( sstr, "BS_LEFTTEXT " );
        }
        switch( wstyle & ~BS_LEFTTEXT ) {
        case BS_PUSHBUTTON:
            strcat( sstr, "BS_PUSHBUTTON " );
            break;
        case BS_DEFPUSHBUTTON:
            strcat( sstr, "BS_DEFPUSHBUTTON " );
            break;
        case BS_CHECKBOX:
            strcat( sstr, "BS_CHECKBOX " );
            break;
        case BS_AUTOCHECKBOX:
            strcat( sstr, "BS_AUTOCHECKBOX " );
            break;
        case BS_RADIOBUTTON:
            strcat( sstr, "BS_RADIOBUTTON " );
            break;
        case BS_3STATE:
            strcat( sstr, "BS_3STATE " );
            break;
        case BS_AUTO3STATE:
            strcat( sstr, "BS_AUTO3STATE " );
            break;
        case BS_GROUPBOX:
            strcat( sstr, "BS_GROUPBOX " );
            break;
        case BS_USERBUTTON:
            strcat( sstr, "BS_USERBUTTON " );
            break;
        case BS_AUTORADIOBUTTON:
            strcat( sstr, "BS_AUTORADIOBUTTON " );
            break;
        case BS_OWNERDRAW:
            strcat( sstr, "BS_OWNERDRAW " );
            break;
        }
    } else if( !stricmp( tmp, "edit" ) ) {
        if( style & ES_LEFT ) {
            strcat( sstr, "ES_LEFT " );
        }
        if( style & ES_CENTER ) {
            strcat( sstr, "ES_CENTER " );
        }
        if( style & ES_RIGHT ) {
            strcat( sstr, "ES_RIGHT " );
        }
        if( style & ES_MULTILINE ) {
            strcat( sstr, "ES_MULTILINE " );
        }
        if( style & ES_UPPERCASE ) {
            strcat( sstr, "ES_UPPERCASE " );
        }
        if( style & ES_LOWERCASE ) {
            strcat( sstr, "ES_LOWERCASE " );
        }
        if( style & ES_PASSWORD ) {
            strcat( sstr, "ES_PASSWORD " );
        }
        if( style & ES_AUTOVSCROLL ) {
            strcat( sstr, "ES_AUTOVSCROLL " );
        }
        if( style & ES_AUTOHSCROLL ) {
            strcat( sstr, "ES_AUTOHSCROLL " );
        }
        if( style & ES_NOHIDESEL ) {
            strcat( sstr, "ES_NOHIDESEL " );
        }
        if( style & ES_OEMCONVERT ) {
            strcat( sstr, "ES_OEMCONVERT " );
        }
        if( style & ES_READONLY ) {
            strcat( sstr, "ES_READONLY " );
        }
    } else if( !stricmp( tmp, "static" ) ) {
        if( style & SS_NOPREFIX ) {
            strcat( sstr, "SS_NOPREFIX " );
        }
        switch( wstyle & ~SS_NOPREFIX ) {
        case SS_LEFT:
            strcat( sstr, "SS_LEFT " );
            break;
        case SS_CENTER:
            strcat( sstr, "SS_CENTER " );
            break;
        case SS_RIGHT:
            strcat( sstr, "SS_RIGHT " );
            break;
        case SS_ICON:
            strcat( sstr, "SS_ICON " );
            break;
        case SS_BLACKRECT:
            strcat( sstr, "SS_BLACKRECT " );
            break;
        case SS_GRAYRECT:
            strcat( sstr, "SS_GRAYRECT " );
            break;
        case SS_WHITERECT:
            strcat( sstr, "SS_WHITERECT " );
            break;
        case SS_BLACKFRAME:
            strcat( sstr, "SS_BLACKFRAME " );
            break;
        case SS_GRAYFRAME:
            strcat( sstr, "SS_GRAYFRAME " );
            break;
        case SS_WHITEFRAME:
            strcat( sstr, "SS_WHITEFRAME " );
            break;
//      case SS_USERITEM:
//          strcat( sstr, "SS_USERITEM " );
//          break;
        case SS_SIMPLE:
            strcat( sstr, "SS_SIMPLE " );
            break;
        case SS_LEFTNOWORDWRAP:
            strcat( sstr, "SS_LEFTNOWORDWRAP " );
            break;
        }
    } else if( !stricmp( tmp, "listbox" ) ) {
        if( style & LBS_NOTIFY ) {
            strcat( sstr, "LBS_NOTIFY " );
        }
        if( style & LBS_SORT ) {
            strcat( sstr, "LBS_SORT " );
        }
        if( style & LBS_NOREDRAW ) {
            strcat( sstr, "LBS_NOREDRAW " );
        }
        if( style & LBS_MULTIPLESEL ) {
            strcat( sstr, "LBS_MULTIPLESEL " );
        }
        if( style & LBS_OWNERDRAWFIXED ) {
            strcat( sstr, "LBS_OWNERDRAWFIXED " );
        }
        if( style & LBS_OWNERDRAWVARIABLE ) {
            strcat( sstr, "LBS_OWNERDRAWVARIABLE " );
        }
        if( style & LBS_HASSTRINGS ) {
            strcat( sstr, "LBS_HASSTRINGS " );
        }
        if( style & LBS_USETABSTOPS ) {
            strcat( sstr, "LBS_USETABSTOPS " );
        }
        if( style & LBS_NOINTEGRALHEIGHT ) {
            strcat( sstr, "LBS_NOINTEGRALHEIGHT " );
        }
        if( style & LBS_MULTICOLUMN ) {
            strcat( sstr, "LBS_MULTICOLUMN " );
        }
        if( style & LBS_WANTKEYBOARDINPUT ) {
            strcat( sstr, "LBS_WANTKEYBOARDINPUT " );
        }
        if( style & LBS_EXTENDEDSEL ) {
            strcat( sstr, "LBS_EXTENDEDSEL " );
        }
        if( style & LBS_DISABLENOSCROLL ) {
            strcat( sstr, "LBS_DISABLENOSCROLL " );
        }
    } else if( !stricmp( tmp, "combobox" ) ) {
        if( style & CBS_SIMPLE ) {
            strcat( sstr, "CBS_SIMPLE " );
        }
        if( style & CBS_DROPDOWN ) {
            strcat( sstr, "CBS_DROPDOWN " );
        }
        if( style & CBS_DROPDOWNLIST ) {
            strcat( sstr, "CBS_DROPDOWNLIST " );
        }
        if( style & CBS_OWNERDRAWFIXED ) {
            strcat( sstr, "CBS_OWNERDRAWFIXED " );
        }
        if( style & CBS_OWNERDRAWVARIABLE ) {
            strcat( sstr, "CBS_OWNERDRAWVARIABLE " );
        }
        if( style & CBS_AUTOHSCROLL ) {
            strcat( sstr, "CBS_AUTOHSCROLL " );
        }
        if( style & CBS_OEMCONVERT ) {
            strcat( sstr, "CBS_OEMCONVERT " );
        }
        if( style & CBS_SORT ) {
            strcat( sstr, "CBS_SORT " );
        }
        if( style & CBS_HASSTRINGS ) {
            strcat( sstr, "CBS_HASSTRINGS " );
        }
        if( style & CBS_NOINTEGRALHEIGHT ) {
            strcat( sstr, "CBS_NOINTEGRALHEIGHT " );
        }
        if( style & CBS_DISABLENOSCROLL ) {
            strcat( sstr, "CBS_DISABLENOSCROLL " );
        }
    } else {
        if( style & WS_EX_DLGMODALFRAME ) {
            strcat( sstr, "WS_EX_DLGMODALFRAME " );
        }
        if( style & WS_EX_NOPARENTNOTIFY ) {
            strcat( sstr, "WS_EX_NOPARENTNOTIFY " );
        }
        if( style & WS_EX_TOPMOST ) {
            strcat( sstr, "WS_EX_TOPMOST " );
        }
        if( style & WS_EX_ACCEPTFILES ) {
            strcat( sstr, "WS_EX_ACCEPTFILES " );
        }
        if( style & WS_EX_TRANSPARENT ) {
            strcat( sstr, "WS_EX_TRANSPARENT " );
        }
    }

} /* GetWindowStyleString */

#ifndef __NT__
 #define STYLE_TYPE             WORD
 #define STYLE_HEX_LEN          4
#else
 #define STYLE_TYPE             DWORD
 #define STYLE_HEX_LEN          8
#endif

/*
 * GetClassStyleString - get a string corrosponding to class style bits
 */
void GetClassStyleString( HWND hwnd, char *str, char *sstr )
{
    STYLE_TYPE          style;

    style = GET_CLASS_STYLE( hwnd );

    GetHexStr( str, style, STYLE_HEX_LEN  );
    str[ STYLE_HEX_LEN ] = 0;
    sstr[0] = 0;

    if( style & CS_VREDRAW ) {
        strcat( sstr, "CS_VREDRAW " );
    }
    if( style & CS_HREDRAW ) {
        strcat( sstr, "CS_HREDRAW " );
    }
    if( style & CS_KEYCVTWINDOW ) {
        strcat( sstr, "CS_KEYCVTWINDOW " );
    }
    if( style & CS_DBLCLKS ) {
        strcat( sstr, "CS_DBLCLKS " );
    }
    if( style & CS_OWNDC ) {
        strcat( sstr, "CS_OWNDC " );
    }
    if( style & CS_CLASSDC ) {
        strcat( sstr, "CS_CLASSDC " );
    }
    if( style & CS_PARENTDC ) {
        strcat( sstr, "CS_PARENTDC " );
    }
    if( style & CS_NOKEYCVT ) {
        strcat( sstr, "CS_NOKEYCVT " );
    }
    if( style & CS_NOCLOSE ) {
        strcat( sstr, "CS_NOCLOSE " );
    }
    if( style & CS_SAVEBITS ) {
        strcat( sstr, "CS_SAVEBITS " );
    }
    if( style & CS_BYTEALIGNCLIENT ) {
        strcat( sstr, "CS_BYTEALIGNCLIENT " );
    }
    if( style & CS_BYTEALIGNWINDOW ) {
        strcat( sstr, "CS_BYTEALIGNWINDOW " );
    }
    if( style & CS_GLOBALCLASS ) {
        strcat( sstr, "CS_GLOBALCLASS " );
    }

} /* GetClassStyleString */

/*
 * DumpToComboBox - dump a string of space separated items to a combo box
 */
void DumpToComboBox( char *str, HWND cb )
{
    char        tmp[128];
    int         i;

    SendMessage( cb, CB_RESETCONTENT, 0, 0L );
    while( *str != 0 ) {
        i = 0;
        while( *str != ' ' ) {
            tmp[i++] = *str;
            str++;
        }
        tmp[i] = 0;
        if( i != 0 ) {
            str++;
            SendMessage( cb, CB_ADDSTRING, 0, (LONG) (LPSTR) tmp );
        }
    }
    SendMessage( cb, CB_SETCURSEL, 0, 0L );

} /* DumpToComboBox */

/*
 * FormatSpyMessage - make a spy message string
 */
void FormatSpyMessage( char *msg, LPMSG pmsg, char *res )
{

    memset( res,' ', SPYOUT_LENGTH );
    strcpy( res, msg );
    res[ strlen( msg ) ] = ' ';
    GetHexStr( &res[SPYOUT_HWND], (DWORD) pmsg->hwnd, SPYOUT_HWND_LEN );
    GetHexStr( &res[SPYOUT_MSG], pmsg->message, SPYOUT_MSG_LEN );
    GetHexStr( &res[SPYOUT_WPARAM], pmsg->wParam, SPYOUT_WPARAM_LEN );
    GetHexStr( &res[SPYOUT_LPARAM], pmsg->lParam, SPYOUT_LPARAM_LEN );
    res[SPYOUT_LENGTH] = 0;

} /* FormatSpyMessage */

/*
 * SetSpyState - set current spy state
 */
void SetSpyState( spystate ss )
{
    char        str[130];
    char        *rcstr;
    int         len;

    SpyState = ss;
    len = GetWindowText( SpyMainWindow, &str[1], 128 );
    switch( ss ) {
    case OFF:
        if( str[1] != '<' ) {
            str[0] = '<';
            str[len+1] = '>';
            str[len+2] = 0;
            SetWindowText( SpyMainWindow, str );
        }
        rcstr = GetRCString( STR_SPY_ON );
        ModifyMenu( SpyMenu, SPY_OFFON, MF_BYCOMMAND | MF_ENABLED | MF_STRING,
                        SPY_OFFON, rcstr );
        break;
    case ON:
        rcstr = GetRCString( STR_SPY_OFF );
        ModifyMenu( SpyMenu, SPY_OFFON, MF_BYCOMMAND | MF_ENABLED | MF_STRING,
                        SPY_OFFON, rcstr );
        if( str[1] == '<' ) {
            str[len] = 0;
            SetWindowText( SpyMainWindow, &str[2] );
        }
        break;
    case NEITHER:
        rcstr = GetRCString( STR_SPY_OFF );
        ModifyMenu( SpyMenu, SPY_OFFON, MF_BYCOMMAND | MF_GRAYED | MF_STRING,
                        SPY_OFFON, rcstr );
        if( str[1] == '<' ) {
            str[len] = 0;
            SetWindowText( SpyMainWindow, &str[2] );
        }
        break;
    }
    SetOnOffTool( ss );

} /* SetSpyState */

static char filterList[] = "File (*.*)" \
                        "\0" \
                        "*.*" \
                        "\0\0";

/*
 * GetFileName - get a file name using common dialog stuff
 */
BOOL GetFileName( char *ext, int type, char *fname )
{
    OPENFILENAME        of;
    BOOL                rc;

    fname[ 0 ] = 0;
    memset( &of, 0, sizeof( OPENFILENAME ) );
    of.lStructSize = sizeof( OPENFILENAME );
    of.hwndOwner = SpyMainWindow;
    of.lpstrFilter = (LPSTR) filterList;
    of.lpstrDefExt = ext;
    of.nFilterIndex = 1L;
    of.lpstrFile = fname;
    of.nMaxFile = _MAX_PATH;
    of.lpstrTitle = NULL;
    of.Flags = OFN_HIDEREADONLY;
    if( type == FILE_OPEN ) {
        rc = GetOpenFileName( &of );
    } else {
        rc = GetSaveFileName( &of );
    }
    return( rc );

} /* GetFileName */

BOOL InitGblStrings( void ) {

    DWORD       heading;
    DWORD       heading_uline;

    SpyName = AllocRCString( STR_APP_NAME );
    if( SpyName == NULL ) return( FALSE );

#ifdef __NT__
    heading = STR_HEADINGS_NT;
    heading_uline = STR_HEADING_UNDERLINE_NT;
#else
    heading = STR_HEADINGS_WIN;
    heading_uline = STR_HEADING_UNDERLINE_WIN;
#endif
    TitleBar = AllocRCString( heading );
    TitleBarLen = strlen( TitleBar + 1 );
    if( TitleBar == NULL ) return( FALSE );

    TitleBarULine = AllocRCString( heading_uline );
    if( TitleBarULine == NULL ) return( FALSE );
    return( TRUE );
}
