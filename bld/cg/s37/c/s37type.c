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



#include "standard.h"
#include "cgdefs.h"
#include "typclass.h"
#include "typedef.h"
#include "types.h"
#include "model.h"

#include "s37type.def"

type_def TNearCP= {  TY_NEAR_CODE_PTR,4,      TYPE_POINTER + TYPE_CODE };
//type_def THugeCP= {  TY_NEAR_CODE_PTR,4,      TYPE_POINTER + TYPE_CODE };
type_def TLongCP= {  TY_NEAR_CODE_PTR,4,      TYPE_POINTER + TYPE_CODE };
type_def TNearP = {  TY_NEAR_POINTER, 4,      TYPE_POINTER };
type_def THugeP = {  TY_NEAR_POINTER, 4,      TYPE_POINTER };
type_def TLongP = {  TY_NEAR_POINTER, 4,      TYPE_POINTER };



extern  void    TargTypeInit() {
/**************************/

    TypeAlias( TY_UNSIGNED, TY_UINT_4 );
    TypeAlias( TY_INTEGER, TY_INT_4 );
    TypeAlias( TY_CODE_PTR, TY_NEAR_CODE_PTR );
    TypeAlias( TY_POINTER, TY_NEAR_POINTER );
    TypeAlias( TY_NEAR_INTEGER, TY_INT_4 );
    TypeAlias( TY_LONG_INTEGER, TY_INT_4 );
    TypeAlias( TY_HUGE_INTEGER, TY_INT_4 );
}
