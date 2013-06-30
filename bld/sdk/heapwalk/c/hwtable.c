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


#include "heapwalk.h"

msglist FlagMsg[] = {   LF_FREE,        (char *) STR_FREE,
                        LF_MOVEABLE,    (char *) STR_MOVEABLE,
                        LF_FIXED,       (char *) STR_FIXED,
                        0,              (char *) -1 };

msglist LocalGDIMsg[] = { LT_GDI_PEN,           (char *) STR_PEN,
                          LT_GDI_BRUSH,         (char *) STR_BRUSH,
                          LT_GDI_FONT,          (char *) STR_FONT,
                          LT_GDI_PALETTE,       (char *) STR_PALETTE,
                          LT_GDI_BITMAP,        (char *) STR_BITMAP,
                          LT_GDI_RGN,           (char *) STR_RGN,
                          LT_GDI_DC,            (char *) STR_DEVICE_CONTEXT,
                          LT_GDI_DISABLED_DC,   (char *) STR_DISABLED_DC,
                          LT_GDI_METADC,        (char *) STR_META_DC,
                          LT_GDI_METAFILE,      (char *) STR_METAFILE,
                          LT_GDI_MAX,           (char *) STR_MAX,
                          LT_NORMAL,            (char *) STR_UNKNOWN,
                          LT_FREE,              (char *) STR_FREE,
                          0,                    (char *) -1 };

msglist LocalUSERMsg[] = {
                        LT_USER_CLASS,          (char *) STR_CLASS,
                        LT_USER_WND,            (char *) STR_WND,
                        LT_USER_STRING,         (char *) STR_STRING,
                        LT_USER_MENU,           (char *) STR_MENU,
                        LT_USER_CLIP,           (char *) STR_CLIP,
                        LT_USER_CBOX,           (char *) STR_CBOX,
                        LT_USER_PALETTE,        (char *) STR_PALETTE,
                        LT_USER_ED,             (char *) STR_ED,
                        LT_USER_BWL,            (char *) STR_BWL,
                        LT_USER_OWNERDRAW,      (char *) STR_OWNERDRAWN,
                        LT_USER_SPB,            (char *) STR_SPB,
                        LT_USER_CHECKPOINT,     (char *) STR_CHECKPOINT,
                        LT_USER_DCE,            (char *) STR_DCE,
                        LT_USER_MWP,            (char *) STR_MWP,
                        LT_USER_PROP,           (char *) STR_PROP,
                        LT_USER_LBIV,           (char *) STR_LBIV,
                        LT_USER_MISC,           (char *) STR_MISC,
                        LT_USER_ATOMS,          (char *) STR_ATOMS,
                        LT_USER_LOCKINPUTSTATE, (char *) STR_LOCKINPUTSTATE,
                        LT_USER_HOOKLIST,       (char *) STR_HOOKLIST,
                        LT_USER_USERSEEUSERDOALLOC, (char *)STR_USUDA,
                        LT_USER_HOTKEYLIST,     (char *) STR_HOTKEYLIST,
                        LT_USER_POPUPMENU,      (char *) STR_POPUPMENU,
                        LT_USER_HANDLETABLE,    (char *) STR_HANDLETABLE,
                        LT_USER_MAX,            (char *) STR_MAX,
                        LT_NORMAL,              (char *) STR_UNKNOWN,
                        LT_FREE,                (char *) STR_FREE,
                        0,                      (char *) -1 };

msglist LocalNormMsg[] = {
                        LT_NORMAL,              (char *) STR_UNKNOWN,
                        LT_FREE,                (char *) STR_FREE,
                        0,                      (char *) -1 };

msglist Sort_types[] = {
    HEAPMENU_SORT_ADDR,         (char *) STR_ADDRESS,
    HEAPMENU_SORT_HANDLE,       (char *) STR_HANDLE,
    HEAPMENU_SORT_MODULE,       (char *) STR_LC_MODULE,
    HEAPMENU_SORT_SIZE,         (char *) STR_SIZE,
    HEAPMENU_SORT_TYPE,         (char *) STR_TYPE,
    HEAPMENU_SORT_FLAG,         (char *) STR_FLAG,
    HEAPMENU_SORT_LRU,          (char *) STR_AGE,
    0,                          (char *) -1 };


msglist DPMI_Sort_types[] = {
    HEAPMENU_SORT_ADDR,         (char *) STR_BASE_ADDR,
    HEAPMENU_SORT_HANDLE,       (char *) STR_SELECTOR,
    HEAPMENU_SORT_SIZE,         (char *) STR_LIMIT,
    HEAPMENU_SORT_TYPE,         (char *) STR_TYPE,
    HEAPMENU_SORT_GRAN,         (char *) STR_GRANULARITY,
    HEAPMENU_SORT_DPL,          (char *) STR_DPL,
    0,                          (char *) -1 };

msglist Display_types[] = {
    HEAPMENU_DISPLAY_ENTIRE,    (char *) STR_ALL_HEAP_ITEMS,
    HEAPMENU_DISPLAY_LRU,       (char *) STR_ONLY_LRU_ITEMS,
    HEAPMENU_DISPLAY_FREE,      (char *) STR_ONLY_FREE_ITEMS,
    HEAPMENU_DISPLAY_DPMI,      (char *) STR_SELECTOR_LIST,
    0,                          (char *) -1 };


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
