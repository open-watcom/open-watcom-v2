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
* Description:  Zoiks! Internal compiler error!
*
****************************************************************************/


#include "standard.h"
#include "cgdefs.h"
#include "cg.h"
#include "coderep.h"
#include "interrs.h"
#include "feprotos.h"

extern  bool            WantZoiks2( void );

static  int             Zoiks40 = { 0 };
#define MAX_ZOIKS_40    0x1000

extern  void    Zoiks( int msg )
/*******************************
    put out an internal compiler error.  2 is chunks unfreed which we
    may want to hide.  1 is unfreed code labels which we always hide.
*/
{
    if( ( msg == ZOIKS_001 ) ) return;
    if( ( msg == ZOIKS_002 ) && ( !WantZoiks2() ) ) return;
    if( ( msg == ZOIKS_040 ) && ( ++Zoiks40 < MAX_ZOIKS_40 ) ) return;
    FEMessage( MSG_BACK_END_ERROR, (pointer)msg );
}
