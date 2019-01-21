/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2009-2018 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Global WIPFC definition to cover non-OW environments
*
****************************************************************************/


/* !!! It must be included in each source file as first included header file !!! */


#if defined( __unix__ ) && !defined( __UNIX__ )
    #define __UNIX__ __unix__
#endif

#ifdef HAVE_CONFIG_H
    #include "config.h"
    #ifdef HAVE_CSTDINT
        #include <cstdint>
        typedef std::uint8_t    byte;
        typedef std::uint16_t   word;
        typedef std::uint32_t   dword;
        typedef std::int16_t    sword;
    #else
        #include <stdint.h>
        typedef uint8_t     byte;
        typedef uint16_t    word;
        typedef uint32_t    dword;
        typedef int16_t     sword;
    #endif
#else
    #include <cstdint>
    typedef std::uint8_t    byte;
    typedef std::uint16_t   word;
    typedef std::uint32_t   dword;
    typedef std::int16_t    sword;
#endif

