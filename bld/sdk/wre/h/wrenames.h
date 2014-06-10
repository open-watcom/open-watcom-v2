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


#ifndef WRENAMES_INCLUDED
#define WRENAMES_INCLUDED

#include "wreresin.h"
#include "wresall.h"

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef struct {
    uint_16     type;
    DWORD       name;
    bool        exclude;
    char        *typeName;
} WRETypeName;

/****************************************************************************/
/* function prototypes                                                      */
/****************************************************************************/
extern bool         WREInitResourceWindow( WREResInfo *, uint_16 );
extern bool         WRESetResNamesFromType( WREResInfo *, uint_16, bool, WResID *, int );
extern WRETypeName  *WREGetTypeNameFromRT( uint_16 );
extern char         *WREGetResName( WResResNode *, uint_16 );

extern bool WREAddResNames( WREResInfo *info );

extern void WRESetTotalText( WREResInfo *info );

extern void WREInitTypeNames( void );
extern void WREFiniTypeNames( void );

extern void WREFiniTotalText( void );
extern bool WREInitTotalText( void );

#endif
