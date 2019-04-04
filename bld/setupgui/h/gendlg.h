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


/*
*   COMMENTS: Function prototypes contolling the abstract type "a_dialog_header"
*/


extern a_dialog_header  *AddNewDialog       ( const char * );
extern a_dialog_header  *GetNextDialog      ( void );
extern a_dialog_header  *FindDialogByName   ( const char * );
extern bool             CheckDialog         ( const char * );
extern dlg_state        DoDialog            ( const char * );
extern dlg_state        DoDialogWithParent  ( gui_window *, const char * );
extern dlg_state        DoDialogByPointer   ( gui_window *, a_dialog_header * );
extern void             InitDefaultDialogs  ( void );
extern void             FreeDefaultDialogs  ( void );

extern  dlg_state       GenericDialog       ( gui_window *, a_dialog_header * );
extern  dlg_state       IdToDlgState        ( gui_ctl_id );
