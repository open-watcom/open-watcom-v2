/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2026      The Open Watcom Contributors. All Rights Reserved.
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


#ifndef WDE_WRES_INCLUDED
#define WDE_WRES_INCLUDED

#include "wresall.h"
#include "wderesin.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* function prototypes                                                      */
/****************************************************************************/
extern uint_16                  WdeGetClassOrdinalFromName( const char * );
extern WdeDialogBoxHeader       *WdeAllocDialogBoxHeader( void );
extern void                     WdeFreeDialogBoxHeader( WdeDialogBoxHeader ** );
extern WdeDialogBoxControl      *WdeAllocDialogBoxControl( void );
extern void                     WdeFreeDialogBoxControl( WdeDialogBoxControl ** );
extern WdeDialogBoxHeader       *WdeCopyDialogBoxHeader( const WdeDialogBoxHeader * );
extern WdeDialogBoxControl      *WdeCopyDialogBoxControl( const WdeDialogBoxControl * );
extern ResNameOrOrdinal         *WdeCopyResNameOrOrdinal( const ResNameOrOrdinal * );
extern WResID                   *WdeCopyWResID( const WResID * );
extern char                     *WdeControlClassToStr( const ResNameOrOrdinal * );
extern ResNameOrOrdinal         *WdeStrToControlClass( const char *str );
extern char                     *WdeResNameOrOrdinalToStr( const ResNameOrOrdinal *, int );
extern ResNameOrOrdinal         *WdeStrToResNameOrOrdinal( const char *str );
extern OBJ_ID                   WdeGetCommonControlClassFromClassName( const char *class_name );
extern WResHelpID               *WdeCopyWResHelpID( const WResHelpID *src );

#endif
