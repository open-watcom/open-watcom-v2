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


#ifndef WRESALL_INCLUDED
#define WRESALL_INCLUDED

#include "layer1.h"

#include "wres.h"
#include "mem2.h"
#include "util.h"
#include "wresrtns.h"


#if !defined( min )
    #define min( x, y ) (((x) > (y)) ? (y) : (x))
#endif

/* reference the correct library to include for the memory model that is */
/* being compiled */
#ifndef _NO_LIB_PRAGMAS_
#if defined(__SMALL__)
    #pragma library (wress)
#elif defined(__MEDIUM__)
    #pragma library (wresm)
#elif defined(__COMPACT__)
    #pragma library (wresc)
#elif defined(__LARGE__)
    #pragma library (wresl)
#elif defined(__HUGE__)
    #pragma library (wresh)
#elif defined(__FLAT__)
    #pragma library (wresf)
#elif !defined(__AXP__)
    #error The current memory model is not supported for the WRES library.
#endif
#endif

#endif
