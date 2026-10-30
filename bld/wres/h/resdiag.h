/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2016-2026 The Open Watcom Contributors. All Rights Reserved.
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
    int                 x;
    int                 y;
    int                 width;
    int                 height;
} DialogSizeInfo;

typedef struct DialogBoxHeader {
    uint_16             Version;
    uint_16             Signature;
    uint_32             HelpId;
    DialogExstyle       ExtendedStyle;
    DialogStyle         Style;
    uint_16             NumOfItems;
    DialogSizeInfo      SizeInfo;
    ResNameOrOrdinal    *MenuName;
    ResNameOrOrdinal    *ClassName;
    char                *Caption;       /* '\0' terminated */
    uint_16             PointSize;      /* only here if (Style & DS_SETFONT) */
    uint_16             FontWeight;     /* only here if (Style & DS_SETFONT) */
    uint_8              FontItalic;     /* only here if (Style & DS_SETFONT) */
    uint_8              FontCharset;    /* only here if (Style & DS_SETFONT) */
    char                *FontName;      /* only here if (Style & DS_SETFONT) */
    char                FontWeightDefined;
    char                FontItalicDefined;
    char                FontCharsetDefined;
} DialogBoxHeader;

typedef struct DialogBoxControl {
    uint_32             HelpId;
    DialogExstyle       ExtendedStyle;
    DialogStyle         Style;
    DialogSizeInfo      SizeInfo;
    uint_32             ID;
    ResNameOrOrdinal    *ClassID;
    ResNameOrOrdinal    *Text;
    uint_16             ExtraBytes;
} DialogBoxControl;

/* predefined classes for controls */
#define CLASS_BUTTON    0x80
#define CLASS_EDIT      0x81
#define CLASS_STATIC    0x82
#define CLASS_LISTBOX   0x83
#define CLASS_SCROLLBAR 0x84
#define CLASS_COMBOBOX  0x85

/* reswdiag.c */

extern bool             ResWriteDialogBoxHeader( DialogBoxHeader *head, FILE *fp );
extern bool             ResWriteDialogBoxHeader32( DialogBoxHeader *head, FILE *fp );
extern bool             ResWriteDialogBoxExHeader( DialogBoxHeader *head, FILE *fp );
extern bool             ResWriteDialogBoxControl( DialogBoxControl *control, FILE *fp );
extern bool             ResWriteDialogBoxControl32( DialogBoxControl *control, FILE *fp );
extern bool             ResWriteDialogBoxExControl( DialogBoxControl *control, FILE *fp );
extern ResNameOrOrdinal *ResNormalizeControlClass( const ResNameOrOrdinal *cclass );

/* resrdiag.c */

extern bool             ResReadDialogBoxHeader( DialogBoxHeader *head, FILE *fp );
extern bool             ResReadDialogBoxHeader32( DialogBoxHeader *head, FILE *fp );
extern bool             ResReadDialogBoxExHeader( DialogBoxHeader *head, FILE *fp );
extern bool             ResReadDialogBoxControl( DialogBoxControl *control, FILE *fp );
extern bool             ResReadDialogBoxControl32( DialogBoxControl *control, FILE *fp );
extern bool             ResReadDialogBoxExControl( DialogBoxControl *control, FILE *fp );
extern bool             ResIsDialogBoxEx( FILE *fp );
extern void             ResFreeDialogBoxHeaderPtrs( DialogBoxHeader *head );
extern void             ResFreeDialogBoxHeaderPtrs32( DialogBoxHeader *head );

#endif
