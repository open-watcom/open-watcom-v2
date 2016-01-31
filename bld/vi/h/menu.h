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
* Description:  Menu processing related prototypes.
*
****************************************************************************/


#ifndef _MENU_INCLUDED
#define _MENU_INCLUDED

#define MAX_FLOAT_MENUS 4

/* menu.c */
extern void     BarfMenuData( FILE *f );
extern vi_rc    StartMenu( const char *data );
extern vi_rc    ViEndMenu( void );
extern vi_rc    MenuItem( const char *data );
extern vi_rc    MenuItemFileList( void );
extern vi_rc    MenuItemLastFiles( void );
extern vi_rc    AddMenuItem( const char *data );
extern vi_rc    DoItemDelete( const char *data );
extern vi_rc    DoMenuDelete( const char *data );
extern vi_rc    InitMenu( void );
extern void     FiniMenu( void );
extern vi_rc    DoMenu( void );
extern int      GetMenuIdFromCoord( int x );
extern int      GetCurrentMenuId( void );
extern vi_rc    SetToMenuId( int id );
extern vi_rc    DoFloatMenu( int id, int slen, windim x1, windim y1 );
extern vi_rc    ActivateFloatMenu( const char * );
extern vi_rc    DoWindowGadgetMenu( void );
extern bool     IsMenuHotKey( vi_key );

#endif
