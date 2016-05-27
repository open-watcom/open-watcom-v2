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
* Description:  Display a data resource.
*
****************************************************************************/


#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <windows.h>
#include "restest.h"
#include "resname.h"
#include "verinfo.h"
#include "gettype.h"

static LPCSTR   dataType;
static char     dataTypeBuf[256];
static char     dataName[256];
static void far *dataPtr;
static DWORD    dataSize;

BOOL CALLBACK GetDataTypeDlgProc( HWND hwnd, UINT msg, UINT wparam, DWORD lparam )
{
    char        *end;
    WORD        val;

    lparam = lparam;
    switch( msg ) {
    case WM_COMMAND:
        if( LOWORD( wparam ) == IDOK ) {
            GetDlgItemText( hwnd, TYPE_NAME, dataTypeBuf, sizeof( dataTypeBuf ) );
            if( IsDlgButtonChecked( hwnd, TYPE_IS_NAME ) ) {
                dataType = dataTypeBuf;
            } else {
                val = strtoul( dataTypeBuf, &end, 0 );
                if( *end != '\0' ) {
                    while( isspace( *end ) ) end++;
                    if( *end != '\0' ) {
                        Error( "data", "You did not enter a numeric value" );
                    }
                }
                dataType = (LPCSTR) MAKELONG( val, 0 );
            }
            EndDialog( hwnd, 0 );
        }
        break;
    default:
        return( FALSE );
        break;
    }
    return( TRUE );
}

BOOL CALLBACK GetDataNameDlgProc( HWND hwnd, UINT msg, UINT wparam, DWORD lparam )
{
    lparam = lparam;
    switch( msg ) {
    case WM_COMMAND:
        if( LOWORD( wparam ) == IDOK ) {
            GetDlgItemText( hwnd, INPUT_FIELD, dataName, sizeof( dataName ) );
            EndDialog( hwnd, 0 );
        }
        break;
    default:
        return( FALSE );
        break;
    }
    return( TRUE );
}

BOOL CALLBACK DataDlgProc( HWND hwnd, UINT msg, UINT wparam, DWORD lparam )
{
    HWND        lb;

    wparam = wparam;
    lparam = lparam;
    switch( msg ) {
    case WM_INITDIALOG:
        lb = GetDlgItem( hwnd, LBOX );
        LBDump( lb, dataPtr, dataSize );
        break;
    case WM_CLOSE:
        EndDialog( hwnd, 0 );
        break;
    default:
        return( FALSE );
    }
    return( TRUE );
}

void DisplayData( bool rcdata )
{
    FARPROC     fp;
    HRSRC       rchdl;
    HGLOBAL     rcmemhdl;
    char        buf[256];

    if( rcdata ) {
        dataType = RT_RCDATA;
    } else {
        fp = MakeProcInstance( (FARPROC)GetDataTypeDlgProc, Instance );
        DialogBox( Instance, "GET_RES_TYPE_DLG" , NULL, (DLGPROC)fp );
        FreeProcInstance( fp );
    }
    fp = MakeProcInstance( (FARPROC)GetDataNameDlgProc, Instance );
    DialogBox( Instance, "GET_RES_NAME_DLG" , NULL, (DLGPROC)fp );
    FreeProcInstance( fp );

    rchdl = FindResource( Instance, dataName, dataType );
    if( rchdl == NULL ) {
        sprintf( buf, "Cant Find resource %s", dataName );
        Error( "data", buf );
        return;
    }
    dataSize = SizeofResource( Instance, rchdl );
    rcmemhdl = LoadResource( Instance, rchdl );
    if( rcmemhdl == NULL )  {
        sprintf( buf, "Cant Load resource %s", dataName );
        Error( "data", buf );
        return;
    }
    dataPtr = LockResource( rcmemhdl );
    if( dataPtr == NULL )  {
        sprintf( buf, "Cant Lock resource %s", dataName );
        Error( "data", buf );
        return;
    }

    fp = MakeProcInstance( (FARPROC)DataDlgProc, Instance );
    DialogBox( Instance, "VERINFODLG" , NULL, (DLGPROC)fp );
    FreeProcInstance( fp );
}
