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


#ifndef WACCEL_INCLUDED
#define WACCEL_INCLUDED

#include "wresall.h"
#include "wrdll.h"
#include "wreseapi.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

#define ACCEL_I_HAVE_CLOSED  (WM_USER + 666 + 00 + 0)
#define ACCEL_PLEASE_SAVEME  (WM_USER + 666 + 00 + 1)
#define ACCEL_PLEASE_OPENME  (WM_USER + 666 + 00 + 2)

/****************************************************************************/
/* data types                                                               */
/****************************************************************************/
typedef int WAccelHandle;

typedef struct WAccelInfo {
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
} WAccelInfo;

/****************************************************************************/
/* function prototypes                                                      */
/****************************************************************************/
WRESEDLLENTRY extern WAccelInfo  *WRESEAPI WAccAllocAccelInfo( void );
WRESEDLLENTRY extern void        WRESEAPI WAccFreeAccelInfo( WAccelInfo * );

WRESEDLLENTRY extern void        WRESEAPI WAccelInit( void );
WRESEDLLENTRY extern void        WRESEAPI WAccelFini( void );
WRESEDLLENTRY extern int         WRESEAPI WAccelCloseSession( WAccelHandle, int );
WRESEDLLENTRY extern WAccelHandle WRESEAPI WAccelStartEdit( WAccelInfo * );
WRESEDLLENTRY extern WAccelInfo  *WRESEAPI WAccelEndEdit( WAccelHandle );
WRESEDLLENTRY extern WAccelInfo  *WRESEAPI WAccelGetEditInfo( WAccelHandle );
WRESEDLLENTRY extern int         WRESEAPI WAccelIsModified( WAccelHandle );
WRESEDLLENTRY extern int         WRESEAPI WAccelIsDlgMsg( MSG *msg );
WRESEDLLENTRY extern void        WRESEAPI WAccelShowWindow( WAccelHandle hndl, int show );
WRESEDLLENTRY extern void        WRESEAPI WAccelBringToFront( WAccelHandle hndl );

#endif
