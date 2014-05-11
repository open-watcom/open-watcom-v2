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


#include "precomp.h"
#include <ctype.h>
#include <string.h>

#include "watcom.h"
#include "wglbl.h"
#include "wvk2str.h"
#include "wedit.h"
#include "wsetedit.h"
#include "wmain.h"
#include "wmsg.h"
#include "winst.h"
#include "wnewitem.h"
#include "wkey.h"
#include "sys_rc.h"
#include "wresall.h"
#include "ldstr.h"
#include "rcstr.gh"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define ALTPRESSED_MASK   0x20000000
#define EXTENDED_MASK     0x01000000

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef struct {
    BYTE    scan_code;
    char    key;
} scan_table_entry;

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static void WSetKey( WAccelEditInfo *, BYTE );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static scan_table_entry ScanTable[] = {
    { 0x33, ','     },
    { 0x34, '.'     },
    { 0x35, '/'     },
    { 0x27, ';'     },
    { 0x28, '\''    },
    { 0x1a, '['     },
    { 0x1b, ']'     },
    { 0x2b, '\\'    },
    { 0x29, '`'     },
    { 0x0c, '-'     },
    { 0x0d, '='     },
    { 0x00, 0x0     }
};

static char WMapScanCodeToKey( BYTE scan_code )
{
    int i;

    for( i = 0; ScanTable[i].scan_code != 0x00; i++ ) {
        if( ScanTable[i].scan_code == scan_code ) {
            return( ScanTable[i].key );
        }
    }

    return( 0 );
}


Bool WHandleGetKeyValue( WAccelEditInfo *einfo, Bool ignore_first )
{
    RECT        r;
    Bool        ok;
    char        *text;

    text = NULL;

    ok = (einfo != NULL);

    if( ok ) {
        if( einfo->current_entry == NULL ) {
            ok = WInsertAccelEntry( einfo );
        }
    }

    if( ok ) {
        text = AllocRCString( W_SELECTKEY );
        ok = (text != NULL);
    }

    if( ok ) {
        WGetKeyPressProc( NULL, 0, 0, 0 );
        einfo->key_info.key = 0;
        GetWindowRect( GetDlgItem( einfo->edit_dlg, IDM_ACCEDLIST ), &r );
        MapWindowPoints( (HWND)NULL, einfo->edit_dlg, (POINT *)&r, 2 );
        einfo->key_info.text_win = CreateWindow( "static", text,
            WS_CHILD | WS_VISIBLE | SS_LEFT, r.left, r.top,
            r.right - r.left, r.bottom - r.top, einfo->edit_dlg, (HMENU)NULL,
            WGetEditInstance(), NULL );
        ok = (einfo->key_info.text_win != (HWND)NULL);
    }

    if( ok ) {
        einfo->info->modified = TRUE;
        SetFocus( einfo->win );
        SetCapture( einfo->win );
        einfo->key_info.ignore_first_key = ignore_first;
        einfo->getting_key = TRUE;
    }

    if( text != NULL ) {
        FreeRCString( text );
    }

    return( ok );
}

void WSetKey( WAccelEditInfo *einfo, BYTE scan_code )
{
    Bool        cntl;
    Bool        shift;
    Bool        alt;
    char        *str;
    char        scan_key;
    uint_16     key;
    uint_16     skey;
    Bool        is_virt;
    BYTE        kbstate[256];
    int         ta_ret;
#ifdef __NT__
    WORD        ta_key;
#else
    DWORD       ta_key;
#endif

    if( einfo == NULL ) {
        return;
    }

    is_virt = TRUE;
    key = einfo->key_info.key;
    skey = WMapShiftedKeyToKey( key );
    GetKeyboardState( kbstate );
    cntl = (kbstate[VK_CONTROL] & 0x0080) != 0;
    shift = (kbstate[VK_SHIFT] & 0x0080) != 0;
    alt = (kbstate[VK_MENU] & 0x0080) != 0;

    str = WGetVKeyFromID( key );

    if( str == NULL ) {
        // I am assumming that they key must be alphanumeric
        // as WGetVKeyFromID( key ) would filter out all others
        if( isalpha( key ) ) {
            if( alt ) {
                str = WGetASCIIVKText( key );
            } else {
                if( cntl ) {
                    key = toupper( key ) - '@';
                } else {
                    if( !shift ) {
                        key = tolower( key );
                    }
                }
                str = WGetASCIIKeyText( key );
                is_virt = FALSE;
            }
        } else if( isdigit( key ) ) {
            if( alt || cntl ) {
                str = WGetASCIIVKText( key );
            } else {
                if( shift ) {
                    key = WMapKeyToShiftedKey( key );
                }
                str = WGetASCIIKeyText( key );
                is_virt = FALSE;
            }
        } else if( isdigit( skey ) ) {
            str = WGetASCIIVKText( skey );
        // The remaining keys are the ones where we did not get a
        // virtual key we could translate, an alpha-numeric key,
        // a shifted 0-9 ( ie !@#$%^&*() ).
        // The ToAscii function handles everything except the following
        // keys where cntl is pressed. These keys are `-=[]\;',./
        // So, I throw up my hands in defeat and add a special case
        } else if( cntl && (scan_key = WMapScanCodeToKey( scan_code )) != 0 ) {
            str = WGetASCIIKeyText( scan_key );
        } else {
            ta_ret = ToAscii( key, scan_code, kbstate, &ta_key, 0 );
            if( ta_ret ) {
                if( shift ) {
                    ta_key = WMapShiftedKeyToKey( ta_key );
                }
                str = WGetASCIIKeyText( ta_key );
            }
        }
    }

    WSetEditWithStr( GetDlgItem( einfo->edit_dlg, IDM_ACCEDKEY ), str );
    WSetVirtKey( einfo->edit_dlg, is_virt );
    if( is_virt ) {
        CheckDlgButton( einfo->edit_dlg, IDM_ACCEDALT, alt );
        CheckDlgButton( einfo->edit_dlg, IDM_ACCEDCNTL, cntl );
        CheckDlgButton( einfo->edit_dlg, IDM_ACCEDSHFT, shift );
    }
}

Bool WGetKeyPressProc( WAccelEditInfo *einfo, UINT message, WPARAM wParam, LPARAM lParam )
{
    WORD    w;
    BOOL    ret;
    BYTE    scan_code;

    if( einfo == NULL ) {
        return( FALSE );
    }

    ret = FALSE;

    switch( message ) {
    case WM_SYSKEYUP:
    case WM_KEYUP:
        if( einfo->key_info.ignore_first_key ) {
            einfo->key_info.ignore_first_key = FALSE;
            break;
        }
        w = LOWORD( wParam );
        if( w != VK_MENU && w != VK_CONTROL && w != VK_SHIFT ) {
            einfo->key_info.key = w;
            einfo->key_info.extended = ((lParam & EXTENDED_MASK) != 0);
            scan_code = (uint_16)((uint_32)lParam >> 16) & 0x00ff;
            WSetKey( einfo, scan_code );
            ret = TRUE;
        }
        break;
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_NCLBUTTONDOWN:
    case WM_NCRBUTTONDOWN:
    case WM_NCMBUTTONDOWN:
        /* abort if the user clicks the mouse */
        ret = TRUE;
        break;
    }

    return( ret );
}
