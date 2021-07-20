/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Spy miscellaneous functions.
*
****************************************************************************/


#include "spy.h"
#include <commdlg.h>


/*
 * GetHexStr - convert a number to a hex string, padded out with 0's
 */
void GetHexStr( LPSTR res, ULONG_PTR num, size_t padlen )
{
    char        tmp[20];
    size_t      i;
    size_t      j,k;

#ifdef _WIN64
    i = sprintf( tmp, "%llx", num );
#else
    i = sprintf( tmp, "%lx", num );
#endif
    k = 0;
    for( j = i; j < padlen; j++ ) {
        res[k++] = '0';
    }
    for( j = 0; j < i; j++ ) {
        res[k++] = tmp[j];
    }
    if( padlen == 0 ) {
        res[k] = '\0';
    }

} /* GetHexStr */

/*
 * IsMyWindow - check if a specific window belongs to our task
 */
bool IsMyWindow( HWND hwnd )
{
    if( hwnd == NULL ) {
        return( false );
    }
    if( GetWindowTask( hwnd ) == MyTask ) {
        if( hwnd == GetDesktopWindow() ) {
            return( false );
        }
        return( true );
    }
    return( false );

} /* IsMyWindow */

/*
 * GetWindowName - set up a window name string
 */
void GetWindowName( HWND hwnd, char *str )
{
    int         len;
    char        name[64];
    char        hexstr[20];

    if( hwnd == NULL ) {
        strcpy( str, GetRCString( STR_NONE ) );
        return;
    }
    len = GetWindowText( hwnd, name, sizeof( name ) );
    name[len] = '\0';
    if( len == 0 ) {
        GetHexStr( str, (UINT_PTR)hwnd, HWND_HEX_LEN );
        str[HWND_HEX_LEN] = '\0';
    } else {
        GetHexStr( hexstr, (UINT_PTR)hwnd, HWND_HEX_LEN );
        hexstr[HWND_HEX_LEN] = '\0';
        sprintf( str, "%s: %s", hexstr, name );
    }

} /* GetWindowName */

/*
 * GetWindowStyleString - get string corrosponding to window style bits
 */
void GetWindowStyleString( HWND hwnd, char *str, char *sstr )
{
    UINT        id;
    DWORD       style;
    DWORD       exstyle;
    char        tmp[40];
    int         len;
    const char  *rcstr;
    WORD        i;
    WORD        j;
    char        hexstr[20];

    style = GET_WNDSTYLE( hwnd );
    exstyle = GET_WNDEXSTYLE( hwnd );

    GetHexStr( str, style, STYLE_HEX_LEN );
    str[STYLE_HEX_LEN] = '\0';
    sstr[0] = '\0';

    if( style & WS_CHILD ) {
        id = GET_ID( hwnd );
        GetHexStr( hexstr, id, 0 );
        rcstr = GetRCString( STR_CHILD_ID );
        sprintf( tmp, rcstr, id, hexstr );
        strcat( str, tmp );
    }
    for( i = 0; i < StyleArraySize; i++ ) {
        if( (style & StyleArray[i].mask) == StyleArray[i].flags ) {
            strcat( sstr, StyleArray[i].name );
            strcat( sstr, " " );
        }
    }

    len = GetClassName( hwnd, tmp, sizeof( tmp ) );
    tmp[len] = '\0';
    for( i = 0; i < ClassStylesSize; i++ ) {
        if( stricmp( tmp, ClassStyles[i].class_name ) == 0 ) {
            for( j = 0; j < ClassStyles[i].style_array_size; j++ ) {
                if( (style & ClassStyles[i].style_array[j].mask) == ClassStyles[i].style_array[j].flags ) {
                    strcat( sstr, ClassStyles[i].style_array[j].name );
                    strcat( sstr, " " );
                }
            }
        }
    }

    for( i = 0; i < ExStyleArraySize; i++ ) {
        if( (exstyle & ExStyleArray[i].mask) == ExStyleArray[i].flags ) {
            strcat( sstr, ExStyleArray[i].name );
            strcat( sstr, " " );
        }
    }

} /* GetWindowStyleString */

/*
 * GetClassStyleString - get a string corrosponding to class style bits
 */
