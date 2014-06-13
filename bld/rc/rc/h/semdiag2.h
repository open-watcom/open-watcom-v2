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
* Description:  Semantic actions for processing dialog resources (OS/2 grammar).
*
****************************************************************************/


#ifndef SEMDIAG2_H_INCLUDED
#define SEMDIAG2_H_INCLUDED

typedef struct FullDialogOptions {
    YTOKEN                  token;
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
            uint_8          FontItalic;
            uint_8          FontExtra;  /* don't know what this is but
                                         * MS rc accepts it and it appears in
                                         * some samples */
            char            FontWeightDefined;
            char            FontItalicDefined;
        } Font;
    } Opt;
} FullDialogOptions;

extern FullDiagCtrlListOS2      *SemOS2NewDiagCtrlList( FullDialogBoxControlOS2 *ctrl,
                                    DataElemList *, PresParamListOS2 * );
extern FullDiagCtrlListOS2      *SemOS2AddDiagCtrlList( FullDiagCtrlListOS2 *list,
                                    FullDialogBoxControlOS2 *ctrl, DataElemList *,
                                    PresParamListOS2 * );
extern FullDialogBoxControlOS2  *SemOS2NewDiagCtrl( YTOKEN token, FullDiagCtrlOptionsOS2 opts,
                                    PresParamListOS2 * );
extern FullDialogBoxControlOS2  *SemOS2SetWindowData( FullDiagCtrlOptionsOS2, IntMask,
                                    PresParamListOS2 *, FullDiagCtrlListOS2 *, YTOKEN );
extern FullDialogBoxControlOS2  *SemOS2SetControlData( ResNameOrOrdinal *name, uint_32 id,
                                    DialogSizeInfo size, ResNameOrOrdinal *ctlclass,
                                    IntMask style, FullDiagCtrlListOS2 *, PresParamListOS2 * );
extern PresParamListOS2         *SemOS2NewPresParamList( PresParamsOS2 presparam );
extern PresParamListOS2         *SemOS2AppendPresParam( PresParamListOS2 *list,
                                    PresParamsOS2 presparam );

extern void                     SemOS2WriteDialogTemplate( WResID *name, ResMemFlags,
                                    uint_32, FullDiagCtrlListOS2 * );
extern void                     SemOS2AddDlgincResource( WResID *name, char *filename );

#endif
