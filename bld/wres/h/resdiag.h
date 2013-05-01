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


#ifndef RESDIAG_INCLUDED
#define RESDIAG_INCLUDED

#include "resnamor.h"

typedef uint_32     DialogStyle;
typedef uint_32     DialogExstyle;

#include "pushpck1.h"
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
    ResNameOrOrdinal *  MenuName;
    ResNameOrOrdinal *  ClassName;
    char *              Caption;        /* '\0' terminated */
    uint_16             PointSize;      /* only here if (Style & DS_SETFONT) */
    char *              FontName;       /* only here if (Style & DS_SETFONT) */
} _WCUNALIGNED DialogBoxHeader;
#include "poppck.h"

#include "pushpck2.h"
typedef struct DialogBoxHeader32 {
    DialogStyle           Style;
    uint_32               ExtendedStyle;
    uint_16               NumOfItems;
    DialogSizeInfo        Size;
    ResNameOrOrdinal     *MenuName;
    ResNameOrOrdinal     *ClassName;
    char                 *Caption;      /* '\0' terminated */
    uint_16               PointSize;    /* only here if (Style & DS_SETFONT) */
    char                 *FontName;     /* only here if (Style & DS_SETFONT) */
} _WCUNALIGNED DialogBoxHeader32;

typedef struct DialogExHeader32 {
    uint_16             FontWeight;
    uint_8              FontItalic;
    uint_8              FontExtra;
    uint_32             HelpId;
    char                FontWeightDefined;
    char                FontItalicDefined;
    char                FontExtraDefined;
} DialogExHeader32;
#include "poppck.h"

#include "pushpck1.h"
typedef union ControlClass {
    uint_8              Class;          /* if (class & 0x80) */
    char                ClassName[1];   /* '\0' terminated */
} ControlClass;

typedef struct DialogBoxControl {
    DialogSizeInfo      Size;
    uint_16             ID;
    uint_32             Style;
    ControlClass       *ClassID;
    ResNameOrOrdinal   *Text;
    uint_8              ExtraBytes;         /* should be 0 */
} _WCUNALIGNED DialogBoxControl;
#include "poppck.h"

#include "pushpck2.h"
typedef struct DialogBoxControl32 {
    uint_32               Style;
    uint_32               ExtendedStyle;
    DialogSizeInfo        Size;
    uint_16               ID;
    ControlClass         *ClassID;
    ResNameOrOrdinal     *Text;
    uint_16               ExtraBytes;       /* should be 0 */
} _WCUNALIGNED DialogBoxControl32;

typedef struct DialogBoxExControl32 {
    uint_32               HelpId;
    uint_32               ExtendedStyle;
    uint_32               Style;
    DialogSizeInfo        Size;
    uint_32               ID;
    ControlClass         *ClassID;
    ResNameOrOrdinal     *Text;
    uint_16               ExtraBytes;
} DialogBoxExControl32;
#include "poppck.h"

/* predefined classes for controls */
#define CLASS_BUTTON    0x80
#define CLASS_EDIT      0x81
#define CLASS_STATIC    0x82
#define CLASS_LISTBOX   0x83
#define CLASS_SCROLLBAR 0x84
#define CLASS_COMBOBOX  0x85

extern int ResWriteDialogBoxHeader( DialogBoxHeader *, WResFileID handle );
extern int ResWriteDialogBoxHeader32( DialogBoxHeader32 * head, WResFileID handle );
extern int ResWriteDialogBoxControl( DialogBoxControl *, WResFileID handle );
extern int ResWriteDialogBoxControl32( DialogBoxControl32 * control, WResFileID handle );
extern ControlClass * ResNameOrOrdToControlClass( const ResNameOrOrdinal *);
extern ControlClass * ResNumToControlClass( uint_16 classnum );
extern void ResFreeDialogBoxHeaderPtrs( DialogBoxHeader * head );
extern void ResFreeDialogBoxHeader32Ptrs( DialogBoxHeader32 * head );
extern int ResIsDialogEx( WResFileID handle );
extern int ResReadDialogBoxHeader( DialogBoxHeader * head, WResFileID handle );
extern int ResReadDialogBoxHeader32( DialogBoxHeader32 * head, WResFileID handle );
extern int ResReadDialogExHeader32( DialogBoxHeader32 *, DialogExHeader32 *, WResFileID handle );
extern int ResReadDialogBoxControl( DialogBoxControl *, WResFileID handle );
extern int ResReadDialogBoxControl32( DialogBoxControl32 *, WResFileID handle );
extern int ResReadDialogExControl32( DialogBoxExControl32 *, WResFileID handle );

extern int ResWriteDialogExHeader32( DialogBoxHeader32 *head,
                 DialogExHeader32 *exhead, WResFileID handle );
extern int ResWriteDialogExControl32( DialogBoxExControl32 *control, WResFileID handle );

#endif
