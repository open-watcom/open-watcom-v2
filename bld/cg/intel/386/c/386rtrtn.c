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
* Description:  Generate calls to runtime support routines.
*
****************************************************************************/


#include "cgstd.h"
#include "coderep.h"
#include "zoiks.h"
#include "seldef.h"
#include "cgauxinf.h"
#include "makeins.h"
#include "data.h"
#include "rtrtn.h"
#include "objout.h"
#include "namelist.h"
#include "makeblk.h"
#include "insutil.h"
#include "rgtbl.h"
#include "rtcall.h"
#include "inssegs.h"


extern  name            *GenFloat( name *, type_class_def );
extern  void            UpdateLive( instruction *, instruction * );
extern  bool            SegIsSS( name * );
extern  conflict_node   *NameConflict( instruction *, name * );
extern  name            *AddrConst( name *, int, constant_class );

/*
 * If you add a new routine, let John know as the debugger recognizes
 * these.
 */

rtn_info RTInfo[] = {
    #define PICK(e,name,op,class,left,right,result) {name, op, class, left, right, result},
    #include "_rtinfo.h"
    #undef PICK
};

static call_class       rt_cclass = 0;

static  struct STRUCT_byte_seq( 2 ) Scn1 = {
     2, false,
    {0xF2,           /*       repne     */
     0xAE}           /*       scasb     */
};

static  struct STRUCT_byte_seq( 6 ) Scn1ES = {
     6, false,
    {0x06,          /*      push    es  */
     0x0e,          /*      push    cs  */
     0x07,          /*      pop     es  */
     0xF2,          /*      repne       */
     0xAE,          /*      scasb       */
     0x07}          /*      pop     es  */
};

static  struct STRUCT_byte_seq( 3 ) Scn2 = {    /* or Scn4 in USE16 */
     3, false,
    {0xF2,          /*      repne       */
     0x66, 0xAF}    /*      scasw       */
};

static  struct STRUCT_byte_seq( 7 ) Scn2ES = {  /* or Scn4 in USE16 */
     7, false,
    {0x06,          /*      push    es  */
     0x0e,          /*      push    cs  */
     0x07,          /*      pop     es  */
     0xF2,          /*      repne       */
     0x66, 0xAF,    /*      scasw       */
     0x07}          /*      pop     es  */
};

static  struct STRUCT_byte_seq( 2 ) Scn4 = {    /* or Scn2 in USE16 */
     2, false,
    {0xF2,          /*      repne       */
     0xAF}          /*      scasd       */
};

static  struct STRUCT_byte_seq( 6 ) Scn4ES = {  /* or Scn2 in USE16 */
     6, false,
    {0x06,          /*      push    es  */
     0x0e,          /*      push    cs  */
     0x07,          /*      pop     es  */
     0xF2,          /*      repne       */
     0xAF,          /*      scasd       */
     0x07}          /*      pop     es  */
};


const char  *AskRTName( rt_class rtindex )
/****************************************/
{
    if( _IsTargetModel( INDEXED_GLOBALS ) ) {
        switch( rtindex ) {
        case RT_FDA:
            return( "__FXA" );
        case RT_FDS:
            return( "__FXS" );
        case RT_FDM:
            return( "__FXM" );
        case RT_FDD:
            return( "__FXD" );
        case RT_FDC:
            return( "__FXC" );
        case RT_DPOW:
            return( "RT@XPOW" );
        case RT_DATAN2:
            return( "IF@XATAN2" );
        case RT_DFMOD:
            return( "IF@XFMOD" );
        }
    }
    return( RTInfo[rtindex].nam );
}


bool    RTLeaveOp2( instruction *ins )
/*************************************
    return true if it's a bad idea to put op2 into a temporary since we're
    gonna take the bugger's address in rMAKECALL.
*/
{
    ins = ins;
    return( false );
}


