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


#ifndef WSTRING_INCLUDED
#define WSTRING_INCLUDED

#ifndef WINEXPORT
#define WINEXPORT   __export PASCAL
#endif

#include "wresall.h"
#include "wrdll.h"
#include "wrhash.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

#define STRING_I_HAVE_CLOSED  (WM_USER + 666 + 20 + 0)
#define STRING_PLEASE_SAVEME  (WM_USER + 666 + 20 + 1)
#define STRING_PLEASE_OPENME  (WM_USER + 666 + 20 + 2)

/****************************************************************************/
/* data types                                                               */
/****************************************************************************/
typedef int WStringHandle;

typedef struct WStringNode {
    WResID              *block_name;
    WResLangType        lang;
    int                 data_size;
    void                *data;
    uint_16             MemFlags;
    struct WStringNode  *next;
} WStringNode;

typedef struct WStringInfo {
    HWND                parent;
    HINSTANCE           inst;
    char                *file_name;
    WRHashTable         *symbol_table;
    char                *symbol_file;
    int                 is32bit;
    int                 edit_active;
    int                 modified;
    int                 stand_alone;
    WStringNode         *tables;
} WStringInfo;

/****************************************************************************/
/* function prototypes                                                      */
/****************************************************************************/
WStringInfo         * WINEXPORT WStrAllocStringInfo ( void );
void                  WINEXPORT WStrFreeStringInfo  ( WStringInfo * );

extern void           WINEXPORT WStringInit         ( void );
extern void           WINEXPORT WStringFini         ( void );
extern int            WINEXPORT WStringCloseSession ( WStringHandle, int );
extern WStringHandle  WINEXPORT WRStringStartEdit   ( WStringInfo * );
extern WStringInfo  * WINEXPORT WStringEndEdit      ( WStringHandle );
extern WStringInfo  * WINEXPORT WStringGetEditInfo  ( WStringHandle );
extern int            WINEXPORT WStringIsModified   ( WStringHandle );
extern int            WINEXPORT WStringIsDlgMsg     ( MSG *msg );
extern void           WINEXPORT WStringShowWindow   ( WStringHandle hndl, int show );
extern void           WINEXPORT WStringBringToFront ( WStringHandle hndl );

#endif
