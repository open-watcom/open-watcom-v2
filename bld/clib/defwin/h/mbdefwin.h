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


#ifndef _MBDEFWIN_H_INCLUDED
#define _MBDEFWIN_H_INCLUDED

#include "variety.h"
#include <mbctype.h>
#include <mbstring.h>
#include "mbyte.h"

typedef unsigned int            mb_char;

/*** Avoid excessive conditional compilation ***/
#if defined( _M_I86 )
    #define FAR_mbsninc         __fmbsninc
    #define FAR_mbsnbcnt        _fmbsnbcnt
    #define FAR_mbsncpy         _fmbsncpy
    #define FAR_mbsnbcpy        _fmbsnbcpy
    #define FAR_mbslen          __fmbslen
    #define FAR_mbccpy          _fmbccpy
    #define FAR_mbsnextc        _fmbsnextc
    #define FAR_mbvtop          _fmbvtop
#else
    #define FAR_mbsninc         __mbsninc
    #define FAR_mbsnbcnt        _mbsnbcnt
    #define FAR_mbsncpy         _mbsncpy
    #define FAR_mbsnbcpy        _mbsnbcpy
    #define FAR_mbslen          __mbslen
    #define FAR_mbccpy          _mbccpy
    #define FAR_mbsnextc        _mbsnextc
    #define FAR_mbvtop          _mbvtop
#endif

#endif
