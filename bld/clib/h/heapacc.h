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


#ifndef _HEAPACC_H_INCLUDED
#define _HEAPACC_H_INCLUDED
/* macros for serialization of accesses to the heap */
#if defined(__SW_BM)
    #if defined(__386__) || defined(__AXP__) || defined(__PPC__)
        extern void (*_AccessFHeap)( void );
        extern void (*_ReleaseFHeap)( void );
        extern void (*_AccessNHeap)( void );
        extern void (*_ReleaseNHeap)( void );
    #else
        extern void __AccessFHeap( void );
        extern void __ReleaseFHeap( void );
        extern void __AccessNHeap( void );
        extern void __ReleaseNHeap( void );
        #define _AccessFHeap()  __AccessFHeap()
        #define _ReleaseFHeap() __ReleaseFHeap()
        #define _AccessNHeap()  __AccessNHeap()
        #define _ReleaseNHeap() __ReleaseNHeap()
    #endif
#else
    #define _AccessFHeap()
    #define _ReleaseFHeap()
    #define _AccessNHeap()
    #define _ReleaseNHeap()
#endif
#endif