void GetClassStyleString( HWND hwnd, char *str, char *sstr )
{
    DWORD       style;
    int         i;

    style = GET_CLASS_STYLE( hwnd );

    GetHexStr( str, style, STYLE_HEX_LEN  );
    str[STYLE_HEX_LEN] = '\0';
    sstr[0] = '\0';

    for( i = 0; i < ClassStyleArraySize; i++ ) {
        if( (style & ClassStyleArray[i].mask) == ClassStyleArray[i].flags ) {
            strcat( sstr, ClassStyleArray[i].name );
            strcat( sstr, " " );
        }
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
    while( *str != '\0' ) {
        i = 0;
        while( *str != ' ' ) {
            tmp[i++] = *str;
            str++;
        }
        tmp[i] = '\0';
        if( i != 0 ) {
            str++;
            SendMessage( cb, CB_ADDSTRING, 0, (LPARAM)(LPCSTR)tmp );
        }
    }
    SendMessage( cb, CB_SETCURSEL, 0, 0L );

} /* DumpToComboBox */

/*
 * FormatSpyMessage - make a spy message string
 */
void FormatSpyMessage( const char *msg, LPMSG pmsg, char *res )
{
    memset( res,' ', SPYOUT_LENGTH );
    memcpy( res, msg, strlen( msg ) );
    GetHexStr( res + SPYOUT_HWND, (UINT_PTR)pmsg->hwnd, SPYOUT_HWND_LEN );
    GetHexStr( res + SPYOUT_MSG, pmsg->message, SPYOUT_MSG_LEN );
    GetHexStr( res + SPYOUT_WPARAM, pmsg->wParam, SPYOUT_WPARAM_LEN );
    GetHexStr( res + SPYOUT_LPARAM, pmsg->lParam, SPYOUT_LPARAM_LEN );
    res[SPYOUT_LENGTH] = '\0';

} /* FormatSpyMessage */

/*
 * SetSpyState - set current spy state
 */
void SetSpyState( spystate ss )
{
    char        str[130];
    const char  *rcstr;
    int         len;

    SpyState = ss;
    len = GetWindowText( SpyMainWindow, str + 1, sizeof( str ) - 2 );
    switch( ss ) {
    case OFF:
        if( str[1] != '<' ) {
            str[0] = '<';
            str[len + 1] = '>';
            str[len + 2] = '\0';
            SetWindowText( SpyMainWindow, str );
        }
        rcstr = GetRCString( STR_SPY_ON );
        ModifyMenu( SpyMenu, SPY_OFFON, MF_BYCOMMAND | MF_ENABLED | MF_STRING, SPY_OFFON, rcstr );
        break;
    case ON:
        rcstr = GetRCString( STR_SPY_OFF );
        ModifyMenu( SpyMenu, SPY_OFFON, MF_BYCOMMAND | MF_ENABLED | MF_STRING, SPY_OFFON, rcstr );
        if( str[1] == '<' ) {
            str[len] = '\0';
            SetWindowText( SpyMainWindow, str + 2 );
        }
        break;
    case NEITHER:
        rcstr = GetRCString( STR_SPY_OFF );
        ModifyMenu( SpyMenu, SPY_OFFON, MF_BYCOMMAND | MF_GRAYED | MF_STRING, SPY_OFFON, rcstr );
        if( str[1] == '<' ) {
            str[len] = '\0';
            SetWindowText( SpyMainWindow, str + 2 );
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
bool GetFileName( char *ext, file_dlg_type type, char *fname )
{
    OPENFILENAME        of;
    BOOL                rc;

    fname[0] = '\0';
    memset( &of, 0, sizeof( OPENFILENAME ) );
    of.lStructSize = sizeof( OPENFILENAME );
    of.hwndOwner = SpyMainWindow;
    of.lpstrFilter = (LPSTR)filterList;
    of.lpstrDefExt = ext;
    of.nFilterIndex = 1L;
    of.lpstrFile = fname;
    of.nMaxFile = _MAX_PATH;
    of.lpstrTitle = NULL;
    of.Flags = OFN_HIDEREADONLY;
    if( type == DLG_FILE_OPEN ) {
        rc = GetOpenFileName( &of );
    } else {
        rc = GetSaveFileName( &of );
    }
    return( rc != 0 );

} /* GetFileName */

bool InitGblStrings( void )
{
    msg_id      heading;
    msg_id      heading_uline;

    SpyName = AllocRCString( STR_APP_NAME );
    if( SpyName == NULL ) {
        return( false );
    }

#ifdef __WINDOWS__
    heading = STR_HEADINGS_WIN;
    heading_uline = STR_HEADING_UNDERLINE_WIN;
#elif defined( _WIN64 )
    heading = STR_HEADINGS_NT64;
    heading_uline = STR_HEADING_UNDERLINE_NT64;
#else
    heading = STR_HEADINGS_NT;
    heading_uline = STR_HEADING_UNDERLINE_NT;
#endif
    TitleBar = AllocRCString( heading );
    if( TitleBar == NULL ) {
        return( false );
    }
    TitleBarULine = AllocRCString( heading_uline );
    if( TitleBarULine == NULL ) {
        return( false );
    }
    TitleBarLen = strlen( TitleBar );
    return( true );

} /* InitGblStrings */
