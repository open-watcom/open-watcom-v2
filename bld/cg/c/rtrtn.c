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
#include "coderep.h"
#include "opcodes.h"
#include "regset.h"
#include "rttable.h"
#include "rtclass.h"
#include "zoiks.h"

static    import_handle RTHdls[RTSIZE];
static    label_handle  RTLbls[RTSIZE];

extern  label_handle    AskRTLabel(sym_handle);

extern  void    LookupRoutine( instruction *ins ) {
/*************************************************/

    rtn_info    *rtn;
    int         idx;
    opcode_defs opcode;

    rtn = &RTInfo;
    idx = 0;
    opcode = ins->head.opcode;
    if( opcode >= FIRST_CONDITION ) {
        opcode = OP_CMP;
    }
    for(;;) {
        if( rtn->op == opcode && rtn->operand_class == ins->type_class ) break;
        ++ rtn;
        ++ idx;
        if( rtn->op == OP_NOP ) {
            _Zoiks( ZOIKS_021 );
            break;
        }
    }
    RoutineNum = idx;
}


extern  void    InitRT() {
/************************/

    int i       ;

    i = 0;
    while( i < RTSIZE ) {
        RTHdls[  i  ] = NOT_IMPORTED;
        RTLbls[  i  ] = NULL;
        i++;
    }
}


extern  import_handle   AskRTHandle( int rtindex ) {
/**************************************************/

    return( RTHdls[  rtindex  ] );
}


extern  void    TellRTHandle( int rtindex, import_handle hdl ) {
/**************************************************************/

    RTHdls[  rtindex  ] = hdl;
}


extern  label_handle    RTLabel( int rtindex ) {
/**********************************************/

    label_handle        lbl;

    lbl = RTLbls[  rtindex  ];
    if( lbl == NULL ) {
        lbl = AskRTLabel( (sym_handle *)rtindex );
        RTLbls[  rtindex  ] = lbl;
    }
    return( lbl );
}


extern  int     FindRTLabel( label_handle hdl ) {
/***********************************************/

    int i;

    i = 0;
    while( i < RTSIZE ) {
        if( RTLbls[  i  ] == hdl ) break;
        i++;
    }
    return( i + BEG_RTNS );
}
