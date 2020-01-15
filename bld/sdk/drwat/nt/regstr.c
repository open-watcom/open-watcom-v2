/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2016 The Open Watcom Contributors. All Rights Reserved.
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


#include "drwatcom.h"
#include <errno.h>
#include "wclbproc.h"
#include "regstr.h"
#include "mad.h"
#include "regcmbo.rh"
#include "jdlg.h"
#include "bitman.h"
#include "madcli.h"
#include "regedit.rh"
#include "reglist.h"
#include "strcnv.h"


/* Local Window callback functions prototypes */
WINEXPORT INT_PTR CALLBACK ChangeRegisterDialogDlgProc( HWND hwnd, UINT msg,WPARAM  wparam, LPARAM lparam);
WINEXPORT LRESULT CALLBACK RegStringProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );


HWND CreateRegString( HWND parent, int x, int y, int width, int height, char *text, int id )
{
    HWND    ret;

    ret = CreateWindow( "REG_STRING",
        text,
        WS_VISIBLE|WS_CHILD,
        x,
        y,
        width,
        height,
        parent,
        (HMENU)id,
        Instance,
        NULL );
    MoveWindow( ret, x, y, width, height, FALSE );

    SET_WNDINFO( ret, 0 );
    return( ret );
}

void UpdateRegString( HWND string, HWND list, int x, int y, int width, int height, char *text )
{
    int     len;
    char    *cmp;
    DWORD   flags;

    /* unused parameters */ (void)list;

    len = GetWindowTextLength( string ) + 1;
    cmp = alloca( len );
    GetWindowText( string, cmp, len );
    if( strcmp( text, cmp ) != 0 ) {
        SetWindowText( string, text );
        InvalidateRect( string, NULL, FALSE );
    }
    MoveWindow( string, x, y, width, height, TRUE );

    flags = GET_WNDINFO( string );
    flags &= ~REG_DESTROY;
    SET_WNDINFO( string, flags );
}

void SetRegStringDestroyFlag(HWND hwnd)
{
    DWORD   flags;

    flags = GET_WNDINFO( hwnd );
    flags |= REG_DESTROY;
    SET_WNDINFO( hwnd, flags );
}

bool GetRegStringDestroyFlag(HWND hwnd)
{
    DWORD   flags;

    flags = GET_WNDINFO( hwnd );
    return( (flags & REG_DESTROY) != 0 );
}

static void RegStrPaint(HWND hwnd)
{
    PAINTSTRUCT ps;
    HDC         hdc;
    RECT        r;
    char        buff[255];
    int         len;
    SIZE        size;
    int         top;
    COLORREF    fground;
    COLORREF    bground;
    DWORD       flags;

    flags = GET_WNDINFO( hwnd );
    hdc = BeginPaint( hwnd, &ps );

    SelectObject( hdc, GetMonoFont() );
    GetClientRect( hwnd, &r );
    GetWindowText( hwnd, buff, sizeof( buff ) );
    len = strlen( buff );
    GetTextExtentPoint32( hdc, buff, len, &size );
    if( flags & REG_SELECTED ){
        fground = GetSysColor( COLOR_HIGHLIGHTTEXT );
        bground = GetSysColor( COLOR_HIGHLIGHT );
    } else {
        fground = GetSysColor( COLOR_WINDOWTEXT );
        bground = GetSysColor( COLOR_WINDOW );
    }

    SetTextColor( hdc, fground );
    SetBkColor( hdc, bground );
    top = ( r.bottom - r.top - size.cy ) / 2;
    ExtTextOut( hdc, 0, top, ETO_OPAQUE, &r, buff, len, NULL );
    EndPaint( hwnd, &ps );
}

static bool SetRegSelectFlag( HWND hwnd, bool setting )
{
    DWORD   flags;

    flags = GET_WNDINFO( hwnd );
    if( setting && (flags & REG_SELECTED) == 0 ) {
        flags |= REG_SELECTED;
        InvalidateRect( hwnd, NULL, FALSE );
        SET_WNDINFO( hwnd, flags );
        return( true );
    }
    if( !setting && (flags & REG_SELECTED) ) {
        flags &= ~REG_SELECTED;
        InvalidateRect( hwnd, NULL, FALSE );
        SET_WNDINFO( hwnd, flags );
        return( true );
    }
    return( false );
}

