/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2018-2020 The Open Watcom Contributors. All Rights Reserved.
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


#include "cgstd.h"
#include "watcom.h"
#include "typedef.h"
#include "cgapi.h"

/*
 * FP80BIT_DEVELOPMENT macro switch on all development changes for 80-bit FP stuff
 * it is Work-In-Progress therefore __FP80BIT macro is used for guard original code
 * this macro must be used for any new code which can change existing behaviour
 * now CG use internaly 80-bit FP for FPU inline code and front end long double type
 * is mapped to double type
 *
 * initialy all mapping from long double to double type were removed that front end
 * long double type is recognized by CG, but appropriate missing stuff for long double
 * type in CG must be implemented
 *
 * as soon as work will finish then these macros can be removed
 */

//#define FP80BIT_DEVELOPMENT

#ifdef FP80BIT_DEVELOPMENT
#define __FP80BIT(ld,d)                 ld
#else
#define __FP80BIT(ld,d)                 d
#endif

#define _IsPowerOfTwo( x )              ( ( (x) & ( 1 - (x) ) ) == (x) )

#ifdef __AXP__
#define _AlignmentCheck( ptr, size )    assert( (((unsigned)ptr) & ((size)-1)) == 0 )
#else
#define _AlignmentCheck( ptr, size )    {}
#endif
