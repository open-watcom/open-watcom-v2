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
* Description:  UI debugging support.
*
****************************************************************************/


/* This could be done much cleaner with C99 variable argument macros, but
 * we might need to build this code with compilers that don't support that.
 */

//#define UI_DEBUG
#if defined UI_DEBUG
extern void UIDebugPrintf( const char *f, ... );
#define UIDebugPrintf0( f )                 UIDebugPrintf( f )
#define UIDebugPrintf1( f, a )              UIDebugPrintf( f, a )
#define UIDebugPrintf2( f, a, b )           UIDebugPrintf( f, a, b )
#define UIDebugPrintf3( f, a, b, c )        UIDebugPrintf( f, a, b, c )
#define UIDebugPrintf4( f, a, b, c, d )     UIDebugPrintf( f, a, b, c, d )
#else
#define UIDebugPrintf0( f )
#define UIDebugPrintf1( f, a )
#define UIDebugPrintf2( f, a, b)
#define UIDebugPrintf3( f, a, b, c )
#define UIDebugPrintf4( f, a, b, c, d )

#define UIDebugPrintf DONT USE THIS DIRECTLY
#endif