void GetChildPos( HWND parent, HWND child, RECT *c_rect )
{
    DWORD           style;
    DWORD           ex_style;
    RECT            p_rect;

    GetWindowRect( parent, &p_rect );
    GetClientRect( parent, c_rect );
    style = GET_WNDSTYLE( parent );
    ex_style = GET_WNDEXSTYLE( parent );
    AdjustWindowRectEx( c_rect, style, FALSE, ex_style );
    p_rect.left -= c_rect->left;
    p_rect.top -= c_rect->top;
    GetWindowRect( child, c_rect );
    c_rect->right -= c_rect->left;
    c_rect->left -= p_rect.left;
    c_rect->bottom -= c_rect->top;
    c_rect->top -= p_rect.top;
}

static void InitChangeRegisterDialog(HWND hwnd,LPARAM lparam)
{
    RegModifyData   *data;
    char            *name;
    unsigned        len;
    mad_radix       radix;
    mad_type_info   mti;
    mad_type_info   cmp_mti;
    char            s[255];
    RECT            p_rect;
    RECT            c_rect;
    HWND            field;
    int             i;
    unsigned        max_len;
    HDC             dc;
    TEXTMETRIC      tm;
    HWND            cancel;

    SET_DLGDATA( hwnd, lparam );
    data = (RegModifyData *)lparam;
    len = MADRegFullName( data->curr_info, ".", NULL, 0 );
    if( len > 0 ) {
        name = alloca( ( len + 1 ) * sizeof( char ) );
        MADRegFullName( data->curr_info, ".", name, len );
        SetWindowText( hwnd, name );
    } else {
        SetWindowText( hwnd, "" );
    }

    if( data->num_possible == 1 ) {
        field = GetDlgItem( hwnd, REG_EDIT_FIELD );
    } else {
        field = GetDlgItem( hwnd, CH_REG_COMBO_LIST );
    }
    SetMonoFont( field );
    GetChildPos( hwnd, field, &c_rect);
    dc = GetDC( field );
    GetTextMetrics( dc, &tm );
    MADTypeInfo( data->mth, &mti );
    radix = MADTypePreferredRadix( data->mth );

    if( data->num_possible == 1 ) {
        if( data->maxv == 0 ) {
            len = 255;
        } else {
            len = data->maxv;
        }
        MADTypeToString( radix, &mti, data->curr_value, s, &len );
        if( data->maxv == 0 ) {
            max_len = strlen( s );
        } else {
            max_len = data->maxv;
        }
        SetDlgItemText(hwnd,REG_EDIT_FIELD,s);
    } else {
        MADTypeInfo( data->curr_info->mth, &cmp_mti );
        max_len = 0;
        for( i = 0; i < data->num_possible; i++ ) {
            if( data->m_list[i].name == MAD_MSTR_NIL ) {
                len = sizeof( s );
                MADTypeToString( radix, &mti, data->m_list[i].data, s, &len );
            } else {
                MADCli( String )( data->m_list[i].name, s, sizeof( s ) );
            }
            if( max_len < strlen( s ) )
                max_len = strlen( s );
            SendDlgItemMessage( hwnd, CH_REG_COMBO_LIST, CB_ADDSTRING, 0, (LPARAM)(LPCSTR)s );
            if( memcmp( data->curr_value, data->m_list[i].data, BITS2BYTES( cmp_mti.b.bits ) ) == 0 ) {
                SendDlgItemMessage( hwnd, CH_REG_COMBO_LIST, CB_SETCURSEL, (WPARAM)i, 0 );
            }
        }
        c_rect.bottom += SendMessage( field, CB_GETITEMHEIGHT, 0, 0 ) * (i + 1);
    }
    max_len *= tm.tmMaxCharWidth;
    if( max_len > c_rect.right ) {
        max_len -= max_len%2;
        GetWindowRect( hwnd, &p_rect );
        p_rect.right -= p_rect.left;
        p_rect.bottom -= p_rect.top;
        p_rect.left -= ( max_len - c_rect.right )/2;
        p_rect.right += ( max_len - c_rect.right );
        MoveWindow( hwnd, p_rect.left, p_rect.top, p_rect.right, p_rect.bottom, FALSE );
        cancel = GetDlgItem( hwnd, IDCANCEL );
        GetChildPos( hwnd, cancel, &p_rect );
        p_rect.left += ( max_len - c_rect.right );
        MoveWindow( cancel, p_rect.left, p_rect.top, p_rect.right, p_rect.bottom, FALSE );
        c_rect.right = max_len;
    }
    ReleaseDC( field, dc );
    MoveWindow( field, c_rect.left, c_rect.top, c_rect.right, c_rect.bottom, FALSE );
}

