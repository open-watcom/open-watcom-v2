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


#ifndef WDEACTN_INCLUDED
#define WDEACTN_INCLUDED

#define pick_ACTION_GET_WINDOW_HANDLE(o,x)      x##( GET_WINDOW_HANDLE,    GetWindowHandle,    (o *, HWND *, void *) )
#define pick_ACTION_GET_WINDOW_CLASS(o,x)       x##( GET_WINDOW_CLASS,     GetWindowClass,     (o *, char **, void *) )
#define pick_ACTION_GET_SUBOBJ_LIST(o,x)        x##( GET_SUBOBJ_LIST,      GetSubObjectList,   (o *, LIST **, void *) )
#define pick_ACTION_GET_OBJECT_INFO(o,x)        x##( GET_OBJECT_INFO,      GetObjectInfo,      (o *, void **, void **) )
#define pick_ACTION_SET_OBJECT_INFO(o,x)        x##( SET_OBJECT_INFO,      SetObjectInfo,      (o *, void *, void *) )
#define pick_ACTION_GET_OBJECT_HELPINFO(o,x)    x##( GET_OBJECT_HELPINFO,  GetObjectHelpInfo,  (o *, void **, char **) )
#define pick_ACTION_SET_OBJECT_HELPINFO(o,x)    x##( SET_OBJECT_HELPINFO,  SetObjectHelpInfo,  (o *, void *, char *) )
#define pick_ACTION_CREATE_WINDOW(o,x)          x##( CREATE_WINDOW,        CreateWindow,       (o *, bool *, void *) )
#define pick_ACTION_DESTROY_WINDOW(o,x)         x##( DESTROY_WINDOW,       DestroyWindow,      (o *, bool *, bool *) )
#define pick_ACTION_SHOW_WIN(o,x)               x##( SHOW_WIN,             ShowWindow,         (o *, bool *, void *) )
#define pick_ACTION_GET_RESIZER(o,x)            x##( GET_RESIZER,          GetResizer,         (o *, WdeResizeRatio *, OBJPTR *) )
#define pick_ACTION_GET_FONT(o,x)               x##( GET_FONT,             GetFont,            (o *, HFONT *, void *) )
#define pick_ACTION_SET_FONT(o,x)               x##( SET_FONT,             SetFont,            (o *, HFONT *, WdeResizeRatio *) )
#define pick_ACTION_BECOME_FIRST_CHILD(o,x)     x##( BECOME_FIRST_CHILD,   FirstChild,         (o *, void *, void *) )
#define pick_ACTION_PUT_ME_FIRST(o,x)           x##( PUT_ME_FIRST,         PutChildFirst,      (o *, OBJPTR, void *) )
#define pick_ACTION_GET_FIRST_CHILD(o,x)        x##( GET_FIRST_CHILD,      GetFirstChild,      (o *, OBJPTR *, void *) )
#define pick_ACTION_GET_NC_SIZE(o,x)            x##( GET_NC_SIZE,          GetNCSize,          (o *, RECT *, void *) )
#define pick_ACTION_GET_RES_SIZE(o,x)           x##( GET_RES_SIZE,         GetResSize,         (o *, void *, void *) )
#define pick_ACTION_SAVE_OBJECT(o,x)            x##( SAVE_OBJECT,          SaveObject,         (o *, WORD *, void *) )
#define pick_ACTION_TEST(o,x)                   x##( TEST,                 Test,               (o *, TEMPLATE_HANDLE *, size_t *) )
#define pick_ACTION_TESTEX(o,x)                 x##( TESTEX,               TestEX,             (o *, TEMPLATE_HANDLE *, size_t *) )
#define pick_ACTION_ON_TOP(o,x)                 x##( ON_TOP,               OnTop,              (o *, void *, void *) )
#define pick_ACTION_IDENTIFY(o,x)               x##( IDENTIFY,             Identify,           (o *, OBJ_ID *, void *) )
#define pick_ACTION_SET_CLEAR_INT(o,x)          x##( SET_CLEAR_INT,        SetClearInt,        (o *, bool *, void *) )
#define pick_ACTION_IS_OBJECT_CLEAR(o,x)        x##( IS_OBJECT_CLEAR,      GetClearInt,        (o *, bool *, void *) )
#define pick_ACTION_RESTORE_OBJECT(o,x)         x##( RESTORE_OBJECT,       Restore,            (o *, void *, void *) )
#define pick_ACTION_RESOLVE_SYMBOL(o,x)         x##( RESOLVE_SYMBOL,       ResolveSymbol,      (o *, bool *, bool *) )
#define pick_ACTION_RESOLVE_HELPSYMBOL(o,x)     x##( RESOLVE_HELPSYMBOL,   ResolveHelpSymbol,  (o *, bool *, bool *) )
#define pick_ACTION_MODIFY_INFO(o,x)            x##( MODIFY_INFO,          ModifyInfo,         (o *, WdeInfoStruct *, void *) )
#define pick_ACTION_GET_NEXT_CHILD(o,x)         x##( GET_NEXT_CHILD,       GetNextChild,       (o *, OBJPTR *, bool *) )
#define pick_ACTION_SET_ORDER_MODE(o,x)         x##( SET_ORDER_MODE,       SetOrderMode,       (o *, WdeOrderMode *, WdeSetOrderLists **) )
#define pick_ACTION_GET_ORDER_MODE(o,x)         x##( GET_ORDER_MODE,       GetOrderMode,       (o *, WdeOrderMode *, WdeSetOrderLists *) )
#define pick_ACTION_SIZE_TO_TEXT(o,x)           x##( SIZE_TO_TEXT,         SizeToText,         (o *, void *, void *) )
#define pick_ACTION_GET_WND_PROC(o,x)           x##( GET_WND_PROC,         GetWndProc,         (o *, WNDPROC *, void *) )
#define pick_ACTION_PICK(o,x)                   x##( PICK,                 Pick,               (o *, void *, void *) )

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
enum {
#define pick(e,n,a)     e
    pick_ACTION_GET_WINDOW_HANDLE(void,pick) = FIRST_USER_ACTION_ID,
    pick_ACTION_GET_WINDOW_CLASS(void,pick),
    pick_ACTION_GET_SUBOBJ_LIST(void,pick),
    pick_ACTION_GET_OBJECT_INFO(void,pick),
    pick_ACTION_SET_OBJECT_INFO(void,pick),
    pick_ACTION_GET_OBJECT_HELPINFO(void,pick),
    pick_ACTION_SET_OBJECT_HELPINFO(void,pick),
    pick_ACTION_CREATE_WINDOW(void,pick),
    pick_ACTION_DESTROY_WINDOW(void,pick),
    pick_ACTION_SHOW_WIN(void,pick),
    pick_ACTION_GET_RESIZER(void,pick),
    pick_ACTION_GET_FONT(void,pick),
    pick_ACTION_SET_FONT(void,pick),
    pick_ACTION_BECOME_FIRST_CHILD(void,pick),
    pick_ACTION_PUT_ME_FIRST(void,pick),
    pick_ACTION_GET_FIRST_CHILD(void,pick),
    pick_ACTION_GET_NC_SIZE(void,pick),
    pick_ACTION_GET_RES_SIZE(void,pick),
    pick_ACTION_SAVE_OBJECT(void,pick),
    pick_ACTION_TEST(void,pick),
    pick_ACTION_TESTEX(void,pick),
    pick_ACTION_ON_TOP(void,pick),
    pick_ACTION_IDENTIFY(void,pick),
    pick_ACTION_SET_CLEAR_INT(void,pick),
    pick_ACTION_IS_OBJECT_CLEAR(void,pick),
    pick_ACTION_RESTORE_OBJECT(void,pick),
    pick_ACTION_RESOLVE_SYMBOL(void,pick),
    pick_ACTION_RESOLVE_HELPSYMBOL(void,pick),
    pick_ACTION_MODIFY_INFO(void,pick),
    pick_ACTION_GET_NEXT_CHILD(void,pick),
    pick_ACTION_SET_ORDER_MODE(void,pick),
    pick_ACTION_GET_ORDER_MODE(void,pick),
    pick_ACTION_SIZE_TO_TEXT(void,pick),
    pick_ACTION_GET_WND_PROC(void,pick),
    pick_ACTION_PICK(void,pick)
#undef pick
};

enum {
    SET_LOCATION = FIRST_USER_NOTE_ID
};

#endif
