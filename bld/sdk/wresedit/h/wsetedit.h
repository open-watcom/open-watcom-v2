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


#ifndef WSETEDIT_INCLUDED
#define WSETEDIT_INCLUDED

#include "wresall.h"

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* function prototypes                                                      */
/****************************************************************************/
extern char     *WConvertFrom( char *str, char from, char to );
extern char     *WConvertStringFrom( char *str, char *from, char *to );
extern char     *WConvertTo( char *str, char to, char from );
extern char     *WConvertStringTo( char *str, char *to, char *from );

extern bool     WSetEditWithSINT32( HWND, int_32, int );
extern bool     WSetEditWithWResID( HWND, WResID * );
extern bool     WSetEditWithStr( HWND, char * );
extern bool     WSetLBoxWithStr( HWND, char *, void * );
extern bool     WInsertLBoxWithStr( HWND, int, char *, void * );
extern bool     WSetLBoxWithWResID( HWND, WResID *, void * );

extern char     *WGetStrFromEdit( HWND, bool * );
extern WResID   *WGetWResIDFromEdit( HWND, bool * );
extern int_32   WGetSINT32FromEdit( HWND, bool * );

extern char     *WGetStrFromComboLBox( HWND combo, int index );

#endif
