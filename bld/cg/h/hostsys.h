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


#ifndef _HOST_INCLUDED

/*  host processors (_HOST) */
#define   _IAPX86   1
#define   _VAX      2
#define   _IBM_370  4
#define   _80386    8

/*  host operating systems (_OS) */
#define   _DOS    1
#define   _OSTWO  2
#define   _VMS    4
#define   _CMS    8
#define   _PLDT   16
#define   _QNX    32
#define   _WIN386 64
#define   _NT     128
#define   _OS220  256
#define   _OSI    512

/*  host character sets (_CSET) */
#define   _ASCII    1
#define   _EBCDIC   2

/*  host word ordering (_MEMORY) */
#define   _LOW_FIRST       1
#define   _HIGH_FIRST      2

/*  memory tracking levels */
#define   _NO_TRACKING     4
#define   _CHUNK_TRACKING  8
#define   _FULL_TRACKING  16

#ifdef _TRACK
    #define _TRACKING | _FULL_TRACKING
#else
    #define _TRACKING | _CHUNK_TRACKING
#endif

#ifdef _HOSTOS
    #if (_HOSTOS & _QNX) && defined(__386__)
        #define _HOST _80386
        #define _HOST_CSET _ASCII
        #define _HOST_INTEGER 32
        #define _HOST_MEMORY  ( _LOW_FIRST _TRACKING )
    #elif _HOSTOS & ( _DOS | _OSTWO | _QNX )
        #define _HOST _IAPX86
        #define _HOST_CSET _ASCII
        #define _HOST_INTEGER 16
        #error Ouch - DOS or other 16-bit OS
        #define _HOST_MEMORY  ( _LOW_FIRST _TRACKING )
    #elif _HOSTOS & ( _OS220 | _PLDT | _WIN386 | _NT | _OSI )
        #define _HOST _80386
        #define _HOST_CSET _ASCII
        #define _HOST_INTEGER 32
        #define _HOST_MEMORY  ( _LOW_FIRST _TRACKING )
    #endif
#else
    #define _HOST_MEMORY ( _LOW_FIRST _TRACKING )
#endif

#define _HOST_INCLUDED

#endif
