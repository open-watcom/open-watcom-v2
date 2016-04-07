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


//
// THRDUTIL     : Thread manipulation utilities
//

#include "ftnstd.h"
#include <stdlib.h>
#include "rtstack.h"
#include "fthread.h"
#include "thread.h"


#pragma off (check_stack)

static unsigned         OtherStackLow;
static bool             Init = FALSE;   // have we initialized it to ASTACKLOW


// Do not switch stacks unless we have to.
static  void    __NullSwitchStackLow(void) {}
void            (*__SwitchStkLow)(void) = &__NullSwitchStackLow;


static void     SwitchThreadStackLow( void ) {
//============================================

        thread_data     *tdata;
        unsigned        tmp;

        if( !Init ) {
            OtherStackLow = (unsigned)(__ASTACKPTR - __ASTACKSIZ);
            Init = TRUE;
        }
        tdata = __GetThreadPtr();
        if( tdata ) {
            tmp = tdata->__stklowP;
            tdata->__stklowP = OtherStackLow;
            OtherStackLow = tmp;
        }
}


void            __InitMultiThreadIO( void ) {
//===========================================

    __SwitchStkLow = &SwitchThreadStackLow;
}
