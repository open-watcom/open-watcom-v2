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
* Description:  Covers for commdlg.dll functions.
*
****************************************************************************/


#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <dos.h>
#include <windows.h>
#include <commdlg.h>
#include "stubs.h"

#pragma aux BackPatch_commdlg parm [ax];
extern LPVOID FAR BackPatch_commdlg( char *str );

static BOOL  (FAR PASCAL *commdlgChooseColor)(LPCHOOSECOLOR);
static HWND  (FAR PASCAL *commdlgReplaceText)(LPFINDREPLACE);
static HWND  (FAR PASCAL *commdlgFindText)(LPFINDREPLACE);
static BOOL  (FAR PASCAL *commdlgChooseFont)(LPCHOOSEFONT);
static BOOL  (FAR PASCAL *commdlgGetOpenFileName)(LPOPENFILENAME);
static BOOL  (FAR PASCAL *commdlgGetSaveFileName)(LPOPENFILENAME);
static BOOL  (FAR PASCAL *commdlgPrintDlg)(LPPRINTDLG);

/*
 * __ChooseColor - cover for common dialog ChooseColor
 */
BOOL  FAR PASCAL __ChooseColor( LPCHOOSECOLOR pcc )
{
    BOOL        rc;
    LPVOID      psave;
    LPVOID      pnew;

    if( commdlgChooseColor == NULL ) {
        commdlgChooseColor = BackPatch_commdlg( "ChooseColor" );
        if( commdlgChooseColor == NULL ) return( 0 );
    }
    pnew = psave = pcc->lpCustColors;
    GetAlias( &pnew );
    pcc->lpCustColors = pnew;
    rc = commdlgChooseColor( pcc );
    ReleaseAlias( psave, pnew );
    pcc->lpCustColors = psave;

    return( rc );

} /* ChooseColor */

/*
 * __ReplaceText - cover for common dialog ReplaceText
 */
HWND FAR PASCAL __ReplaceText( LPFINDREPLACE pfr )
{
    HWND        rc;
    LPVOID      psave1,psave2,psave3;
    LPVOID      pnew1,pnew2,pnew3;

    if( commdlgReplaceText == NULL ) {
        commdlgReplaceText = BackPatch_commdlg( "ReplaceText" );
        if( commdlgReplaceText == NULL ) return( 0 );
    }
    pnew1 = psave1 = pfr->lpstrFindWhat;
    pnew2 = psave2 = pfr->lpstrReplaceWith;
    pnew3 = psave3 = pfr->lpTemplateName;
    GetAlias( &pnew1 );
    GetAlias( &pnew2 );
    GetAlias( &pnew3 );
    pfr->lpstrFindWhat = pnew1;
    pfr->lpstrReplaceWith = pnew2;
    pfr->lpTemplateName = pnew3;

    rc = commdlgReplaceText( pfr );

    ReleaseAlias( psave1, pnew1 );
    ReleaseAlias( psave2, pnew2 );
    ReleaseAlias( psave3, pnew3 );
    pfr->lpstrFindWhat = psave1;
    pfr->lpstrReplaceWith = psave2;
    pfr->lpTemplateName = psave3;
    return( rc );

} /* __ReplaceText */

/*
 * __FindText - cover for common dialog FindText
 */
HWND FAR PASCAL __FindText( LPFINDREPLACE pfr )
{
    HWND        rc;
    LPVOID      psave1,psave2;
    LPVOID      pnew1,pnew2;


    if( commdlgFindText == NULL ) {
        commdlgFindText = BackPatch_commdlg( "FindText" );
        if( commdlgFindText == NULL ) return( 0 );
    }
    pnew1 = psave1 = pfr->lpstrFindWhat;
    pnew2 = psave2 = pfr->lpTemplateName;
    GetAlias( &pnew1 );
    GetAlias( &pnew2 );
    pfr->lpstrFindWhat = pnew1;
    pfr->lpTemplateName = pnew2;

    rc = commdlgFindText( pfr );

    ReleaseAlias( psave1, pnew1 );
    ReleaseAlias( psave2, pnew2 );
    pfr->lpstrFindWhat = psave1;
    pfr->lpTemplateName = psave2;
    return( rc );

} /* __FindText */

/*
 * __ChooseFont - cover for common dialog ChooseFont
 */
BOOL FAR PASCAL __ChooseFont( LPCHOOSEFONT pcf )
{
    BOOL        rc;
    LPVOID      psave1,psave2,psave3;
    LPVOID      pnew1,pnew2,pnew3;

    if( commdlgChooseFont == NULL ) {
        commdlgChooseFont = BackPatch_commdlg( "ChooseFont" );
        if( commdlgChooseFont == NULL ) return( 0 );
    }
    pnew1 = psave1 = pcf->lpLogFont;
    pnew2 = psave2 = pcf->lpTemplateName;
    pnew3 = psave3 = pcf->lpszStyle;
    GetAlias( &pnew1 );
    GetAlias( &pnew2 );
    GetAlias( &pnew3 );
    pcf->lpLogFont = pnew1;
    pcf->lpTemplateName = pnew2;
    pcf->lpszStyle = pnew3;

    rc = commdlgChooseFont( pcf );

    ReleaseAlias( psave1, pnew1 );
    ReleaseAlias( psave2, pnew2 );
    ReleaseAlias( psave3, pnew3 );
    pcf->lpLogFont = psave1;
    pcf->lpTemplateName = psave2;
    pcf->lpszStyle = psave3;
    return( rc );

} /* __ChooseFont */

