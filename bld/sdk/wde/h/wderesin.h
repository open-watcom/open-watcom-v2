/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2026 The Open Watcom Contributors. All Rights Reserved.
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


#ifndef WDERESIN_INCLUDED
#define WDERESIN_INCLUDED

#include "wrdll.h"
#include "wresall.h"
#include "wdehash.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define GETHDR_STYLE(ph)            ((ph)->head.Style)
#define GETHDR_NUMITEMS(ph)         ((ph)->head.NumOfItems)
#define GETHDR_SIZE(ph)             ((ph)->head.SizeInfo)
#define GETHDR_PSIZE(ph)            (&((ph)->head.SizeInfo))
#define GETHDR_SIZEX(ph)            ((ph)->head.SizeInfo.x)
#define GETHDR_SIZEY(ph)            ((ph)->head.SizeInfo.y)
#define GETHDR_SIZEW(ph)            ((ph)->head.SizeInfo.width)
#define GETHDR_SIZEH(ph)            ((ph)->head.SizeInfo.height)
#define GETHDR_MENUNAME(ph)         ((ph)->head.MenuName)
#define GETHDR_CLASSNAME(ph)        ((ph)->head.ClassName)
#define GETHDR_CAPTION(ph)          ((ph)->head.Caption)
#define GETHDR_FONTPOINTSIZE(ph)    ((ph)->head.PointSize)
#define GETHDR_FONTFACENAME(ph)     ((ph)->head.FontName)
#define GETHDR_FONTWEIGHT(ph)       ((ph)->head.FontWeight)
#define GETHDR_FONTITALIC(ph)       ((ph)->head.FontItalic)
#define GETHDR_FONTCHARSET(ph)      ((ph)->head.FontCharset)
#define GETHDR_HELPID(ph)           ((ph)->head.HelpId)
#define GETHDR_EXSTYLE(ph)          ((ph)->head.ExtendedStyle)

#define SETHDR_STYLE(ph,s)          ((ph)->head.Style = (s))
#define SETHDR_NUMITEMS(ph,s)       ((ph)->head.NumOfItems = (s))
#define SETHDR_SIZE(ph,s)           ((ph)->head.SizeInfo = (s))
#define SETHDR_SIZEX(ph,s)          ((ph)->head.SizeInfo.x = (s))
#define SETHDR_SIZEY(ph,s)          ((ph)->head.SizeInfo.y = (s))
#define SETHDR_SIZEW(ph,s)          ((ph)->head.SizeInfo.width = (s))
#define SETHDR_SIZEH(ph,s)          ((ph)->head.SizeInfo.height = (s))
#define SETHDR_MENUNAME(ph,s)       ((ph)->head.MenuName = (s))
#define SETHDR_CLASSNAME(ph,s)      ((ph)->head.ClassName = (s))
#define SETHDR_CAPTION(ph,s)        ((ph)->head.Caption = (s))
#define SETHDR_FONTPOINTSIZE(ph,s)  ((ph)->head.PointSize = (s))
#define SETHDR_FONTFACENAME(ph,s)   ((ph)->head.FontName = (s))
#define SETHDR_FONTWEIGHT(ph,s)     ((ph)->head.FontWeight = (s))
#define SETHDR_FONTITALIC(ph,s)     ((ph)->head.FontItalic = (s))
#define SETHDR_FONTCHARSET(ph,s)    ((ph)->head.FontCharset = (s))
#define SETHDR_HELPID(ph,s)         ((ph)->head.HelpId = (s))
#define SETHDR_EXSTYLE(ph,s)        ((ph)->head.ExtendedStyle = (s))

