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
* Description:  The cfloat library internals.
*
****************************************************************************/


#include "cfloat.h"

#define NULLCHAR        '\0'

#define CF_MAX_PREC     32
#define CF_ERR_EXP      INT_MAX

#define MAX_SIG_DIGITS  17

#define _IsDigit( ch )  ( ch >= '0' && ch <= '9' )

extern  unsigned_32     U32ModDiv( unsigned_32 *, unsigned_32 );
extern  int             CFOrder( cfloat *float1, cfloat *float2 );
extern  int             CFAccess( cfloat *f, int index );
extern  void            CFDeposit( cfloat *f, int index, int data );
extern  void            CFClean( cfloat *f );
