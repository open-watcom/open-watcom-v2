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


#ifndef WRERESIN_INCLUDED
#define WRERESIN_INCLUDED

#include "wretoolb.h"
#include "wrdll.h"
#include "wrhash.h"

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef struct WREResInfoStruct {
    WRInfo      *info;
    WRHashTable *symbol_table;
    char        *symbol_file;
    HWND        res_win;
    HWND        info_win;
    uint_16     current_type;
    Bool        is32bit;
    Bool        modified;
    Bool        active;
    int         window_num;
} WREResInfo;

/****************************************************************************/
/* function prototypes                                                      */
/****************************************************************************/
extern void              WREFreeResInfo       ( WREResInfo *);
extern WREResInfo       *WREAllocResInfo      ( void );
extern Bool              WREIsResModified     ( WREResInfo * );
extern void              WRESetResModified    ( WREResInfo *, Bool );

#endif
