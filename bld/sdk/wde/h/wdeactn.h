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

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
enum {
    GET_WINDOW_HANDLE = APP_ACTIONS,
    GET_WINDOW_CLASS,
    GET_SUBOBJ_LIST,
    GET_OBJECT_INFO,
    SET_OBJECT_INFO,
    GET_OBJECT_HELPINFO,
    SET_OBJECT_HELPINFO,
    CREATE_WINDOW,
    DESTROY_WINDOW,
    SHOW_WIN,
    GET_RESIZER,
    GET_FONT,
    SET_FONT,
    BECOME_FIRST_CHILD,
    PUT_ME_FIRST,
    GET_FIRST_CHILD,
    GET_NC_SIZE,
    GET_RES_SIZE,
    SAVE_OBJECT,
    TEST,
    TESTEX,
    ON_TOP,
    IDENTIFY,
    SET_CLEAR_INT,
    IS_OBJECT_CLEAR,
    RESTORE_OBJECT,
    RESOLVE_SYMBOL,
    RESOLVE_HELPSYMBOL,
    MODIFY_INFO,
    GET_NEXT_CHILD,
    SET_ORDER_MODE,
    GET_ORDER_MODE,
    SIZE_TO_TEXT,
    GET_WND_PROC,
    PICK
};

enum {
    SET_LOCATION = APP_NOTE_IDS
};

#endif
