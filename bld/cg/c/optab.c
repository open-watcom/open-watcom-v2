/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2016 The Open Watcom Contributors. All Rights Reserved.
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
#include "cgdefs.h"
#include "coderep.h"
#include "tables.h"
#include "zoiks.h"
#include "optab.h"
#include "feprotos.h"

extern  opcode_entry*   OpcodeTable(table_def);


extern table_def OpTable[];

opcode_entry    *CodeTable( instruction *ins )
/********************************************/
{
    int         idx;

    idx = ins->head.opcode;
    idx *= ( XX + 1 );
    idx += ins->type_class;
    idx = OpTable[idx];
    if( idx == BAD ) {
        _Zoiks( ZOIKS_052 );
    }
#if _TARGET & _TARG_RISC
    if( idx == NYI ) {
        _Zoiks( ZOIKS_091 );
    }
#endif
    return( OpcodeTable( idx ) );
}


void    DoNothing( instruction *ins )
/***********************************/
{
    ins->table = OpcodeTable( DONOTHING );
    ins->u.gen_table = ins->table;
}


bool    DoesSomething( instruction *ins )
/***************************************/
{
    if( ins->u.gen_table == NULL )
        return( true );
    if( ins->u.gen_table->generate != G_NO )
        return( true );
    return( false );
}
