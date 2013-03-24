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


#include "windows.h"
#include "regstr.h"
#include "reglist.h"
#include "drwatcom.h"
#include "mem.h"
#include "malloc.h"
#include "bitman.h"
#include "madcli.h"
#include "regcrt.h"

mad_registers * RegListGetMadRegisters( HWND list )
{
    HWND stat;
    stat = GetParent( list );
    return( StatGetMadRegisters( stat ) );
}

mad_reg_set_data * RegListGetMadRegSetData( HWND list )
{
    RegListData *data;
    data = (RegListData *)GetWindowLong( list, 0 );
    return( data->reg_set );
}

static void ScrollRegList( HWND hwnd, UINT msg, WPARAM wparam )
{
    WORD    ScrollCode;
    WORD    nPos;
    int     ScrollPos;
    int     min_pos;
    int     max_pos;
    int     scroll_handle;
    int     dx;
    int     dy;
    int     *delta;

    ScrollCode = LOWORD( wparam );
    nPos = HIWORD( wparam );

    if( msg == WM_VSCROLL ) {
        scroll_handle = SB_VERT;
        delta = &dy;
        dx = 0;
    } else {
        scroll_handle = SB_HORZ;
        delta = &dx;
        dy = 0;
    }

    ScrollPos = GetScrollPos( hwnd, scroll_handle );
    GetScrollRange( hwnd, scroll_handle, &min_pos, &max_pos );

    switch (ScrollCode){
        case SB_PAGERIGHT:  // SB_PAGERIGHT == SB_PAGEDOWN
            *delta = -80;
            break;
        case SB_LINERIGHT:  // SB_LINERIGHT == SB_LINEDOWN
            *delta = -10;
            break;
        case SB_PAGELEFT:   // SB_PAGELEFT == SB_PAGEUP
            *delta = 80;
            break;
        case SB_LINELEFT:   // SB_LINELEFT == SB_LINEUP
            *delta = 10;
            break;
        case SB_THUMBPOSITION:
            *delta = ScrollPos - nPos;
            break;
        default:
            return;
    }


    if ( ( ScrollPos - ( *delta ) ) < min_pos ){
        *delta = ScrollPos - min_pos;
    }
    if( ( ScrollPos - ( *delta ) ) > max_pos ){
         *delta = ScrollPos - max_pos;
    }
    ScrollPos -= ( *delta );
    if ( dx == 0 && dy == 0 ){
        return;
    }
    ScrollWindow( hwnd, dx, dy, NULL, NULL );
    SetScrollPos( hwnd, scroll_handle, ScrollPos, TRUE );
}

static void SetupRegList( HWND list, mad_reg_set_data *reg_set, RegStringCreateData **create, int *nregs, int *ncolumns, int *sp, int *ht, int *extra )
{
    HDC                 dc;
    TEXTMETRIC          tm;
    RECT                rect;
    int                 space;
    int                 height;
    int                 scroll_width;
    int                 scroll_height;
    int                 line_width;
    int                 num_columns;
    int                 num_regs;
    int                 i;
    int                 num_rows;
    mad_registers       *regs;



    dc = GetDC( list );
    SelectObject( dc, GetMonoFont() );
    GetTextMetrics( dc, &tm );
    regs = RegListGetMadRegisters( list );
    GetWindowRect( list, &rect );
    rect.right = rect.right - rect.left - GetSystemMetrics( SM_CXBORDER );
    rect.bottom = rect.bottom - rect.top - GetSystemMetrics( SM_CYBORDER );
    scroll_height = GetSystemMetrics( SM_CYHSCROLL );
    scroll_width = GetSystemMetrics( SM_CXVSCROLL );
    GetRegStringCreate( regs, reg_set, ( rect.right - scroll_width - tm.tmMaxCharWidth ) / tm.tmMaxCharWidth, create,  &num_regs, &num_columns );
    line_width = 0;
    for( i = 0; i < num_columns; i++ ){
        ( *create )[i].length *= tm.tmMaxCharWidth;
        line_width += ( *create )[i].length;
    }
    num_rows = ( num_regs + num_columns - 1 ) / num_columns;
    if( ( rect.right - line_width ) < 0 ) {
        space = 0;
        rect.bottom -= scroll_height;
    } else {
        space = 1;
    }
    height = tm.tmHeight * 1.1;
    if ( height * num_rows > rect.bottom ){
        rect.right -= scroll_width;
        if ( space == 1 ){
            if ( ( rect.right - line_width ) < 0 ) {
                rect.bottom -= scroll_height;
                space = 0;
            }
        }
        SetScrollRange( list, SB_VERT, 0, height * num_rows - rect.bottom + tm.tmHeight, FALSE );
    }else {
        SetScrollRange( list, SB_VERT, 0, 0, FALSE );
    }
    if ( space == 0 ){
        *extra = 0;
        space = 2 *tm.tmMaxCharWidth;
        line_width += space * ( num_columns + 1 );
        SetScrollRange( list, SB_HORZ, 0, line_width - rect.right, FALSE );
    } else {
        space = ( rect.right - line_width ) / ( num_columns + 1);
        line_width += space * ( num_columns + 1 );
        *extra = rect.right - line_width;
        SetScrollRange( list, SB_HORZ, 0, 0, FALSE );
    }
    ReleaseDC( list, dc );
    *sp = space;
    *ht = height;
    *ncolumns = num_columns;
    *nregs = num_regs;
}

