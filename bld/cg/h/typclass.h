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


typedef enum {
        U1,
        I1,
        U2,
        I2,
        U4,
        I4,
        U8,
        I8,
        CP,
        PT,
        FS,
        FD,
        FL,
        XX,
        X8,
/*  following included for use only by GENTYPE.WSL */
        ER
} type_class_def;

#define _IsI64( c ) ( (c) >= U8 && (c) <= I8 )
#define _IsFloating( c ) ( (c) >= FS && (c) <= FL )
#define _IsPointer( c ) ( (c) == PT || (c) == CP )
#define _IsIntegral( c ) ( (c) >= U1 && (c) <= PT )

#include "targsys.h"

#if _TARGET & _TARG_80386
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
#else
    typedef signed_16     type_length;
    #define MAX_TYPE_LENGTH 0x7fff
#endif
