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


#ifndef __CONTROL_INCLUDED__
#define __CONTROL_INCLUDED__

#ifdef __V__
#define __286__
#endif
#if defined(__OS2__) && defined(__386__)
#define __OS2V2__
#endif
#if defined(__WINDOWS__)
#define NOXMS
#define NOEMS
#define NOXTD
#define _FAR
#define __WIN__
#elif defined(__OS2__) || defined(__QNX__) || defined(__NT__)
#define NOXMS
#define NOEMS
#define NOXTD
#define _FAR
#ifndef __WIN__
#define __VIO__
#endif
#elif defined( __386__ )
#define NOXMS
#define NOEMS
#define NOXTD
#if !defined( __4G__ )
#define _FAR    __far
#else
#define _FAR
#endif
#elif defined(__286__)
//#define __IDE__
#define _FAR
#elif defined(__86__)
#define NOXMS
#define NOXTD
#define _FAR
#endif

#endif
