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


#ifndef WRMAIN_INCLUDED
#define WRMAIN_INCLUDED

#include "wrinfo.h"

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef struct WRSaveIntoData {
    WRInfo                      *info;
    WResID                      *type;
    WResID                      *name;
    void                        *data;
    WResLangType                lang;
    uint_32                     size;
    uint_16                     MemFlags;
    struct WRSaveIntoData       *next;
} WRSaveIntoData;

/****************************************************************************/
/* function prototypes                                                      */
/****************************************************************************/
extern void     WR_EXPORT WRInit           ( void );
extern void     WR_EXPORT WRFini           ( void );
extern WRInfo * WR_EXPORT WRLoadResource   ( const char *, WRFileType );
extern int      WR_EXPORT WRUpdateTmp      ( WRInfo *info );
extern int      WR_EXPORT WRSaveResource   ( WRInfo *, int );
extern int      WR_EXPORT WRSaveObjectAs   ( const char *, WRFileType,
                                             WRSaveIntoData * );
extern int      WR_EXPORT WRSaveObjectInto ( const char *, WRSaveIntoData *,
                                             int * );
extern int      WR_EXPORT WRFindAndSetData( WResDir dir, WResID *type, WResID *name,
                                            WResLangType *lang, void *data );
extern WResLangNode * WR_EXPORT WRFindLangNode( WResDir dir, WResID *type,
                                       WResID *name, WResLangType *lang );

#endif
