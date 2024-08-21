/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2024      The Open Watcom Contributors. All Rights Reserved.
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


#include "_cgstd.h"
#include "optwif.h"


void    InsertQueue( ins_entry *old_ins, ins_entry *new_ins )
/***********************************************************/
{
  optbegin
    if( FirstIns == NULL ) {
        FirstIns = old_ins;
        LastIns = old_ins;
        FirstIns->ins.prev = NULL;
        FirstIns->ins.next = NULL;
    } else if( new_ins == NULL ) {
        FirstIns->ins.prev = old_ins;
        old_ins->ins.next = FirstIns;
        old_ins->ins.prev = NULL;
        FirstIns = old_ins;
    } else if( new_ins == LastIns ) {
        LastIns->ins.next = old_ins;
        old_ins->ins.next = NULL;
        old_ins->ins.prev = LastIns;
        LastIns = old_ins;
    } else {
        old_ins->ins.next = new_ins->ins.next;
        old_ins->ins.prev = new_ins;
        new_ins->ins.next->ins.prev = old_ins;
        new_ins->ins.next = old_ins;
    }
    ++QCount;
  optend
}


void    DeleteQueue( ins_entry *old_ins )
/***************************************/
{
  optbegin
    if( old_ins == FirstIns ) {
        FirstIns = FirstIns->ins.next;
        if( FirstIns != NULL ) {
            FirstIns->ins.prev = NULL;
        } else {
            LastIns = NULL;
        }
    } else if( old_ins == LastIns ) {
        LastIns = LastIns->ins.prev;
        LastIns->ins.next = NULL;
    } else {
        old_ins->ins.next->ins.prev = old_ins->ins.prev;
        old_ins->ins.prev->ins.next = old_ins->ins.next;
    }
    --QCount;
  optend
}
