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
* Description:  Display a version resource.
*
****************************************************************************/


#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#ifndef __NT__
#include <ver.h>
#endif
#include "verinfo.h"
#include "restest.h"

char *VerStringTypes[] = {
    "Comments",
    "CompanyName",
    "FileDescription",
    "FileVersion",
    "InternalName",
    "LegalCopyright",
    "LegalTrademarks",
    "OriginalFileName",
    "PrivateBuild",
    "ProductName",
    "ProductVersion",
    "SpecialBuild"
};

int NumVerStrings = sizeof( VerStringTypes ) / sizeof( char * );

void FormatFixedInfo( HWND lb, VS_FIXEDFILEINFO far *info )
{
    LBPrintf( lb, "FIXED INFO" );
    LBPrintf( lb, "    signature                 %08lX", info->dwSignature );
    LBPrintf( lb, "    StrucVersion              %08lX", info->dwStrucVersion );
    LBPrintf( lb, "    FileVersionMS             %08lX", info->dwFileVersionMS );
    LBPrintf( lb, "    FileVersionLS             %08lX", info->dwFileVersionLS );
    LBPrintf( lb, "    ProductMS                 %08lX", info->dwProductVersionMS );
    LBPrintf( lb, "    ProductLS                 %08lX", info->dwProductVersionLS );
    LBPrintf( lb, "    FileFlagsMask             %08lX", info->dwFileFlagsMask );
    LBPrintf( lb, "    FileFlags                 %08lX", info->dwFileFlags );
    LBPrintf( lb, "    FileOS                    %08lX", info->dwFileOS );
    LBPrintf( lb, "    FileType                  %08lX", info->dwFileType );
    LBPrintf( lb, "    FileSubtype               %08lX", info->dwFileSubtype );
    LBPrintf( lb, "    FileDateMS                %08lX", info->dwFileDateMS );
    LBPrintf( lb, "    FileDateLS                %08lX", info->dwFileDateLS );
}

void FormatTranslationInfo( HWND lb, WORD far *info, UINT infosize )
{
    LBPrintf( lb, "" );
    LBPrintf( lb, "Translation INFO" );
    while( infosize >= 4 ) {
        LBPrintf( lb, "    language: %04X   charset: %04X", *info, *(info + 1 ) );
        info +=2;
        infosize -=4;
    }
    if( infosize != 0 ) {
        Error( "verinfo", "Translation Info size not a multiple of 4" );
    }
}

void DoStringSection( HWND lb, void *info, WORD far *lang, UINT infosize )
{
    char        namebuf[100];
    WORD        i;
    void far    *ptr;
    UINT        size;

    while( infosize >= 4 ) {
        LBPrintf( lb, "" );
        LBPrintf( lb, "STRING SECTION %04X%04X", *lang, *(lang + 1 ) );
        for( i = 0; i < NumVerStrings; i++ ) {
            sprintf( namebuf, "\\StringFileInfo\\%04X%04X\\%s",
                     *lang, *(lang + 1 ), VerStringTypes[i] );
            LBPrintf( lb, "" );
            LBPrintf( lb, VerStringTypes[i] );
#ifdef __NT__
            {
                WORD    unibuf[100];
                WORD    j;

                for( j=0; j < 100; j++ ) unibuf[j] = namebuf[j];
                if( !VerQueryValueW( info, unibuf, &ptr, &size ) ) {
                    LBPrintf( lb, "    Not Available" );
                } else {
                    LBDump( lb, ptr, size * 2 );
                }
            }
#else
            if( !VerQueryValue( info, namebuf, &ptr, &size ) ) {
                LBPrintf( lb, "    Not Available" );
            } else {
                LBDump( lb, ptr, size );
            }
#endif
        }
        lang +=2;
        infosize -=4;
    }
}

BOOL CALLBACK VerInfoDlgProc( HWND hwnd, UINT msg, UINT wparam, DWORD lparam )
{
    DWORD               infohdl;
    UINT                infosize;
    void                *info;
    HWND                lb;
    void far *          ptr;
    char                name[512];

    wparam = wparam;
    lparam = lparam;
    switch( msg ) {
    case WM_INITDIALOG:
        GetModuleFileName( Instance, name, sizeof( name ) );
        infosize = GetFileVersionInfoSize( name, &infohdl );
        if( infosize == 0 ) {
            Error( "verinfo", "GetFileVersionInfoSize failed" );
            SendMessage( hwnd, WM_CLOSE, 0, 0 );
            break;
        }
        info = malloc( infosize );
        if( info == NULL ) {
            Error( "verinfo", "malloc failed" );
            SendMessage( hwnd, WM_CLOSE, 0, 0 );
            break;
        }
        if( !GetFileVersionInfo( name, infohdl, infosize, info ) ) {
            Error( "verinfo", "GetFileVersionInfo failed" );
            SendMessage( hwnd, WM_CLOSE, 0, 0 );
            break;
        }
        lb = GetDlgItem( hwnd, LBOX );
        if( !VerQueryValue( info, "\\", &ptr, &infosize ) ) {
            Error( "verinfo", "VerQueryValue root failed" );
            SendMessage( hwnd, WM_CLOSE, 0, 0 );
            break;
        }
        FormatFixedInfo( lb, ptr );
        if( !VerQueryValue( info, "\\VarFileInfo\\Translation", &ptr, &infosize ) ) {
            Error( "verinfo", "VerQueryValue translation failed" );
            SendMessage( hwnd, WM_CLOSE, 0, 0 );
            break;
        }
        FormatTranslationInfo( lb, ptr, infosize );
        DoStringSection( lb, info, ptr, infosize );
        break;
    case WM_CLOSE:
        EndDialog( hwnd, 0 );
        break;
    default:
        return( FALSE );
    }
    return( TRUE );
}

void DisplayVerInfo( void )
{
    FARPROC     fp;

    fp = MakeProcInstance( (FARPROC)VerInfoDlgProc, Instance );
    DialogBox( Instance, "VERINFODLG" , NULL, (DLGPROC)fp );
    FreeProcInstance( fp );
}
