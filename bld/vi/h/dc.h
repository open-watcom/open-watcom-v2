/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2016 The Open Watcom Contributors. All Rights Reserved.
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


#ifndef _DC_INCLUDED
#define _DC_INCLUDED
#include "sstyle.h"

#define MIN_CACHE_LINE_LENGTH    100

extern void     DCCreate( void );
extern void     DCResize( info * );
extern void     DCScroll( int );
extern void     DCDestroy( void );
extern vi_rc    DCUpdate( void );
extern void     DCUpdateAll( void );
extern void     DCDisplaySomeLines( int, int );
extern void     DCDisplayAllLines( void );
extern void     DCInvalidateSomeLines( int, int );
extern void     DCInvalidateAllLines( void );
extern dc_line  *DCFindLine( int, window_id );
extern void     DCValidateLine( dc_line *, int, char * );

#endif