/* adds all registers */
static void AddRegisters(HWND list,mad_reg_set_data *reg_set )
{
    int                     i;
    int                     j;
    int                     x;
    int                     y;
    int                     id;
    int                     num_regs;
    RegStringCreateData     *reg_create;
    int                     space;
    int                     height;
    int                     num_columns;
    int                     extra;

    SetupRegList( list, reg_set, &reg_create, &num_regs, &num_columns, &space, &height, &extra );
    y = j = id = 0;
    x = space;
    for( i = 0; i < num_regs; i++ ) {
        if( extra < j ) {
            x++;
        }
        if( IsEmptyString( reg_create[i].buffer ) == FALSE ) {
            CreateRegString( list, x, y, reg_create[j].length, height, reg_create[i].buffer, id );
        }
        x += reg_create[j].length + space;
        id++;
        j++;
        if( j == num_columns ) {
            y += height;
            x = space;
            j = 0;
        }
    }
    FreeRegStringCreate( reg_create, num_regs );
}

#define BUFF_SIZE 64
static void CreateRegListMenu( RegListData *data )
{
    const mad_toggle_strings    *mts;
    UINT                i;
    char                buffer[BUFF_SIZE];
    unsigned            init_value;

    data->menu = CreatePopupMenu();

    mts = MADRegSetDisplayToggleList( data->reg_set );
    init_value = MADRegSetDisplayToggle( data->reg_set, 0, 0 );

    AppendMenu( data->menu, MF_STRING, 1, "Modify..." );
    AppendMenu( data->menu, MF_SEPARATOR, 2, "" );

    for( i = 0 ; mts[ i ].menu != MAD_MSTR_NIL; i++ ) {
        MADCliString( mts[ i ].menu , BUFF_SIZE - 1, buffer );
        if( init_value & ( 1 << i ) ) {
            AppendMenu( data->menu, MF_STRING | MF_CHECKED, i + MAD_MENU_FIRST_ITEM,
             buffer );
        } else {
            AppendMenu( data->menu, MF_STRING,  i + MAD_MENU_FIRST_ITEM, buffer );
        }
    }

}

HWND CreateRegList( CreateRegListData *data )
{
    WORD        xconv;
    WORD        yconv;
    LONG        base_units;
    HWND        list;
    RegListData *list_data;

    base_units = GetDialogBaseUnits();
    xconv = LOWORD( base_units ) / 4;
    yconv = HIWORD( base_units ) / 8;
    list = CreateWindow( "REG_LIST",  //class name
        "Registers",
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | WS_BORDER | WS_TABSTOP,
        2 * xconv,          //x coord
        40 * yconv,         //y coord
        277 * xconv,        //width
        75 * yconv,         //height
        data->parent,       //parent
        (HMENU)( REG_LIST_FIRST + data->index ),    //id
        Instance,
        NULL );
    list_data = MemAlloc( sizeof( RegListData ) );
    SetWindowLong( list, 0, (LONG)list_data );
    AddRegisters( list, data->reg_set );
    list_data->curr_reg = GetWindow( list, GW_CHILD );
    list_data->reg_set = data->reg_set;
    CreateRegListMenu( list_data );
    return ( list );
}

