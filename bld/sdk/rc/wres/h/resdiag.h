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

#include "watcom.h"
#ifdef WIN_GUI
// the following is a temporary measure to get around the fact
// that winreg.h defines a type called ppvalue
#define _WINREG_
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#include "resnamor.h"
#include "layer0.h"

#ifdef __ALPHA__
#pragma pack(1);
#endif

typedef uint_32     DialogStyle;
typedef uint_32     DialogExstyle;

/* style bits used by name for Dialog box headers and controls */
/* these statements same as found in windows.h */

/* Wes Nelson -- Added the following #ifndef's to allow windows.h
 *               to be included before this file
 */

#if !defined( WIN_GUI ) && !defined( RES2WXF_COMPILE )

#define WS_POPUP            0x80000000L
#define WS_CHILD            0x40000000L
#define WS_VISIBLE          0x10000000L
#define WS_BORDER           0x00800000L
#define WS_DLGFRAME         0x00400000L
#define WS_CAPTION          ( WS_BORDER | WS_DLGFRAME )
#define WS_VSCROLL          0x00200000L
#define WS_HSCROLL          0x00100000L
#define WS_SYSMENU          0x00080000L
#define WS_THICKFRAME       0x00040000L
#define WS_GROUP            0x00020000L
#define WS_TABSTOP          0x00010000L
#define WS_MINIMIZEBOX      0x00020000L
#define WS_MAXIMIZEBOX      0x00010000L
#define DS_SETFONT          0x00000040L
/* edit control styles */
#define ES_LEFT             0x00000000L
#define ES_WANTRETURN       0x00001000L
#define ES_MULTILINE        0x00000004L
/* button control styles */
#define BS_PUSHBUTTON       0x00000000L
#define BS_DEFPUSHBUTTON    0x00000001L
#define BS_CHECKBOX         0x00000002L
#define BS_AUTOCHECKBOX     0x00000003L
#define BS_RADIOBUTTON      0x00000004L
#define BS_3STATE           0x00000005L
#define BS_AUTO3STATE       0x00000006L
#define BS_GROUPBOX         0x00000007L
#define BS_AUTORADIOBUTTON  0x00000009L
/* listbox control styles */
#define LBS_NOTIFY          0x00000001L
#define LBS_SORT            0x00000002L
#define LBS_NOINTEGRALHEIGHT        0x00000100L
/* static control styles */
#define SS_LEFT             0x00000000L
#define SS_CENTER           0x00000001L
#define SS_RIGHT            0x00000002L
#define SS_ICON             0x00000003L
#define SS_LEFTNOWORDWRAP   0x0000000CL
#define SS_NOPREFIX         0x00000080L
/* combobox styles */
#define CBS_SIMPLE          0x00000001L
#define CBS_DROPDOWN        0x00000002L
#define CBS_DROPDOWNLIST    0x00000003L
#define CBS_SORT            0x00000100L
#define CBS_NOINTEGRALHEIGHT        0x00000400L
/* scrollbar styles */
#define SBS_HORZ            0x00000000L

#endif

#if !defined( NATURAL_PACK )
#include "pshpk1.h"
#endif

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

#if !defined( NATURAL_PACK )
#include "poppk.h"
#include "pshpk2.h"
#endif

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

#if !defined( NATURAL_PACK )
#include "poppk.h"
#include "pshpk1.h"
#endif

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

#if !defined( NATURAL_PACK )
#include "poppk.h"
#include "pshpk2.h"
#endif

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

#if !defined( NATURAL_PACK )
#include "poppk.h"
//#include "pshpk1.h"
#endif

/* predefined classes for controls */
#define CLASS_BUTTON    0x80
#define CLASS_EDIT      0x81
#define CLASS_STATIC    0x82
#define CLASS_LISTBOX   0x83
#define CLASS_SCROLLBAR 0x84
#define CLASS_COMBOBOX  0x85

extern int ResWriteDialogBoxHeader( DialogBoxHeader *, WResFileID );
extern int ResWriteDialogBoxHeader32( DialogBoxHeader32 * head,
                                            WResFileID handle );
extern int ResWriteDialogBoxControl( DialogBoxControl *, WResFileID );
extern int ResWriteDialogBoxControl32( DialogBoxControl32 * control,
                WResFileID handle );
extern ControlClass * ResNameOrOrdToControlClass( const ResNameOrOrdinal *);
extern ControlClass * ResNumToControlClass( uint_16 classnum );
extern void ResFreeDialogBoxHeaderPtrs( DialogBoxHeader * head );
extern void ResFreeDialogBoxHeader32Ptrs( DialogBoxHeader32 * head );
extern int ResIsDialogEx( WResFileID handle );
extern int ResReadDialogBoxHeader( DialogBoxHeader * head, WResFileID handle );
extern int ResReadDialogBoxHeader32( DialogBoxHeader32 * head, WResFileID handle );
extern int ResReadDialogExHeader32( DialogBoxHeader32 *, DialogExHeader32 *, WResFileID );
extern int ResReadDialogBoxControl( DialogBoxControl *, WResFileID );
extern int ResReadDialogBoxControl32( DialogBoxControl32 *, WResFileID );
extern int ResReadDialogExControl32( DialogBoxExControl32 *, WResFileID );

extern int ResWriteDialogExHeader32( DialogBoxHeader32 *head,
                 DialogExHeader32 *exhead, WResFileID handle );
extern int ResWriteDialogExControl32( DialogBoxExControl32 *control,
                                        WResFileID );

#ifdef __ALPHA__
#pragma pack();
#endif
#endif
