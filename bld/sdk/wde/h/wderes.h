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


#ifndef WDERES_INCLUDED
#define WDERES_INCLUDED

#include "wderesin.h"
#include "fmedit.def"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* function prototypes                                                      */
/****************************************************************************/
extern  Bool    WdeInitResStrings       ( void );
extern  void    WdeFiniResStrings       ( void );
extern  char    *WdeSelectSaveFilter    ( WRFileType ftype );

extern Bool        WdeDestroyResourceWindow ( WdeResInfo * );
extern void        WdeShowResourceWindows   ( int );
extern Bool        WdeRegisterResClass      ( HINSTANCE );
extern Bool        WdeQueryKillApp          ( Bool );
extern void        WdeFreeResList           ( void );
extern WdeResInfo *WdeCreateNewResource     ( char * );
extern Bool        WdeSaveResource          ( WdeResInfo *, Bool );
extern Bool        WdeOpenResource          ( char * );
extern int         WdeGetNumRes             ( void );
extern WdeResInfo *WdeGetCurrentRes         ( void );
extern OBJPTR      WdeGetCurrentDialog      ( void );
extern void        WdeAddResDlgItemToResInfo( WdeResInfo *, WdeResDlgItem *);
extern Bool        WdeSetObjectInfo         ( OBJPTR, WdeResInfo **,
                                              WdeResDlgItem **, WResID *);
extern Bool        WdeRemoveDialogFromResInfo ( WdeResInfo *, WdeResDlgItem *,
                                                Bool );

#endif
