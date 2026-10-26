/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2026      The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Semantic actions for processing dialog resources.
*
****************************************************************************/


#ifndef SEMDIAG_INCLUDED
#define SEMDIAG_INCLUDED

typedef unsigned long       help_id;

typedef struct IntMask {
    uint_32     Mask;
    uint_32     Value;
    boolbit     longVal :1;
    boolbit     unsgVal :1;
} IntMask;

typedef struct FullDialogBoxHeader {
    DialogBoxHeader         Head;
    bool                    StyleGiven;
} FullDialogBoxHeader;

typedef struct FullDialogBoxControlWIN {
    LL_ITEM( struct FullDialogBoxControlWIN );
    DialogBoxControl        ctrl;
    DataElemList            *dataListHead;
} FullDialogBoxControlWIN;

typedef struct FullDialogCtrlListWIN {
    FullDialogBoxControlWIN *head;
    FullDialogBoxControlWIN *tail;
    uint_16                 numctrls; /* Win16 only support upto 255 controls. */
} FullDialogCtrlListWIN;

typedef struct FullDialogCtrlOptionsWIN {
    DialogSizeInfo          SizeInfo;
    IntMask                 Style;
    uint_16                 ID;
    ResNameOrOrdinal        *Text;
    uint_32                 ExtendedStyle;
    help_id                 HelpId;
} FullDialogCtrlOptionsWIN;

typedef struct PresParamsOS2 {
    LL_ITEM( struct PresParamsOS2 );
    ResNameOrOrdinal        *Name;
    DataElemList            *dataList;
    uint_32                 size;
} PresParamsOS2;

typedef struct PresParamListOS2 {
    PresParamsOS2           *head;
    PresParamsOS2           *tail;
    uint_32                 size;
} PresParamListOS2;

typedef struct FullDialogBoxControlOS2 {
    LL_ITEM( struct FullDialogBoxControlOS2 );
    DialogBoxControl                ctrl;
    DataElemList                    *dataListHead;
    PresParamListOS2                *presParams;
    struct FullDialogCtrlListOS2    *children;
    DialogTemplateItemOS2           *tmpl;
    uint_32                         framectl;
} FullDialogBoxControlOS2;

typedef struct FullDialogCtrlListOS2 {
    FullDialogBoxControlOS2         *head;
    FullDialogBoxControlOS2         *tail;
    uint_8                          numctrls;
} FullDialogCtrlListOS2;

typedef struct FullDialogCtrlOptionsOS2 {
    DialogSizeInfo          SizeInfo;
    IntMask                 Style;
    uint_16                 ID;
    ResNameOrOrdinal        *Text;
} FullDialogCtrlOptionsOS2;

#endif
