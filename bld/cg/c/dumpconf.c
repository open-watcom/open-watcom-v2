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
* Description:  Dump conflicts.
*
****************************************************************************/


#include "cgstd.h"
#include "coderep.h"
#include "dumpio.h"
#include "data.h"

extern  void            DumpOperand(name*);
extern  void            DumpGBit(global_bit_set*);
extern  void            DumpLBit(local_bit_set*);

extern  hw_reg_set      *RegSets[];


static  bool    Check( hw_reg_set *name, hw_reg_set test ) {
/*********************************************************************/

    if( !HW_Subset( *name, test ) ) return( false );
    HW_TurnOff( *name, test );
    return( true );
}


extern  void    DumpRegName( hw_reg_set regname ) {
/*************************************************/

    bool        first;
    hw_reg_set  name;

    first = true;
    name = regname;
    if( !HW_COvlap( name, HW_UNUSED ) ) {
        while( !HW_CEqual( name, HW_EMPTY ) ) {
            if( first ) {
                first = false;
            } else {
                DumpChar( ':' );
            }
#if _TARGET & _TARG_370
            if( Check(&name,HW_G0) ) { DumpLiteral( "G0" ); continue; }
            if( Check(&name,HW_G1) ) { DumpLiteral( "G1" ); continue; }
            if( Check(&name,HW_G2) ) { DumpLiteral( "G2" ); continue; }
            if( Check(&name,HW_G3) ) { DumpLiteral( "G3" ); continue; }
            if( Check(&name,HW_G4) ) { DumpLiteral( "G4" ); continue; }
            if( Check(&name,HW_G5) ) { DumpLiteral( "G5" ); continue; }
            if( Check(&name,HW_G6) ) { DumpLiteral( "G6" ); continue; }
            if( Check(&name,HW_G7) ) { DumpLiteral( "G7" ); continue; }
            if( Check(&name,HW_G8) ) { DumpLiteral( "G8" ); continue; }
            if( Check(&name,HW_G9) ) { DumpLiteral( "G9" ); continue; }
            if( Check(&name,HW_G10)) { DumpLiteral( "G10" ); continue; }
            if( Check(&name,HW_G11)) { DumpLiteral( "G11" ); continue; }
            if( Check(&name,HW_G12)) { DumpLiteral( "G12" ); continue; }
            if( Check(&name,HW_G13)) { DumpLiteral( "G13" ); continue; }
            if( Check(&name,HW_G14)) { DumpLiteral( "G14" ); continue; }
            if( Check(&name,HW_G15)) { DumpLiteral( "G15" ); continue; }
            if( Check(&name,HW_E0) ) { DumpLiteral( "E0" ); continue; }
            if( Check(&name,HW_E4) ) { DumpLiteral( "E4" ); continue; }
            if( Check(&name,HW_D0) ) { DumpLiteral( "D0" ); continue; }
            if( Check(&name,HW_D2) ) { DumpLiteral( "D2" ); continue; }
            if( Check(&name,HW_D4) ) { DumpLiteral( "D4" ); continue; }
            if( Check(&name,HW_D6) ) { DumpLiteral( "D6" ); continue; }
            if( Check(&name,HW_Y0) ) { DumpLiteral( "Y0" ); continue; }
            if( Check(&name,HW_Y2) ) { DumpLiteral( "Y2" ); continue; }
            if( Check(&name,HW_Y4) ) { DumpLiteral( "Y4" ); continue; }
            if( Check(&name,HW_Y6) ) { DumpLiteral( "Y6" ); continue; }
#endif
#if _TARGET & (_TARG_PPC | _TARG_AXP | _TARG_MIPS)
            if( Check(&name,HW_R0) ) { DumpLiteral( "R0" ); continue; }
            if( Check(&name,HW_R1) ) { DumpLiteral( "R1" ); continue; }
            if( Check(&name,HW_R2) ) { DumpLiteral( "R2" ); continue; }
            if( Check(&name,HW_R3) ) { DumpLiteral( "R3" ); continue; }
            if( Check(&name,HW_R4) ) { DumpLiteral( "R4" ); continue; }
            if( Check(&name,HW_R5) ) { DumpLiteral( "R5" ); continue; }
            if( Check(&name,HW_R6) ) { DumpLiteral( "R6" ); continue; }
            if( Check(&name,HW_R7) ) { DumpLiteral( "R7" ); continue; }
            if( Check(&name,HW_R8) ) { DumpLiteral( "R8" ); continue; }
            if( Check(&name,HW_R9) ) { DumpLiteral( "R9" ); continue; }
            if( Check(&name,HW_R10) ) { DumpLiteral( "R10" ); continue; }
            if( Check(&name,HW_R11) ) { DumpLiteral( "R11" ); continue; }
            if( Check(&name,HW_R12) ) { DumpLiteral( "R12" ); continue; }
            if( Check(&name,HW_R13) ) { DumpLiteral( "R13" ); continue; }
            if( Check(&name,HW_R14) ) { DumpLiteral( "R14" ); continue; }
            if( Check(&name,HW_R15) ) { DumpLiteral( "R15" ); continue; }
            if( Check(&name,HW_R16) ) { DumpLiteral( "R16" ); continue; }
            if( Check(&name,HW_R17) ) { DumpLiteral( "R17" ); continue; }
            if( Check(&name,HW_R18) ) { DumpLiteral( "R18" ); continue; }
            if( Check(&name,HW_R19) ) { DumpLiteral( "R19" ); continue; }
            if( Check(&name,HW_R20) ) { DumpLiteral( "R20" ); continue; }
            if( Check(&name,HW_R21) ) { DumpLiteral( "R21" ); continue; }
            if( Check(&name,HW_R22) ) { DumpLiteral( "R22" ); continue; }
            if( Check(&name,HW_R23) ) { DumpLiteral( "R23" ); continue; }
            if( Check(&name,HW_R24) ) { DumpLiteral( "R24" ); continue; }
            if( Check(&name,HW_R25) ) { DumpLiteral( "R25" ); continue; }
            if( Check(&name,HW_R26) ) { DumpLiteral( "R26" ); continue; }
            if( Check(&name,HW_R27) ) { DumpLiteral( "R27" ); continue; }
            if( Check(&name,HW_R28) ) { DumpLiteral( "R28" ); continue; }
            if( Check(&name,HW_R29) ) { DumpLiteral( "R29" ); continue; }
            if( Check(&name,HW_R30) ) { DumpLiteral( "R30" ); continue; }
            if( Check(&name,HW_R31) ) { DumpLiteral( "R31" ); continue; }
    
            if( Check(&name,HW_D0) ) { DumpLiteral( "D0" ); continue; }
            if( Check(&name,HW_D1) ) { DumpLiteral( "D1" ); continue; }
            if( Check(&name,HW_D2) ) { DumpLiteral( "D2" ); continue; }
            if( Check(&name,HW_D3) ) { DumpLiteral( "D3" ); continue; }
            if( Check(&name,HW_D4) ) { DumpLiteral( "D4" ); continue; }
            if( Check(&name,HW_D5) ) { DumpLiteral( "D5" ); continue; }
            if( Check(&name,HW_D6) ) { DumpLiteral( "D6" ); continue; }
            if( Check(&name,HW_D7) ) { DumpLiteral( "D7" ); continue; }
            if( Check(&name,HW_D8) ) { DumpLiteral( "D8" ); continue; }
            if( Check(&name,HW_D9) ) { DumpLiteral( "D9" ); continue; }
            if( Check(&name,HW_D10) ) { DumpLiteral( "D10" ); continue; }
            if( Check(&name,HW_D11) ) { DumpLiteral( "D11" ); continue; }
            if( Check(&name,HW_D12) ) { DumpLiteral( "D12" ); continue; }
            if( Check(&name,HW_D13) ) { DumpLiteral( "D13" ); continue; }
            if( Check(&name,HW_D14) ) { DumpLiteral( "D14" ); continue; }
            if( Check(&name,HW_D15) ) { DumpLiteral( "D15" ); continue; }
            if( Check(&name,HW_D16) ) { DumpLiteral( "D16" ); continue; }
            if( Check(&name,HW_D17) ) { DumpLiteral( "D17" ); continue; }
            if( Check(&name,HW_D18) ) { DumpLiteral( "D18" ); continue; }
            if( Check(&name,HW_D19) ) { DumpLiteral( "D19" ); continue; }
            if( Check(&name,HW_D20) ) { DumpLiteral( "D20" ); continue; }
            if( Check(&name,HW_D21) ) { DumpLiteral( "D21" ); continue; }
            if( Check(&name,HW_D22) ) { DumpLiteral( "D22" ); continue; }
            if( Check(&name,HW_D23) ) { DumpLiteral( "D23" ); continue; }
            if( Check(&name,HW_D24) ) { DumpLiteral( "D24" ); continue; }
            if( Check(&name,HW_D25) ) { DumpLiteral( "D25" ); continue; }
            if( Check(&name,HW_D26) ) { DumpLiteral( "D26" ); continue; }
            if( Check(&name,HW_D27) ) { DumpLiteral( "D27" ); continue; }
            if( Check(&name,HW_D28) ) { DumpLiteral( "D28" ); continue; }
            if( Check(&name,HW_D29) ) { DumpLiteral( "D29" ); continue; }
            if( Check(&name,HW_D30) ) { DumpLiteral( "D30" ); continue; }
            if( Check(&name,HW_D31) ) { DumpLiteral( "D31" ); continue; }
    
            if( Check(&name,HW_W0) ) { DumpLiteral( "W0" ); continue; }
            if( Check(&name,HW_W1) ) { DumpLiteral( "W1" ); continue; }
            if( Check(&name,HW_W2) ) { DumpLiteral( "W2" ); continue; }
            if( Check(&name,HW_W3) ) { DumpLiteral( "W3" ); continue; }
            if( Check(&name,HW_W4) ) { DumpLiteral( "W4" ); continue; }
            if( Check(&name,HW_W5) ) { DumpLiteral( "W5" ); continue; }
            if( Check(&name,HW_W6) ) { DumpLiteral( "W6" ); continue; }
            if( Check(&name,HW_W7) ) { DumpLiteral( "W7" ); continue; }
            if( Check(&name,HW_W8) ) { DumpLiteral( "W8" ); continue; }
            if( Check(&name,HW_W9) ) { DumpLiteral( "W9" ); continue; }
            if( Check(&name,HW_W10) ) { DumpLiteral( "W10" ); continue; }
            if( Check(&name,HW_W11) ) { DumpLiteral( "W11" ); continue; }
            if( Check(&name,HW_W12) ) { DumpLiteral( "W12" ); continue; }
            if( Check(&name,HW_W13) ) { DumpLiteral( "W13" ); continue; }
            if( Check(&name,HW_W14) ) { DumpLiteral( "W14" ); continue; }
            if( Check(&name,HW_W15) ) { DumpLiteral( "W15" ); continue; }
            if( Check(&name,HW_W16) ) { DumpLiteral( "W16" ); continue; }
            if( Check(&name,HW_W17) ) { DumpLiteral( "W17" ); continue; }
            if( Check(&name,HW_W18) ) { DumpLiteral( "W18" ); continue; }
            if( Check(&name,HW_W19) ) { DumpLiteral( "W19" ); continue; }
            if( Check(&name,HW_W20) ) { DumpLiteral( "W20" ); continue; }
            if( Check(&name,HW_W21) ) { DumpLiteral( "W21" ); continue; }
            if( Check(&name,HW_W22) ) { DumpLiteral( "W22" ); continue; }
            if( Check(&name,HW_W23) ) { DumpLiteral( "W23" ); continue; }
            if( Check(&name,HW_W24) ) { DumpLiteral( "W24" ); continue; }
            if( Check(&name,HW_W25) ) { DumpLiteral( "W25" ); continue; }
            if( Check(&name,HW_W26) ) { DumpLiteral( "W26" ); continue; }
            if( Check(&name,HW_W27) ) { DumpLiteral( "W27" ); continue; }
            if( Check(&name,HW_W28) ) { DumpLiteral( "W28" ); continue; }
            if( Check(&name,HW_W29) ) { DumpLiteral( "W29" ); continue; }
            if( Check(&name,HW_W30) ) { DumpLiteral( "W30" ); continue; }
            if( Check(&name,HW_W31) ) { DumpLiteral( "W31" ); continue; }
    
            if( Check(&name,HW_B0) ) { DumpLiteral( "B0" ); continue; }
            if( Check(&name,HW_B1) ) { DumpLiteral( "B1" ); continue; }
            if( Check(&name,HW_B2) ) { DumpLiteral( "B2" ); continue; }
            if( Check(&name,HW_B3) ) { DumpLiteral( "B3" ); continue; }
            if( Check(&name,HW_B4) ) { DumpLiteral( "B4" ); continue; }
            if( Check(&name,HW_B5) ) { DumpLiteral( "B5" ); continue; }
            if( Check(&name,HW_B6) ) { DumpLiteral( "B6" ); continue; }
            if( Check(&name,HW_B7) ) { DumpLiteral( "B7" ); continue; }
            if( Check(&name,HW_B8) ) { DumpLiteral( "B8" ); continue; }
            if( Check(&name,HW_B9) ) { DumpLiteral( "B9" ); continue; }
            if( Check(&name,HW_B10) ) { DumpLiteral( "B10" ); continue; }
            if( Check(&name,HW_B11) ) { DumpLiteral( "B11" ); continue; }
            if( Check(&name,HW_B12) ) { DumpLiteral( "B12" ); continue; }
            if( Check(&name,HW_B13) ) { DumpLiteral( "B13" ); continue; }
            if( Check(&name,HW_B14) ) { DumpLiteral( "B14" ); continue; }
            if( Check(&name,HW_B15) ) { DumpLiteral( "B15" ); continue; }
            if( Check(&name,HW_B16) ) { DumpLiteral( "B16" ); continue; }
            if( Check(&name,HW_B17) ) { DumpLiteral( "B17" ); continue; }
            if( Check(&name,HW_B18) ) { DumpLiteral( "B18" ); continue; }
            if( Check(&name,HW_B19) ) { DumpLiteral( "B19" ); continue; }
            if( Check(&name,HW_B20) ) { DumpLiteral( "B20" ); continue; }
            if( Check(&name,HW_B21) ) { DumpLiteral( "B21" ); continue; }
            if( Check(&name,HW_B22) ) { DumpLiteral( "B22" ); continue; }
            if( Check(&name,HW_B23) ) { DumpLiteral( "B23" ); continue; }
            if( Check(&name,HW_B24) ) { DumpLiteral( "B24" ); continue; }
            if( Check(&name,HW_B25) ) { DumpLiteral( "B25" ); continue; }
            if( Check(&name,HW_B26) ) { DumpLiteral( "B26" ); continue; }
            if( Check(&name,HW_B27) ) { DumpLiteral( "B27" ); continue; }
            if( Check(&name,HW_B28) ) { DumpLiteral( "B28" ); continue; }
            if( Check(&name,HW_B29) ) { DumpLiteral( "B29" ); continue; }
            if( Check(&name,HW_B30) ) { DumpLiteral( "B30" ); continue; }
            if( Check(&name,HW_B31) ) { DumpLiteral( "B31" ); continue; }
    
            if( Check(&name,HW_F0) ) { DumpLiteral( "F0" ); continue; }
            if( Check(&name,HW_F1) ) { DumpLiteral( "F1" ); continue; }
            if( Check(&name,HW_F2) ) { DumpLiteral( "F2" ); continue; }
            if( Check(&name,HW_F3) ) { DumpLiteral( "F3" ); continue; }
            if( Check(&name,HW_F4) ) { DumpLiteral( "F4" ); continue; }
            if( Check(&name,HW_F5) ) { DumpLiteral( "F5" ); continue; }
            if( Check(&name,HW_F6) ) { DumpLiteral( "F6" ); continue; }
            if( Check(&name,HW_F7) ) { DumpLiteral( "F7" ); continue; }
            if( Check(&name,HW_F8) ) { DumpLiteral( "F8" ); continue; }
            if( Check(&name,HW_F9) ) { DumpLiteral( "F9" ); continue; }
            if( Check(&name,HW_F10) ) { DumpLiteral( "F10" ); continue; }
            if( Check(&name,HW_F11) ) { DumpLiteral( "F11" ); continue; }
            if( Check(&name,HW_F12) ) { DumpLiteral( "F12" ); continue; }
            if( Check(&name,HW_F13) ) { DumpLiteral( "F13" ); continue; }
            if( Check(&name,HW_F14) ) { DumpLiteral( "F14" ); continue; }
            if( Check(&name,HW_F15) ) { DumpLiteral( "F15" ); continue; }
            if( Check(&name,HW_F16) ) { DumpLiteral( "F16" ); continue; }
            if( Check(&name,HW_F17) ) { DumpLiteral( "F17" ); continue; }
            if( Check(&name,HW_F18) ) { DumpLiteral( "F18" ); continue; }
            if( Check(&name,HW_F19) ) { DumpLiteral( "F19" ); continue; }
            if( Check(&name,HW_F20) ) { DumpLiteral( "F20" ); continue; }
            if( Check(&name,HW_F21) ) { DumpLiteral( "F21" ); continue; }
            if( Check(&name,HW_F22) ) { DumpLiteral( "F22" ); continue; }
            if( Check(&name,HW_F23) ) { DumpLiteral( "F23" ); continue; }
            if( Check(&name,HW_F24) ) { DumpLiteral( "F24" ); continue; }
            if( Check(&name,HW_F25) ) { DumpLiteral( "F25" ); continue; }
            if( Check(&name,HW_F26) ) { DumpLiteral( "F26" ); continue; }
            if( Check(&name,HW_F27) ) { DumpLiteral( "F27" ); continue; }
            if( Check(&name,HW_F28) ) { DumpLiteral( "F28" ); continue; }
            if( Check(&name,HW_F29) ) { DumpLiteral( "F29" ); continue; }
            if( Check(&name,HW_F30) ) { DumpLiteral( "F30" ); continue; }
            if( Check(&name,HW_F31) ) { DumpLiteral( "F31" ); continue; }
#endif
#if 0
#if _TARGET & _TARG_PPC
            if( Check(&name,HW_CTR) ) { DumpLiteral( "CTR" ); continue; }
            if( Check(&name,HW_CR) ) { DumpLiteral( "CR" ); continue; }
            if( Check(&name,HW_MQ) ) { DumpLiteral( "MQ" ); continue; }
            if( Check(&name,HW_LR) ) { DumpLiteral( "LR" ); continue; }
#endif
#endif
#if _TARGET & ( _TARG_80386 | _TARG_IAPX86 )
            if( Check(&name,HW_EAX)) { DumpLiteral("EAX"); continue; }
            if( Check(&name,HW_AX) ) { DumpLiteral( "AX"); continue; }
            if( Check(&name,HW_AL) ) { DumpLiteral( "AL"); continue; }
            if( Check(&name,HW_AH) ) { DumpLiteral( "AH"); continue; }
            if( Check(&name,HW_EBX)) { DumpLiteral("EBX"); continue; }
            if( Check(&name,HW_BX) ) { DumpLiteral( "BX"); continue; }
            if( Check(&name,HW_BL) ) { DumpLiteral( "BL"); continue; }
            if( Check(&name,HW_BH) ) { DumpLiteral( "BH"); continue; }
            if( Check(&name,HW_ECX)) { DumpLiteral("ECX"); continue; }
            if( Check(&name,HW_CX) ) { DumpLiteral( "CX"); continue; }
            if( Check(&name,HW_CL) ) { DumpLiteral( "CL"); continue; }
            if( Check(&name,HW_CH) ) { DumpLiteral( "CH"); continue; }
            if( Check(&name,HW_EDX)) { DumpLiteral("EDX"); continue; }
            if( Check(&name,HW_DX) ) { DumpLiteral( "DX"); continue; }
            if( Check(&name,HW_DL) ) { DumpLiteral( "DL"); continue; }
            if( Check(&name,HW_DH) ) { DumpLiteral( "DH"); continue; }
            if( Check(&name,HW_EDI)) { DumpLiteral("EDI"); continue; }
            if( Check(&name,HW_DI) ) { DumpLiteral( "DI"); continue; }
            if( Check(&name,HW_ESI)) { DumpLiteral("ESI"); continue; }
            if( Check(&name,HW_SI) ) { DumpLiteral( "SI"); continue; }
            if( Check(&name,HW_BP))  { DumpLiteral("EBP"); continue; }
            if( Check(&name,HW_SP))  { DumpLiteral("ESP"); continue; }
            if( Check(&name,HW_GS) ) { DumpLiteral( "GS"); continue; }
            if( Check(&name,HW_FS) ) { DumpLiteral( "FS"); continue; }
            if( Check(&name,HW_ES) ) { DumpLiteral( "ES"); continue; }
            if( Check(&name,HW_DS) ) { DumpLiteral( "DS"); continue; }
            if( Check(&name,HW_CS) ) { DumpLiteral( "CS"); continue; }
            if( Check(&name,HW_SS) ) { DumpLiteral( "SS"); continue; }
            if( Check(&name,HW_ST0) ) { DumpLiteral( "ST(0)"); continue; }
            if( Check(&name,HW_ST1) ) { DumpLiteral( "ST(1)"); continue; }
            if( Check(&name,HW_ST2) ) { DumpLiteral( "ST(2)"); continue; }
            if( Check(&name,HW_ST3) ) { DumpLiteral( "ST(3)"); continue; }
            if( Check(&name,HW_ST4) ) { DumpLiteral( "ST(4)"); continue; }
            if( Check(&name,HW_ST5) ) { DumpLiteral( "ST(5)"); continue; }
            if( Check(&name,HW_ST6) ) { DumpLiteral( "ST(6)"); continue; }
            if( Check(&name,HW_ST7) ) { DumpLiteral( "ST(7)"); continue; }
#endif
            break;
        }
    }
}


