/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "coderep.h"
#include "procdef.h"
#include "model.h"
#include "x86objd.h"
#include "zoiks.h"
#include "cgaux.h"
#include "dw.h"
#include "dwarf.h"
#include "dfdbg.h"
#include "dfsupp.h"
#include "objout.h"
#include "x86obj.h"
#include "regset.h"
#include "rgtbl.h"
#include "dbsyms.h"
#include "dfsyms.h"
#include "x86data.h"
#include "cgprotos.h"


typedef enum {
    #define pick(id,name,ci,start,len) DW_REG_ ## id,
    #include "dwregx86.h"
    #undef pick
    DW_REG_MAX
} dw_regs;

struct dbg_seg_names {
    const char      *seg_name;
    const char      *class_name;
};

struct reg_map {
    hw_reg_set reg;
    dw_regs    dwarf;
};

static struct dbg_seg_names DwarfSegNames[DW_DEBUG_MAX] = {
    { ".debug_info",        "DWARF" },
    { ".debug_pubnames",    "DWARF" },
    { ".debug_aranges",     "DWARF" },
    { ".debug_line",        "DWARF" },
    { ".debug_loc",         "DWARF" },
    { ".debug_abbrev",      "DWARF" },
    { ".debug_macinfo",     "DWARF" },
    { ".debug_str",         "DWARF" },
    { ".WATCOM_references", "DWARF" }
};

static struct reg_map    HWRegValues[] = {
   { HW_D( HW_AL ),  DW_REG_AL },
   { HW_D( HW_AH ),  DW_REG_AH },
   { HW_D( HW_BL ),  DW_REG_BL },
   { HW_D( HW_BH ),  DW_REG_BH },
   { HW_D( HW_CL ),  DW_REG_CL },
   { HW_D( HW_CH ),  DW_REG_CH },
   { HW_D( HW_DL ),  DW_REG_DL },
   { HW_D( HW_DH ),  DW_REG_DH },
   { HW_D( HW_AX ),  DW_REG_AX },
   { HW_D( HW_BX ),  DW_REG_BX },
   { HW_D( HW_CX ),  DW_REG_CX },
   { HW_D( HW_DX ),  DW_REG_DX },
   { HW_D( HW_SI ),  DW_REG_SI },
   { HW_D( HW_DI ),  DW_REG_DI },
   { HW_D( HW_BP ),  DW_REG_BP },
   { HW_D( HW_SP ),  DW_REG_SP },
   { HW_D( HW_CS ),  DW_REG_CS },
   { HW_D( HW_SS ),  DW_REG_SS },
   { HW_D( HW_DS ),  DW_REG_DS },
   { HW_D( HW_ES ),  DW_REG_ES },
   { HW_D( HW_ST0 ), DW_REG_ST0 },
   { HW_D( HW_ST1 ), DW_REG_ST1 },
   { HW_D( HW_ST2 ), DW_REG_ST2 },
   { HW_D( HW_ST3 ), DW_REG_ST3 },
   { HW_D( HW_ST4 ), DW_REG_ST4 },
   { HW_D( HW_ST5 ), DW_REG_ST5 },
   { HW_D( HW_ST6 ), DW_REG_ST6 },
   { HW_D( HW_ST7 ), DW_REG_ST7 },
   { HW_D( HW_EAX ), DW_REG_EAX },
   { HW_D( HW_EBX ), DW_REG_EBX },
   { HW_D( HW_ECX ), DW_REG_ECX },
   { HW_D( HW_EDX ), DW_REG_EDX },
   { HW_D( HW_ESI ), DW_REG_ESI },
   { HW_D( HW_EDI ), DW_REG_EDI },
   { HW_D( HW_EBP ), DW_REG_EBP },
   { HW_D( HW_ESP ), DW_REG_ESP },
   { HW_D( HW_FS ),  DW_REG_FS },
   { HW_D( HW_GS ),  DW_REG_GS }
};


