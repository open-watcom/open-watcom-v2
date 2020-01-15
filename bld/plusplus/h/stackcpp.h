/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Prototype for saferecurse
*
****************************************************************************/


#define TO_SR_VALUE(x)      ((void *)(pointer_uint)(x))
#define FROM_SR_VALUE(x,t)  ((t)(pointer_uint)(x))

typedef void *(*func_sr)(void *);

extern void *SafeRecurseCpp( func_sr rtn, void *arg );
#if defined( __WATCOMC__ ) && defined( _M_IX86 ) && !defined( __NT__ )
/* just to be sure! must not put any parametr on the stack */
#pragma aux SafeRecurseCpp __parm __caller [__eax __ebx __ecx __edx]    /* just to be sure! */
#else
// nothing special
#endif
