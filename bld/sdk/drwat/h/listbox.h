/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2016 The Open Watcom Contributors. All Rights Reserved.
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


/*
 * LBoxHdl type is private
 */
typedef struct {
    UINT                ypos;
    UINT                longest_item;
    UINT                text_width;
    UINT                line_cnt;
    HWND                hwnd;
} LBoxHdl;

extern int      LBPrintf( LBoxHdl *lb, DWORD msgid, ... );
extern int      LBStrPrintf( LBoxHdl *lb, char *str, ... );
extern void     SetListBoxFont( LBoxHdl *lb );
extern void     ClearListBox( LBoxHdl *lb );
extern void     MoveListBox( LBoxHdl *lb, int x, int y, int width, int hieght );
extern LBoxHdl  *CreateListBox( HWND parent );
extern HWND     GetListBoxHwnd( LBoxHdl *lb );
extern void     FiniListBox( LBoxHdl *lb );
