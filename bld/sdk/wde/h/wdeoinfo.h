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


#ifndef WDEOINFO_INCLUDED
#define WDEOINFO_INCLUDED

#include "wresall.h"
#include "wdehash.h"
#include "wdefordr.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef void (*WdeSetProc)( void *, HWND );
typedef void (*WdeGetProc)( void *, HWND );
typedef BOOL (*WdeHookProc)( HWND, UINT, WPARAM, LPARAM, DialogStyle );

typedef struct WdeDefineObjectInfoStruct {
    OBJPTR              obj;
    OBJ_ID              obj_id;
    WdeResInfo          *res_info;
    char                *symbol;
    char                *helpsymbol;
    DialogStyle         mask;
    HWND                win;
    union {
        struct {
            WdeDialogBoxControl *info;
        } c;
        struct {
            WdeDialogBoxHeader  *header;
            WResID              *name;
            uint_16             id;
            bool                use_id;
        } d;
    } info;
    WdeSetProc          set_func;
    WdeGetProc          get_func;
    WdeHookProc         hook_func;
} WdeDefineObjectInfo;

#endif
