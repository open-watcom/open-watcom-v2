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
extern OBJPTR   WdeGetNextObject( Bool, OBJPTR, OBJPTR );
extern OBJPTR   WdeCloneObject( OBJPTR, POINT * );
extern void     WdeFreeControlIDs( void );
extern uint_16  WdeGetNextControlID( void );
extern uint_32  WdeHammingDistance( uint_32, uint_32 );
extern Bool     WdeIsClassDefined( char * );
extern char     *WdeRectToStr( RECT * );
extern void     WdeStrToRect( char *, RECT * );
extern void     WdeShowObjectWindow( HWND, Bool );
extern void     WdeBringWindowToTop( HWND );
extern Bool     WdePutObjFirst( OBJPTR, LIST **);
extern Bool     WdeFindObjectsAtPt( POINT *, LIST **, LIST * );
extern Bool     WdeFindSubObjects( SUBOBJ_REQUEST *, LIST **, LIST * );
extern void     WdeDisableChildWindows( HWND );
extern Bool     WdeProcessMouse( HWND, UINT, WPARAM, LPARAM );
extern BOOL     WdeControlDefine( WdeDefineObjectInfo * );
extern Bool     WdeProcessSymbolCombo( HWND, UINT, WPARAM, LPARAM, WdeHashTable *, uint_16, Bool );
extern Bool     WdeProcessHelpSymbolCombo( HWND, UINT, WPARAM, LPARAM, WdeHashTable *, uint_32, Bool );
extern void     WdeSetDefineObjectSymbolInfo( WdeDefineObjectInfo *, HWND );
extern void     WdeGetDefineObjectSymbolInfo( WdeDefineObjectInfo *, HWND );
extern void     WdeGetDefineObjectHelpSymbolInfo( WdeDefineObjectInfo *, HWND );
extern void     WdeMapWindowRect( HWND, HWND, RECT * );
extern Bool     WdeIsStrSpace( char * );
extern Bool     WdeWinStylesHook( HWND, UINT, WPARAM, LPARAM, DialogStyle );
extern void     WdeAddSymbolToObjectHashTable( WdeResInfo *, char *, WdeHashValue );

extern void     WdeEXSetDefineInfo( WdeDefineObjectInfo *, HWND );
extern void     WdeEXGetDefineInfo( WdeDefineObjectInfo *, HWND );

#endif
