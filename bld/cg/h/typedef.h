/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  The type_def typedef.
*
****************************************************************************/


#ifndef TYPEDEF_H_INCLUDED
#define TYPEDEF_H_INCLUDED

#include "targsys.h"
#include "cgdefs.h"


#if _TARGET & _TARG_8086
    typedef signed_16     type_length;
    #define MAX_TYPE_LENGTH 0x7fff
#elif _TARGET & _TARG_80386
    typedef signed_32     type_length;
    #define MAX_TYPE_LENGTH 0x7fffffff
#elif _TARGET & _TARG_370
    typedef signed_32     type_length;
    #define MAX_TYPE_LENGTH 0x7fffffff
#elif _TARGET & _TARG_PPC
    typedef signed_32     type_length;
    #define MAX_TYPE_LENGTH 0x7fffffff
#elif _TARGET & _TARG_AXP
    typedef signed_32     type_length;
    #define MAX_TYPE_LENGTH 0x7fffffff
#elif _TARGET & _TARG_MIPS
    typedef signed_32     type_length;
    #define MAX_TYPE_LENGTH 0x7fffffff
#else
    #error Unknown target
#endif

typedef enum {
        TYPE_FLOAT      = 0x01,
        TYPE_SIGNED     = 0x02,
        TYPE_POINTER    = 0x04,
        TYPE_CODE       = 0x08
} type_attr;

typedef struct type_def {
        cg_type         refno;
        type_length     length;
        type_attr       attr;
#if _TARGET & _TARG_RISC
        type_length     align;
#endif
} type_def;

#endif
