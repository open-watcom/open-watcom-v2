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


#ifndef _FEPROTOS_H_INCLUDED
#define _FEPROTOS_H_INCLUDED

#include "cgapi.h"

// always want the prototypes
#define CGCALLBACKDEF( a, b, c )        extern b _CGCALLBACKAPI a c;
#include "cgfertns.h"
#undef  CGCALLBACKDEF

#ifdef _CGDLL

typedef struct fe_interface {
    #define CGCALLBACKDEF( a, b, c ) b _CGCALLBACKAPI (*a) c;
    #include "cgfertns.h"
    #undef  CGCALLBACKDEF
} fe_interface;

extern fe_interface *FEFuncTable;

#ifdef BY_CG
#include "feprotos.gh"
#endif

#endif

#endif