static  void    DumpRgSet( hw_reg_set *possible )
/**************************************************/
{
    if( possible != NULL ) {
        DumpLiteral( " Choices " );
        for( ; !HW_CEqual( *possible, HW_EMPTY ); ++possible ) {
            DumpRegName( *possible );
            DumpChar( ' ' );
        }
    }
}


static  void    DumpInsRange( conflict_node *conf ) {
/***************************************************/

    if( conf->ins_range.first != NULL ) {
        DumpLiteral( " Instruction " );
        DumpInt( conf->ins_range.first->id );
        if( conf->ins_range.first != conf->ins_range.last
         && conf->ins_range.last != NULL ) {
            DumpLiteral( " to Instruction " );
            DumpInt( conf->ins_range.last->id );
        }
    }
}


extern  void    DumpPossible( byte idx ) {
/****************************************/


    if( idx == RL_NUMBER_OF_SETS ) {
        DumpLiteral( " Choices ANY" );
    } else {
        DumpRgSet( RegSets[  idx  ] );
    }
}


extern  void    DumpRegSet( hw_reg_set reg ) {
/********************************************/

    DumpRegName( reg );
    DumpNL();
}


extern  void    DumpAConf( conflict_node *conf ) {
/************************************************/

    DumpOperand( conf->name );
    DumpLiteral( " id " );
    DumpGBit( &conf->id.out_of_block );
    DumpChar( ' ' );
    DumpLBit( &conf->id.within_block );
    DumpPossible( conf->possible );
    DumpNL();
    DumpLiteral( "    " );
    DumpInsRange( conf );
    DumpLiteral( " Start block " );
    DumpPtr( conf->start_block );
    DumpNL();
    DumpLiteral( "    Conflicts with " );
    DumpGBit( &conf->with.out_of_block );
    DumpChar( ' ' );
    DumpLBit( &conf->with.within_block );
    DumpChar( ' ' );
    DumpRegName( conf->with.regs );
    DumpNL();
    DumpLiteral( "    Constrained " );
    DumpInt( conf->num_constrained );
    DumpLiteral( " vs " );
    DumpInt( conf->available );
    DumpNL();
    if( _Is( conf, SAVINGS_CALCULATED ) ) {
        DumpLiteral( "    Savings " );
        DumpLong( conf->savings );
        DumpNL();
    }
    if( _Is( conf, CONFLICT_ON_HOLD ) ) {
        DumpLiteral( "    On hold" );
        DumpNL();
    }
    if( _Is( conf, CHANGES_OTHERS ) ) {
        DumpLiteral( "    Changes Others" );
        DumpNL();
    }
    if( _Is( conf, NEEDS_SEGMENT ) ) {
        DumpLiteral( "    Needs segment" );
        DumpNL();
    }
    if( _Is( conf, NEEDS_SEGMENT_SPLIT ) ) {
        DumpLiteral( "    Needs segment split" );
        DumpNL();
    }
    if( _Is( conf, SEGMENT_SPLIT ) ) {
        DumpLiteral( "    Is segment split" );
        DumpNL();
    }
    if( _Is( conf, NEEDS_INDEX ) ) {
        DumpLiteral( "    Needs index" );
        DumpNL();
    }
    if( _Is( conf, NEEDS_INDEX_SPLIT ) ) {
        DumpLiteral( "    Needs index split" );
        DumpNL();
    }
    if( _Is( conf, INDEX_SPLIT ) ) {
        DumpLiteral( "    Is index split" );
        DumpNL();
    }
}


extern  void    DumpConflicts() {
/*******************************/

    conflict_node       *conf;

    DumpLiteral( "Conflict graph" );
    DumpNL();
    for( conf = ConfList; conf != NULL; conf = conf->next_conflict ) {
        DumpPtr( conf );
        DumpChar( ' ' );
        DumpAConf( conf );
    }
    DumpNL();
}
