/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Covers for commdlg.dll functions.
*
****************************************************************************/


#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <dos.h>
#include <windows.h>
#include <commdlg.h>
#include "winstubs.h"
#include "_commdlg.h"


extern LPVOID FAR BackPatch_commdlg( char *str );
#pragma aux BackPatch_commdlg __parm [__ax]

static BOOL  (FAR PASCAL *commdlgChooseColor)( LPCHOOSECOLOR );
static HWND  (FAR PASCAL *commdlgReplaceText)( LPFINDREPLACE );
static HWND  (FAR PASCAL *commdlgFindText)( LPFINDREPLACE );
static BOOL  (FAR PASCAL *commdlgChooseFont)( LPCHOOSEFONT );
static BOOL  (FAR PASCAL *commdlgGetOpenFileName)( LPOPENFILENAME );
static BOOL  (FAR PASCAL *commdlgGetSaveFileName)( LPOPENFILENAME );
static BOOL  (FAR PASCAL *commdlgPrintDlg)( LPPRINTDLG );

/*
 * __ChooseColor - cover for common dialog ChooseColor
 */
BOOL  FAR PASCAL __ChooseColor( LPCHOOSECOLOR pcc )
{
    BOOL        rc;
    DWORD       odata;

    if( commdlgChooseColor == NULL ) {
        commdlgChooseColor = BackPatch_commdlg( "ChooseColor" );
        if( commdlgChooseColor == NULL ) {
            return( 0 );
        }
    }
    odata = GETALIAS( &pcc->lpCustColors );
    rc = commdlgChooseColor( pcc );
    RELEASEALIAS( &pcc->lpCustColors, odata );

    return( rc );

} /* ChooseColor */

/*
 * __ReplaceText - cover for common dialog ReplaceText
 */
HWND FAR PASCAL __ReplaceText( LPFINDREPLACE pfr )
{
    HWND        rc;
    DWORD       odata1;
    DWORD       odata2;
    DWORD       odata3;

    if( commdlgReplaceText == NULL ) {
        commdlgReplaceText = BackPatch_commdlg( "ReplaceText" );
        if( commdlgReplaceText == NULL ) {
            return( 0 );
        }
    }
    odata1 = GETALIAS( &pfr->lpstrFindWhat );
    odata2 = GETALIAS( &pfr->lpstrReplaceWith );
    odata3 = GETALIAS( &pfr->lpTemplateName );
    rc = commdlgReplaceText( pfr );
    RELEASEALIAS( &pfr->lpTemplateName, odata3 );
    RELEASEALIAS( &pfr->lpstrReplaceWith, odata2 );
    RELEASEALIAS( &pfr->lpstrFindWhat, odata1 );
    return( rc );

} /* __ReplaceText */

/*
 * __FindText - cover for common dialog FindText
 */
HWND FAR PASCAL __FindText( LPFINDREPLACE pfr )
{
    HWND        rc;
    DWORD       odata1;
    DWORD       odata2;

    if( commdlgFindText == NULL ) {
        commdlgFindText = BackPatch_commdlg( "FindText" );
        if( commdlgFindText == NULL ) {
            return( 0 );
        }
    }
    odata1 = GETALIAS( &pfr->lpstrFindWhat );
    odata2 = GETALIAS( &pfr->lpTemplateName );
    rc = commdlgFindText( pfr );
    RELEASEALIAS( &pfr->lpTemplateName, odata2 );
    RELEASEALIAS( &pfr->lpstrFindWhat, odata1 );
    return( rc );

} /* __FindText */

/*
 * __ChooseFont - cover for common dialog ChooseFont
 */
BOOL FAR PASCAL __ChooseFont( LPCHOOSEFONT pcf )
{
    BOOL        rc;
    DWORD       odata1;
    DWORD       odata2;
    DWORD       odata3;

    if( commdlgChooseFont == NULL ) {
        commdlgChooseFont = BackPatch_commdlg( "ChooseFont" );
        if( commdlgChooseFont == NULL ) {
            return( 0 );
        }
    }
    odata1 = GETALIAS( &pcf->lpLogFont );
    odata2 = GETALIAS( &pcf->lpTemplateName );
    odata3 = GETALIAS( &pcf->lpszStyle );
    rc = commdlgChooseFont( pcf );
    RELEASEALIAS( &pcf->lpszStyle, odata3 );
    RELEASEALIAS( &pcf->lpTemplateName, odata2 );
    RELEASEALIAS( &pcf->lpLogFont, odata1 );
    return( rc );

} /* __ChooseFont */

