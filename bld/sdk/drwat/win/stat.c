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


#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include "wdebug.h"
#include "drwatcom.h"
#include "segmap.h"
#include "jdlg.h"

static ADDRESS          currAddr;
static ADDRESS          firstAddr;
static interrupt_struct oldIntData;

#ifdef __NT__
static HANDLE           processHdl;
static DWORD            processID;

void SetProcessInfo( HANDLE hdl, DWORD procid ) {
    processHdl = hdl;
    processID = procid;
}
#endif

#define MAXRANGE 8192

/*
 * DisplayAsmLines - display all assembler lines
 *      hwnd     - dialog handle
 *      paddr    - address to start dissassembly
 *      flagaddr - address of instruction to mark
 *      idlo     - id of first text field
 *      idhi     - id of last text field (text field id's must be contiguous)
 *      sbid     - scroll bar id
 */
void DisplayAsmLines( HWND hwnd, ADDRESS *paddr, ADDRESS *flagaddr, int idlo,
                        int idhi, int sbid )
{
    int         i;
    ADDRESS     addr;
    char        buff[256];
    HWND        hscrl;
    BOOL        markit;
    DWORD       max;
    int         curr;

    addr = *paddr;
    for( i=idlo;i<=idhi;i++ ) {
        markit = FALSE;
        if( (addr.seg == flagaddr->seg ) &&
            (addr.offset == flagaddr->offset )) markit = TRUE;
        addr.offset += Disassemble( &addr, buff+1, FALSE );
        if( markit ) {
            buff[0] = '*';
        } else {
            buff[0] = ' ';
        }
        SetDlgItemText( hwnd, i, buff );
    }

#ifdef __NT__
    max = max;
    curr = curr;
    hscrl = GetDlgItem( hwnd, sbid );
    SetScrollRange( hscrl, SB_CTL, 0, 2, FALSE);
    SetScrollPos( hscrl, SB_CTL, 1, TRUE );
#else
    max = GetASelectorLimit( paddr->seg );
    if( max > MAXRANGE ) {
        curr = (MAXRANGE*paddr->offset)/max;
        max = MAXRANGE;
    } else {
        curr = paddr->offset;
    }
    hscrl = GetDlgItem( hwnd, sbid );
    SetScrollRange( hscrl, SB_CTL, 0, max, FALSE);
    SetScrollPos( hscrl, SB_CTL, curr, TRUE );
#endif
} /* DisplayAsmLines */

/*
 * ScrollAsmDisplay - move asm display in response to a scroll request
 */
void ScrollAsmDisplay( HWND hwnd, WORD wparam, ADDRESS *paddr,
                       ADDRESS *flagaddr, int idlo, int idhi, int sbid )
{
    int         i;
    char        buff[256];

    switch( wparam ) {
    case SB_PAGEDOWN:
        for( i=0;i<8;i++ ) {
            paddr->offset += Disassemble( paddr, buff, FALSE );
        }
        break;
    case SB_PAGEUP:
        for( i=0;i<8;i++ ) {
            InstructionBackup( 1, paddr );
        }
        break;
    case SB_LINEDOWN:
        paddr->offset += Disassemble( paddr, buff, FALSE );
        break;
    case SB_LINEUP:
        InstructionBackup( 1, paddr );
        break;
    default:
        return;
    }
    DisplayAsmLines( hwnd, paddr, flagaddr, idlo, idhi, sbid );

} /* ScrollAsmDisplay */

/*
 * InitStatDialog
 */
