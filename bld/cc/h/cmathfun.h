/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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


#if _INTEL_CPU
mathfunc( "__LOG",   "log",     1, O_LOG ),
mathfunc( "__COS",   "cos",     1, O_COS ),
mathfunc( "__SIN",   "sin",     1, O_SIN ),
mathfunc( "__TAN",   "tan",     1, O_TAN ),
mathfunc( "__SQRT",  "sqrt",    1, O_SQRT ),
mathfunc( "__FABS",  "fabs",    1, O_FABS ),
mathfunc( "__POW",   "pow",     2, O_POW ),
mathfunc( "__ATAN2", "atan2",   2, O_ATAN2 ),
mathfunc( "__FMOD",  "fmod",    2, O_FMOD ),
mathfunc( "__ACOS",  "acos",    1, O_ACOS ),
mathfunc( "__ASIN",  "asin",    1, O_ASIN ),
mathfunc( "__ATAN",  "atan",    1, O_ATAN ),
mathfunc( "__COSH",  "cosh",    1, O_COSH ),
mathfunc( "__EXP",   "exp",     1, O_EXP ),
mathfunc( "__LOG10", "log10",   1, O_LOG10 ),
mathfunc( "__SINH",  "sinh",    1, O_SINH ),
mathfunc( "__TANH",  "tanh",    1, O_TANH ),
#endif
