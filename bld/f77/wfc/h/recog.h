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
* Description:  declaration for recognizing specific tokens routines
*
****************************************************************************/


#include "opr.h"

extern bool    RecEquSign( void );
extern bool    ReqOperator( OPR operator, int error );
extern bool    ReqEquSign( void );
extern bool    RecColon( void );
extern bool    RecComma( void );
extern bool    ReqComma( void );
extern bool    ReqColon( void );
extern bool    RecCloseParen( void );
extern bool    ReqCloseParen( void );
extern bool    RecOpenParen( void );
extern bool    ReqOpenParen( void );
extern bool    RecMul( void );
extern bool    ReqMul( void );
extern bool    RecDiv( void );
extern bool    ReqDiv( void );
extern bool    RecPlus( void );
extern bool    RecMin( void );
extern bool    RecCat( void );
extern bool    RecNOpr( void );
extern bool    RecFBr( void );
extern bool    RecTrmOpr( void );
extern bool    RecEOS( void );
extern bool    ReqEOS( void );
extern bool    RecNOpn( void );
extern bool    ReqNOpn( void );
extern bool    RecKeyWord( char *key );
extern bool    RecName( void );
extern bool    ReqName( int index );
extern bool    RecNWL( void );
extern bool    RecNumber( void );
extern bool    RecLiteral( void );
extern bool    RecNextOpr( OPR operator );
extern bool    ReqNextOpr( OPR operator, int error );
extern bool    RecIntVar( void );
extern bool    ReqIntVar( void );
extern bool    ReqDoVar( void );
extern bool    RecArrName( void );
