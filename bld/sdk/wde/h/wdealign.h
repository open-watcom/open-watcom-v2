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


#ifndef WDEALIGN_INCLUDED
#define WDEALIGN_INCLUDED

#include "fmedit.def"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define WDE_ALIGN_LEFT      0x01
#define WDE_ALIGN_RIGHT     0x02
#define WDE_ALIGN_TOP       0x04
#define WDE_ALIGN_BOTTOM    0x08
#define WDE_ALIGN_HCENTRE   0x10
#define WDE_ALIGN_VCENTRE   0x20

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef uint_8 WdeAlignValue;

/****************************************************************************/
/* function prototypes                                                      */
/****************************************************************************/
extern Bool WdeAlignControls             ( WdeAlignValue );
extern Bool WdeSameSize                  ( RESIZE_ID );
extern Bool WdeGetBoundingRectFromSelect ( RECT * );
extern Bool WdeGetBoundingRectFromList   ( LIST *, RECT * );

#endif