static void CheckForRegisterChange( HWND hwnd )
{
    RegModifyData   *data;
    mad_type_info   target_mti;
    mad_type_info   host_mti;
    int             size;
    char            *s;
    char            *endptr;
    void            *test;
    addr_seg        seg;
    InputUnion      in;

    data = (RegModifyData *)GET_DLGDATA( hwnd );
    MADTypeInfo( data->curr_info->mth, &target_mti );
    if( data->num_possible == 1 ) {
        size = SendDlgItemMessage( hwnd, REG_EDIT_FIELD, WM_GETTEXTLENGTH, 0, 0 ) + 1 ;
        s = alloca( size );
        GetDlgItemText( hwnd, REG_EDIT_FIELD, s, 255 );
        test = alloca( BITS2BYTES( target_mti.b.bits ) );
        memset( &seg, 0, sizeof( seg ) );
        errno = 0;
        size = 0;
        switch ( target_mti.b.kind ) {
        case MTK_INTEGER:
            if( !StrToU64( s, &( in.i ), ( target_mti.i.nr != MNR_UNSIGNED ) ) ) {
                MessageBox( hwnd, "Unrecognized input.", "Error",MB_OK | MB_ICONEXCLAMATION ) ;
                return;
            }
            size = sizeof( unsigned_64 );
            break;
        case MTK_FLOAT:
            in.d = strtod( s, &endptr );
            if( errno == ERANGE ) {
                MessageBox( hwnd, "Value out of range.", "Error",MB_OK | MB_ICONEXCLAMATION ) ;
                return;
            }
            if( *endptr != '\0' ) {
                MessageBox( hwnd, "Unrecognized input.", "Error",MB_OK | MB_ICONEXCLAMATION ) ;
                return;
            }
            size = sizeof( in.d );
            break;
        default:
            EndDialog( hwnd, 0 );
            break;
        }
        MADTypeInfoForHost( target_mti.b.kind, size, &host_mti );
        MADTypeConvert( &host_mti, &in, &target_mti, test, seg );
        if( memcmp( data->curr_value, test, BITS2BYTES( target_mti.b.bits ) ) == 0 ) {
            EndDialog( hwnd, 0 );
        } else {
            memcpy( data->curr_value, test, BITS2BYTES( target_mti.b.bits ) );
            EndDialog( hwnd, 1 );
        }
    } else {
        int i = (int)SendDlgItemMessage( hwnd, CH_REG_COMBO_LIST, CB_GETCURSEL, 0, 0L );
        if( memcmp( data->curr_value, data->m_list[i].data, BITS2BYTES( target_mti.b.bits ) ) == 0 ) {
            EndDialog( hwnd, 0 );
        } else {
            memcpy( data->curr_value, data->m_list[i].data, BITS2BYTES( target_mti.b.bits ) );
            EndDialog( hwnd, 1 );
        }
    }
}

INT_PTR CALLBACK ChangeRegisterDialogDlgProc( HWND hwnd, UINT msg,WPARAM  wparam, LPARAM lparam)
{
    WORD        cmd;
    bool        ret;

    ret = false;

    switch( msg ){
    case WM_INITDIALOG:
        InitChangeRegisterDialog( hwnd, lparam );
        ret = true;
        break;

    case WM_CLOSE:
        PostMessage( hwnd, WM_COMMAND, IDCANCEL, 0L );
        ret = true;
        break;

    case WM_COMMAND:
        cmd = LOWORD( wparam );
        switch( cmd ) {
        case IDCANCEL:
            EndDialog( hwnd, 0 );
            ret = true;
            break;
        case IDOK:
            CheckForRegisterChange( hwnd );
            ret = true;
            break;
        }
   }
   return( ret );
}

