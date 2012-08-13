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
* Description:  OS/2 resource file related structures and definitions.
*
****************************************************************************/

#ifndef __OS2RES_H__
#define __OS2RES_H__

/* OS/2 resource types */
#define OS2_RT_POINTER      1
#define OS2_RT_BITMAP       2
#define OS2_RT_MENU         3
#define OS2_RT_DIALOG       4
#define OS2_RT_STRING       5
#define OS2_RT_FONTDIR      6
#define OS2_RT_FONT         7
#define OS2_RT_ACCELTABLE   8
#define OS2_RT_RCDATA       9
#define OS2_RT_MESSAGE      10
#define OS2_RT_DLGINCLUDE   11
#define OS2_RT_VKEYTBL      12
#define OS2_RT_KEYTBL       13
#define OS2_RT_CHARTBL      14
#define OS2_RT_DISPLAYINFO  15
#define OS2_RT_FKASHORT     16
#define OS2_RT_FKALONG      17
#define OS2_RT_HELPTABLE    18
#define OS2_RT_HELPSUBTABLE 19
#define OS2_RT_FDDIR        20
#define OS2_RT_FD           21
#define OS2_RT_DEFAULTICON  22
#define OS2_RT_MAX          22
#define OS2_RT_RESNAMES     255

/* Select OS/2 window classes (encoded Windows style) */
#define OS2_WC_FRAME           0x81
#define OS2_WC_COMBOBOX        0x82
#define OS2_WC_BUTTON          0x83
#define OS2_WC_MENU            0x84
#define OS2_WC_STATIC          0x85
#define OS2_WC_ENTRYFIELD      0x86
#define OS2_WC_LISTBOX         0x87
#define OS2_WC_SCROLLBAR       0x88
#define OS2_WC_TITLEBAR        0x89
#define OS2_WC_MLE             0x8A
#define OS2_WC_SPINBUTTON      0xA0
#define OS2_WC_CONTAINER       0xA5
#define OS2_WC_SLIDER          0xA6
#define OS2_WC_VALUESET        0xA7
#define OS2_WC_NOTEBOOK        0xA8

/* OS/2 window styles */
#define OS2_WS_VISIBLE         0x80000000
#define OS2_WS_DISABLED        0x40000000
#define OS2_WS_CLIPCHILDREN    0x20000000
#define OS2_WS_CLIPSIBLINGS    0x10000000
#define OS2_WS_PARENTCLIP      0x08000000
#define OS2_WS_SAVEBITS        0x04000000
#define OS2_WS_SYNCPAINT       0x02000000
#define OS2_WS_MINIMIZED       0x01000000
#define OS2_WS_MAXIMIZED       0x00800000
#define OS2_WS_ANIMATE         0x00400000
#define OS2_WS_GROUP           0x00010000
#define OS2_WS_TABSTOP         0x00020000
#define OS2_WS_MULTISELECT     0x00040000

/* OS/2 frame window styles */
#define OS2_FS_ICON            0x00000001
#define OS2_FS_ACCELTABLE      0x00000002
#define OS2_FS_SHELLPOSITION   0x00000004
#define OS2_FS_TASKLIST        0x00000008
#define OS2_FS_NOBYTEALIGN     0x00000010
#define OS2_FS_NOMOVEWITHOWNER 0x00000020
#define OS2_FS_SYSMODAL        0x00000040
#define OS2_FS_DLGBORDER       0x00000080
#define OS2_FS_BORDER          0x00000100
#define OS2_FS_SCREENALIGN     0x00000200
#define OS2_FS_MOUSEALIGN      0x00000400
#define OS2_FS_SIZEBORDER      0x00000800
#define OS2_FS_AUTOICON        0x00001000
#define OS2_FS_DBE_APPSTAT     0x00008000

/* OS/2 frame creation flags */
#define OS2_FCF_TITLEBAR        0x00000001
#define OS2_FCF_SYSMENU         0x00000002
#define OS2_FCF_MENU            0x00000004
#define OS2_FCF_SIZEBORDER      0x00000008
#define OS2_FCF_MINBUTTON       0x00000010
#define OS2_FCF_MAXBUTTON       0x00000020
#define OS2_FCF_MINMAX          0x00000030
#define OS2_FCF_VERTSCROLL      0x00000040
#define OS2_FCF_HORZSCROLL      0x00000080
#define OS2_FCF_DLGBORDER       0x00000100
#define OS2_FCF_BORDER          0x00000200
#define OS2_FCF_SHELLPOSITION   0x00000400
#define OS2_FCF_TASKLIST        0x00000800
#define OS2_FCF_NOBYTEALIGN     0x00001000
#define OS2_FCF_NOMOVEWITHOWNER 0x00002000
#define OS2_FCF_ICON            0x00004000
#define OS2_FCF_ACCELTABLE      0x00008000
#define OS2_FCF_SYSMODAL        0x00010000
#define OS2_FCF_SCREENALIGN     0x00020000
#define OS2_FCF_MOUSEALIGN      0x00040000
#define OS2_FCF_HIDEBUTTON      0x01000000
#define OS2_FCF_HIDEMAX         0x01000020
#define OS2_FCF_CLOSEBUTTON     0x04000000
#define OS2_FCF_AUTOICON        0x40000000
#define OS2_FCF_DBE_APPSTAT     0x80000000