/*
 * GetSaveOrOpenFileName
 */
static BOOL GetSaveOrOpenFileName( FARPROC fp, LPOPENFILENAME pofn )
{
    BOOL        rc;
    DWORD       odata1;
    DWORD       odata2;
    DWORD       odata3;
    DWORD       odata4;
    DWORD       odata5;
    DWORD       odata6;
    DWORD       odata7;
    DWORD       odata8;

    odata1 = GETALIAS( &pofn->lpstrFilter );
    odata2 = GETALIAS( &pofn->lpstrCustomFilter );
    odata3 = GETALIAS( &pofn->lpstrFile );
    odata4 = GETALIAS( &pofn->lpstrFileTitle );
    odata5 = GETALIAS( &pofn->lpstrInitialDir );
    odata6 = GETALIAS( &pofn->lpstrTitle );
    odata7 = GETALIAS( &pofn->lpstrDefExt );
    odata8 = GETALIAS( &pofn->lpTemplateName );
    rc = ((BOOL(FAR PASCAL *)(LPOPENFILENAME))fp)( (LPOPENFILENAME)pofn );
    RELEASEALIAS( &pofn->lpTemplateName, odata8 );
    RELEASEALIAS( &pofn->lpstrDefExt, odata7 );
    RELEASEALIAS( &pofn->lpstrTitle, odata6 );
    RELEASEALIAS( &pofn->lpstrInitialDir, odata5 );
    RELEASEALIAS( &pofn->lpstrFileTitle, odata4 );
    RELEASEALIAS( &pofn->lpstrFile, odata3 );
    RELEASEALIAS( &pofn->lpstrCustomFilter, odata2 );
    RELEASEALIAS( &pofn->lpstrFilter, odata1 );
    return( rc );

} /* GetSaveOrOpenFileName */

/*
 * __GetOpenFileName - cover for common dialog GetOpenFileName
 */

BOOL FAR PASCAL __GetOpenFileName( LPOPENFILENAME pofn )
{
    if( commdlgGetOpenFileName == NULL ) {
        commdlgGetOpenFileName = BackPatch_commdlg( "GetOpenFileName" );
        if( commdlgGetOpenFileName == NULL ) {
            return( 0 );
        }
    }
    return( GetSaveOrOpenFileName( (FARPROC)commdlgGetOpenFileName, pofn ) );
} /* __GetOpenFileName */

/*
 * __GetSaveFileName - cover for common dialog GetSaveFileName
 */
BOOL FAR PASCAL __GetSaveFileName( LPOPENFILENAME pofn )
{
    if( commdlgGetSaveFileName == NULL ) {
        commdlgGetSaveFileName = BackPatch_commdlg( "GetSaveFileName" );
        if( commdlgGetSaveFileName == NULL ) {
            return( 0 );
        }
    }
    return( GetSaveOrOpenFileName( (FARPROC)commdlgGetSaveFileName, pofn ) );

} /* __GetSaveFileName */

/*
 * __PrintDlg - cover for common dialog PrintDlg
 */
BOOL FAR PASCAL __PrintDlg( LPPRINTDLG ppd )
{
    BOOL        rc;
    DWORD       odata1;
    DWORD       odata2;

    if( commdlgPrintDlg == NULL ) {
        commdlgPrintDlg = BackPatch_commdlg( "PrintDlg" );
        if( commdlgPrintDlg == NULL ) {
            return( 0 );
        }
    }
    odata1 = GETALIAS( &ppd->lpPrintTemplateName );
    odata2 = GETALIAS( &ppd->lpSetupTemplateName );
    rc = commdlgPrintDlg( ppd );
    RELEASEALIAS( &ppd->lpSetupTemplateName, odata2 );
    RELEASEALIAS( &ppd->lpPrintTemplateName, odata1 );
    return( rc );

} /* __PrintDlg */