extern  name    *ScanCall( tbl_control *table, name *value, type_class_def class )
/*********************************************************************************
    generates a fake call to a runtime routine that looks up "value" in a table
    and jumps to the appropriate case, using either a pointer or index
    returned by the "routine". The "routine" will be generated inline later.
    See BEAuxInfo for the code sequences generated. That will explain
    how the jump destination is determined as well.
*/
{
    instruction *new_ins;
    name        *reg_name;
    name        *result;
    name        *label;
    hw_reg_set  tmp;
    rt_class    rtindex;

    switch( class ) {
    case U1:
        rtindex = RT_SCAN1;
        break;
    case U2:
        rtindex = RT_SCAN2;
        break;
    case U4:
        rtindex = RT_SCAN4;
        break;
    default:
        rtindex = RT_NOP;
        break;
    }

    reg_name = AllocRegName( FirstReg( RTInfo[rtindex].left ) );
    new_ins = MakeConvert( value, reg_name, class, value->n.name_class );
    AddIns( new_ins );

    reg_name = AllocRegName( HW_ECX );
    new_ins = MakeMove( AllocIntConst( table->size + 1 ), reg_name, U4 );
    AddIns( new_ins );

    reg_name = AllocRegName( HW_EDI );
    label = AllocMemory( table, 0, CG_VTB, U4 );
    label = AddrConst( label, AskBackSeg(), CONS_OFFSET );
    new_ins = MakeMove( label, reg_name, U4 );
    AddIns( new_ins );

    new_ins = NewIns( 3 );
    new_ins->head.opcode = OP_CALL;
    new_ins->type_class = U2;
    tmp = FirstReg( RTInfo[rtindex].left );
    HW_CTurnOn( tmp, HW_ECX );
    HW_CTurnOn( tmp, HW_EDI );
    HW_CTurnOn( tmp, HW_ES );
    new_ins->operands[CALL_OP_USED] = AllocRegName( tmp );
    new_ins->operands[CALL_OP_USED2] = new_ins->operands[CALL_OP_USED];
    new_ins->operands[CALL_OP_ADDR] = AllocMemory( RTLabel( rtindex ), 0, CG_LBL, U4 );
    new_ins->result = NULL;
    new_ins->num_operands = 2;
    AddIns( new_ins );

    result = AllocMemory( table, 0, CG_TBL, U4 ); /* so table gets freed!*/
    if( class == U4 ) {
        HW_CAsgn( tmp, HW_ECX );
        HW_CTurnOn( tmp, HW_EDI );
        new_ins->zap = &AllocRegName( tmp )->r;
        new_ins->result = AllocRegName( HW_EDI );
        HW_CAsgn( tmp, HW_CS );
        HW_CTurnOn( tmp, HW_EDI );
        result = AllocRegName( tmp );
        result = AllocIndex( result, NULL, ( table->size - 1 ) * 4, U4 );
        new_ins = MakeMove( result, AllocTemp( WD ), WD );
        AddIns( new_ins );
        result = new_ins->result;
    } else {
        HW_CAsgn( tmp, HW_ECX );
        HW_CTurnOn( tmp, HW_EDI );
        new_ins->zap = &AllocRegName( tmp )->r;
        new_ins->result = AllocRegName( HW_ECX );
        new_ins = MakeMove( new_ins->result, AllocTemp( WD ), WD );
        AddIns( new_ins );
        result = AllocIndex( new_ins->result, result, 0, U4 );
        result->i.scale = 2; /* 2**2 == 4 */
    }
    return( result );
}


extern  name    *Addressable( name *cons, type_class_def class )
/***************************************************************
    make sure a floating point constant is addressable (dropped
    it into memory if it isnt)
*/
{
    if( cons->n.class == N_CONSTANT )
        return( GenFloat( cons, class ) );
    return( cons );
}


extern  pointer BEAuxInfo( pointer hdl, aux_class request )
/**********************************************************
    see ScanCall for explanation
*/
{
    switch( request ) {
    case AUX_LOOKUP:
        switch( FindRTLabel( hdl ) ) {
        case RT_SCAN1:
            if( _IsntTargetModel( FLAT_MODEL ) )
                return( &Scn1ES );
            return( &Scn1 );
        case RT_SCAN2:
            if( _IsntTargetModel( USE_32 ) ) {
                if( _IsntTargetModel( FLAT_MODEL ) )
                    return( &Scn4ES );
                return( &Scn4 );
            } else {
                if( _IsntTargetModel( FLAT_MODEL ) )
                    return( &Scn2ES );
                return( &Scn2 );
            }
        case RT_SCAN4:
            if( _IsntTargetModel( USE_32 ) ) {
                if( _IsntTargetModel( FLAT_MODEL ) )
                    return( &Scn2ES );
                return( &Scn2 );
            } else {
                if( _IsntTargetModel( FLAT_MODEL ) )
                    return( &Scn4ES );
                return( &Scn4 );
            }
        default:
            return( NULL );
        }
    case CALL_CLASS:
        return( &rt_cclass );
    case CALL_BYTES:
        return( hdl );
    default:
        _Zoiks( ZOIKS_128 );
        return( NULL );
    }
}

extern  instruction     *rMAKEFNEG( instruction *ins )
/*****************************************************
    this is intentionally a stub for the 386.
*/
{
    return( ins );
}