static void MakeStringCurrent(HWND list, HWND string, RegListData *data ){
    RECT    string_rect;
    RECT    list_rect;
    int     dx;
    int     dy;
    int     pos;

    if ( data->curr_reg != string ){
        SendMessage( data->curr_reg, REG_STRING_DESELECTED, 0, 0 );
        data->curr_reg = string;
    }

    GetWindowRect( list, &list_rect );
    GetClientRect( list, &string_rect );
    list_rect.right = list_rect.left + string_rect.right;
    list_rect.bottom = list_rect.top + string_rect.bottom;
    GetWindowRect( string, &string_rect);

    if( string_rect.left < list_rect.left ) {
        dx = list_rect.left - string_rect.left;
    } else {
        if( string_rect.right > list_rect.right ) {
            dx = list_rect.right - string_rect.right;
        } else {
            dx = 0;
        }
    }

    if( string_rect.top < list_rect.top ) {
        dy = list_rect.top - string_rect.top;
    } else {
        if( string_rect.bottom > list_rect.bottom ) {
            dy = list_rect.bottom - string_rect.bottom;
        } else {
            dy = 0;
        }
    }

    if( dx || dy ) {
        ScrollWindow( list, dx, dy, NULL, NULL );
        if( dx ) {
            pos = GetScrollPos( list, SB_HORZ );
            SetScrollPos( list, SB_HORZ, pos - dx, TRUE );
        }
        if( dy ) {
            pos = GetScrollPos( list, SB_VERT );
            SetScrollPos( list, SB_VERT, pos - dy, TRUE );
        }
    }
    SendMessage( string, REG_STRING_SELECTED, 0, 0 );
}

static void UpdateRegList(HWND list, RegListData *list_data)
{
    int                 i;
    int                 j;
    int                 num_columns;
    int                 height;
    int                 x;
    int                 y;
    int                 left;
    int                 id;
    int                 num_regs;
    RegStringCreateData *reg_create;
    int                 space;
    int                 old_maxx;
    int                 new_maxx;
    int                 new_maxy;
    int                 min;
    int                 extra;
    HWND                string;
    HWND                d_string;
    mad_reg_set_data    *reg_set;

    reg_set = list_data->reg_set;

    string = GetWindow( list, GW_CHILD );
    while(string != NULL ) {
        SetRegStringDestroyFlag( string );
        string = GetWindow( string, GW_HWNDNEXT );
    }


    left =  GetScrollPos( list, SB_HORZ );
    y =  GetScrollPos( list, SB_VERT );
    GetScrollRange( list, SB_HORZ, &min, &old_maxx );
    SetupRegList( list, reg_set, &reg_create, &num_regs, &num_columns, &space, &height, &extra );
    GetScrollRange( list, SB_HORZ, &min, &new_maxx );
    GetScrollRange( list, SB_VERT, &min, &new_maxy );
    left -= old_maxx - new_maxx;
    y = min( new_maxy, y );
    SetScrollPos( list, SB_HORZ, left, TRUE );
    SetScrollPos( list, SB_VERT, y, TRUE );
    left *= -1;
    y   *= -1;
    left += space;
    x = left;
    j = id = 0;
    for( i = 0; i < num_regs; i++ ) {
        if( extra < j ){
            x++;
        }
        if( IsEmptyString( reg_create[i].buffer ) == FALSE ) {
            string = GetDlgItem( list, id );
            if( string == NULL ) {
                CreateRegString( list, x, y, reg_create[j].length, height, reg_create[i].buffer, id );
            } else {
                UpdateRegString( string, list, x, y, reg_create[j].length, height, reg_create[i].buffer );
            }

        }
        x += reg_create[j].length + space;
        id++;
        j++;
        if( j == num_columns ) {
             y += height;
             x = left;
             j = 0;
        }
    }

    string = GetWindow( list, GW_CHILD );
    while( string ) {
        if( GetRegStringDestroyFlag( string ) == TRUE ) {
            if( list_data->curr_reg == string ) {
                list_data->curr_reg = NULL;
            }
            d_string = string;
            string = GetWindow( string, GW_HWNDNEXT );
            DestroyWindow( d_string );
        } else {
            string = GetWindow( string, GW_HWNDNEXT );
        }
    }

    if( list_data->curr_reg == NULL ) {
        list_data->curr_reg = GetWindow( list, GW_CHILD );
        if( list == GetFocus() ) {
            MakeStringCurrent( list, list_data->curr_reg, list_data );
        }
    }

    FreeRegStringCreate( reg_create, num_regs );
}

static void ShowRegListMenu(HWND list, int x, int y, RegListData *data )
{
    TrackPopupMenu( data->menu,
        TPM_LEFTBUTTON,// | TPM_RIGHTBUTTON ,
        x,
        y,
        0,
        list,
        NULL );
}