/*
 * GetSaveOrOpenFileName
 */
static BOOL GetSaveOrOpenFileName( FARPROC fp, LPOPENFILENAME pofn )
{
    BOOL                rc;
    LPVOID              psave1,psave2,psave3,psave4;
    LPVOID              psave5,psave6,psave7,psave8;
    LPVOID              pnew1,pnew2,pnew3,pnew4;
    LPVOID              pnew5,pnew6,pnew7,pnew8;

    pnew1 = psave1 = pofn->lpstrFilter;
    pnew2 = psave2 = pofn->lpstrCustomFilter;
    pnew3 = psave3 = pofn->lpstrFile;
    pnew4 = psave4 = pofn->lpstrFileTitle;
    pnew5 = psave5 = pofn->lpstrInitialDir;
    pnew6 = psave6 = pofn->lpstrTitle;
    pnew7 = psave7 = pofn->lpstrDefExt;
    pnew8 = psave8 = pofn->lpTemplateName;

    GetAlias( &pnew1 );
    GetAlias( &pnew2 );
    GetAlias( &pnew3 );
    GetAlias( &pnew4 );
    GetAlias( &pnew5 );
    GetAlias( &pnew6 );
    GetAlias( &pnew7 );
    GetAlias( &pnew8 );
    pofn->lpstrFilter = pnew1;
    pofn->lpstrCustomFilter = pnew2;
    pofn->lpstrFile = pnew3;
    pofn->lpstrFileTitle = pnew4;
    pofn->lpstrInitialDir = pnew5;
    pofn->lpstrTitle = pnew6;
    pofn->lpstrDefExt = pnew7;
    pofn->lpTemplateName = pnew8;

    rc = fp( (LPOPENFILENAME) pofn );

    ReleaseAlias( psave1, pnew1 );
    ReleaseAlias( psave2, pnew2 );
    ReleaseAlias( psave3, pnew3 );
    ReleaseAlias( psave4, pnew4 );
    ReleaseAlias( psave5, pnew5 );
    ReleaseAlias( psave6, pnew6 );
    ReleaseAlias( psave7, pnew7 );
    ReleaseAlias( psave8, pnew8 );
    pofn->lpstrFilter = psave1;
    pofn->lpstrCustomFilter = psave2;
    pofn->lpstrFile = psave3;
    pofn->lpstrFileTitle = psave4;
    pofn->lpstrInitialDir = psave5;
    pofn->lpstrTitle = psave6;
    pofn->lpstrDefExt = psave7;
    pofn->lpTemplateName = psave8;
    return( rc );

} /* GetSaveOrOpenFileName */

/*
 * __GetOpenFileName - cover for common dialog GetOpenFileName
 */
BOOL FAR PASCAL __GetOpenFileName( LPOPENFILENAME pofn )
{
    if( commdlgGetOpenFileName == NULL ) {
        commdlgGetOpenFileName = BackPatch_commdlg( "GetOpenFileName" );
        if( commdlgGetOpenFileName == NULL ) return( 0 );
    }
    return( GetSaveOrOpenFileName( commdlgGetOpenFileName, pofn ) );
} /* __GetOpenFileName */

/*
 * __GetSaveFileName - cover for common dialog GetSaveFileName
 */
BOOL FAR PASCAL __GetSaveFileName( LPOPENFILENAME pofn )
{
    if( commdlgGetSaveFileName == NULL ) {
        commdlgGetSaveFileName = BackPatch_commdlg( "GetSaveFileName" );
        if( commdlgGetSaveFileName == NULL ) return( 0 );
    }
    return( GetSaveOrOpenFileName( commdlgGetSaveFileName, pofn ) );

} /* __GetSaveFileName */

/*
 * __PrintDlg - cover for common dialog PrintDlg
 */
BOOL FAR PASCAL __PrintDlg( LPPRINTDLG ppd )
{
    BOOL        rc;
    LPVOID      psave1,psave2;
    LPVOID      pnew1,pnew2;

    if( commdlgPrintDlg == NULL ) {
        commdlgPrintDlg = BackPatch_commdlg( "PrintDlg" );
        if( commdlgPrintDlg == NULL ) return( 0 );
    }
    pnew1 = psave1 = ppd->lpPrintTemplateName;
    pnew2 = psave2 = ppd->lpSetupTemplateName;
    GetAlias( &pnew1 );
    GetAlias( &pnew2 );
    ppd->lpPrintTemplateName = pnew1;
    ppd->lpSetupTemplateName = pnew2;

    rc = commdlgPrintDlg( ppd );

    ReleaseAlias( psave1, pnew1 );
    ReleaseAlias( psave2, pnew2 );
    ppd->lpPrintTemplateName = psave1;
    ppd->lpSetupTemplateName = psave1;
    return( rc );

} /* __PrintDlg */
