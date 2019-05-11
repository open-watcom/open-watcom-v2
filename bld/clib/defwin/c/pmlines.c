/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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


#include "variety.h"
#include <stdio.h>
#define INCL_WIN
#include <wos2.h>
#include "win.h"

/*
 * _SaveAllLines - save all lines to a file
 */
void _SaveAllLines( LPWDATA w )
{
    char                fname[CCHMAXPATH + 1];
    FILEDLG             fdlg;
    HWND                hwmenu;
    FILE                *f;
    LPLDATA             ld;

    /*
     * go get file name
     */
    fname[0] = 0;
    memset( &fdlg, 0, sizeof( FILEDLG ) );
    fdlg.cbSize = sizeof( FILEDLG );
    fdlg.fl = FDS_SAVEAS_DIALOG | FDS_CENTER | FDS_PRELOAD_VOLINFO | FDS_ENABLEFILELB;
    fdlg.pszTitle = "Save File Name Selection";
    fdlg.pszOKButton = "~Save";
    strcpy( fdlg.szFullFile, "*.*" );

    hwmenu = WinWindowFromID( _MainFrameWindow, FID_MENU );
    if( WinFileDlg( HWND_DESKTOP, hwmenu, &fdlg ) ) {
        /*
         * save lines
         */
        if( fdlg.lReturn != DID_OK ) {
            return;
        }
        f = fopen( fdlg.szFullFile, "w" );
        if( f == NULL ) {
            _Error( hwmenu, fdlg.szFullFile, "Error opening file" );
            return;
        }
        for( ld = w->LineHead; ld != NULL; ld = ld->next ) {
            fprintf( f,"%s\n", ld->data );
        }
        fclose( f );
        _Error( hwmenu, fdlg.szFullFile, "Data saved to file" );
    } else {
        _Error( hwmenu, "Error", "Could not create dialog box" );
    }

} /* _SaveAllLines */

#define MAX_BYTES       0xfffeL
/*
 * _CopyAllLines - copy lines to clipboard
 */
void _CopyAllLines( LPWDATA w )
{
    LPLDATA     ld;
    ULONG       total;
    unsigned    len;
    char        *data;
    char        *ptr;
    unsigned    slen;
    LONG        rc;

    /*
     * get number of bytes
     */
    total = 0;
    for( ld = w->LineHead; ld != NULL; ld = ld->next ) {
        total += strlen( ld->data ) + 2;
    }
    if( total > MAX_BYTES ) {
        len = (unsigned) MAX_BYTES;
    } else {
        len = total;
    }

    /*
     * get memory block
     */
    rc = PAG_COMMIT | OBJ_GIVEABLE | PAG_WRITE;
    rc = DosAllocSharedMem( (PPVOID)&data, NULL, len + 1, rc );
    if( rc ) {
        _Error( NULLHANDLE, "Copy to Clipboard Error", "Out of Memory" );
        return;
    }

    /*
     * copy data into block
     */
    total = 0;
    ptr = data;
    for( ld = w->LineHead; ld != NULL; ld = ld->next ) {
        slen = strlen( ld->data ) + 2;
        if( total + slen > MAX_BYTES )
            break;
        memcpy( &ptr[total], ld->data, slen - 2 );
        ptr[total + slen - 2] = 0x0d;
        ptr[total + slen - 1] = 0x0a;
        total += slen;
    }
    ptr[total] = 0;

    /*
     * dump data to the clipboard
     */
    if( WinOpenClipbrd( _AnchorBlock ) ) {
        WinEmptyClipbrd( _AnchorBlock );
        WinSetClipbrdData( _AnchorBlock, (ULONG)data, CF_TEXT, CFI_POINTER );
        WinCloseClipbrd( _AnchorBlock );
    }

} /* _CopyAllLines */