static LRESULT ProcessKeyEvent(HWND hwnd,UINT msg, WPARAM wparam,LPARAM lparam)
{
    HWND        string;
    RECT        rectold;
    RECT        rectnew;
    UINT        direction;
    RegListData *data;
    HWND        parent;
    WORD        repeat;

    repeat = LOWORD( lparam );
    data = (RegListData *)GetWindowLong( hwnd, 0 );
    direction = GW_HWNDNEXT;
    string = data->curr_reg;

    switch (wparam){
    case VK_TAB:
        parent = GetParent( hwnd );
        SetFocus( GetNextDlgTabItem( parent, hwnd, ( GetKeyState( VK_SHIFT) < 0 ) ? TRUE : FALSE ) );
        return( 0L );

    case VK_RETURN:
        SendMessage( string, WM_LBUTTONDBLCLK, 0, 0 );
        return ( 0L );
    case VK_LEFT:
        direction = GW_HWNDPREV;
        /* fall through */
    case VK_RIGHT:
        while( repeat > 0 ){
            string = GetWindow( string, direction );
            repeat--;
        }
        break;
    case VK_UP:
        direction = GW_HWNDPREV;
        /* fall through */
    case VK_DOWN:
        GetWindowRect( string, &rectold );
        while( repeat > 0 ) {
            while( string ) {
                string = GetWindow( string, direction );
                if( string == NULL ) {
                    return( 0L );
                }
                GetWindowRect( string, &rectnew );
                if ( rectold.left == rectnew.left ){
                    break;
                }
            }
        repeat--;
        }
        break;
    default:
        return( DefWindowProc( hwnd, msg, wparam, lparam) );
    }
    if ( string ){
        MakeStringCurrent( hwnd, string, data );
    }
    return ( 0L );
}

LRESULT CALLBACK RegListProc(HWND hwnd, UINT msg,WPARAM wparam, LPARAM lparam)
{
    RegListData *data;
    RECT        rect;
    WORD        item;

    data = (RegListData *)GetWindowLong( hwnd, 0 );
    switch( msg ){
    case CHILD_R_CLICK:
        SetFocus( hwnd );
        MakeStringCurrent( hwnd, (HWND)wparam, data );
        EnableMenuItem( data->menu, 1, MF_BYCOMMAND | MF_ENABLED );
        GetWindowRect( data->curr_reg, &rect );
        ShowRegListMenu( hwnd, rect.left + LOWORD( lparam ), rect.top + HIWORD( lparam ), data );
        break;
    case WM_RBUTTONDOWN:
        EnableMenuItem( data->menu, 1, MF_BYCOMMAND | MF_GRAYED );
        GetWindowRect( hwnd, &rect );
        ShowRegListMenu( hwnd, rect.left + LOWORD( lparam ), rect.top + HIWORD( lparam ), data );
        break;
    case WM_SETFOCUS:
        SendMessage( data->curr_reg, REG_STRING_SELECTED, 0, 0 );
        break;
    case WM_KILLFOCUS:
        SendMessage( data->curr_reg, REG_STRING_DESELECTED, 0, 0 );
        break;
    case WM_KEYDOWN:
        ProcessKeyEvent( hwnd, msg, wparam, lparam );
        break;
    case WM_GETDLGCODE:
        return( DLGC_WANTALLKEYS );
    case CHILD_L_CLICK:
        SetFocus( hwnd );
        MakeStringCurrent( hwnd, (HWND)wparam, data );
        break;
    case HIDE_REG_LIST:
        ShowWindow( hwnd, SW_HIDE );
        break;
    case UNHIDE_REG_LIST:
        ShowWindow( hwnd, SW_SHOW );
        break;
    case UPDATE_REG_LIST:
        UpdateRegList( hwnd, data );
        break;
    case WM_HSCROLL:
    case WM_VSCROLL:
        ScrollRegList( hwnd, msg, wparam );
        break;
    case WM_DESTROY:
        DestroyMenu( data->menu );
        MemFree( data );
        break;
    case WM_COMMAND:
        if ( HIWORD( wparam ) == 0 ){
            item = LOWORD( wparam );
            switch( item  ){
            case 1:
                SendMessage( data->curr_reg, WM_LBUTTONDBLCLK, 0, 0 );
                break;
            default:
                if( CheckMenuItem( data->menu, item, MF_BYCOMMAND ) == MF_CHECKED ){
                    CheckMenuItem( data->menu, item, MF_UNCHECKED | MF_BYCOMMAND );
                } else {
                    CheckMenuItem( data->menu, item, MF_CHECKED | MF_BYCOMMAND );
                }
                item -= MAD_MENU_FIRST_ITEM;
                item = 1 << item;
                MADRegSetDisplayToggle( data->reg_set, item, item );
                UpdateRegList( hwnd, data );
            }
            break;
        }
        /* fall through */
        default:
            return DefWindowProc( hwnd, msg, wparam, lparam );
    }
    return ( 0L );
}


void InitRegList(void)
{
    WNDCLASS            wc;
    wc.style = CS_BYTEALIGNWINDOW | CS_BYTEALIGNCLIENT;
    wc.lpfnWndProc = RegListProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 4;
    wc.hInstance = Instance;
    wc.hIcon = NULL;
    wc.hCursor = LoadCursor( NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)( COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "REG_LIST";
    RegisterClass( &wc );
    InitRegString();
}
