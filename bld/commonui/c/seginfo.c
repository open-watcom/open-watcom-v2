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
* Description:  Display information about a memory segment.
*
****************************************************************************/


#include "precomp.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include "types.h"
#include "seginfo.h"
#include "segmem.h"
#include "mythelp.h"
#include "dlgmod.h"
#include "font.h"
#include "wpi.h"
#include "ctl3d.h"

void SetFont( HWND, int );

static ADDRESS  siAddr;
static int      isDPMI;
static DWORD    memLimit;
#define BYTES_LINE      8
#define MAXRANGE        8192

/*
 * DisplayMemLines - display all assembler lines
 */
void DisplayMemLines( HWND hwnd, ADDRESS *addr, int idlo, int idhi, int sbid )
{
    int         i,j;
    char        bytes[BYTES_LINE];
    char        data[BYTES_LINE + 1];
    char        buff[256];
    char        tbuff[10];
    DWORD       max;
    DWORD       curr;
    ADDRESS     maddr;
    HWND        hscrl;

    maddr = *addr;
    for( i = idlo; i <= idhi; i++ ) {
        ReadMem( maddr.seg, maddr.offset, bytes, BYTES_LINE );
        sprintf( buff, "%08lx: ", maddr.offset );
        for( j = 0; j < BYTES_LINE; j++ ) {
            data[j] = '.';
            if( maddr.offset + j < memLimit ) {
                sprintf( tbuff, "%02x ", (unsigned)bytes[j] );
                if( isprint( bytes[j] ) ) {
                    data[j] = bytes[j];
                }
            } else {
                sprintf( tbuff, "?? " );
            }
            strcat( buff, tbuff );
        }
        data[BYTES_LINE] = 0;
        strcat( buff, data );
        SetDlgItemText( hwnd, i, buff );
        maddr.offset += BYTES_LINE;
    }

    max = memLimit;
    if( max > MAXRANGE ) {
        curr = (DWORD)((double)MAXRANGE * (double)addr->offset / (double)max);
        max = MAXRANGE;
    } else {
        curr = addr->offset;
    }

    hscrl = GetDlgItem( hwnd, sbid );
    SetScrollRange( hscrl, SB_CTL, 0, max, FALSE);
    SetScrollPos( hscrl, SB_CTL, curr, TRUE );

} /* DisplayMemLines */

/*
 * scrollMemDisplay - move asm display in response to a scroll request
 */
static void scrollMemDisplay( HWND hwnd, WORD wparam, WORD pos, ADDRESS *addr,
                              int idlo, int idhi, int sbid )
{
    switch( wparam ) {
    case SB_PAGEDOWN:
        addr->offset += 7 * BYTES_LINE;
        break;
    case SB_PAGEUP:
        addr->offset -= 7 * BYTES_LINE;
        break;
    case SB_LINEDOWN:
        addr->offset += BYTES_LINE;
        break;
    case SB_LINEUP:
        addr->offset -= BYTES_LINE;
        break;
//  case SB_THUMBPOSITION:
    case SB_THUMBTRACK:
        if( memLimit > MAXRANGE ) {
            addr->offset = (DWORD)(((double)pos * (double)memLimit) / (double)MAXRANGE);
        } else {
            addr->offset = pos;
        }
        break;
    default:
        return;
    }
    if( (long)addr->offset < 0 ) {
        addr->offset = 0;
    }
    if( addr->offset >= memLimit ) {
        addr->offset = memLimit - 1;
    }
    DisplayMemLines( hwnd, addr, idlo, idhi, sbid );

} /* ScrollMemDisplay */


/*
 *  SegInfoDialog - display info about a segment
 */
BOOL __export FAR PASCAL SegInfoDialog( HWND hwnd, WORD msg, WPARAM wparam, LPARAM lparam )
{
    GLOBALENTRY ge;
    char        buff[64];
    int         i;

    lparam = lparam;

    switch( msg ) {
    case WM_INITDIALOG:
        if( !isDPMI ) {
            MyGlobalEntryHandle( &ge, (HINSTANCE)siAddr.seg );
            SetWORDStaticField( hwnd, SEGINFO_HANDLE, (WORD)ge.hBlock );
            SetDWORDStaticField( hwnd, SEGINFO_ADDR, ge.dwAddress );
            SetDWORDStaticField( hwnd, SEGINFO_BLOCKSIZE, ge.dwBlockSize );
            SetWORDStaticField( hwnd, SEGINFO_LOCKCOUNT, ge.wcLock );
            SetWORDStaticField( hwnd, SEGINFO_PAGELOCKCOUNT, ge.wcPageLock );
            SetFont( hwnd, SEGINFO_TYPE );
            switch( ge.wType ) {
            case GT_CODE:
                SetDlgItemText( hwnd, SEGINFO_TYPE, "CODE" );
                break;
            case GT_DATA:
                SetDlgItemText( hwnd, SEGINFO_TYPE, "DATA" );
                break;
            case GT_DGROUP:
                SetDlgItemText( hwnd, SEGINFO_TYPE, "DGROUP" );
                break;
            }
        }
        sprintf( buff, "Selector %04x", (WORD)siAddr.seg );
        SetWindowText( hwnd, buff );
        for( i = SEGINFO_LINE1; i <= SEGINFO_LINE7; i++ ) {
            SetFont( hwnd, i );
        }
        DisplayMemLines( hwnd, &siAddr, SEGINFO_LINE1, SEGINFO_LINE7, 0 );
        return( TRUE );
#ifndef NOUSE3D
    case WM_SYSCOLORCHANGE:
        Ctl3dColorChange();
        break;
#endif

    case WM_VSCROLL:
        scrollMemDisplay( hwnd, wparam, LOWORD( lparam ), &siAddr,
                          SEGINFO_LINE1, SEGINFO_LINE7, SEGINFO_SCROLL );
        return( TRUE );

    case WM_CLOSE:
        PostMessage( hwnd, WM_COMMAND, SEGINFO_OK, 0L );
        return( TRUE );

    case WM_COMMAND:
        if( wparam == SEGINFO_OK ) {
            EndDialog( hwnd, 0 );
            return( TRUE );
        }
    }
    return( FALSE );

} /* SegInfoDialog */

/*
 * DoSegInfo - display the seg info dialog
 */
void DoSegInfo( HWND hwnd, WORD seg, BOOL isdpmi, HANDLE inst )
{
    FARPROC     fp;

    siAddr.offset = 0;
    siAddr.seg = seg;
    isDPMI = isdpmi;
    memLimit = GetASelectorLimit( seg );
    fp = MakeProcInstance( (FARPROC)SegInfoDialog, inst );
    DialogBox( inst, "SEGMENTINFO", hwnd, (DLGPROC)fp );
    FreeProcInstance( fp );

} /* DoSegInfo */
