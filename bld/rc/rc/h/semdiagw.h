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
* Description:  Semantic actions for processing dialog resources (WIN grammar).
*
****************************************************************************/


#ifndef SEMDIAGW_H_INCLUDED
#define SEMDIAGW_H_INCLUDED

typedef struct FullDialogOptions {
    YYTOKENTYPE             token;
    union {
        ResNameOrOrdinal *  Name;
        char *              Str;
        DialogStyle         Style;
        DialogExstyle       Exstyle;
        WResLangType        lang;
        struct {
            uint_16         PointSize;
            char *          FontName;
            uint_16         FontWeight;
            bool            FontItalic;
            uint_8          FontExtra;  /* don't know what this is but
                                         * MS rc accepts it and it appears in
                                         * some samples */
            bool            FontWeightDefined;
            bool            FontItalicDefined;
        } Font;
    } Opt;
} FullDialogOptions;

extern FullDialogBoxHeader  *SemWINNewDiagOptions( FullDialogOptions * opt );
extern FullDialogBoxHeader  *SemWINDiagOptions( FullDialogBoxHeader * head,
                                FullDialogOptions * opt );
extern FullDiagCtrlList     *SemWINNewDiagCtrlList( FullDialogBoxControl * ctrl,
                                DataElemList * );
extern FullDiagCtrlList     *SemWINAddDiagCtrlList( FullDiagCtrlList * list,
                                FullDialogBoxControl * ctrl, DataElemList * );
extern FullDialogBoxControl *SemWINNewDiagCtrl( YYTOKENTYPE token,
                                FullDiagCtrlOptions opts );
extern void                 SemWINWriteDialogBox( WResID * name, ResMemFlags, DialogSizeInfo,
                                FullDialogBoxHeader *, FullDiagCtrlList *, DlgHelpId,
                                YYTOKENTYPE );
extern FullDiagCtrlList     *SemWINEmptyDiagCtrlList( void );
extern FullDialogBoxControl *SemWINSetControlData( IntMask, uint_16, DialogSizeInfo,
                                WResID *, ResNameOrOrdinal *, uint_32, DlgHelpId * );

#endif