/* OS/2 static control styles */
#define OS2_SS_TEXT          0x0001
#define OS2_SS_GROUPBOX      0x0002
#define OS2_SS_ICON          0x0003
#define OS2_SS_BITMAP        0x0004
#define OS2_SS_FGNDRECT      0x0005
#define OS2_SS_HALFTONERECT  0x0006
#define OS2_SS_BKGNDRECT     0x0007
#define OS2_SS_FGNDFRAME     0x0008
#define OS2_SS_HALFTONEFRAME 0x0009
#define OS2_SS_BKGNDFRAME    0x000A
#define OS2_SS_SYSICON       0x000B
#define OS2_SS_AUTOSIZE      0x0040

/* OS/2 button control styles */
#define OS2_BS_PUSHBUTTON          0
#define OS2_BS_CHECKBOX            1
#define OS2_BS_AUTOCHECKBOX        2
#define OS2_BS_RADIOBUTTON         3
#define OS2_BS_AUTORADIOBUTTON     4
#define OS2_BS_3STATE              5
#define OS2_BS_AUTO3STATE          6
#define OS2_BS_USERBUTTON          7
#define OS2_BS_NOTEBOOKBUTTON      8
#define OS2_BS_DEFAULT        0x0400

/* OS/2 combobox control styles */
#define OS2_CBS_SIMPLE             1
#define OS2_CBS_DROPDOWN           2
#define OS2_CBS_DROPDOWNLIST       4

/* OS/2 container control styles */
#define OS2_CCS_EXTENDSEL          1
#define OS2_CCS_MULTIPLESEL        2
#define OS2_CCS_SINGLESEL          4
#define OS2_CCS_AUTOPOSITION       8

/* OS/2 entryfield control styles */
#define OS2_ES_LEFT                0
#define OS2_ES_CENTER              1
#define OS2_ES_RIGHT               2
#define OS2_ES_AUTOSCROLL          4

/* OS/2 MLE control styles */
#define OS2_MLS_WORDWRAP           1
#define OS2_MLS_BORDER             2
#define OS2_MLS_VSCROLL            4
#define OS2_MLS_HSCROLL            8

/* OS/2 text centering flags */
#define OS2_DT_LEFT            0x00000000
#define OS2_DT_CENTER          0x00000100
#define OS2_DT_RIGHT           0x00000200

/* OS/2 menu styles */
#define OS2_MIS_TEXT        1
#define OS2_MIS_BITMAP      2
#define OS2_MIS_SEPARATOR   4
#define OS2_MIS_OWNERDRAW   8
#define OS2_MIS_SUBMENU     16

/* OS/2 menu attributes */
#define OS2_MIA_NODISMISS 0x0020
#define OS2_MIA_FRAMED    0x1000
#define OS2_MIA_CHECKED   0x2000
#define OS2_MIA_DISABLED  0x4000
#define OS2_MIA_HILITED   0x8000

/* OS/2 accelerator flags */
#define OS2_ACCEL_NONE        0x0000
#define OS2_ACCEL_CHAR        0x0001
#define OS2_ACCEL_VIRTUALKEY  0x0002
#define OS2_ACCEL_SCANCODE    0x0004
#define OS2_ACCEL_SHIFT       0x0008
#define OS2_ACCEL_CTRL        0x0010
#define OS2_ACCEL_ALT         0x0020
#define OS2_ACCEL_LONEKEY     0x0040
#define OS2_ACCEL_SYSCOMMAND  0x0100
#define OS2_ACCEL_HELP        0x0200

/* Hack to get the bootstrap wrc building with gcc; note that the _Packed
 * keyword is probably not necessary for most of these structs.
 */
#if defined( __GNUC__ ) || defined( __SUNPRO_C ) || defined( _MSC_VER )
    #define _Packed
#endif

typedef _Packed struct MenuHeaderOS2 {
    uint_32 Size;
    uint_16 Codepage;
    uint_16 Class;
    uint_16 NumItems;
} MenuHeaderOS2;

typedef struct MenuItemOS2 {
    uint_16 ItemStyle;
    uint_16 ItemAttrs;
    uint_16 ItemCmd;
    char    *ItemText;
} MenuItemOS2;

typedef _Packed struct DialogHeaderOS2 {
    uint_16 Size;
    uint_16 Type;
    uint_16 Codepage;
    uint_16 OffsetFirstTmpl;
    uint_16 TemplateStatus;
    uint_16 ItemFocus;
    uint_16 OffsetPresParams;
} DialogHeaderOS2;

typedef _Packed struct DialogTemplateItemOS2 {
    uint_16 fsItemStatus;
    uint_16 cChildren;
    uint_16 cchClassName;
    uint_16 offClassName;
    uint_16 cchText;
    uint_16 offText;
    uint_32 flStyle;
    uint_16 x;
    uint_16 y;
    uint_16 cx;
    uint_16 cy;
    uint_16 id;
    uint_16 offPresParams;
    uint_16 offCtlData;
} DialogTemplateItemOS2;

typedef _Packed struct AccelTableEntryOS2 {
    uint_16     Flags;
    uint_16     Ascii;
    uint_16     Id;
} AccelTableEntryOS2;

typedef _Packed struct HelpTableEntryOS2 {
    uint_16     WindowId;
    uint_16     SubtableId;
    uint_16     Dummy;
    uint_16     ExtendedId;
} HelpTableEntryOS2;

#endif