#define GETCTL_SIZE(pc)             ((pc)->ctrl.SizeInfo)
#define GETCTL_PSIZE(pc)            (&((pc)->ctrl.SizeInfo))
#define GETCTL_SIZEX(pc)            ((pc)->ctrl.SizeInfo.x)
#define GETCTL_SIZEY(pc)            ((pc)->ctrl.SizeInfo.y)
#define GETCTL_SIZEW(pc)            ((pc)->ctrl.SizeInfo.width)
#define GETCTL_SIZEH(pc)            ((pc)->ctrl.SizeInfo.height)
#define GETCTL_ID(pc)               ((pc)->ctrl.ID)
#define GETCTL_STYLE(pc)            ((pc)->ctrl.Style)
#define GETCTL_CLASSID(pc)          ((pc)->ctrl.ClassID)
#define GETCTL_TEXT(pc)             ((pc)->ctrl.Text)
#define GETCTL_EXTRABYTES(pc)       ((pc)->ctrl.ExtraBytes)
#define GETCTL_HELPID(pc)           ((pc)->ctrl.HelpId)
#define GETCTL_EXSTYLE(pc)          ((pc)->ctrl.ExtendedStyle)

#define SETCTL_SIZE(pc,s)           ((pc)->ctrl.SizeInfo = (s))
#define SETCTL_SIZEX(pc,s)          ((pc)->ctrl.SizeInfo.x = (s))
#define SETCTL_SIZEY(pc,s)          ((pc)->ctrl.SizeInfo.y = (s))
#define SETCTL_SIZEW(pc,s)          ((pc)->ctrl.SizeInfo.width = (s))
#define SETCTL_SIZEH(pc,s)          ((pc)->ctrl.SizeInfo.height = (s))
#define SETCTL_ID(pc,s)             ((pc)->ctrl.ID = (s))
#define SETCTL_STYLE(pc,s)          ((pc)->ctrl.Style = (s))
#define SETCTL_CLASSID(pc,s)        ((pc)->ctrl.ClassID = (s))
#define SETCTL_TEXT(pc,s)           ((pc)->ctrl.Text = (s))
#define SETCTL_EXTRABYTES(pc,s)     ((pc)->ctrl.ExtraBytes = (s))
#define SETCTL_HELPID(pc,s)         ((pc)->ctrl.HelpId = (s))
#define SETCTL_EXSTYLE(pc,s)        ((pc)->ctrl.ExtendedStyle = (s))

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

typedef struct WdeResInfoStruct {
    WRInfo              *info;
    char                *sym_name;
    WResTypeNode        *dlg_entry;         /* type node for dialogs           */
    LIST                *ditem_list;        /* list of WdeResDlgItem's         */
    OBJPTR              next_current;
    WRHashTable         *hash_table;
    HWND                res_win;
    HWND                edit_win;
    HWND                forms_win;
    bool                modified;
    bool                symbols_dirty;
    bool                active;
    bool                editting;
    bool                is32bit;
} WdeResInfo;

typedef struct WdeDialogBoxHeader {
    DialogBoxHeader     head;
    char                *symbol;
    char                *helpsymbol;
    bool                is32bit;
    bool                is32bitEx;
} WdeDialogBoxHeader;


typedef struct WdeDialogBoxControl {
    DialogBoxControl    ctrl;
    char                *symbol;
    char                *helpsymbol;
} WdeDialogBoxControl;

typedef struct WdeDialogBoxInfoStruct {
    WdeDialogBoxHeader  *header;
    LIST                *control_list;      /* list of DialogBoxControl's      */
    ResMemFlags         res_flags;
} WdeDialogBoxInfo;

typedef struct WdeResDlgItem {
    WdeDialogBoxInfo    *dlg_info;
    OBJPTR              object;
    WResID              *dialog_name;
    bool                modified;
    WResResNode         *resnode;
    WResLangNode        *langnode;
    bool                is32bit;
} WdeResDlgItem;

/****************************************************************************/
/* function prototypes                                                      */
/****************************************************************************/
extern bool             WdeFreeResInfo( WdeResInfo *res_info );
extern bool             WdeFreeDialogBoxInfo( WdeDialogBoxInfo *dlg_info );
extern void             WdeFreeResDlgItem( WdeResDlgItem **pditem, bool );
extern WdeResDlgItem    *WdeAllocResDlgItem( void );
extern WdeResInfo       *WdeAllocResInfo( void );
extern bool             WdeIsResModified( WdeResInfo *res_info );
extern void             WdeSetResModified( WdeResInfo *res_info, bool modified );

#endif
