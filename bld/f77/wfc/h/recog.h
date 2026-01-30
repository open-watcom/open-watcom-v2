/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2025-2026 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  declaration for recognizing specific tokens routines
*
****************************************************************************/


#include "opr.h"

extern bool    RecEquOpr( void );
extern bool    ReqOperator( OPR operator, int error );
extern bool    ReqEquOpr( void );
extern bool    RecColonOpr( void );
extern bool    ReqColonOpr( void );
extern bool    RecCommaOpr( void );
extern bool    ReqCommaOpr( void );
extern bool    RecCloseParenOpr( void );
extern bool    ReqCloseParenOpr( void );
extern bool    RecOpenParenOpr( void );
extern bool    ReqOpenParenOpr( void );
extern bool    RecMulOpr( void );
extern bool    ReqMulOpr( void );
extern bool    RecDivOpr( void );
extern bool    ReqDivOpr( void );
extern bool    RecPlusOpr( void );
extern bool    RecMinOpr( void );
extern bool    RecCatOpr( void );
extern bool    RecNoOpr( void );
extern bool    RecFBrOpr( void );
extern bool    RecTrmOpr( void );
extern bool    RecEOSOpr( void );
extern bool    ReqEOSOpr( void );
extern bool    RecNoOpn( void );
extern bool    ReqNoOpn( void );
extern bool    RecKeyWordOpn( const char *key );
extern bool    RecNameOpn( void );
extern bool    ReqNameOpn( clsname_id clsname );
extern bool    RecNWL( void );
extern bool    RecNumberOpn( void );
extern bool    RecLiteralOpn( void );
extern bool    RecNextOpr( OPR operator );
extern bool    ReqNextOpr( OPR operator, int error );
extern bool    RecIntVarOpn( void );
extern bool    ReqIntVarOpn( void );
extern bool    ReqDoVarOpn( void );
extern bool    RecArrName( void );
