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
extern uint_8                   WdeGetClassFromClassName( char *);
extern WdeDialogBoxHeader       *WdeAllocDialogBoxHeader( void );
extern void                     WdeFreeDialogBoxHeader( WdeDialogBoxHeader **);
extern WdeDialogBoxControl      *WdeAllocDialogBoxControl( void );
extern void                     WdeFreeDialogBoxControl( WdeDialogBoxControl **);
extern WdeDialogBoxHeader       *WdeCopyDialogBoxHeader( WdeDialogBoxHeader * );
extern WdeDialogBoxControl      *WdeCopyDialogBoxControl( WdeDialogBoxControl * );
extern ControlClass             *WdeCopyControlClass( ControlClass *);
extern ResNameOrOrdinal         *WdeCopyResNameOr( ResNameOrOrdinal *);
extern WResID                   *WdeCopyWResID( WResID *);
extern char                     *WdeControlClassToStr( ControlClass *);
extern ControlClass             *WdeStrToControlClass( char *);
extern char                     *WdeResNameOrOrdinalToStr( ResNameOrOrdinal *, int );
extern ResNameOrOrdinal         *WdeStrToResNameOrOrdinal( char *str );
extern OBJ_ID                   WdeGetCommonControlClassFromClassName( char *class_name );
extern WResHelpID               *WdeCopyWResHelpID( WResHelpID *src );

#endif
