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
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "watcom.h"
#include "wglbl.h"
#include "wmsg.h"
#include "winst.h"
#include "wclip.h"
#include "rcstr.gh"
#include "wrdll.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/

bool WGetClipData( HWND main, UINT fmt, void *_data, uint_32 *dsize )
{
    void        **data = _data;
    HANDLE      hclipdata;
    void        *mem;
    bool        clipbd_open;
    bool        ok;

    hclipdata = (HANDLE)NULL;
    clipbd_open = FALSE;
    mem = NULL;
    ok = (fmt != 0 && data != NULL && dsize != NULL);

    if( ok ) {
        ok = OpenClipboard( main ) != 0;
    }

    if( ok ) {
        clipbd_open = TRUE;
        hclipdata = GetClipboardData( fmt );
        ok = (hclipdata != NULL);
    }

    if( ok ) {
        mem = GlobalLock( hclipdata );
        ok = (mem != NULL);
    }

    if( ok ) {
        *dsize = (uint_32)GlobalSize( hclipdata );
        ok = (*dsize != 0);
    }

    if( ok ) {
        if( *dsize >= INT_MAX ) {
            WDisplayErrorMsg( W_RESTOOBIGTOPASTE );
            ok = false;
        }
    }

    if( ok ) {
        *data = WRMemAlloc( *dsize );
        ok = (*data != NULL);
    }

    if( ok ) {
        memcpy( *data, mem, *dsize );
    }

    if( !ok ) {
        if( *data != NULL ) {
            WRMemFree( *data );
            *data = NULL;
            *dsize = 0;
        }
    }

    if( mem != NULL ) {
        GlobalUnlock( hclipdata );
    }

    if( clipbd_open ) {
        CloseClipboard();
    }

    return( ok );
}

bool WCopyClipData( HWND main, UINT fmt, void *data, uint_32 dsize )
{
    HBITMAP     hdsp_bitmap;
    HGLOBAL     hmem;
    BYTE        *mem;
    HINSTANCE   inst;
    bool        clipbd_open;
    bool        ok;

    clipbd_open = FALSE;
    hdsp_bitmap = (HBITMAP)NULL;
    hmem = (HGLOBAL)NULL;
    mem = NULL;

    ok = (fmt != 0 && data != NULL && dsize != 0);

    if( ok ) {
        inst = WGetEditInstance();
        hdsp_bitmap = LoadBitmap( inst, "PrivateFmt" );
        ok = (hdsp_bitmap != (HBITMAP)NULL);
    }

    if( ok ) {
        ok = OpenClipboard( main ) != 0;
    }

    if( ok ) {
        clipbd_open = TRUE;
        hmem = GlobalAlloc( GMEM_MOVEABLE, dsize );
        ok = (hmem != (HGLOBAL)NULL);
    }

    if( ok ) {
        mem = GlobalLock( hmem );
        ok = (mem != NULL);
    }

    if( ok ) {
        memcpy( mem, data, dsize );
        GlobalUnlock( hmem );
        mem = NULL;
        EmptyClipboard();
        SetClipboardData( fmt, hmem );
        SetClipboardData( CF_DSPBITMAP, hdsp_bitmap );
        hmem = (HGLOBAL)NULL;
        hdsp_bitmap = (HBITMAP)NULL;
    }

    if( clipbd_open ) {
        CloseClipboard();
    }

    if( !ok ) {
        if( hdsp_bitmap != (HBITMAP)NULL ) {
            DeleteObject( hdsp_bitmap );
        }
        if( mem != NULL ) {
            GlobalUnlock( hmem );
        }
        if( hmem != (HGLOBAL)NULL ) {
            GlobalFree( hmem );
        }
    }

    return( ok );
}
