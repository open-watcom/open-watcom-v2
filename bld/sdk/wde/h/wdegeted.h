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


#ifndef WDEGETED_INCLUDED
#define WDEGETED_INCLUDED

#include "wresall.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* function prototypes                                                      */
/****************************************************************************/
extern ResNameOrOrdinal *WdeGetResNameOrFromEdit( HWND, int, Bool *);
extern WResID           *WdeGetWResIDFromEdit( HWND, int, Bool *);
extern void             WdeSetEditWithWResID( WResID *, HWND, int );
extern void             WdeSetEditWithResNameOr( ResNameOrOrdinal *, HWND, int );
extern void             WdeSetEditWithControlClass( ControlClass *, HWND, int );
extern ControlClass     *WdeGetControlClassFromEdit( HWND, int, Bool * );
extern void             WdeSetComboWithStr( char *, HWND, int );
extern void             WdeSetEditWithStr( char *, HWND, int );
extern void             WdeSetEditWithUINT32( uint_32, int, HWND, int );
extern void             WdeSetEditWithSINT32( int_32, int, HWND, int );
extern void             WdeSetEditWithUINT16( uint_16, int, HWND, int );
extern void             WdeSetEditWithSINT16( int_16, int, HWND, int );
extern int_32           WdeGetSINT32FromEdit( HWND, int, Bool * );
extern uint_32          WdeGetUINT32FromEdit( HWND, int, Bool * );
extern int_16           WdeGetSINT16FromEdit( HWND, int, Bool * );
extern uint_16          WdeGetUINT16FromEdit( HWND, int, Bool * );
extern char             *WdeGetStrFromEdit( HWND, int, Bool * );
extern char             *WdeGetStrFromCombo( HWND, int );
extern uint_32          WdeGetUINT32FromCombo( HWND, int );
extern char             *WdeGetStrFromListBox( HWND, int, int );
extern char             *WdeGetStrFromComboLBox( HWND, int, int );

#endif
