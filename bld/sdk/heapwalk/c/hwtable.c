/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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


#include "heapwalk.h"

msglist FlagMsg[] = {
    LF_FREE,                    (char *)(pointer_uint)STR_FREE,
    LF_MOVEABLE,                (char *)(pointer_uint)STR_MOVEABLE,
    LF_FIXED,                   (char *)(pointer_uint)STR_FIXED,
    0,                          NULL
};

msglist LocalGDIMsg[] = {
    LT_GDI_PEN,                 (char *)(pointer_uint)STR_PEN,
    LT_GDI_BRUSH,               (char *)(pointer_uint)STR_BRUSH,
    LT_GDI_FONT,                (char *)(pointer_uint)STR_FONT,
    LT_GDI_PALETTE,             (char *)(pointer_uint)STR_PALETTE,
    LT_GDI_BITMAP,              (char *)(pointer_uint)STR_BITMAP,
    LT_GDI_RGN,                 (char *)(pointer_uint)STR_RGN,
    LT_GDI_DC,                  (char *)(pointer_uint)STR_DEVICE_CONTEXT,
    LT_GDI_DISABLED_DC,         (char *)(pointer_uint)STR_DISABLED_DC,
    LT_GDI_METADC,              (char *)(pointer_uint)STR_META_DC,
    LT_GDI_METAFILE,            (char *)(pointer_uint)STR_METAFILE,
    LT_GDI_MAX,                 (char *)(pointer_uint)STR_MAX,
    LT_NORMAL,                  (char *)(pointer_uint)STR_UNKNOWN,
    LT_FREE,                    (char *)(pointer_uint)STR_FREE,
    0,                          NULL
};

msglist LocalUSERMsg[] = {
    LT_USER_CLASS,              (char *)(pointer_uint)STR_CLASS,
    LT_USER_WND,                (char *)(pointer_uint)STR_WND,
    LT_USER_STRING,             (char *)(pointer_uint)STR_STRING,
    LT_USER_MENU,               (char *)(pointer_uint)STR_MENU,
    LT_USER_CLIP,               (char *)(pointer_uint)STR_CLIP,
    LT_USER_CBOX,               (char *)(pointer_uint)STR_CBOX,
    LT_USER_PALETTE,            (char *)(pointer_uint)STR_PALETTE,
    LT_USER_ED,                 (char *)(pointer_uint)STR_ED,
    LT_USER_BWL,                (char *)(pointer_uint)STR_BWL,
    LT_USER_OWNERDRAW,          (char *)(pointer_uint)STR_OWNERDRAWN,
    LT_USER_SPB,                (char *)(pointer_uint)STR_SPB,
    LT_USER_CHECKPOINT,         (char *)(pointer_uint)STR_CHECKPOINT,
    LT_USER_DCE,                (char *)(pointer_uint)STR_DCE,
    LT_USER_MWP,                (char *)(pointer_uint)STR_MWP,
    LT_USER_PROP,               (char *)(pointer_uint)STR_PROP,
    LT_USER_LBIV,               (char *)(pointer_uint)STR_LBIV,
    LT_USER_MISC,               (char *)(pointer_uint)STR_MISC,
    LT_USER_ATOMS,              (char *)(pointer_uint)STR_ATOMS,
    LT_USER_LOCKINPUTSTATE,     (char *)(pointer_uint)STR_LOCKINPUTSTATE,
    LT_USER_HOOKLIST,           (char *)(pointer_uint)STR_HOOKLIST,
    LT_USER_USERSEEUSERDOALLOC, (char *)(pointer_uint)STR_USUDA,
    LT_USER_HOTKEYLIST,         (char *)(pointer_uint)STR_HOTKEYLIST,
    LT_USER_POPUPMENU,          (char *)(pointer_uint)STR_POPUPMENU,
    LT_USER_HANDLETABLE,        (char *)(pointer_uint)STR_HANDLETABLE,
    LT_USER_MAX,                (char *)(pointer_uint)STR_MAX,
    LT_NORMAL,                  (char *)(pointer_uint)STR_UNKNOWN,
    LT_FREE,                    (char *)(pointer_uint)STR_FREE,
    0,                          NULL
};

msglist LocalNormMsg[] = {
    LT_NORMAL,                  (char *)(pointer_uint)STR_UNKNOWN,
    LT_FREE,                    (char *)(pointer_uint)STR_FREE,
    0,                          NULL
};

msglist Sort_types[] = {
    HEAPMENU_SORT_ADDR,         (char *)(pointer_uint)STR_ADDRESS,
    HEAPMENU_SORT_HANDLE,       (char *)(pointer_uint)STR_HANDLE,
    HEAPMENU_SORT_MODULE,       (char *)(pointer_uint)STR_LC_MODULE,
    HEAPMENU_SORT_SIZE,         (char *)(pointer_uint)STR_SIZE,
    HEAPMENU_SORT_TYPE,         (char *)(pointer_uint)STR_TYPE,
    HEAPMENU_SORT_FLAG,         (char *)(pointer_uint)STR_FLAG,
    HEAPMENU_SORT_LRU,          (char *)(pointer_uint)STR_AGE,
    0,                          NULL
};

msglist DPMI_Sort_types[] = {
    HEAPMENU_SORT_ADDR,         (char *)(pointer_uint)STR_BASE_ADDR,
    HEAPMENU_SORT_HANDLE,       (char *)(pointer_uint)STR_SELECTOR,
    HEAPMENU_SORT_SIZE,         (char *)(pointer_uint)STR_LIMIT,
    HEAPMENU_SORT_TYPE,         (char *)(pointer_uint)STR_TYPE,
    HEAPMENU_SORT_GRAN,         (char *)(pointer_uint)STR_GRANULARITY,
    HEAPMENU_SORT_DPL,          (char *)(pointer_uint)STR_DPL,
    0,                          NULL
};

msglist Display_types[] = {
    HEAPMENU_DISPLAY_ENTIRE,    (char *)(pointer_uint)STR_ALL_HEAP_ITEMS,
    HEAPMENU_DISPLAY_LRU,       (char *)(pointer_uint)STR_ONLY_LRU_ITEMS,
    HEAPMENU_DISPLAY_FREE,      (char *)(pointer_uint)STR_ONLY_FREE_ITEMS,
    HEAPMENU_DISPLAY_DPMI,      (char *)(pointer_uint)STR_SELECTOR_LIST,
    0,                          NULL
};

void InitializeStringTables( void ) {

    InitSrchTable( Instance, FlagMsg );
    InitSrchTable( Instance, LocalGDIMsg );
    InitSrchTable( Instance, LocalUSERMsg );
    InitSrchTable( Instance, LocalNormMsg );
    InitSrchTable( Instance, Sort_types );
    InitSrchTable( Instance, DPMI_Sort_types );
    InitSrchTable( Instance, Display_types );
    InitListStrTables();
    HeapWalkName = HWGetRCString( STR_APP_NAME);
    HeapTitles = HWGetRCString( STR_GBL_HEADINGS );
    HeapDPMITitles = HWGetRCString( STR_SELECTOR_HEADINGS );
    HeapLocalTitles = HWGetRCString( STR_LCL_HEADINGS );
}