void    DFDefSegs( void )
/***********************/
{
    if( _IsModel( CGSW_GEN_DBG_LOCALS )
      || _IsModel( CGSW_GEN_DBG_TYPES ) ) {
        dw_sectnum  i;

        for( i = 0; i < DW_DEBUG_MAX; ++i ) {
            DFSetSection( i, NULL, DbgSegDef( DwarfSegNames[i].seg_name, DwarfSegNames[i].class_name, SEG_COMB_NORMAL + SEG_USE_32 ) );
        }
    } else if( _IsModel( CGSW_GEN_DBG_NUMBERS ) ) {
        DFSetSection( DW_DEBUG_LINE, NULL, DbgSegDef( DwarfSegNames[DW_DEBUG_LINE].seg_name, DwarfSegNames[DW_DEBUG_LINE].class_name, SEG_COMB_NORMAL + SEG_USE_32 ) );
    }
}

static dw_regs  DFRegMap( hw_reg_set hw_reg )
/*******************************************/
{
    int     i;

    for( i = 0; i < sizeof( HWRegValues ) / sizeof( HWRegValues[0] ); i++ ) {
        if( HW_Equal( HWRegValues[i].reg, hw_reg ) ) {
            return( HWRegValues[i].dwarf );
        }
    }
    Zoiks( ZOIKS_085 );/* reg not found */
    return( DW_REG_MAX );
}

void   DFOutReg( dw_loc_id locid, name *reg )
/*******************************************/
{
    hw_reg_set  hw_reg;
    hw_reg_set  hw_low;
    dw_regs     dw_reg;

    hw_reg = reg->r.reg;
#if _TARGET & _TARG_8086
    if( HW_CEqual( hw_reg, HW_ABCD ) ) {
        DWLocReg( Client, locid, DW_REG_DX );
        DWLocPiece( Client, locid, WD );
        DWLocReg( Client, locid, DW_REG_CX );
        DWLocPiece( Client, locid, WD );
        DWLocReg( Client, locid, DW_REG_BX );
        DWLocPiece( Client, locid, WD );
        DWLocReg( Client, locid, DW_REG_AX );
        DWLocPiece( Client, locid, WD );
        return;
    }
#endif
#if _TARGET & _TARG_8086
    hw_low = Low32Reg( hw_reg );
#elif _TARGET & _TARG_80386
    hw_low = Low64Reg( hw_reg );
#endif
    if( HW_CEqual( hw_low, HW_EMPTY ) ) {
        dw_reg = DFRegMap( hw_reg );
        DWLocReg( Client, locid, dw_reg );
   } else {
        dw_reg =  DFRegMap( hw_low );
        DWLocReg( Client, locid, dw_reg );
        DWLocPiece( Client, locid, WD );
        HW_TurnOff( hw_reg, hw_low );
        dw_reg = DFRegMap( hw_reg );
        DWLocReg( Client, locid, dw_reg );
        DWLocPiece( Client, locid, WD );
    }
}

void   DFOutRegInd( dw_loc_id locid, name *reg )
/**********************************************/
{
    dw_regs     dw_reg;

    dw_reg = DFRegMap(  reg->r.reg );
    DWLocOp( Client, locid, DW_LOC_breg, dw_reg, 0 );
}

uint DFStkReg( void )
/*******************/
{
    dw_regs    dw_reg;
    hw_reg_set stk;

    stk = StackReg();
    dw_reg = DFRegMap( stk );
    return( dw_reg );
}

uint DFDisplayReg( void )
/***********************/
{
    dw_regs    dw_reg;
    hw_reg_set dsp;

    dsp = DisplayReg();
    dw_reg = DFRegMap( dsp );
    return( dw_reg );
}

void DFAbbrevRef( void )
/**********************/
{
    back_handle bck;

    bck =  BENewBack( NULL );
    BackImpPtr( ABBREV_NAME, bck, 0 );
    BEFreeBack( bck );
}

void DFAbbrevDef( void )
/**********************/
{
    OutBckExport( ABBREV_NAME, true );
}
