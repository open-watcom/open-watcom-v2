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
* Description:  Special 386 pointer types.
*
****************************************************************************/


#include "cgstd.h"
#include "typclass.h"
#include "typedef.h"
#include "types.h"
#include "model.h"

// type_def THugeCP= {  TY_HUGE_CODE_PTR,6,      TYPE_POINTER + TYPE_CODE };
type_def TLongCP= {  TY_LONG_CODE_PTR,6,      TYPE_POINTER + TYPE_CODE };
type_def TNearCP= {  TY_NEAR_CODE_PTR,4,      TYPE_POINTER + TYPE_CODE };
type_def THugeP = {  TY_HUGE_POINTER, 6,      TYPE_POINTER };
type_def TLongP = {  TY_LONG_POINTER, 6,      TYPE_POINTER };
type_def TNearP = {  TY_NEAR_POINTER, 4,      TYPE_POINTER };

extern type_def *PTInteger;
extern type_def *PTUnsigned;
extern type_def *PTPointer;
extern type_def *PTCodePointer;


extern  void    TargTypeInit( void )
/**********************************/
{
    TypeAlias( TY_UNSIGNED, TY_UINT_4 );
    TypeAlias( TY_INTEGER, TY_INT_4 );

    PTInteger = TypeAddress( TY_INT_4 );
    PTUnsigned = TypeAddress( TY_UINT_4 );

    if( _IsTargetModel( BIG_CODE ) ) {
        TypeAlias( TY_CODE_PTR, TY_LONG_CODE_PTR );
        PTCodePointer = TypeAddress( TY_LONG_CODE_PTR );
    } else {
        TypeAlias( TY_CODE_PTR, TY_NEAR_CODE_PTR );
        PTCodePointer = TypeAddress( TY_NEAR_CODE_PTR );
    }
    if( _IsTargetModel( BIG_DATA ) ) {
        TypeAlias( TY_POINTER, TY_LONG_POINTER );
        PTPointer = TypeAddress( TY_LONG_POINTER );
    } else {
        TypeAlias( TY_POINTER, TY_NEAR_POINTER );
        PTPointer = TypeAddress( TY_NEAR_POINTER );
    }
    TypeAlias( TY_NEAR_INTEGER, TY_INT_4 );
    TypeAlias( TY_LONG_INTEGER, TY_INT_4 );
    TypeAlias( TY_HUGE_INTEGER, TY_INT_4 );
}