static void InitStatDialog( HWND hwnd )
{
    int         i;
    char        buff[256];
    syminfo     si;
    ADDRESS     addr;

    SetDWORDEditField( hwnd, STAT_AX, IntData.EAX );
    SetDWORDEditField( hwnd, STAT_BX, IntData.EBX );
    SetDWORDEditField( hwnd, STAT_CX, IntData.ECX );
    SetDWORDEditField( hwnd, STAT_DX, IntData.EDX );
    SetDWORDEditField( hwnd, STAT_SI, IntData.ESI );
    SetDWORDEditField( hwnd, STAT_DI, IntData.EDI );
    SetDWORDEditField( hwnd, STAT_BP, IntData.EBP );

    SetWORDEditField( hwnd, STAT_DS, IntData.DS );
    SetWORDEditField( hwnd, STAT_ES, IntData.ES );
    SetWORDEditField( hwnd, STAT_FS, IntData.FS );
    SetWORDEditField( hwnd, STAT_GS, IntData.GS );

    SetWORDEditField( hwnd, STAT_CS, IntData.CS );
    SetDWORDEditField( hwnd, STAT_IP, IntData.EIP );
    SetWORDEditField( hwnd, STAT_SS, IntData.SS );
    SetDWORDEditField( hwnd, STAT_SP, IntData.ESP );

    CheckDlgButton( hwnd, STAT_FLAG_A, IntData.EFlags & FLAG_A_MASK );
    CheckDlgButton( hwnd, STAT_FLAG_C, IntData.EFlags & FLAG_C_MASK );
    CheckDlgButton( hwnd, STAT_FLAG_D, IntData.EFlags & FLAG_D_MASK );
    CheckDlgButton( hwnd, STAT_FLAG_I, IntData.EFlags & FLAG_I_MASK );
    CheckDlgButton( hwnd, STAT_FLAG_Z, IntData.EFlags & FLAG_Z_MASK );
    CheckDlgButton( hwnd, STAT_FLAG_S, IntData.EFlags & FLAG_S_MASK );
    CheckDlgButton( hwnd, STAT_FLAG_P, IntData.EFlags & FLAG_P_MASK );
    CheckDlgButton( hwnd, STAT_FLAG_O, IntData.EFlags & FLAG_O_MASK );

    CheckDlgButton( hwnd, STAT_SYMBOLS, StatShowSymbols );

    /*
     * fill in source information
     */
    addr.seg = IntData.CS;
    addr.offset = IntData.EIP;
    if( FindWatSymbol( &addr, &si, TRUE ) == FOUND ) {
        RCsprintf( buff, STR_SRC_INFO_FMT, si.linenum, si.filename );
    } else {
        RCsprintf( buff, STR_N_A );
    }
    SetDlgMonoFont( hwnd, STAT_SRC_INFO );
    SetDlgItemText( hwnd, STAT_SRC_INFO, buff );

#ifdef __NT__
    {
        ProcStats       procinfo;
        HWND            button;

        if( GetProcessInfo( processID, &procinfo ) ) {
            RCsprintf( buff, STR_STATUS_4_PROC_X, processID, procinfo.name );
            SetWindowText( hwnd, buff );
        }
        CopyRCString( STR_VIEW_MEM_HT_KEY, buff, sizeof( buff ) );
        SetDlgItemText( hwnd, STAT_SEG_MAP, buff );
        button = GetDlgItem( hwnd, STAT_STACK_TRACE );
        ShowWindow( button, SW_HIDE );
    }
#endif

    /*
     * display code, starting 2 instructions before hand
     */
    currAddr.seg = IntData.CS;
    currAddr.offset = IntData.EIP;
    firstAddr = currAddr;
    InstructionBackup( 2, &currAddr );
    for( i=STAT_DISASM_1;i<=STAT_DISASM_8;i++ ) {
        SetDlgCourierFont( hwnd, i );
    }
    DisplayAsmLines( hwnd, &currAddr, &firstAddr, STAT_DISASM_1,
                        STAT_DISASM_8, STAT_SCROLL );

} /* InitStatDialog */

/*
 * GetStatRegisters - get noodled registers
 */
static void GetStatRegisters( HWND hwnd )
{
    GetDWORDEditField( hwnd, STAT_AX, &IntData.EAX );
    GetDWORDEditField( hwnd, STAT_BX, &IntData.EBX );
    GetDWORDEditField( hwnd, STAT_CX, &IntData.ECX );
    GetDWORDEditField( hwnd, STAT_DX, &IntData.EDX );
    GetDWORDEditField( hwnd, STAT_SI, &IntData.ESI );
    GetDWORDEditField( hwnd, STAT_DI, &IntData.EDI );
    GetDWORDEditField( hwnd, STAT_BP, &IntData.EBP );

    GetWORDEditField( hwnd, STAT_DS, &IntData.DS );
    GetWORDEditField( hwnd, STAT_ES, &IntData.ES );
    GetWORDEditField( hwnd, STAT_FS, &IntData.FS );
    GetWORDEditField( hwnd, STAT_GS, &IntData.GS );

    GetWORDEditField( hwnd, STAT_CS, &IntData.CS );
    GetDWORDEditField( hwnd, STAT_IP, &IntData.EIP );
    GetWORDEditField( hwnd, STAT_SS, &IntData.SS );
    GetDWORDEditField( hwnd, STAT_SP, &IntData.ESP );
    IntData.EFlags = 0;
    if( IsDlgButtonChecked( hwnd, STAT_FLAG_A ) ) {
        IntData.EFlags |= FLAG_A_MASK;
    }
    if( IsDlgButtonChecked( hwnd, STAT_FLAG_C ) ) {
        IntData.EFlags |= FLAG_C_MASK;
    }
    if( IsDlgButtonChecked( hwnd, STAT_FLAG_D ) ) {
        IntData.EFlags |= FLAG_D_MASK;
    }
    if( IsDlgButtonChecked( hwnd, STAT_FLAG_I ) ) {
        IntData.EFlags |= FLAG_I_MASK;
    }
    if( IsDlgButtonChecked( hwnd, STAT_FLAG_O ) ) {
        IntData.EFlags |= FLAG_O_MASK;
    }
    if( IsDlgButtonChecked( hwnd, STAT_FLAG_P ) ) {
        IntData.EFlags |= FLAG_P_MASK;
    }
    if( IsDlgButtonChecked( hwnd, STAT_FLAG_S ) ) {
        IntData.EFlags |= FLAG_S_MASK;
    }
    if( IsDlgButtonChecked( hwnd, STAT_FLAG_Z ) ) {
        IntData.EFlags |= FLAG_Z_MASK;
    }
} /* GetStatRegisters */

