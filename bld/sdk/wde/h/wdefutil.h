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


#ifndef WDEFUTIL_INCLUDED
#define WDEFUTIL_INCLUDED

#include "wresall.h"
#include "wdeobjid.h"
#include "wdeoinfo.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* function prototypes                                                      */
/****************************************************************************/
extern UINT     WdeTranslateNCMouseMsg( UINT );
extern void     WdeSnapPointToGrid( POINT *pt );
extern OBJPTR   WdeGetNextObject( bool, OBJPTR, OBJPTR );
extern OBJPTR   WdeCloneObject( OBJPTR, POINT * );
extern void     WdeFreeControlIDs( void );
extern uint_16  WdeGetNextControlID( void );
extern uint_32  WdeHammingDistance( uint_32, uint_32 );
extern bool     WdeIsClassDefined( char * );
extern char     *WdeRectToStr( RECT * );
extern void     WdeStrToRect( char *, RECT * );
extern void     WdeShowObjectWindow( HWND, bool );
extern void     WdeBringWindowToTop( HWND );
extern bool     WdePutObjFirst( OBJPTR, LIST **);
extern bool     WdeFindObjectsAtPt( POINT *, LIST **, LIST * );
extern bool     WdeFindSubObjects( SUBOBJ_REQUEST *, LIST **, LIST * );
extern void     WdeDisableChildWindows( HWND );
extern bool     WdeProcessMouse( HWND, UINT, WPARAM, LPARAM );
extern BOOL     WdeControlDefine( WdeDefineObjectInfo * );
extern bool     WdeProcessSymbolCombo( HWND, UINT, WPARAM, LPARAM, WdeHashTable *, uint_16, bool );
extern bool     WdeProcessHelpSymbolCombo( HWND, UINT, WPARAM, LPARAM, WdeHashTable *, uint_32, bool );
extern void     WdeSetDefineObjectSymbolInfo( WdeDefineObjectInfo *, HWND );
extern void     WdeGetDefineObjectSymbolInfo( WdeDefineObjectInfo *, HWND );
extern void     WdeGetDefineObjectHelpSymbolInfo( WdeDefineObjectInfo *, HWND );
extern void     WdeMapWindowRect( HWND, HWND, RECT * );
extern bool     WdeIsStrSpace( char * );
extern BOOL     WdeWinStylesHook( HWND, UINT, WPARAM, LPARAM, DialogStyle );
extern void     WdeAddSymbolToObjectHashTable( WdeResInfo *, char *, WdeHashValue );

extern void     WdeEXSetDefineInfo( WdeDefineObjectInfo *, HWND );
extern void     WdeEXGetDefineInfo( WdeDefineObjectInfo *, HWND );

#endif
