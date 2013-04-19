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


#ifndef WRBITMAP_INCLUDED
#define WRBITMAP_INCLUDED

#include "bitmap.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* function prototypes                                                      */
/****************************************************************************/
WRDLLENTRY extern void     WRAPI WRForgetBitmapName( void );
WRDLLENTRY extern void     WRAPI WRRememberBitmapName( WResID *name );
WRDLLENTRY extern WResID * WRAPI WRRecallBitmapName( void );
WRDLLENTRY extern int      WRAPI WRAddBitmapFileHeader( BYTE **data, uint_32 *size );
WRDLLENTRY extern int      WRAPI WRStripBitmapFileHeader( BYTE **data, uint_32 *size );
WRDLLENTRY extern HBITMAP  WRAPI WRBitmapFromData( BYTE *data, bitmap_info *info );
WRDLLENTRY extern int      WRAPI WRWriteBitmapToData( HBITMAP hbitmap, BYTE **data, uint_32 *size );

#endif
