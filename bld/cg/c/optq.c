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


#include "optwif.h"


extern  void    InsertQueue( ins_entry *old, ins_entry *new ) {
/***********************************************************/

  optbegin
    if( FirstIns == NULL ) {
        FirstIns = old;
        LastIns = old;
        FirstIns->ins.prev = NULL;
        FirstIns->ins.next = NULL;
    } else if( new == NULL ) {
        FirstIns->ins.prev = old;
        old->ins.next = FirstIns;
        old->ins.prev = NULL;
        FirstIns = old;
    } else if( new == LastIns ) {
        LastIns->ins.next = old;
        old->ins.next = NULL;
        old->ins.prev = LastIns;
        LastIns = old;
    } else {
        old->ins.next = new->ins.next;
        old->ins.prev = new;
        new->ins.next->ins.prev = old;
        new->ins.next = old;
    }
    ++QCount;
  optend
}


extern  void    DeleteQueue( ins_entry *old ) {
/********************************************/

  optbegin
    if( old == FirstIns ) {
        FirstIns = FirstIns->ins.next;
        if( FirstIns != NULL ) {
            FirstIns->ins.prev = NULL;
        } else {
            LastIns = NULL;
        }
    } else if( old == LastIns ) {
        LastIns = LastIns->ins.prev;
        LastIns->ins.next = NULL;
    } else {
        old->ins.next->ins.prev = old->ins.prev;
        old->ins.prev->ins.next = old->ins.next;
    }
    --QCount;
  optend
}
