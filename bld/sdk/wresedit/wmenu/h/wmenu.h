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

#ifndef WINEXPORT
    #define WINEXPORT   __export PASCAL
#endif

#include "wresall.h"
#include "wrdll.h"
#include "wrhash.h"

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
    int                 is32bit;
    int                 data_size;
    void                *data;
    int                 edit_active;
    int                 modified;
    int                 stand_alone;
} WMenuInfo;

/****************************************************************************/
/* function prototypes                                                      */
/****************************************************************************/
WMenuInfo *         WINEXPORT WMenuAllocMenuInfo( void );
void                WINEXPORT WMenuFreeMenuInfo( WMenuInfo * );

extern void         WINEXPORT WMenuInit( void );
extern void         WINEXPORT WMenuFini( void );
extern int          WINEXPORT WMenuCloseSession( WMenuHandle, int );
extern WMenuHandle  WINEXPORT WRMenuStartEdit( WMenuInfo * );
extern WMenuInfo *  WINEXPORT WMenuEndEdit( WMenuHandle );
extern WMenuInfo *  WINEXPORT WMenuGetEditInfo( WMenuHandle );
extern int          WINEXPORT WMenuIsModified( WMenuHandle );
extern int          WINEXPORT WMenuIsDlgMsg( MSG *msg );
extern void         WINEXPORT WMenuShowWindow( WMenuHandle hndl, int show );
extern void         WINEXPORT WMenuBringToFront( WMenuHandle hndl );

#endif
