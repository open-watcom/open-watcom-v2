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


#include "plusplus.h"
#include "cgback.h"
#include "codegen.h"
#include "cgbackut.h"
#include "cgaux.h"
#include "vstk.h"
#include "initdefs.h"


typedef struct sw_ctl SW_CTL;
struct sw_ctl                   // SW_CTL -- for each select statement
{   unsigned cases;             // - number of cases (includes default)
    cg_type type;               // - type for expression
    label_handle label;         // - label for selection code
    sel_handle id;              // - handle for select
    temp_handle temp;           // - temp containing select expression
};

static VSTK_CTL stack_switches; // stack: switches
static VSTK_CTL stack_labs_sw;  // stack: labels (switches)


static SW_CTL *switch_ctl(      // GET CURRENT SW_CTL
    void )
{
    return VstkTop( &stack_switches );
}


static sel_handle switch_id(    // GET CURRENT SWITCH ID
    void )
{
    return( switch_ctl()->id );
}


static label_handle switch_label(   // PLANT SWITCH LABEL
    void )
{
    label_handle tmp_label;         // - new label handle
    label_handle *stacked;          // - stacked label handle

    tmp_label = BENewLabel();
    CgLabel( tmp_label );
    ++ switch_ctl()->cases;
    stacked = VstkPush( &stack_labs_sw );
    *stacked = tmp_label;
    return( tmp_label );
}


label_handle CgSwitchBeg        // GENERATE CODE FOR START OF SWITCH STMT
    ( FN_CTL* fctl )            // - function control
{
    SW_CTL *ctl;                // - control for switch
    cg_name sw_expr;            // - switch expression
    cg_type sw_type;            // - switch type

    sw_expr = CgExprPopType( &sw_type );
    CgExprDtored( sw_expr, sw_type, DGRP_TEMPS, fctl );
    sw_expr = CgExprPopType( &sw_type );
    ctl = VstkPush( &stack_switches );
    ctl->id = CGSelInit();
    ctl->cases = 0;
    ctl->temp = CGTemp( sw_type );
    ctl->type = sw_type;
    ctl->label = BENewLabel();
    CgAssign( CGTempName( ctl->temp, sw_type ), sw_expr, sw_type );
    return ctl->label;
}


void CgSwitchCase               // GENERATE CODE FOR SWITCH CASE
    ( int case_value )          // - case value
{
    CGSelCase( switch_id(), switch_label(), case_value );
}


void CgSwitchDefault            // GENERATE CODE FOR DEFAULT STMT (CODED)
    ( void )
{
    CGSelOther( switch_id(), switch_label() );
}


void CgSwitchDefaultGen         // GENERATE CODE FOR DEFAULT STMT (GEN'ED)
    ( label_handle lab )        // - label to be used
{
    CGSelOther( switch_id(), lab );
}


void CgSwitchEnd                // GENERATE CODE FOR END OF SWITCH STMT
    ( void )
{
    SW_CTL *ctl;                // - control for switch

    ctl = VstkTop( &stack_switches );
    CgLabel( ctl->label );
    CGSelect( ctl->id, CgFetchTemp( ctl->temp, ctl->type ) );
    BEFiniLabel( ctl->label );
    VstkPop( &stack_switches );
    CgLabelsPop( &stack_labs_sw, ctl->cases );
}


static void init(               // INITIALIZATION FOR MODULE
    INITFINI* defn )            // - definition
{
    defn = defn;
    VstkOpen( &stack_switches, sizeof( SW_CTL ), 4 );
    VstkOpen( &stack_labs_sw, sizeof( label_handle ), 32 );
}


static void fini(               // COMPLETION FOR MODULE
    INITFINI* defn )            // - definition
{
    defn = defn;
    VstkClose( &stack_switches );
    VstkClose( &stack_labs_sw );
}


INITDEFN( cg_switch, init, fini )
