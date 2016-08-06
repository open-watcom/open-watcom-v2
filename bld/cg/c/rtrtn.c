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


#include "cgstd.h"
#include "coderep.h"
#include "zoiks.h"
#include "data.h"
#include "rtrtn.h"

static  import_handle   RTHdls[RTSIZE];
static  label_handle    RTLbls[RTSIZE];

rt_class    LookupRoutine( instruction *ins )
/*******************************************/
{
    rtn_info    *rtn;
    rt_class    rtindex;
    opcode_defs opcode;

    opcode = ins->head.opcode;
    if( opcode >= FIRST_CONDITION ) {
        opcode = OP_CMP;
    }
    rtn = RTInfo;
    rtindex = 0;
    while( rtn->op != opcode || rtn->operand_class != ins->type_class ) {
        ++rtn;
        ++rtindex;
        if( rtn->op == OP_NOP ) {
            _Zoiks( ZOIKS_021 );
            break;
        }
    }
    return( rtindex );
}


void    InitRT( void )
/********************/
{
    rt_class    rtindex;

    for( rtindex = 0; rtindex < RTSIZE; ++rtindex ) {
        RTHdls[rtindex] = NOT_IMPORTED;
        RTLbls[rtindex] = NULL;
    }
}


import_handle   AskRTHandle( rt_class rtindex )
/*********************************************/
{
    return( RTHdls[rtindex] );
}


void    TellRTHandle( rt_class rtindex, import_handle imphdl )
/************************************************************/
{
    RTHdls[rtindex] = imphdl;
}


label_handle RTLabel( rt_class rtindex )
/**************************************/
{
    label_handle    lbl;

    lbl = RTLbls[rtindex];
    if( lbl == NULL ) {
        lbl = AskForRTLabel( RTIDX2SYM( rtindex ) );
        RTLbls[rtindex] = lbl;
    }
    return( lbl );
}


rt_class    FindRTLabel( label_handle lbl )
/*****************************************/
{
    rt_class    rtindex;

    for( rtindex = 0; rtindex < RTSIZE; ++rtindex ) {
        if( RTLbls[rtindex] == lbl ) {
            break;
        }
    }
    return( rtindex );
}