static void GetNewRegValue( HWND hwnd )
{
    HWND            owner;
    DLGPROC         dlgproc;
    INT_PTR         reg_modified;
    RegModifyData   modify_data;
    const char      *descript;
    unsigned        max_descript;
    mad_type_info   mti;
    mad_registers   *regs;

    owner = GetParent( hwnd );
    regs =  RegListGetMadRegisters( owner );
    modify_data.reg_set = RegListGetMadRegSetData( owner );
    owner = GetParent( owner );

    MADRegSetDisplayGetPiece( modify_data.reg_set, regs, GetDlgCtrlID( hwnd ),
        &descript, &max_descript, (const mad_reg_info **) (&( modify_data.curr_info )),
        &( modify_data.mth ), &( modify_data.maxv ) );
    MADTypeInfo( modify_data.curr_info->mth, &mti );
    modify_data.curr_value = alloca( BITS2BYTES( mti.b.bits ) );
    BitGet( modify_data.curr_value, (unsigned char *)regs, modify_data.curr_info->bit_start, modify_data.curr_info->bit_size);
    MADRegSetDisplayModify( modify_data.reg_set, modify_data.curr_info,
        (const mad_modify_list **)( &( modify_data.m_list ) ),
        &(modify_data.num_possible) );

    switch( modify_data.num_possible ) {
    case 2:
        if( memcmp( modify_data.curr_value, modify_data.m_list[0].data, BITS2BYTES( mti.b.bits ) ) == 0 ){
            memcpy( modify_data.curr_value, modify_data.m_list[1].data, BITS2BYTES( mti.b.bits ) );
        }else {
            memcpy( modify_data.curr_value, modify_data.m_list[0].data, BITS2BYTES( mti.b.bits ) );
        }
        reg_modified = 1;
        break;
    case 1:
        dlgproc = MakeProcInstance_DLG( ChangeRegisterDialogDlgProc, Instance );
        reg_modified = JDialogBoxParam( Instance, "CHANGE_REG_EDIT", owner, dlgproc, (LPARAM)( &modify_data ) );
        FreeProcInstance_DLG( dlgproc );
        break;
    default:
        dlgproc = MakeProcInstance_DLG( ChangeRegisterDialogDlgProc, Instance );
        reg_modified = JDialogBoxParam( Instance, "CHANGE_REG_COMBO", owner, dlgproc, (LPARAM)( &modify_data ) );
        FreeProcInstance_DLG( dlgproc );
    }
    if( reg_modified == 1 ) {
        MADRegUpdateStart( regs, modify_data.curr_info->flags, modify_data.curr_info->bit_start, modify_data.curr_info->bit_size );
        BitPut( (unsigned char *)regs, modify_data.curr_info->bit_start, modify_data.curr_value, modify_data.curr_info->bit_size );
        MADRegUpdateEnd( regs, modify_data.curr_info->flags, modify_data.curr_info->bit_start, modify_data.curr_info->bit_size );
    }
}

LRESULT CALLBACK RegStringProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    HWND list;

    list = GetParent( hwnd );
    switch (msg){
    case WM_PAINT:
        RegStrPaint( hwnd );
        break;
    case WM_RBUTTONDOWN:
        SendMessage( list, CHILD_R_CLICK, (WPARAM) hwnd, lparam );
        break;
    case WM_LBUTTONDOWN:
        SendMessage( list, CHILD_L_CLICK, (WPARAM) hwnd, lparam );
        break;
    case REG_STRING_DESELECTED:
        SetRegSelectFlag( hwnd, false );
        break;
    case REG_STRING_SELECTED:
        SetRegSelectFlag( hwnd, true );
        break;
    case WM_LBUTTONDBLCLK:
        GetNewRegValue( hwnd );
        break;
    default:
        return DefWindowProc( hwnd, msg, wparam, lparam );
    }
    return ( 0L );
}

void InitRegString(void)
{
    WNDCLASS            wc;

    wc.style = CS_DBLCLKS|CS_BYTEALIGNWINDOW|CS_BYTEALIGNCLIENT|CS_PARENTDC;
    wc.lpfnWndProc = RegStringProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 4;
    wc.hInstance = Instance;
    wc.hIcon = NULL;
    wc.hCursor = LoadCursor( NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "REG_STRING";
    RegisterClass( &wc );
}
