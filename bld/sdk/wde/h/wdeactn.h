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

#define pick_ACT_GET_WINDOW_HANDLE(o)      pick( GET_WINDOW_HANDLE,    GetWindowHandle,    (o *, HWND *, void *) )
#define pick_ACT_GET_WINDOW_CLASS(o)       pick( GET_WINDOW_CLASS,     GetWindowClass,     (o *, char **, void *) )
#define pick_ACT_GET_SUBOBJ_LIST(o)        pick( GET_SUBOBJ_LIST,      GetSubObjectList,   (o *, LIST **, void *) )
#define pick_ACT_GET_OBJECT_INFO(o)        pick( GET_OBJECT_INFO,      GetObjectInfo,      (o *, void **, void **) )
#define pick_ACT_SET_OBJECT_INFO(o)        pick( SET_OBJECT_INFO,      SetObjectInfo,      (o *, void *, void *) )
#define pick_ACT_GET_OBJECT_HELPINFO(o)    pick( GET_OBJECT_HELPINFO,  GetObjectHelpInfo,  (o *, void **, char **) )
#define pick_ACT_SET_OBJECT_HELPINFO(o)    pick( SET_OBJECT_HELPINFO,  SetObjectHelpInfo,  (o *, void *, char *) )
#define pick_ACT_CREATE_WINDOW(o)          pick( CREATE_WINDOW,        CreateWindow,       (o *, bool *, void *) )
#define pick_ACT_DESTROY_WINDOW(o)         pick( DESTROY_WINDOW,       DestroyWindow,      (o *, bool *, bool *) )
#define pick_ACT_SHOW_WIN(o)               pick( SHOW_WIN,             ShowWindow,         (o *, bool *, void *) )
#define pick_ACT_GET_RESIZER(o)            pick( GET_RESIZER,          GetResizer,         (o *, WdeResizeRatio *, OBJPTR *) )
#define pick_ACT_GET_FONT(o)               pick( GET_FONT,             GetFont,            (o *, HFONT *, void *) )
#define pick_ACT_SET_FONT(o)               pick( SET_FONT,             SetFont,            (o *, HFONT *, WdeResizeRatio *) )
#define pick_ACT_BECOME_FIRST_CHILD(o)     pick( BECOME_FIRST_CHILD,   FirstChild,         (o *, void *, void *) )
#define pick_ACT_PUT_ME_FIRST(o)           pick( PUT_ME_FIRST,         PutChildFirst,      (o *, void *, void *) )
#define pick_ACT_GET_FIRST_CHILD(o)        pick( GET_FIRST_CHILD,      GetFirstChild,      (o *, OBJPTR *, void *) )
#define pick_ACT_GET_NC_SIZE(o)            pick( GET_NC_SIZE,          GetNCSize,          (o *, RECT *, void *) )
#define pick_ACT_GET_RES_SIZE(o)           pick( GET_RES_SIZE,         GetResSize,         (o *, void *, void *) )
#define pick_ACT_SAVE_OBJECT(o)            pick( SAVE_OBJECT,          SaveObject,         (o *, WORD *, void *) )
#define pick_ACT_TEST(o)                   pick( TEST,                 Test,               (o *, GLOBALHANDLE *, void *) )
#define pick_ACT_TESTEX(o)                 pick( TESTEX,               TestEX,             (o *, GLOBALHANDLE *, void *) )
#define pick_ACT_ON_TOP(o)                 pick( ON_TOP,               OnTop,              (o *, void *, void *) )
#define pick_ACT_IDENTIFY(o)               pick( IDENTIFY,             Identify,           (o *, OBJ_ID *, void *) )
#define pick_ACT_SET_CLEAR_INT(o)          pick( SET_CLEAR_INT,        SetClearInt,        (o *, BOOL *, void *) )
#define pick_ACT_IS_OBJECT_CLEAR(o)        pick( IS_OBJECT_CLEAR,      GetClearInt,        (o *, BOOL *, void *) )
#define pick_ACT_RESTORE_OBJECT(o)         pick( RESTORE_OBJECT,       Restore,            (o *, void *, void *) )
#define pick_ACT_RESOLVE_SYMBOL(o)         pick( RESOLVE_SYMBOL,       ResolveSymbol,      (o *, bool *, bool *) )
#define pick_ACT_RESOLVE_HELPSYMBOL(o)     pick( RESOLVE_HELPSYMBOL,   ResolveHelpSymbol,  (o *, bool *, bool *) )
#define pick_ACT_MODIFY_INFO(o)            pick( MODIFY_INFO,          ModifyInfo,         (o *, WdeInfoStruct *, void *) )
#define pick_ACT_GET_NEXT_CHILD(o)         pick( GET_NEXT_CHILD,       GetNextChild,       (o *, OBJPTR *, bool *) )
#define pick_ACT_SET_ORDER_MODE(o)         pick( SET_ORDER_MODE,       SetOrderMode,       (o *, WdeOrderMode *, WdeSetOrderLists **) )
#define pick_ACT_GET_ORDER_MODE(o)         pick( GET_ORDER_MODE,       GetOrderMode,       (o *, WdeOrderMode *, WdeSetOrderLists *) )
#define pick_ACT_SIZE_TO_TEXT(o)           pick( SIZE_TO_TEXT,         SizeToText,         (o *, void *, void *) )
#define pick_ACT_GET_WND_PROC(o)           pick( GET_WND_PROC,         GetWndProc,         (o *, WNDPROC *, void *) )
#define pick_ACT_PICK(o)                   pick( PICK,                 Pick,               (o *, void *, void *) )

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
enum {
#define pick(e,n,a)     e
    pick_ACT_GET_WINDOW_HANDLE( void ) = APP_ACTIONS,
    pick_ACT_GET_WINDOW_CLASS( void ),
    pick_ACT_GET_SUBOBJ_LIST( void ),
    pick_ACT_GET_OBJECT_INFO( void ),
    pick_ACT_SET_OBJECT_INFO( void ),
    pick_ACT_GET_OBJECT_HELPINFO( void ),
    pick_ACT_SET_OBJECT_HELPINFO( void ),
    pick_ACT_CREATE_WINDOW( void ),
    pick_ACT_DESTROY_WINDOW( void ),
    pick_ACT_SHOW_WIN( void ),
    pick_ACT_GET_RESIZER( void ),
    pick_ACT_GET_FONT( void ),
    pick_ACT_SET_FONT( void ),
    pick_ACT_BECOME_FIRST_CHILD( void ),
    pick_ACT_PUT_ME_FIRST( void ),
    pick_ACT_GET_FIRST_CHILD( void ),
    pick_ACT_GET_NC_SIZE( void ),
    pick_ACT_GET_RES_SIZE( void ),
    pick_ACT_SAVE_OBJECT( void ),
    pick_ACT_TEST( void ),
    pick_ACT_TESTEX( void ),
    pick_ACT_ON_TOP( void ),
    pick_ACT_IDENTIFY( void ),
    pick_ACT_SET_CLEAR_INT( void ),
    pick_ACT_IS_OBJECT_CLEAR( void ),
    pick_ACT_RESTORE_OBJECT( void ),
    pick_ACT_RESOLVE_SYMBOL( void ),
    pick_ACT_RESOLVE_HELPSYMBOL( void ),
    pick_ACT_MODIFY_INFO( void ),
    pick_ACT_GET_NEXT_CHILD( void ),
    pick_ACT_SET_ORDER_MODE( void ),
    pick_ACT_GET_ORDER_MODE( void ),
    pick_ACT_SIZE_TO_TEXT( void ),
    pick_ACT_GET_WND_PROC( void ),
    pick_ACT_PICK( void )
#undef pick
};

enum {
    SET_LOCATION = APP_NOTE_IDS
};

#endif
