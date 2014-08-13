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


#ifndef WMENU_INCLUDED
#define WMENU_INCLUDED

#include "wresall.h"
#include "wrdll.h"
#include "wreseapi.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

#define MENU_I_HAVE_CLOSED  (WM_USER + 666 + 10 + 0)
#define MENU_PLEASE_SAVEME  (WM_USER + 666 + 10 + 1)
#define MENU_PLEASE_OPENME  (WM_USER + 666 + 10 + 2)

/****************************************************************************/
/* data types                                                               */
/****************************************************************************/
typedef int WMenuHandle;

typedef struct WMenuInfo {
    HWND                parent;
    HINSTANCE           inst;
    char                *file_name;
    WRHashTable         *symbol_table;
    char                *symbol_file;
    WResID              *res_name;
    WResLangType        lang;
    uint_16             MemFlags;
    size_t              data_size;
    void                *data;
    bool                is32bit;
    bool                edit_active;
    bool                modified;
    bool                stand_alone;
} WMenuInfo;

/****************************************************************************/
/* function prototypes                                                      */
/****************************************************************************/
WRESEDLLENTRY WMenuInfo *         WRESEAPI WMenuAllocMenuInfo( void );
WRESEDLLENTRY void                WRESEAPI WMenuFreeMenuInfo( WMenuInfo * );

WRESEDLLENTRY extern void         WRESEAPI WMenuInit( void );
WRESEDLLENTRY extern void         WRESEAPI WMenuFini( void );
WRESEDLLENTRY extern int          WRESEAPI WMenuCloseSession( WMenuHandle, int );
WRESEDLLENTRY extern WMenuHandle  WRESEAPI WRMenuStartEdit( WMenuInfo * );
WRESEDLLENTRY extern WMenuInfo *  WRESEAPI WMenuEndEdit( WMenuHandle );
WRESEDLLENTRY extern WMenuInfo *  WRESEAPI WMenuGetEditInfo( WMenuHandle );
WRESEDLLENTRY extern int          WRESEAPI WMenuIsModified( WMenuHandle );
WRESEDLLENTRY extern int          WRESEAPI WMenuIsDlgMsg( MSG *msg );
WRESEDLLENTRY extern void         WRESEAPI WMenuShowWindow( WMenuHandle hndl, int show );
WRESEDLLENTRY extern void         WRESEAPI WMenuBringToFront( WMenuHandle hndl );

#endif
