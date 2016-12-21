/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2016-2016 The Open Watcom Contributors. All Rights Reserved.
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


#ifndef RESDIAG_INCLUDED
#define RESDIAG_INCLUDED

#include "resnamor.h"

typedef uint_32         DialogStyle;
typedef uint_32         DialogExstyle;

typedef struct DialogSizeInfo {
    uint_16             x;
    uint_16             y;
    uint_16             width;
    uint_16             height;
} DialogSizeInfo;

typedef struct DialogBoxHeader {
    DialogStyle         Style;
    uint_8              NumOfItems;
    DialogSizeInfo      Size;
    ResNameOrOrdinal    *MenuName;
    ResNameOrOrdinal    *ClassName;
    char                *Caption;       /* '\0' terminated */
    uint_16             PointSize;      /* only here if (Style & DS_SETFONT) */
    char                *FontName;      /* only here if (Style & DS_SETFONT) */
} DialogBoxHeader;

typedef struct DialogBoxHeader32 {
    DialogStyle         Style;
    DialogExstyle       ExtendedStyle;
    uint_16             NumOfItems;
    DialogSizeInfo      Size;
    ResNameOrOrdinal    *MenuName;
    ResNameOrOrdinal    *ClassName;
    char                *Caption;       /* '\0' terminated */
    uint_16             PointSize;      /* only here if (Style & DS_SETFONT) */
    char                *FontName;      /* only here if (Style & DS_SETFONT) */
} DialogBoxHeader32;

typedef struct DialogBoxExHeader32 {
    uint_16             Version;
    uint_16             Signature;
    uint_32             HelpId;
    DialogExstyle       ExtendedStyle;
    DialogStyle         Style;
    uint_16             NumOfItems;
    DialogSizeInfo      Size;
    ResNameOrOrdinal    *MenuName;
    ResNameOrOrdinal    *ClassName;
    char                *Caption;       /* '\0' terminated */
    uint_16             PointSize;      /* only here if (Style & DS_SETFONT) */
    uint_16             FontWeight;     /* only here if (Style & DS_SETFONT) */
    uint_8              FontItalic;     /* only here if (Style & DS_SETFONT) */
    uint_8              FontCharset;    /* only here if (Style & DS_SETFONT) */
    char                *FontName;      /* only here if (Style & DS_SETFONT) */
} DialogBoxExHeader32;

typedef struct DialogExHeader32 {
    uint_16             FontWeight;
    uint_8              FontItalic;
    uint_8              FontCharset;
    uint_32             HelpId;
    char                FontWeightDefined;
    char                FontItalicDefined;
    char                FontCharsetDefined;
} DialogExHeader32;

typedef union ControlClass {
    uint_8              Class;          /* if (class & 0x80) */
    char                ClassName[1];   /* '\0' terminated */
} ControlClass;

typedef struct DialogBoxControl {
    DialogSizeInfo      Size;
    uint_16             ID;
    DialogStyle         Style;
    ControlClass        *ClassID;
    ResNameOrOrdinal    *Text;
    uint_8              ExtraBytes;     /* should be 0 */
} DialogBoxControl;

typedef struct DialogBoxControl32 {
    DialogStyle         Style;
    DialogExstyle       ExtendedStyle;
    DialogSizeInfo      Size;
    uint_16             ID;
    ControlClass        *ClassID;
    ResNameOrOrdinal    *Text;
    uint_16             ExtraBytes;     /* should be 0 */
} DialogBoxControl32;

typedef struct DialogBoxExControl32 {
    uint_32             HelpId;
    DialogExstyle       ExtendedStyle;
    DialogStyle         Style;
    DialogSizeInfo      Size;
    uint_32             ID;
    ControlClass        *ClassID;
    ResNameOrOrdinal    *Text;
    uint_16             ExtraBytes;
} DialogBoxExControl32;

/* predefined classes for controls */
#define CLASS_BUTTON    0x80
#define CLASS_EDIT      0x81
#define CLASS_STATIC    0x82
#define CLASS_LISTBOX   0x83
#define CLASS_SCROLLBAR 0x84
#define CLASS_COMBOBOX  0x85

/* reswdiag.c */

extern bool ResWriteDialogBoxHeader( DialogBoxHeader *, WResFileID fid );
extern bool ResWriteDialogBoxHeader32( DialogBoxHeader32 *head, WResFileID fid );
extern bool ResWriteDialogBoxControl( DialogBoxControl *, WResFileID fid );
extern bool ResWriteDialogBoxControl32( DialogBoxControl32 *control, WResFileID fid );
extern bool ResWriteDialogExHeader32( DialogBoxHeader32 *head, DialogExHeader32 *exhead, WResFileID fid );
extern bool ResWriteDialogExControl32( DialogBoxExControl32 *control, WResFileID fid );
extern ControlClass *ResNameOrOrdToControlClass( const ResNameOrOrdinal *);
extern ControlClass *ResNumToControlClass( uint_16 classnum );

/* resrdiag.c */

extern bool ResReadDialogBoxHeader( DialogBoxHeader *head, WResFileID fid );
extern bool ResReadDialogBoxHeader32( DialogBoxHeader32 *head, WResFileID fid );
extern bool ResReadDialogExHeader32( DialogBoxHeader32 *, DialogExHeader32 *, WResFileID fid );
extern bool ResReadDialogBoxControl( DialogBoxControl *, WResFileID fid );
extern bool ResReadDialogBoxControl32( DialogBoxControl32 *, WResFileID fid );
extern bool ResReadDialogExControl32( DialogBoxExControl32 *, WResFileID fid );
extern bool ResIsDialogEx( WResFileID fid );
extern void ResFreeDialogBoxHeaderPtrs( DialogBoxHeader *head );
extern void ResFreeDialogBoxHeader32Ptrs( DialogBoxHeader32 *head );

#endif
