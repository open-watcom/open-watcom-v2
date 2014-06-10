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


#ifndef WDEDEBUG_INCLUDED
#define WDEDEBUG_INCLUDED

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

#if 0
#define WDE_DEBUG
#define WDE_DEBUG_ON_STATUS
#define WDE_DEBUG_DISPATCH
#define WDE_DEBUG_CREATE
#endif

#include "wdeactn.h"

/****************************************************************************/
/* function prototypes                                                      */
/****************************************************************************/
#ifdef WDE_DEBUG

#ifdef WDE_DEBUG_DISPATCH
extern void WdeDebugDispatch( char *, ACTION, void *, void *, void * );
#else
#define WdeDebugDispatch( cp, a, obj, p1, p2 ) {}
#endif /* WDE_DEBUG_DISPATCH */

#ifdef WDE_DEBUG_CREATE
extern void WdeDebugCreate( char *, void *, RECT *, void * );
#else
#define WdeDebugCreate( cp, obj, rect, handle ) {}
#endif /* WDE_DEBUG_CREATE */

extern bool WdeWriteTrail( const char * );
extern bool WdeWriteTrailn( const char *, int );

#ifdef WDE_DEBUG_ON_STATUS
extern bool WdeWriteStatus( const char * );
#else
#define WdeWriteStatus( cp ) {}
#endif /* WDE_DEBUG_ON_STATUS */

#else

#define WdeDebugDispatch( cp, a, obj, p1, p2 )      {}
#define WdeDebugCreate( cp, obj, rect, handle  )    {}
#define WdeWriteTrail( cp )                         {}
#define WdeWriteTrailn( cp, i )                     {}
#define WdeWriteStatus( cp )                        {}

#endif /* WDE_DEBUG */

#endif /* WDEDEBUG_INCLUDED */