#ifndef __NT__
BOOL __export FAR PASCAL SegMapDlgProc( HWND hwnd, UINT msg, UINT wparam,
                                    DWORD lparam )
{
    char        buff[128];
    WORD        i;
    WORD        seg;

    lparam = lparam;
    switch( msg ) {
    case WM_INITDIALOG:
        /*
         * fill out segment mappings
         */
        SetDlgCourierFont( hwnd, SEGMAP_LIST );
        SendDlgItemMessage( hwnd, SEGMAP_LIST, LB_RESETCONTENT, 0, 0L );
        for( i=0; i<= 1024; i++ ) {
            seg = NumToAddr( DTTaskEntry.hModule, i );
            if( seg != 0 ) {
                sprintf( buff,"%04d->%04x", i, seg );
                SendDlgItemMessage( hwnd, SEGMAP_LIST, LB_ADDSTRING, 0,
                                (LONG)(LPSTR)buff );
            }
        }
        break;
    case WM_COMMAND:
        switch( wparam ) {
        case IDOK:
            EndDialog( hwnd, 0 );
            break;
        case SEGMAP_LIST:
            if( HIWORD( lparam ) == LBN_DBLCLK ) {

                GLOBALENTRY     ge;
                char            str[100];
                int             sel;

                sel = SendDlgItemMessage( hwnd, SEGMAP_LIST, LB_GETCURSEL,
                                          0, 0L );
                SendDlgItemMessage( hwnd, SEGMAP_LIST, LB_GETTEXT, sel,
                            (DWORD) (LPSTR) str );
                str[4] = 0;
                seg = atoi( str );
                if( DoGlobalEntryModule( &ge, DTTaskEntry.hModule, seg ) ) {
                    DispMem( Instance, hwnd, (WORD)ge.hBlock, (ge.dwSize == 1) );
                }
            }
            break;
        default:
            return( FALSE );
        }
        break;
    default:
        return( FALSE );
    }
    return( TRUE );
}
#endif

/*
 * StatDialog - show task status
 */
BOOL __export FAR PASCAL StatDialog( HWND hwnd, UINT msg, UINT wparam,
                                    DWORD lparam )
{
    WORD        cmd;
    FARPROC     fp;

    fp = fp;
    lparam = lparam;
    switch( msg ) {
    case WM_INITDIALOG:
        InitStatDialog( hwnd );
        oldIntData = IntData;
        return( TRUE );
        break;
    case WM_VSCROLL:
        ScrollAsmDisplay( hwnd, wparam, &currAddr, &firstAddr,
                        STAT_DISASM_1, STAT_DISASM_8, STAT_SCROLL );
        break;
    case WM_USER:
        /* an owned window is being destroyed make sure
         * Windows doesn't take the focus away from us*/
        SetFocus( hwnd );
        break;
    case WM_CLOSE:
        PostMessage( hwnd, WM_COMMAND, STAT_CANCEL, 0L );
        return( TRUE );
    case WM_COMMAND:
        cmd = LOWORD( wparam );
        switch( cmd ) {
#ifndef __NT__
        case STAT_SEG_MAP:
            fp = MakeProcInstance( (FARPROC)SegMapDlgProc, Instance );
            JDialogBox( Instance, "SEG_MAP_DLG", hwnd, (DLGPROC)fp );
            FreeProcInstance( fp );
            break;
        case STAT_STACK_TRACE:
            StartStackTraceDialog( hwnd );
            break;
#else
        case STAT_SEG_MAP:
            {
                HANDLE                  hdl;
                DuplicateHandle(
                            GetCurrentProcess(),
                            processHdl,
                            GetCurrentProcess(),
                            &hdl,
                            0,
                            FALSE,
                            DUPLICATE_SAME_ACCESS );
                WalkMemory( hwnd, hdl, processID );
            }
            break;
#endif
        case STAT_SYMBOLS:
            if( StatShowSymbols ) {
                StatShowSymbols = FALSE;
            } else {
                StatShowSymbols = TRUE;
            }
            CheckDlgButton( hwnd, STAT_SYMBOLS, StatShowSymbols );
            DisplayAsmLines( hwnd, &currAddr, &firstAddr, STAT_DISASM_1,
                        STAT_DISASM_8, STAT_SCROLL );
            break;
        case STAT_APPLY:
            GetStatRegisters( hwnd );
            InitStatDialog( hwnd );
            return( TRUE );
        case IDCANCEL:
            IntData = oldIntData;
            EndDialog( hwnd, 0 );
            return( TRUE );
        case IDOK:
            GetStatRegisters( hwnd );
            EndDialog( hwnd, 0 );
            return( TRUE );
        }
    }
    return( FALSE );

} /* StatDialog */

/*
 * DoStatDialog - run the stat dialog
 */
void DoStatDialog( HWND hwnd )
{
    FARPROC     fp;

    fp = MakeProcInstance( (FARPROC)StatDialog, Instance );
    JDialogBox( Instance, "TASKSTATUS", hwnd, (DLGPROC)fp );
    FreeProcInstance( fp );

} /* DoStatDialog */
